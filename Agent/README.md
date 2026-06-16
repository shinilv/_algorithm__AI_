# Mini Learning Agent

这是一个从“最小 agent loop”逐步扩展出来的本地资料问答 Agent 项目。它适合用来学习并展示：

- Agent loop：模型决策、工具调用、observation 回传、最终回答
- Tool calling：把普通 Python 函数注册成模型可调用工具
- OpenAI-compatible API：接入真实大模型和 embeddings 模型
- 本地文件问答：列文件、读文件、关键词搜索、语义搜索
- RAG：文档分块、embedding、向量索引、余弦相似度召回
- FastAPI + 前端页面：把命令行 Agent 做成可访问的 Web 应用

这个项目最初是教学型 demo，现在已经具备一个“本地资料库问答 Agent”的雏形。

## 当前能力

项目现在支持三种使用方式：

```text
命令行使用
  python -m mini_agent.main ...

Web API 使用
  POST /chat

网页使用
  http://127.0.0.1:8000/
```

核心能力包括：

- `calculator`：安全计算简单数学表达式
- `save_note` / `search_notes`：保存和搜索学习笔记
- `weather`：调用真实天气 API 查询天气
- `list_files`：列出本地可读文本文件
- `read_file`：读取指定本地文本文件
- `search_files`：关键词搜索本地文件
- `semantic_search`：基于 embedding 和余弦相似度做语义检索
- `trace`：记录每次 Agent 执行过程，方便复盘和调试

## 项目结构

```text
mini_agent/
  agent.py       Agent loop、JSON 解析、trace 记录
  api.py         FastAPI 后端接口和网页静态资源挂载
  llm.py         MockLLM、OpenAI-compatible chat completions 客户端
  main.py        命令行入口
  prompts.py     系统提示词，约束模型输出 JSON action/final
  rag.py         RAG：分块、embedding、向量索引、语义检索
  tools.py       工具注册表和所有可调用工具

web/
  index.html     前端页面
  style.css      页面样式
  app.js         调用 /chat 的前端逻辑

tests/
  test_agent.py
  test_tools.py
  test_rag.py

data/
  notes.jsonl          学习笔记
  trace.jsonl          Agent 执行轨迹
  vector_index.jsonl   RAG 向量索引
```

## 安装依赖

建议使用 Python 3.10+。

```powershell
python -m pip install -r requirements.txt
```

如果 Windows 上 `python` 不可用，可以试：

```powershell
py -m pip install -r requirements.txt
```

当前依赖：

```text
fastapi
uvicorn
```

其他核心功能主要使用 Python 标准库。

## 环境变量

复制 `.env.example` 为 `.env`，然后配置真实模型 API。

示例：

```env
OPENAI_API_KEY=你的API_KEY
OPENAI_MODEL=qwen-plus
OPENAI_BASE_URL=https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions

EMBEDDING_MODEL=text-embedding-v1
EMBEDDING_BASE_URL=https://dashscope.aliyuncs.com/compatible-mode/v1/embeddings
```

说明：

- `OPENAI_*` 用于聊天模型，也就是让模型决定下一步 action。
- `EMBEDDING_*` 用于 RAG 语义检索，把文本转成向量。
- 如果没有配置 `EMBEDDING_API_KEY`，项目会复用 `OPENAI_API_KEY`。

不要把真实 `.env` 提交到 GitHub。

## 快速运行

### 1. 命令行 mock 模式

mock 模式不需要 API Key，适合先理解 agent loop。

```powershell
python -m mini_agent.main --debug "帮我算一下 24 * (7 + 5)，然后把结果保存成笔记"
```

你会看到类似流程：

```text
[step 1] action: calculator
[tool] calculator -> 288

[step 2] action: save_note
[tool] save_note -> Saved note: Agent learning note

[step 3] final: 完成：Saved note: Agent learning note
```

### 2. 命令行真实模型模式

```powershell
python -m mini_agent.main --backend openai --debug "搜索项目里哪里定义了 ToolRegistry"
```

真实模式下，模型会根据 system prompt 里列出的工具说明，返回类似：

```json
{
  "thought": "I should search the local files.",
  "action": "search_files",
  "action_input": {
    "query": "ToolRegistry",
    "directory": "."
  }
}
```

Python 程序再根据这个 JSON 真正执行工具。

### 3. 启动 FastAPI 和网页

```powershell
python -m uvicorn mini_agent.api:app --reload --host 127.0.0.1 --port 8000
```

浏览器打开：

