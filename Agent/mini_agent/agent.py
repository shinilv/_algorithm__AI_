from __future__ import annotations

import json
from dataclasses import dataclass
from typing import Any, Protocol

from .prompts import SYSTEM_PROMPT
from .tools import ToolRegistry


class LLM(Protocol):
    def complete(self, messages: list[dict[str, str]], tools: ToolRegistry) -> str:
        ...


@dataclass
class AgentResult:
    answer: str
    steps: int


class Agent:
    def __init__(self, llm: LLM, tools: ToolRegistry, max_steps: int = 8, debug: bool = False) -> None:
        self.llm = llm
        self.tools = tools
        self.max_steps = max_steps
        self.debug = debug

    def run(self, task: str) -> AgentResult:
        messages = [
            {"role": "system", "content": self._system_prompt()},
            {"role": "user", "content": task},
        ]

        for step in range(1, self.max_steps + 1):
            raw = self.llm.complete(messages, self.tools)
            event = parse_json_object(raw)
            self._trace(step, event)

            if "final" in event:
                return AgentResult(answer=str(event["final"]), steps=step)

            action = str(event.get("action", ""))
            action_input = event.get("action_input", {})
            if not isinstance(action_input, dict):
                raise ValueError("action_input must be a JSON object")

            try:
                observation = self.tools.call(action, action_input)
            except Exception as exc:
                observation = f"Tool error: {exc}"

            if self.debug:
                print(f"[tool] {action} -> {observation}")

            messages.append({"role": "assistant", "content": json.dumps(event, ensure_ascii=False)})
            messages.append({"role": "tool", "content": observation})

        return AgentResult(
            answer=f"Reached max_steps={self.max_steps}. Try making the task smaller or improving the prompt.",
            steps=self.max_steps,
        )

    def _system_prompt(self) -> str:
        return f"{SYSTEM_PROMPT}\n\nAvailable tools:\n{self.tools.descriptions()}"

    def _trace(self, step: int, event: dict[str, Any]) -> None:
        if not self.debug:
            return
        thought = event.get("thought", "")
        action = event.get("action")
        final = event.get("final")
        print(f"\n[step {step}] thought: {thought}")
        if action:
            print(f"[step {step}] action: {action}")
            print(f"[step {step}] input: {event.get('action_input', {})}")
        if final:
            print(f"[step {step}] final: {final}")


def parse_json_object(text: str) -> dict[str, Any]:
    try:
        value = json.loads(text)
    except json.JSONDecodeError:
        start = text.find("{")
        end = text.rfind("}")
        if start == -1 or end == -1 or end <= start:
            raise ValueError(f"Model did not return JSON: {text}") from None
        value = json.loads(text[start : end + 1])

    if not isinstance(value, dict):
        raise ValueError("Model response must be a JSON object")
    return value

