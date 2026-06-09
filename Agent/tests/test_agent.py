import unittest

from mini_agent.llm import MockLLM
from mini_agent.agent import parse_json_object
from mini_agent.tools import build_default_registry


class ParseJsonObjectTest(unittest.TestCase):
    def test_parse_plain_json(self) -> None:
        self.assertEqual(parse_json_object('{"final": "ok"}'), {"final": "ok"})

    def test_parse_json_inside_text(self) -> None:
        self.assertEqual(parse_json_object('note {"final": "ok"} tail'), {"final": "ok"})


class MockLLMTest(unittest.TestCase):
    def test_search_intent_wins_over_number(self) -> None:
        llm = MockLLM()
        response = llm.complete(
            [{"role": "user", "content": "查找包含 288 的笔记"}],
            build_default_registry(),
        )
        event = parse_json_object(response)
        self.assertEqual(event["action"], "search_notes")
        self.assertEqual(event["action_input"], {"query": "288"})

    def test_weather_intent_uses_weather_tool(self) -> None:
        llm = MockLLM()
        response = llm.complete(
            [{"role": "user", "content": "\u5317\u4eac\u5929\u6c14\u600e\u4e48\u6837"}],
            build_default_registry(),
        )
        event = parse_json_object(response)
        self.assertEqual(event["action"], "weather")
        self.assertEqual(event["action_input"], {"city": "\u5317\u4eac"})


if __name__ == "__main__":
    unittest.main()
