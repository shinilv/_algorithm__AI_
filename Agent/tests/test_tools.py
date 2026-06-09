import unittest

from mini_agent.tools import calculator, weather


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


if __name__ == "__main__":
    unittest.main()
