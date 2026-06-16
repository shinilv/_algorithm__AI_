from __future__ import annotations

"""RAG 相关工具：把本地资料切块，并构建最简单的向量索引。

这个文件目前负责 RAG 的前两步：

1. Chunking：把本地文本文件切成较小的 DocumentChunk。
2. Embedding Indexing：调用 embeddings API，把每个 chunk 转成向量并保存。

后续的 semantic_search 会基于这里生成的 data/vector_index.jsonl 做余弦相似度检索。
"""

import argparse
import json
import math
import os
import sys
import urllib.error
import urllib.request
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Iterable, Protocol

from .llm import load_dotenv


# 只索引这些常见文本文件。PDF、Word、图片等格式需要额外解析器，先不放进第一版。
READABLE_TEXT_EXTENSIONS = {
    ".csv",
    ".ini",
    ".json",
    ".jsonl",
    ".md",
    ".py",
    ".toml",
    ".txt",
    ".yaml",
    ".yml",
}

# 这些目录通常是依赖、缓存或版本控制数据，不应该进入知识库。
BLOCKED_DIR_NAMES = {".git", ".venv", "__pycache__", "node_modules", "venv"}

# 这些文件即使是文本，也不允许被路径解析访问。例如 .env 里可能有 API Key。
BLOCKED_FILE_NAMES = {".env"}

# 这些文件不是敏感文件，但它们是运行过程中产生的数据。
# 如果把 trace、notes 或旧向量索引也拿去做 RAG，会导致“检索到自己的日志”，污染问答结果。
SKIPPED_INDEX_INPUT_FILES = {"notes.jsonl", "trace.jsonl", "traces.jsonl", "chunks.jsonl", "vector_index.jsonl"}

# 每个 chunk 的最大字符数。太大：上下文噪声多；太小：语义可能被切碎。
DEFAULT_MAX_CHARS = 1200

# 相邻 chunk 保留几行重叠内容，减少“关键信息刚好被切断”的问题。
DEFAULT_OVERLAP_LINES = 2

# 调 embeddings API 时一次提交多少个 chunk。批量请求比逐条请求更快，也更省网络开销。
DEFAULT_BATCH_SIZE = 16

# 语义检索默认返回几个最相关片段。太少可能漏证据，太多会让模型上下文变吵。
DEFAULT_TOP_K = 5


@dataclass(frozen=True)
class DocumentChunk:
    """还没有向量化的文本片段。

    一个 DocumentChunk 对应原始资料中的一小段文本。
    path/start_line/end_line 是来源信息，后面回答问题时可以用来展示引用来源。
    """

    id: str
    path: str
    start_line: int
    end_line: int
    text: str

    def to_dict(self) -> dict[str, object]:
        """把 dataclass 转成普通 dict，方便写入 JSONL。"""

        return asdict(self)


@dataclass(frozen=True)
class EmbeddedChunk:
    """已经向量化的文本片段。

    它比 DocumentChunk 多了 embedding 字段。
    data/vector_index.jsonl 里保存的就是 EmbeddedChunk 的 JSON 形式。
    """

    id: str
    path: str
    start_line: int
    end_line: int
    text: str
    embedding: list[float]

    def to_dict(self) -> dict[str, object]:
        """把向量记录转成普通 dict，方便逐行写入 JSONL。"""

        return asdict(self)


class EmbeddingClient(Protocol):
    """embedding 客户端协议。

    这里用 Protocol 是为了让真实客户端和测试里的 FakeEmbeddingClient 共用同一套接口。
    单元测试不应该真的请求外部 API，所以测试会传入一个假的 embedding client。
    """

    def embed(self, texts: list[str]) -> list[list[float]]:
        ...


