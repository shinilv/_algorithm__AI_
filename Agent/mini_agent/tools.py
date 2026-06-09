from __future__ import annotations

import ast
import json
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Any, Callable


ToolFunction = Callable[..., str]


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
        return self._tools[name].func(**arguments)

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
            name="now",
            description="Get the current local time.",
            parameters={},
            func=now,
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


def now() -> str:
    return datetime.now().isoformat(timespec="seconds")


def _notes_path() -> Path:
    return Path.cwd() / "data" / "notes.jsonl"

