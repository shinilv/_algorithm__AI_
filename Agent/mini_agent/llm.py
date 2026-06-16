from __future__ import annotations

import json
import os
import re
import urllib.error
import urllib.request
from pathlib import Path
from typing import Any

from .tools import ToolRegistry


class MockLLM:
    """A deterministic fake LLM for learning the agent loop without an API key."""

    def complete(self, messages: list[dict[str, str]], tools: ToolRegistry) -> str:
        last = messages[-1]
        original_task = next(message["content"] for message in messages if message["role"] == "user")

        if last["role"] == "user":
            if _is_file_list_task(original_task):
                return _json(
                    thought="I should list readable files in the workspace.",
                    action="list_files",
                    action_input={"directory": "."},
                )
            if _is_file_read_task(original_task):
                return _json(
                    thought="I should read the requested workspace file.",
                    action="read_file",
                    action_input={"path": _extract_file_path(original_task)},
                )
            if _is_file_search_task(original_task):
                return _json(
                    thought="I should search readable workspace files.",
                    action="search_files",
                    action_input={"query": _extract_file_query(original_task), "directory": "."},
                )
            if "查找" in original_task or "搜索" in original_task or "search" in original_task.lower():
                query = _extract_query(original_task)
                return _json(
                    thought="I should search the saved notes.",
                    action="search_notes",
                    action_input={"query": query},
                )
            if _is_weather_task(original_task):
                city = _extract_city(original_task)
                return _json(
                    thought="I should check the weather tool.",
                    action="weather",
                    action_input={"city": city},
                )
            expression = _extract_expression(original_task)
            if expression:
                return _json(
                    thought="I should calculate the arithmetic expression first.",
                    action="calculator",
                    action_input={"expression": expression},
                )
            if "时间" in original_task or "time" in original_task.lower():
                return _json(thought="I should check the current time.", action="now", action_input={})
            return _json(
                thought="This simple task can be answered directly.",
                final="这是 mock 模型的直接回答。你可以切换到 --backend openai 使用真实模型。",
            )

        observation = last["content"]
        if _should_save(original_task) and not _already_used(messages, "save_note"):
            return _json(
                thought="The user asked to save the result, so I should write a note.",
                action="save_note",
                action_input={"title": "Agent learning note", "content": f"Result: {observation}"},
            )

        return _json(thought="The required tool work is complete.", final=f"完成：{observation}")


