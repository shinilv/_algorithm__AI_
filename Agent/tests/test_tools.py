import unittest

from mini_agent.tools import calculator, list_files, read_file, search_files, weather


class CalculatorTest(unittest.TestCase):
    def test_basic_arithmetic(self) -> None:
        self.assertEqual(calculator("24 * (7 + 5)"), "288")

    def test_rejects_function_calls(self) -> None:
        with self.assertRaises(ValueError):
            calculator("__import__('os').system('dir')")


class WeatherTest(unittest.TestCase):
    def test_known_city(self) -> None:
        result = weather("Beijing")
        self.assertIn("Mock weather for Beijing", result)
        self.assertIn("sunny", result)


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