```text
http://127.0.0.1:8000/
```

接口文档：

```text
http://127.0.0.1:8000/docs
```

健康检查：

```text
http://127.0.0.1:8000/health
```

如果端口冲突，可以换端口：

```powershell
python -m uvicorn mini_agent.api:app --reload --host 127.0.0.1 --port 8010
```

如果要让同一局域网其他设备访问：

```powershell
python -m uvicorn mini_agent.api:app --reload --host 0.0.0.0 --port 8010
```

然后用本机 WLAN IPv4 访问，例如：

```text
http://你的局域网IP:8010/
```

注意：当前项目没有登录鉴权，不建议暴露到公网。

## Agent Loop 是怎么工作的

核心循环在 `mini_agent/agent.py` 的 `Agent.run()`。

```text
用户任务
  ↓
组装 messages：system prompt + user task + 历史步骤
  ↓
调用 LLM complete(messages, tools)
  ↓
解析模型返回的 JSON
  ↓
如果有 action：调用工具
  ↓
把工具结果 observation 加回 messages
  ↓
继续下一轮
  ↓
如果有 final：返回最终答案
```

`run()` 外部只调用一次，但内部通过：

```python
for step in range(1, self.max_steps + 1):
```

完成多轮工具调用。`max_steps` 是安全边界，用来避免模型无限循环。

## 工具调用是怎么实现的

工具注册在 `mini_agent/tools.py`。

每个工具由四部分组成：

```python
Tool(
    name="search_files",
    description="Search readable text files in a workspace directory by keyword.",
    parameters={
        "query": "Keyword to search for",
        "directory": "Workspace directory to search, for example: .",
    },
    func=search_files,
)
```

真实 API 并不会读取 Python 函数源码。项目会把工具说明转成文本，拼进 system prompt：

```text
Available tools:
- search_files(query: ..., directory: ...) -> Search readable text files...
- semantic_search(query: ..., top_k: ...) -> Search the local vector index...
```

模型根据这些工具说明输出 JSON。程序再解析 JSON 并执行对应 Python 函数。

## 本地文件问答

当前有两类本地检索：

```text
关键词检索：search_files
语义检索：semantic_search
```

关键词检索适合明确知道关键词时使用：

```powershell
python -m mini_agent.main --backend openai --debug "搜索项目里哪里定义了 ToolRegistry"
```

语义检索适合自然语言问题：

```powershell
python -m mini_agent.main --backend openai --debug "工具注册表是怎么工作的？"
```

只要模型选择了 `semantic_search`，Agent 就会从 `data/vector_index.jsonl` 里召回最相关的 chunk。

## RAG 使用流程

RAG 的核心文件是 `mini_agent/rag.py`。

当前完整链路：

```text
本地资料
  ↓
build_chunks：按行切成 chunks
  ↓
embedding API：每个 chunk 转成向量
  ↓
data/vector_index.jsonl：保存向量索引
  ↓
semantic_search：用户问题转向量，计算余弦相似度
  ↓
返回 top_k 相关片段
  ↓
模型基于片段生成回答
```

### 1. 构建向量索引

索引 `mini_agent` 目录：

```powershell
python -m mini_agent.rag build mini_agent --output data/vector_index.jsonl
```

旧命令也兼容：

```powershell
python -m mini_agent.rag mini_agent --output data/vector_index.jsonl
```

常用参数：

```powershell
python -m mini_agent.rag build mini_agent `
  --output data/vector_index.jsonl `
  --max-chars 1200 `
  --overlap-lines 2 `
  --batch-size 8
```

参数含义：

- `--max-chars`：每个 chunk 最大字符数
- `--overlap-lines`：相邻 chunk 重叠行数
- `--batch-size`：每次 embeddings API 请求包含多少个 chunk

### 2. 执行语义检索

```powershell
python -m mini_agent.rag search "ToolRegistry 是怎么工作的" --top-k 3
```

返回示例：

```text
[1] mini_agent/tools.py:50-80 score=0.5232
if tool.name in self._tools:
    ...

---

[2] mini_agent/tools.py:79-115 score=0.5550
registry.register(...)
```

结果里包含：

- 来源文件
- 起止行号
- 相似度分数
- chunk 原文

### 3. 新增资料后怎么办

如果你新增、删除、修改了本地资料，需要重新构建索引。

例如你新增了：

```text
docs/agent_notes.md
docs/rag_intro.md
```

就重新执行：

```powershell
python -m mini_agent.rag build docs --output data/vector_index.jsonl
```