class OpenAICompatibleEmbeddingClient:
    """OpenAI-compatible embeddings API 客户端。

    DashScope 的 compatible-mode embeddings 接口和 OpenAI embeddings 接口形态类似：
    POST 一个包含 model/input 的 JSON，返回 data[i].embedding。
    """

    def __init__(self, api_key: str, model: str, base_url: str) -> None:
        self.api_key = api_key
        self.model = model
        self.base_url = base_url

    @classmethod
    def from_env(cls) -> "OpenAICompatibleEmbeddingClient":
        """从 .env 读取 embedding 配置。

        EMBEDDING_API_KEY 可选；如果没有，就复用 OPENAI_API_KEY。
        这样你现在用 DashScope 时，只需要已有的 OPENAI_API_KEY 加上 EMBEDDING_MODEL/BASE_URL。
        """

        load_dotenv()
        api_key = os.getenv("EMBEDDING_API_KEY", "").strip() or os.getenv("OPENAI_API_KEY", "").strip()
        model = os.getenv("EMBEDDING_MODEL", "").strip()
        base_url = os.getenv("EMBEDDING_BASE_URL", "https://api.openai.com/v1/embeddings").strip()

        missing = [name for name, value in {"OPENAI_API_KEY or EMBEDDING_API_KEY": api_key, "EMBEDDING_MODEL": model}.items() if not value]
        if missing:
            joined = ", ".join(missing)
            raise RuntimeError(f"Missing environment variable(s): {joined}. See .env.example.")

        return cls(api_key=api_key, model=model, base_url=base_url)

    def embed(self, texts: list[str]) -> list[list[float]]:
        """把一批文本转成向量。

        输入：
            ["文本 A", "文本 B"]
        输出：
            [[...向量 A...], [...向量 B...]]

        返回顺序必须和输入顺序一致，否则后面 chunk 和 embedding 会配错。
        """

        if not texts:
            return []

        # OpenAI-compatible embeddings 接口的核心请求体：模型名 + 一批输入文本。
        payload = {
            "model": self.model,
            "input": texts,
        }
        data = json.dumps(payload).encode("utf-8")
        request = urllib.request.Request(
            self.base_url,
            data=data,
            headers={
                "Authorization": f"Bearer {self.api_key}",
                "Content-Type": "application/json",
            },
            method="POST",
        )

        try:
            with urllib.request.urlopen(request, timeout=60) as response:
                body = json.loads(response.read().decode("utf-8"))
        except urllib.error.HTTPError as exc:
            detail = exc.read().decode("utf-8", errors="replace")
            raise RuntimeError(f"Embedding request failed: HTTP {exc.code}: {detail}") from exc

        # 兼容接口通常返回 {"data": [{"index": 0, "embedding": [...]}, ...]}。
        items = body.get("data", [])
        if not isinstance(items, list):
            raise RuntimeError(f"Embedding response missing data list: {body}")

        # 有些服务会带 index 字段。排序后再取 embedding，可以避免响应顺序和输入顺序不一致。
        items = sorted(items, key=lambda item: item.get("index", 0) if isinstance(item, dict) else 0)
        embeddings = [item["embedding"] for item in items]
        if len(embeddings) != len(texts):
            raise RuntimeError(f"Embedding count mismatch: expected {len(texts)}, got {len(embeddings)}")
        return embeddings


def build_chunks(
    directory: str,
    max_chars: int = DEFAULT_MAX_CHARS,
    overlap_lines: int = DEFAULT_OVERLAP_LINES,
) -> list[DocumentChunk]:
    """扫描目录并构建文本 chunks。

    这是 RAG 的第一步：把较长的本地资料切成适合检索和塞进上下文的小片段。

    directory:
        要索引的目录，例如 mini_agent、docs、notes。
    max_chars:
        每个 chunk 最多包含多少字符。
    overlap_lines:
        相邻 chunk 重叠多少行，避免上下文被硬切断。
    """

    if max_chars <= 0:
        raise ValueError("max_chars must be greater than 0")
    if overlap_lines < 0:
        raise ValueError("overlap_lines must not be negative")

    target = _resolve_workspace_path(directory)
    if not target.exists():
        raise ValueError(f"Directory does not exist: {directory}")
    if not target.is_dir():
        raise ValueError(f"Path is not a directory: {directory}")

    chunks: list[DocumentChunk] = []
    # 对目录下每个允许读取的文本文件分别切块，再汇总成一个列表。
    for path in _iter_readable_files(target):
        chunks.extend(_chunk_file(path, max_chars=max_chars, overlap_lines=overlap_lines))
    return chunks


def write_chunks_jsonl(chunks: Iterable[DocumentChunk], output_path: str = "data/chunks.jsonl") -> str:
    """把未向量化的 chunks 写入 JSONL。

    这个函数主要用于调试：你可以先看切块效果，再决定是否调用 embedding API。
    """

    target = _resolve_workspace_path(output_path)
    if target.name in BLOCKED_FILE_NAMES:
        raise ValueError(f"Output file is blocked: {output_path}")

    target.parent.mkdir(parents=True, exist_ok=True)
    count = 0
    with target.open("w", encoding="utf-8") as file:
        for chunk in chunks:
            file.write(json.dumps(chunk.to_dict(), ensure_ascii=False) + "\n")
            count += 1

    return f"Wrote {count} chunks to {_relative_path(target)}"


