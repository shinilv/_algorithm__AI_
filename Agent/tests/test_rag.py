import json
import tempfile
import unittest
from pathlib import Path

from mini_agent.rag import build_chunks, build_vector_index, embed_chunks, write_chunks_jsonl, write_vector_index


class BuildChunksTest(unittest.TestCase):
    def test_build_chunks_with_line_ranges(self) -> None:
        with tempfile.TemporaryDirectory(dir=Path.cwd()) as temp_dir:
            path = Path(temp_dir) / "notes.md"
            path.write_text(
                "\n".join(
                    [
                        "# Agent Notes",
                        "ToolRegistry stores available tools.",
                        "Agent.run asks the model for the next action.",
                        "The observation is appended to messages.",
                        "The final answer ends the loop.",
                    ]
                ),
                encoding="utf-8",
            )

            chunks = build_chunks(temp_dir, max_chars=90, overlap_lines=1)

        self.assertGreaterEqual(len(chunks), 2)
        self.assertTrue(chunks[0].path.endswith("notes.md"))
        self.assertEqual(chunks[0].start_line, 1)
        self.assertGreaterEqual(chunks[0].end_line, chunks[0].start_line)
        self.assertIn("ToolRegistry", chunks[0].text)
        self.assertEqual(chunks[1].start_line, chunks[0].end_line)

    def test_build_chunks_skips_blocked_files_and_dirs(self) -> None:
        with tempfile.TemporaryDirectory(dir=Path.cwd()) as temp_dir:
            root = Path(temp_dir)
            (root / ".env").write_text("OPENAI_API_KEY=secret", encoding="utf-8")
            (root / "trace.jsonl").write_text('{"task": "secret"}', encoding="utf-8")
            (root / "__pycache__").mkdir()
            (root / "__pycache__" / "cached.py").write_text("print('skip')", encoding="utf-8")
            (root / "public.md").write_text("Visible document", encoding="utf-8")

            chunks = build_chunks(temp_dir)

        paths = {chunk.path for chunk in chunks}
        self.assertTrue(any(path.endswith("public.md") for path in paths))
        self.assertFalse(any(path.endswith(".env") for path in paths))
        self.assertFalse(any(path.endswith("trace.jsonl") for path in paths))
        self.assertFalse(any("__pycache__" in path for path in paths))

    def test_write_chunks_jsonl(self) -> None:
        with tempfile.TemporaryDirectory(dir=Path.cwd()) as temp_dir:
            root = Path(temp_dir)
            (root / "doc.txt").write_text("alpha\nbeta\ngamma", encoding="utf-8")
            chunks = build_chunks(temp_dir, max_chars=20, overlap_lines=0)
            output_path = root / "chunks.jsonl"

            message = write_chunks_jsonl(chunks, str(output_path))
            lines = output_path.read_text(encoding="utf-8").splitlines()

        self.assertIn("Wrote", message)
        self.assertEqual(len(lines), len(chunks))
        first = json.loads(lines[0])
        self.assertIn("id", first)
        self.assertIn("path", first)
        self.assertIn("start_line", first)
        self.assertIn("end_line", first)
        self.assertIn("text", first)

    def test_embed_chunks_with_fake_client(self) -> None:
        with tempfile.TemporaryDirectory(dir=Path.cwd()) as temp_dir:
            root = Path(temp_dir)
            (root / "doc.txt").write_text("alpha\nbeta\ngamma", encoding="utf-8")
            chunks = build_chunks(temp_dir, max_chars=20, overlap_lines=0)

            records = embed_chunks(chunks, FakeEmbeddingClient(), batch_size=2)

        self.assertEqual(len(records), len(chunks))
        self.assertEqual(records[0].id, chunks[0].id)
        self.assertEqual(records[0].embedding, [float(len(chunks[0].text)), 1.0, 0.5])

    def test_write_vector_index(self) -> None:
        with tempfile.TemporaryDirectory(dir=Path.cwd()) as temp_dir:
            root = Path(temp_dir)
            (root / "doc.txt").write_text("alpha\nbeta\ngamma", encoding="utf-8")
            chunks = build_chunks(temp_dir, max_chars=20, overlap_lines=0)
            records = embed_chunks(chunks, FakeEmbeddingClient())
            output_path = root / "vector_index.jsonl"

            message = write_vector_index(records, str(output_path))
            lines = output_path.read_text(encoding="utf-8").splitlines()

        self.assertIn("embedded chunks", message)
        self.assertEqual(len(lines), len(records))
        first = json.loads(lines[0])
        self.assertIn("embedding", first)
        self.assertIsInstance(first["embedding"], list)

    def test_build_vector_index_with_fake_client(self) -> None:
        with tempfile.TemporaryDirectory(dir=Path.cwd()) as temp_dir:
            root = Path(temp_dir)
            (root / "doc.txt").write_text("alpha\nbeta\ngamma", encoding="utf-8")
            output_path = root / "vector_index.jsonl"

            message = build_vector_index(
                temp_dir,
                output_path=str(output_path),
                max_chars=20,
                overlap_lines=0,
                embedding_client=FakeEmbeddingClient(),
            )

        self.assertIn("Wrote", message)


class FakeEmbeddingClient:
    def embed(self, texts: list[str]) -> list[list[float]]:
        return [[float(len(text)), 1.0, 0.5] for text in texts]


if __name__ == "__main__":
    unittest.main()
