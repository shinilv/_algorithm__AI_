from __future__ import annotations

import ast
import json
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Any, Callable
import urllib.parse
import urllib.request


ToolFunction = Callable[..., str]
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
BLOCKED_DIR_NAMES = {".git", ".venv", "__pycache__", "node_modules", "venv"}
BLOCKED_FILE_NAMES = {".env", "notes.jsonl", "trace.jsonl", "traces.jsonl"}
MAX_LIST_FILES = 80
MAX_READ_CHARS = 6000
MAX_SEARCH_MATCHES = 20


@dataclass(frozen=True)
class Tool:
    name: str
    description: str
    parameters: dict[str, str]
    func: ToolFunction

    def schema_text(self) -> str:
        params = ", ".join(f"{name}: {desc}" for name, desc in self.parameters.items())
        return f"- {self.name}({params}) -> {self.description}"


class ToolRegistry:
    def __init__(self) -> None:
        self._tools: dict[str, Tool] = {}

    def register(self, tool: Tool) -> None:
        if tool.name in self._tools:
            raise ValueError(f"Tool already registered: {tool.name}")
        self._tools[tool.name] = tool

    def call(self, name: str, arguments: dict[str, Any]) -> str:
        if name not in self._tools:
            known = ", ".join(sorted(self._tools))
            raise ValueError(f"Unknown tool: {name}. Known tools: {known}")

        tool = self._tools[name]
        # 参数校验（简单版本）
        missing = set(tool.parameters) - set(arguments)
        if missing:
            raise ValueError(f"Missing argument(s) for {name}: {', '.join(sorted(missing))}")

        extra = set(arguments) - set(tool.parameters)
        if extra:
            raise ValueError(f"Unknown argument(s) for {name}: {', '.join(sorted(extra))}")

        return tool.func(**arguments)

    def descriptions(self) -> str:
        return "\n".join(tool.schema_text() for tool in self._tools.values())


def build_default_registry() -> ToolRegistry:
    registry = ToolRegistry()
    registry.register(
        Tool(
            name="calculator",
            description="Evaluate a safe arithmetic expression.",
            parameters={"expression": "Arithmetic expression, for example: 24 * (7 + 5)"},
            func=calculator,
        )
    )
    registry.register(
        Tool(
            name="save_note",
            description="Save a short learning note to data/notes.jsonl.",
            parameters={"title": "Note title", "content": "Note content"},
            func=save_note,
        )
    )
    registry.register(
        Tool(
            name="search_notes",
            description="Search saved notes by keyword.",
            parameters={"query": "Keyword to search in titles and content"},
            func=search_notes,
        )
    )
    registry.register(
        Tool(
            name="weather",
            description="Return mock weather for a city. This demo does not call a real weather API.",
            parameters={"city": "City name, for example: Beijing"},
            func=weather,
        )
    )
    registry.register(
        Tool(
            name="now",
            description="Get the current local time.",
            parameters={},
            func=now,
        )
    )
    registry.register(
        Tool(
            name="echo",
            description="Return the input text. Useful for testing.",
            parameters={"text": "Any text to echo back"},
            func=lambda text: text,
        )
    )
    registry.register(
        Tool(
            name="list_files",
            description="List readable text files under a workspace directory.",
            parameters={"directory": "Workspace directory to list, for example: . or mini_agent"},
            func=list_files,
        )
    )
    registry.register(
        Tool(
            name="read_file",
            description="Read a readable text file in the workspace. Long files are truncated.",
            parameters={"path": "Workspace file path, for example: README.md or mini_agent/agent.py"},
            func=read_file,
        )
    )
    registry.register(
        Tool(
            name="search_files",
            description="Search readable text files in a workspace directory by keyword.",
            parameters={"query": "Keyword to search for", "directory": "Workspace directory to search, for example: ."},
            func=search_files,
        )
    )
    return registry


def calculator(expression: str) -> str:
    tree = ast.parse(expression, mode="eval")
    result = _eval_node(tree.body)
    return str(result)


def _eval_node(node: ast.AST) -> float | int:
    if isinstance(node, ast.Constant) and isinstance(node.value, (int, float)):
        return node.value
    if isinstance(node, ast.UnaryOp) and isinstance(node.op, (ast.UAdd, ast.USub)):
        value = _eval_node(node.operand)
        return value if isinstance(node.op, ast.UAdd) else -value
    if isinstance(node, ast.BinOp):
        left = _eval_node(node.left)
        right = _eval_node(node.right)
        if isinstance(node.op, ast.Add):
            return left + right
        if isinstance(node.op, ast.Sub):
            return left - right
        if isinstance(node.op, ast.Mult):
            return left * right
        if isinstance(node.op, ast.Div):
            return left / right
        if isinstance(node.op, ast.FloorDiv):
            return left // right
        if isinstance(node.op, ast.Mod):
            return left % right
        if isinstance(node.op, ast.Pow):
            return left**right
    raise ValueError(f"Unsupported expression: {ast.dump(node)}")


def save_note(title: str, content: str) -> str:
    path = _notes_path()
    path.parent.mkdir(parents=True, exist_ok=True)
    record = {
        "title": title,
        "content": content,
        "created_at": datetime.now().isoformat(timespec="seconds"),
    }
    with path.open("a", encoding="utf-8") as file:
        file.write(json.dumps(record, ensure_ascii=False) + "\n")
    return f"Saved note: {title}"


