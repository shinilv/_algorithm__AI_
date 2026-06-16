import json
import unittest
from unittest.mock import patch

from mini_agent.tools import calculator, list_files, read_file, search_files, weather


class CalculatorTest(unittest.TestCase):
    def test_basic_arithmetic(self) -> None:
        self.assertEqual(calculator("24 * (7 + 5)"), "288")

    def test_rejects_function_calls(self) -> None:
        with self.assertRaises(ValueError):
            calculator("__import__('os').system('dir')")


class WeatherTest(unittest.TestCase):
    def test_known_city(self) -> None:
        payload = {
            "current": {
                "temperature_2m": 30.5,
                "relative_humidity_2m": 49,
                "wind_speed_10m": 4.8,
                "weather_code": 3,
            }
        }
        with (
            patch(
                "mini_agent.tools._geocode_city",
                return_value={"name": "Beijing", "latitude": 39.9, "longitude": 116.4},
            ),
            patch("mini_agent.tools.urllib.request.urlopen", return_value=FakeResponse(payload)),
        ):
            result = weather("Beijing")

        self.assertIn("Weather for Beijing", result)
        self.assertIn("temperature 30.5 C", result)
        self.assertIn("humidity 49%", result)


class FakeResponse:
    def __init__(self, payload: dict) -> None:
        self.payload = payload

    def __enter__(self) -> "FakeResponse":
        return self

    def __exit__(self, exc_type, exc, traceback) -> None:
        return None

    def read(self) -> bytes:
        return json.dumps(self.payload).encode("utf-8")


class FileToolsTest(unittest.TestCase):
    def test_list_files_in_workspace(self) -> None:
        result = list_files(".")
        self.assertIn("README.md", result)
        self.assertIn("mini_agent/tools.py", result)

    def test_read_file(self) -> None:
        result = read_file("README.md")
        self.assertIn("File: README.md", result)
        self.assertIn("Mini Learning Agent", result)

    def test_search_files(self) -> None:
        result = search_files("class ToolRegistry", ".")
        self.assertIn("mini_agent/tools.py", result)
        self.assertIn("class ToolRegistry", result)

    def test_rejects_env_file(self) -> None:
        with self.assertRaises(ValueError):
            read_file(".env")

    def test_rejects_path_outside_workspace(self) -> None:
        with self.assertRaises(ValueError):
            read_file("../outside.txt")


if __name__ == "__main__":
    unittest.main()