因为 `data/vector_index.jsonl` 是某一时刻资料的“向量快照”，不会自动感知新文件。

后续可以升级成增量索引：记录文件 hash 或修改时间，只重建变化过的文件。当前阶段先全量重建，最简单稳定。

## 文件安全边界

项目不会索引或读取这些目录：

```text
.git
.venv
__pycache__
node_modules
venv
```

不会读取 `.env`。

RAG 构建时也会跳过运行数据：

```text
notes.jsonl
trace.jsonl
traces.jsonl
chunks.jsonl
vector_index.jsonl
```

这是为了避免：

- API Key 泄露
- 检索到运行日志
- 新索引把旧索引也索引进去

## API 使用

`POST /chat`

请求体：

```json
{
  "task": "搜索项目里哪里定义了 ToolRegistry",
  "backend": "openai",
  "max_steps": 8,
  "debug": false
}
```

返回：

```json
{
  "answer": "...",
  "steps": 3
}
```

PowerShell 示例：

```powershell
Invoke-RestMethod `
  -Method Post `
  -Uri "http://127.0.0.1:8000/chat" `
  -ContentType "application/json; charset=utf-8" `
  -Body '{"task":"搜索项目里哪里定义了 ToolRegistry","backend":"openai","max_steps":8}'
```

## 测试

运行所有测试：

```powershell
python -m unittest discover -s tests
```

当前测试覆盖：

- JSON 解析
- mock LLM 的基础意图判断
- calculator 安全限制
- 天气工具的离线 mock 测试
- 文件读取、搜索、安全边界
- RAG 分块
- vector index 写入
- 余弦相似度
- semantic_search top_k 排序

测试不会请求真实 embeddings API。真实 API 只在你手动构建索引或执行真实语义检索时调用。

## 常见问题

### 为什么模型能调用工具？

模型不会直接调用 Python 函数。它只输出 JSON：

```json
{
  "action": "semantic_search",
  "action_input": {
    "query": "工具注册表怎么实现的",
    "top_k": 5
  }
}
```

Python 程序解析这个 JSON，再通过 `ToolRegistry.call()` 执行真正的函数。

### RAG 能完全避免幻觉吗？

不能。

RAG 的作用是先从外部资料中找证据，再让模型基于证据回答，从而降低幻觉概率。但如果检索结果不相关、chunk 切得不好、top_k 太少或太多，模型仍然可能答偏。

更准确地说：

```text
RAG = 检索相关资料 + 把资料作为上下文 + 要求模型基于上下文回答
```

### 关键词搜索和语义搜索有什么区别？

关键词搜索：

```text
必须命中 ToolRegistry 这个词
```

语义搜索：

```text
用户问“工具注册表怎么实现”，也可能找到 ToolRegistry 相关代码
```

### 为什么要有 trace？

Agent 的行为不是单次函数调用，而是一系列模型决策和工具调用。

trace 可以记录：

- 用户任务
- 每一步模型输出
- 每次工具调用参数
- 工具返回结果
- 最终答案

这对调试和简历项目展示都很有价值。

## 简历描述参考

项目经历可以写：

```text
本地资料问答 Agent | Python, FastAPI, LLM, Tool Calling, RAG

- 基于 Python 实现轻量级 Agent 执行循环，支持 system prompt、JSON action 解析、工具注册、工具调用和 observation 回传。
- 接入 OpenAI-compatible 大模型 API，支持 mock/openai 后端切换、最大执行步数控制和执行轨迹记录。
- 实现本地文件工具，包括文件列表、文件读取、关键词搜索，并加入路径安全边界，避免访问敏感文件。
- 实现最小 RAG 流程，支持文档分块、embedding 向量化、JSONL 向量索引、余弦相似度召回和来源片段返回。
- 使用 FastAPI 封装 /chat 接口，并实现原生 HTML/CSS/JS 前端页面，支持网页输入问题并展示 Agent 回答。
```

技能栏可以写：

```text
AI Agent / LLM 应用开发：Prompt Engineering、Tool Calling、Agent Loop、RAG、Embedding、Vector Search、FastAPI
```

## 下一步方向

推荐继续做这些增强：

1. 让最终回答强制附带引用来源。
2. 在前端展示每次 Agent trace。
3. 增加增量索引，只重建变化过的文件。
4. 支持 PDF / Word / Markdown 资料解析。
5. 给 `/chat` 增加简单鉴权，避免局域网误用。
6. 把 semantic search 和关键词搜索组合成混合检索。