class OpenAICompatibleLLM:
    def __init__(self, api_key: str, model: str, base_url: str) -> None:
        self.api_key = api_key
        self.model = model
        self.base_url = base_url

    @classmethod
    def from_env(cls) -> "OpenAICompatibleLLM":
        load_dotenv()
        api_key = os.getenv("OPENAI_API_KEY", "").strip()
        model = os.getenv("OPENAI_MODEL", "").strip()
        base_url = os.getenv("OPENAI_BASE_URL", "https://api.openai.com/v1/chat/completions").strip()

        missing = [name for name, value in {"OPENAI_API_KEY": api_key, "OPENAI_MODEL": model}.items() if not value]
        if missing:
            joined = ", ".join(missing)
            raise RuntimeError(f"Missing environment variable(s): {joined}. See .env.example.")

        return cls(api_key=api_key, model=model, base_url=base_url)

    def complete(self, messages: list[dict[str, str]], tools: ToolRegistry) -> str:
        payload = {
            "model": self.model,
            "messages": messages,
            "temperature": 0.2,
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
            raise RuntimeError(f"API request failed: HTTP {exc.code}: {detail}") from exc

        return body["choices"][0]["message"]["content"]


def load_dotenv(path: str | Path = ".env") -> None:
    env_path = Path(path)
    if not env_path.exists():
        return
    for raw_line in env_path.read_text(encoding="utf-8").splitlines():
        line = raw_line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        key, value = line.split("=", 1)
        os.environ[key.strip()] = value.strip().strip('"').strip("'")


def _json(**payload: Any) -> str:
    return json.dumps(payload, ensure_ascii=False)


def _extract_expression(text: str) -> str | None:
    matches = re.findall(r"[0-9][0-9\s+\-*/().%]*[0-9)]", text)
    if not matches:
        return None
    return max(matches, key=len).strip()


def _extract_query(text: str) -> str:
    quoted = re.findall(r"[\"'“”‘’](.+?)[\"'“”‘’]", text)
    if quoted:
        return quoted[0].strip()
    contains_match = re.search(r"包含\s*([\w.+\-*/()]+)", text)
    if contains_match:
        return contains_match.group(1).strip()
    match = re.search(r"(?:包含|查找|搜索)\s*([\w\u4e00-\u9fff.+\-*/() ]+)", text)
    if match:
        return match.group(1).strip()
    return text.strip()


def _is_file_list_task(text: str) -> bool:
    lowered = text.lower()
    return ("list files" in lowered or "列出" in text or "有哪些文件" in text) and _mentions_workspace(text)


def _is_file_read_task(text: str) -> bool:
    lowered = text.lower()
    has_read_intent = "read" in lowered or "读取" in text or "打开" in text
    return has_read_intent and (_has_explicit_file_path(text) or "readme" in lowered or "文件" in text)


def _is_file_search_task(text: str) -> bool:
    lowered = text.lower()
    has_search_intent = "search" in lowered or "查找" in text or "搜索" in text or "哪里定义" in text
    return has_search_intent and _mentions_workspace(text)


def _mentions_workspace(text: str) -> bool:
    lowered = text.lower()
    return any(
        keyword in lowered
        for keyword in ["file", "files", "project", "workspace", "code", ".py", ".md", ".txt", "readme"]
    ) or any(keyword in text for keyword in ["文件", "项目", "代码", "仓库", "目录", "工程", "哪里定义"])


def _extract_file_path(text: str) -> str:
    match = re.search(r"([\w./\\-]+\.(?:csv|ini|json|jsonl|md|py|toml|txt|ya?ml))", text, flags=re.IGNORECASE)
    if match:
        return match.group(1).replace("\\", "/")
    return "README.md"


def _has_explicit_file_path(text: str) -> bool:
    return re.search(r"[\w./\\-]+\.(?:csv|ini|json|jsonl|md|py|toml|txt|ya?ml)", text, flags=re.IGNORECASE) is not None


def _extract_file_query(text: str) -> str:
    quoted = re.findall(r"[\"'“”‘’](.+?)[\"'“”‘’]", text)
    if quoted:
        return quoted[0].strip()

    for pattern in [
        r"(class\s+\w+)",
        r"(def\s+\w+)",
        r"哪里定义了?\s*([A-Za-z_][\w_]*)",
        r"(?:搜索|查找).*?(?:文件|项目|代码|仓库|工程).*?(?:里|中)?\s*([A-Za-z_][\w_]*)",
    ]:
        match = re.search(pattern, text, flags=re.IGNORECASE)
        if match:
            return match.group(1).strip()

    return _extract_query(text)


def _is_weather_task(text: str) -> bool:
    return "weather" in text.lower() or "\u5929\u6c14" in text


def _extract_city(text: str) -> str:
    quoted = re.findall(r"[\"'](.+?)[\"']", text)
    if quoted:
        return quoted[0].strip()

    english = re.search(r"weather\s+(?:in|for)\s+([\w .-]+)", text, flags=re.IGNORECASE)
    if english:
        return english.group(1).strip(" ?,.")

    before_weather = re.search(r"([\w\u4e00-\u9fff]+)\s*\u7684?\s*\u5929\u6c14", text)
    if before_weather:
        city = before_weather.group(1).strip()
        prefixes = [
            "\u5e2e\u6211\u770b\u770b",
            "\u5e2e\u6211\u67e5\u67e5",
            "\u5e2e\u6211\u67e5",
            "\u67e5\u8be2",
            "\u770b\u770b",
            "\u67e5\u67e5",
            "\u67e5",
        ]
        for prefix in prefixes:
            if city.startswith(prefix):
                city = city[len(prefix) :]
                break
        return city.strip() or "local"

    return "local"


def _should_save(text: str) -> bool:
    lowered = text.lower()
    return any(keyword in lowered for keyword in ["保存", "记录", "save", "note"])


def _already_used(messages: list[dict[str, str]], action: str) -> bool:
    needle = f'"action": "{action}"'
    compact_needle = f'"action":"{action}"'
    return any(needle in message["content"] or compact_needle in message["content"] for message in messages)
