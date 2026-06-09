import unittest

from mini_agent.tools import calculator


class CalculatorTest(unittest.TestCase):
    def test_basic_arithmetic(self) -> None:
        self.assertEqual(calculator("24 * (7 + 5)"), "288")

    def test_rejects_function_calls(self) -> None:
        with self.assertRaises(ValueError):
            calculator("__import__('os').system('dir')")


if __name__ == "__main__":
    unittest.main()