def embed_chunks(
    chunks: Iterable[DocumentChunk],
    embedding_client: EmbeddingClient,
    batch_size: int = DEFAULT_BATCH_SIZE,
) -> list[EmbeddedChunk]:
    """把 DocumentChunk 批量转成 EmbeddedChunk。

    这是 RAG 的第二步：调用 embedding 模型，把每个文本片段转成向量。
    这里不关心具体使用哪家 embedding 服务，只要求 embedding_client 实现 embed(texts)。
    """

    if batch_size <= 0:
        raise ValueError("batch_size must be greater than 0")

    chunk_list = list(chunks)
    embedded: list[EmbeddedChunk] = []

    # 分批调用 embedding API，避免一次请求文本过多导致接口失败。
    for start in range(0, len(chunk_list), batch_size):
        batch = chunk_list[start : start + batch_size]
        embeddings = embedding_client.embed([chunk.text for chunk in batch])
        if len(embeddings) != len(batch):
            raise RuntimeError(f"Embedding count mismatch: expected {len(batch)}, got {len(embeddings)}")

        for chunk, embedding in zip(batch, embeddings):
            embedded.append(
                EmbeddedChunk(
                    id=chunk.id,
                    path=chunk.path,
                    start_line=chunk.start_line,
                    end_line=chunk.end_line,
                    text=chunk.text,
                    embedding=embedding,
                )
            )

    return embedded


def write_vector_index(records: Iterable[EmbeddedChunk], output_path: str = "data/vector_index.jsonl") -> str:
    """把向量索引写入 JSONL。

    data/vector_index.jsonl 是当前知识库的“向量快照”。
    新增、删除、修改本地资料后，需要重新构建这个文件，否则检索用的还是旧内容。
    """

    target = _resolve_workspace_path(output_path)
    if target.name in BLOCKED_FILE_NAMES:
        raise ValueError(f"Output file is blocked: {output_path}")

    target.parent.mkdir(parents=True, exist_ok=True)
    count = 0
    with target.open("w", encoding="utf-8") as file:
        for record in records:
            file.write(json.dumps(record.to_dict(), ensure_ascii=False) + "\n")
            count += 1

    return f"Wrote {count} embedded chunks to {_relative_path(target)}"


def build_vector_index(
    directory: str,
    output_path: str = "data/vector_index.jsonl",
    max_chars: int = DEFAULT_MAX_CHARS,
    overlap_lines: int = DEFAULT_OVERLAP_LINES,
    batch_size: int = DEFAULT_BATCH_SIZE,
    embedding_client: EmbeddingClient | None = None,
) -> str:
    """一键构建向量索引。

    完整流程：
        1. build_chunks(directory)
        2. embed_chunks(chunks)
        3. write_vector_index(records)

    embedding_client 参数主要给测试使用；真实运行时不传，会自动从 .env 创建客户端。
    """

    client = embedding_client or OpenAICompatibleEmbeddingClient.from_env()
    chunks = build_chunks(directory, max_chars=max_chars, overlap_lines=overlap_lines)
    embedded = embed_chunks(chunks, embedding_client=client, batch_size=batch_size)
    return write_vector_index(embedded, output_path=output_path)


def semantic_search(
    query: str,
    top_k: int | str = DEFAULT_TOP_K,
    index_path: str = "data/vector_index.jsonl",
    embedding_client: EmbeddingClient | None = None,
) -> str:
    """基于向量索引做语义检索。

    完整流程：
        1. 把用户 query 转成 query embedding
        2. 读取 data/vector_index.jsonl 里的 chunk embedding
        3. 对 query 和每个 chunk 计算余弦相似度
        4. 返回分数最高的 top_k 个片段

    这就是当前项目里的最小版向量检索。
    """

    query_text = query.strip()
    if not query_text:
        raise ValueError("query must not be empty")

    limit = _parse_top_k(top_k)
    records = load_vector_index(index_path)
    if not records:
        return f"No vector index records found in {index_path}. Build the index first."

    client = embedding_client or OpenAICompatibleEmbeddingClient.from_env()
    query_embeddings = client.embed([query_text])
    if len(query_embeddings) != 1:
        raise RuntimeError(f"Expected 1 query embedding, got {len(query_embeddings)}")

    query_embedding = query_embeddings[0]
    scored: list[tuple[float, EmbeddedChunk]] = []
    for record in records:
        score = cosine_similarity(query_embedding, record.embedding)
        scored.append((score, record))

    scored.sort(key=lambda item: item[0], reverse=True)
    return format_search_results(scored[:limit])