def search_notes(query: str) -> str:
    path = _notes_path()
    if not path.exists():
        return "No notes found yet."

    matches: list[str] = []
    query_lower = query.lower()
    with path.open("r", encoding="utf-8") as file:
        for line in file:
            note = json.loads(line)
            text = f"{note['title']}\n{note['content']}".lower()
            if query_lower in text:
                matches.append(f"{note['title']}: {note['content']}")

    if not matches:
        return f"No notes matched: {query}"
    return "\n".join(matches[:5])


def weather(city: str) -> str:
    city_name = city.strip()
    if not city_name:
        raise ValueError("city must not be empty")

    location = _geocode_city(city_name)
    latitude = location["latitude"]
    longitude = location["longitude"]

    params = urllib.parse.urlencode({
        "latitude": latitude,
        "longitude": longitude,
        "current": "temperature_2m,relative_humidity_2m,weather_code,wind_speed_10m",
        "timezone": "auto",
    })
    url = f"https://api.open-meteo.com/v1/forecast?{params}"

    with urllib.request.urlopen(url, timeout=10) as response:
        data = json.loads(response.read().decode("utf-8"))

    current = data["current"]
    return (
        f"Weather for {location['name']}: "
        f"temperature {current['temperature_2m']} C, "
        f"humidity {current['relative_humidity_2m']}%, "
        f"wind speed {current['wind_speed_10m']} km/h, "
        f"weather code {current['weather_code']}."
    )


def _geocode_city(city: str) -> dict[str, Any]:
    params = urllib.parse.urlencode({
        "name": city,
        "count": 1,
        "language": "zh",
        "format": "json",
    })
    url = f"https://geocoding-api.open-meteo.com/v1/search?{params}"

    with urllib.request.urlopen(url, timeout=10) as response:
        data = json.loads(response.read().decode("utf-8"))

    results = data.get("results", [])
    if not results:
        raise ValueError(f"Could not find city: {city}")

    return results[0]

def now() -> str:
    return datetime.now().isoformat(timespec="seconds")


def list_files(directory: str) -> str:
    target = _resolve_workspace_path(directory)
    if not target.exists():
        raise ValueError(f"Directory does not exist: {directory}")
    if not target.is_dir():
        raise ValueError(f"Path is not a directory: {directory}")

    files: list[str] = []
    for path in _iter_readable_files(target):
        files.append(_relative_path(path))
        if len(files) >= MAX_LIST_FILES:
            files.append(f"... truncated after {MAX_LIST_FILES} files")
            break

    if not files:
        return f"No readable text files found in {_relative_path(target)}."
    return "\n".join(files)


def read_file(path: str) -> str:
    target = _resolve_workspace_path(path)
    if not target.exists():
        raise ValueError(f"File does not exist: {path}")
    if not target.is_file():
        raise ValueError(f"Path is not a file: {path}")
    if not _is_readable_text_file(target):
        raise ValueError(f"File type is not allowed: {_relative_path(target)}")

    content = target.read_text(encoding="utf-8", errors="replace")
    truncated = len(content) > MAX_READ_CHARS
    if truncated:
        content = content[:MAX_READ_CHARS]

    header = f"File: {_relative_path(target)}"
    if truncated:
        return f"{header}\n\n{content}\n\n[truncated after {MAX_READ_CHARS} characters]"
    return f"{header}\n\n{content}"


def search_files(query: str, directory: str) -> str:
    query_text = query.strip()
    if not query_text:
        raise ValueError("query must not be empty")

    target = _resolve_workspace_path(directory)
    if not target.exists():
        raise ValueError(f"Directory does not exist: {directory}")
    if not target.is_dir():
        raise ValueError(f"Path is not a directory: {directory}")

    matches: list[str] = []
    query_lower = query_text.lower()
    for path in _iter_readable_files(target):
        with path.open("r", encoding="utf-8", errors="replace") as file:
            for line_number, line in enumerate(file, start=1):
                if query_lower in line.lower():
                    preview = line.strip()
                    matches.append(f"{_relative_path(path)}:{line_number}: {preview}")
                    if len(matches) >= MAX_SEARCH_MATCHES:
                        return "\n".join(matches)

    if not matches:
        return f"No matches for {query_text!r} in {_relative_path(target)}."
    return "\n".join(matches)


def _notes_path() -> Path:
    return Path.cwd() / "data" / "notes.jsonl"


def _workspace_root() -> Path:
    return Path.cwd().resolve()


def _resolve_workspace_path(raw_path: str) -> Path:
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
    files: list[Path] = []
    for path in sorted(directory.rglob("*")):
        try:
            relative_parts = path.relative_to(_workspace_root()).parts
        except ValueError:
            continue
        if any(part in BLOCKED_DIR_NAMES for part in relative_parts):
            continue
        if path.is_file() and _is_readable_text_file(path):
            files.append(path)
    return files


def _is_readable_text_file(path: Path) -> bool:
    return path.name not in BLOCKED_FILE_NAMES and path.suffix.lower() in READABLE_TEXT_EXTENSIONS


def _relative_path(path: Path) -> str:
    root = _workspace_root()
    try:
        return path.resolve().relative_to(root).as_posix() or "."
    except ValueError:
        return str(path)
