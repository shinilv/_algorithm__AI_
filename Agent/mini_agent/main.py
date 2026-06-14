from __future__ import annotations

import argparse

from .agent import Agent
from .llm import MockLLM, OpenAICompatibleLLM
from .tools import build_default_registry


def main() -> None:
    parser = argparse.ArgumentParser(description="Run a tiny educational agent.")
    parser.add_argument("task", help="Task for the agent to solve.")
    parser.add_argument("--backend", choices=["mock", "openai"], default="mock")
    parser.add_argument("--debug", action="store_true", help="Print each reasoning/tool step.")
    parser.add_argument("--max-steps", type=int, default=8)
    args = parser.parse_args()

    llm = MockLLM() if args.backend == "mock" else OpenAICompatibleLLM.from_env()
    agent = Agent(llm=llm, tools=build_default_registry(), max_steps=args.max_steps, debug=args.debug)
    result = agent.run(args.task)

    print("\nAnswer:")
    print(result.answer)
    print(f"\nSteps: {result.steps}")


if __name__ == "__main__":
    main()