def load_vector_index(index_path: str = "data/vector_index.jsonl") -> list[EmbeddedChunk]:
    """读取向量索引 JSONL。

    每一行都应该包含 path/start_line/end_line/text/embedding。
    如果文件不存在，说明还没有执行 build_vector_index。
    """

    target = _resolve_workspace_path(index_path)
    if not target.exists():
        raise ValueError(f"Vector index does not exist: {index_path}. Build it with python -m mini_agent.rag ...")
    if not target.is_file():
        raise ValueError(f"Vector index path is not a file: {index_path}")

    records: list[EmbeddedChunk] = []
    with target.open("r", encoding="utf-8") as file:
        for line_number, line in enumerate(file, start=1):
            if not line.strip():
                continue
            raw = json.loads(line)
            records.append(
                EmbeddedChunk(
                    id=str(raw["id"]),
                    path=str(raw["path"]),
                    start_line=int(raw["start_line"]),
                    end_line=int(raw["end_line"]),
                    text=str(raw["text"]),
                    embedding=[float(value) for value in raw["embedding"]],
                )
            )

    return records


def cosine_similarity(left: list[float], right: list[float]) -> float:
    """计算两个向量的余弦相似度。

    返回值越接近 1，说明两个向量方向越接近，也就是语义越相似。
    """

    if len(left) != len(right):
        raise ValueError(f"Vector dimension mismatch: {len(left)} != {len(right)}")
    if not left:
        raise ValueError("vectors must not be empty")

    dot = sum(a * b for a, b in zip(left, right))
    left_norm = math.sqrt(sum(value * value for value in left))
    right_norm = math.sqrt(sum(value * value for value in right))
    if left_norm == 0 or right_norm == 0:
        return 0.0
    return dot / (left_norm * right_norm)


def format_search_results(results: list[tuple[float, EmbeddedChunk]]) -> str:
    """把检索结果格式化成适合喂给模型的文本。

    返回内容包含来源文件、行号、相似度分数和片段正文。
    后续模型回答问题时，可以基于这些片段生成答案并附带引用。
    """

    if not results:
        return "No semantic search results found."

    blocks: list[str] = []
    for rank, (score, record) in enumerate(results, start=1):
        blocks.append(
            "\n".join(
                [
                    f"[{rank}] {record.path}:{record.start_line}-{record.end_line} score={score:.4f}",
                    record.text,
                ]
            )
        )

    return "\n\n---\n\n".join(blocks)


def _chunk_file(path: Path, max_chars: int, overlap_lines: int) -> list[DocumentChunk]:
    """把单个文件按行切成多个 DocumentChunk。

    这里选择“按行累积字符数”的简单策略，而不是按固定字符硬切。
    好处是不会把一行代码、一个标题或一条配置从中间截断。
    """

    relative_path = _relative_path(path)
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    chunks: list[DocumentChunk] = []

    start_index = 0
    while start_index < len(lines):
        end_index = start_index
        char_count = 0

        # 从 start_index 开始向后累积行，直到接近 max_chars。
        while end_index < len(lines):
            next_line = lines[end_index]
            next_size = len(next_line) + 1
            if end_index > start_index and char_count + next_size > max_chars:
                break
            char_count += next_size
            end_index += 1

        text = "\n".join(lines[start_index:end_index]).strip()
        if text:
            chunk_index = len(chunks)
            chunks.append(
                DocumentChunk(
                    id=f"{relative_path}#{chunk_index}",
                    path=relative_path,
                    start_line=start_index + 1,
                    end_line=end_index,
                    text=text,
                )
            )

        if end_index >= len(lines):
            break

        # 下一块从 end_index 往前退 overlap_lines 行开始，保留一点上下文重叠。
        next_start = max(end_index - overlap_lines, start_index + 1)
        start_index = next_start

    return chunks


def _parse_top_k(top_k: int | str) -> int:
    try:
        value = int(top_k)
    except (TypeError, ValueError):
        raise ValueError(f"top_k must be an integer, got {top_k!r}") from None

    if value <= 0:
        raise ValueError("top_k must be greater than 0")
    return min(value, 20)


