from __future__ import annotations

from pathlib import Path
from typing import Literal

from fastapi import FastAPI, HTTPException
from fastapi.responses import FileResponse
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel, Field

from .agent import Agent
from .llm import MockLLM, OpenAICompatibleLLM
from .tools import build_default_registry


app = FastAPI(title="Mini Agent API", version="0.1.0")
WEB_DIR = Path(__file__).resolve().parent.parent / "web"
app.mount("/static", StaticFiles(directory=WEB_DIR), name="static")


class ChatRequest(BaseModel):
    task: str = Field(..., min_length=1)
    backend: Literal["mock", "openai"] = "mock"
    max_steps: int = Field(default=8, ge=1, le=20)
    debug: bool = False


class ChatResponse(BaseModel):
    answer: str
    steps: int


@app.get("/")
def index() -> FileResponse:
    return FileResponse(WEB_DIR / "index.html")


@app.get("/health")
def health() -> dict[str, str]:
    return {"status": "ok"}


@app.post("/chat", response_model=ChatResponse)
def chat(request: ChatRequest) -> ChatResponse:
    try:
        llm = MockLLM() if request.backend == "mock" else OpenAICompatibleLLM.from_env()
        agent = Agent(
            llm=llm,
            tools=build_default_registry(),
            max_steps=request.max_steps,
            debug=request.debug,
        )
        result = agent.run(request.task)
        return ChatResponse(answer=result.answer, steps=result.steps)
    except Exception as exc:
        raise HTTPException(status_code=500, detail=str(exc)) from exc