def _workspace_root() -> Path:
    """当前项目根目录。

    这个项目默认从项目根目录运行，例如 D:/C/Agent，所以 Path.cwd() 就是工作区根。
    """

    return Path.cwd().resolve()


def _resolve_workspace_path(raw_path: str) -> Path:
    """把用户传入路径解析成工作区内的安全绝对路径。

    这个函数是安全边界：
    - 不允许空路径
    - 不允许跳出当前 workspace
    - 不允许访问 .git、__pycache__ 等目录
    - 不允许访问 .env 等敏感文件
    """

    if not raw_path.strip():
        raise ValueError("path must not be empty")

    root = _workspace_root()
    candidate = Path(raw_path).expanduser()
    if not candidate.is_absolute():
        candidate = root / candidate
    target = candidate.resolve()

    if target != root and root not in target.parents:
        raise ValueError(f"Path is outside workspace: {raw_path}")

    relative_parts = target.relative_to(root).parts if target != root else ()
    if any(part in BLOCKED_DIR_NAMES for part in relative_parts):
        raise ValueError(f"Path is blocked: {raw_path}")
    if target.name in BLOCKED_FILE_NAMES:
        raise ValueError(f"File is blocked: {raw_path}")

    return target


def _iter_readable_files(directory: Path) -> list[Path]:
    """列出目录下可进入索引的文本文件。"""

    files: list[Path] = []
    root = _workspace_root()

    for path in sorted(directory.rglob("*")):
        try:
            relative_parts = path.relative_to(root).parts
        except ValueError:
            continue

        if any(part in BLOCKED_DIR_NAMES for part in relative_parts):
            continue
        if path.is_file() and _is_readable_text_file(path):
            files.append(path)

    return files


def _is_readable_text_file(path: Path) -> bool:
    """判断某个文件是否适合进入 RAG 索引。"""

    return (
        path.name not in BLOCKED_FILE_NAMES
        and path.name not in SKIPPED_INDEX_INPUT_FILES
        and path.suffix.lower() in READABLE_TEXT_EXTENSIONS
    )


def _relative_path(path: Path) -> str:
    """把绝对路径转成相对工作区的路径，方便展示来源。"""

    root = _workspace_root()
    try:
        return path.resolve().relative_to(root).as_posix() or "."
    except ValueError:
        return str(path)


def main() -> None:
    """命令行入口。

    示例：
        python -m mini_agent.rag mini_agent --output data/vector_index.jsonl
    """

    # 兼容旧命令：python -m mini_agent.rag mini_agent ...
    # 如果第一个参数不是 build/search，就默认插入 build 子命令。
    if len(sys.argv) > 1 and sys.argv[1] not in {"build", "search", "-h", "--help"}:
        sys.argv.insert(1, "build")

    parser = argparse.ArgumentParser(description="Build and query a local vector index for Mini Agent RAG.")
    subparsers = parser.add_subparsers(dest="command")

    build_parser = subparsers.add_parser("build", help="Build a vector index")
    build_parser.add_argument("directory", help="Workspace directory to index, for example: mini_agent or docs")
    build_parser.add_argument("--output", default="data/vector_index.jsonl", help="Output JSONL index path")
    build_parser.add_argument("--max-chars", type=int, default=DEFAULT_MAX_CHARS)
    build_parser.add_argument("--overlap-lines", type=int, default=DEFAULT_OVERLAP_LINES)
    build_parser.add_argument("--batch-size", type=int, default=DEFAULT_BATCH_SIZE)

    search_parser = subparsers.add_parser("search", help="Semantic search the vector index")
    search_parser.add_argument("query", help="Search query")
    search_parser.add_argument("--index", default="data/vector_index.jsonl", help="Vector index JSONL path")
    search_parser.add_argument("--top-k", type=int, default=DEFAULT_TOP_K)
    args = parser.parse_args()

    if args.command in (None, "build"):
        if args.command is None:
            parser.error("missing command. Use 'build' or 'search'.")
        message = build_vector_index(
            directory=args.directory,
            output_path=args.output,
            max_chars=args.max_chars,
            overlap_lines=args.overlap_lines,
            batch_size=args.batch_size,
        )
        print(message)
        return

    if args.command == "search":
        print(semantic_search(args.query, top_k=args.top_k, index_path=args.index))


if __name__ == "__main__":
    main()
