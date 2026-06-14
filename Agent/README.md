# Mini Learning Agent

这是一个给初学者练手的“小型学习型 agent”项目。它不是为了做成复杂产品，而是让你看清楚 agent 开发里最核心的几件事：

- 如何把用户任务、系统提示词和历史消息组织起来
- 如何让模型用结构化 JSON 决定下一步
- 如何注册工具，并让 agent 调用工具
- 如何把工具结果重新喂给模型，形成循环
- 如何用 mock 模型先本地跑通，再切换到真实 API

如果你现在直接干看代码有点吃力，正常。agent 项目最容易卡住的地方不是语法，而是你不知道“这一行代码在整个循环里负责哪一步”。这份 README 建议你先按运行结果理解流程，再回到代码里逐段对上。

## 先建立一个直觉

这个项目里的 agent 可以先理解成一个循环：

```text
用户提出任务
  ↓
组装 messages：系统提示词 + 用户任务 + 历史步骤
  ↓
LLM 返回一个 JSON
  ↓
如果 JSON 里有 action，就调用对应工具
  ↓
把工具结果 observation 放回 messages
  ↓
继续问 LLM 下一步
  ↓
如果 JSON 里有 final，就结束
```

也就是说，这个项目并不是让模型“随便聊天”。它要求模型每次只能做两类事：

1. 给出一个 `action`，让程序去调用工具。
2. 给出一个 `final`，表示任务完成。

这是很多 agent 框架背后的基本思想。框架会更复杂，但核心循环很像。

## 快速运行

本项目默认使用 mock 模型，不需要 API Key。

先确认你有 Python：

```powershell
python --version
```

如果 Windows 上的 `python` 命令不可用，可以试：

```powershell
py --version
```

然后运行第一个例子：

```powershell
python -m mini_agent.main --debug "帮我算一下 24 * (7 + 5)，然后把结果保存成笔记"
```

你应该能看到类似这样的流程：

```text
[step 1] action: calculator
[tool] calculator -> 288

[step 2] action: save_note
[tool] save_note -> Saved note: Agent learning note

[step 3] final: 完成：Saved note: Agent learning note
```

这里最重要的不是结果 `288`，而是理解这三步：

- 第 1 步：模型决定先调用 `calculator`。
- 第 2 步：程序把计算结果交回模型，模型决定再调用 `save_note`。
- 第 3 步：程序把保存结果交回模型，模型认为任务完成，于是返回 `final`。

再试两个例子：

```powershell
python -m mini_agent.main --debug "查找包含 288 的笔记"
```

```powershell
python -m mini_agent.main --debug "北京天气怎么样"
```

## 先不要急着看所有代码

刚开始建议你先忽略这些内容：

- 先别深挖 `calculator()` 里面的 `ast` 解析。
- 先别急着接真实 API。
- 先别纠结 mock 模型为什么能“识别意图”，它只是用规则模拟模型行为。
- 先别改复杂功能，先把一次 agent 循环看明白。

你第一轮只需要抓住四个问题：

1. 用户任务从哪里进来？
2. 模型输出在哪里被解析？
3. 工具在哪里被注册、查找、调用？
4. 工具结果如何重新进入下一轮对话？

## 推荐阅读顺序

### 第 1 步：看 `mini_agent/main.py`

这是命令行入口。你只需要看懂一条主线：

```text
解析命令行参数
  ↓
选择 MockLLM 或 OpenAICompatibleLLM
  ↓
创建默认工具注册表
  ↓
创建 Agent
  ↓
调用 agent.run(task)
```

对应代码大概是：

```python
llm = MockLLM() if args.backend == "mock" else OpenAICompatibleLLM.from_env()
agent = Agent(llm=llm, tools=build_default_registry(), max_steps=args.max_steps, debug=args.debug)
result = agent.run(args.task)
```

你可以把 `main.py` 理解成“把零件装起来，然后启动机器”。

### 第 2 步：看 `mini_agent/tools.py`

这里负责两件事：

1. 定义“工具长什么样”。
2. 注册项目默认提供的工具。

核心概念是 `Tool`：

```python
@dataclass(frozen=True)
class Tool:
    name: str
    description: str
    parameters: dict[str, str]
    func: ToolFunction
```

每个工具有四部分：

- `name`：模型要调用的工具名，比如 `calculator`。
- `description`：告诉模型这个工具能做什么。
- `parameters`：告诉模型参数应该怎么填。
- `func`：真正被 Python 调用的函数。

比如 `calculator` 被注册成这样：

```python
Tool(
    name="calculator",
    description="Evaluate a safe arithmetic expression.",
    parameters={"expression": "Arithmetic expression, for example: 24 * (7 + 5)"},
    func=calculator,
)
```

这段代码的意思是：如果模型输出：

```json
{
  "action": "calculator",
  "action_input": {
    "expression": "24 * (7 + 5)"
  }
}
```

程序最终会执行：

```python
calculator(expression="24 * (7 + 5)")
```

这是 agent “能做事”的关键。

### 第 3 步：看 `mini_agent/prompts.py`

这里是系统提示词，负责约束模型输出格式。

它告诉模型：

- 你是一个 educational agent。
- 你可以一步步思考并使用工具。
- 你必须只输出一个 JSON object。
- 如果要调用工具，就输出 `thought`、`action`、`action_input`。
- 如果完成了，就输出 `thought`、`final`。

这个文件很短，但非常重要。因为 agent 不是只靠 Python 代码工作的，也靠 prompt 把模型限制在一个可执行协议里。

### 第 4 步：看 `mini_agent/agent.py`

这是整个项目的心脏。重点只看 `run()` 方法。

你可以把它拆成 6 个动作：

```python
messages = [
    {"role": "system", "content": self._system_prompt()},
    {"role": "user", "content": task},
]
```

先创建对话消息。这里包含系统提示词和用户任务。

```python
raw = self.llm.complete(messages, self.tools)
event = parse_json_object(raw)
```

问模型下一步做什么，并把模型输出解析成 Python 字典。

```python
if "final" in event:
    return AgentResult(answer=str(event["final"]), steps=step)
```

如果模型说完成了，就结束。

```python
action = str(event.get("action", ""))
action_input = event.get("action_input", {})
```

如果还没完成，就取出工具名和工具参数。

```python
observation = self.tools.call(action, action_input)
```

真正调用工具。

```python
messages.append({"role": "assistant", "content": json.dumps(event, ensure_ascii=False)})
messages.append({"role": "tool", "content": observation})
```

把模型刚才的决定和工具结果都放回历史消息里。下一轮模型就能“看到”刚才发生了什么。

这就是 agent loop。

### 第 5 步：看 `mini_agent/llm.py`

这里有两个 LLM：

- `MockLLM`：不用 API，靠规则假装模型会做决定，适合学习流程。
- `OpenAICompatibleLLM`：调用真实的 OpenAI-compatible chat completions API。

刚开始你只需要看 `MockLLM.complete()`。

它并不是真正理解中文，而是用一些规则判断：

- 如果用户说“查找”或“搜索”，就返回 `search_notes`。
- 如果用户问天气，就返回 `weather`。
- 如果用户输入数学表达式，就返回 `calculator`。
- 如果上一步工具执行完了，并且用户要求保存，就返回 `save_note`。
- 否则返回 `final`。

它的作用不是展示模型能力，而是让你在没有 API key 的情况下，把 agent 循环跑通。

## 一次任务的完整链路

以这个命令为例：

```powershell
python -m mini_agent.main --debug "帮我算一下 24 * (7 + 5)，然后把结果保存成笔记"
```

你可以这样对照代码：

```text
1. main.py 收到命令行参数
   task = "帮我算一下 24 * (7 + 5)，然后把结果保存成笔记"

2. main.py 创建 MockLLM、ToolRegistry、Agent

3. Agent.run() 创建 messages
   system: 来自 prompts.py
   user: 用户任务

4. MockLLM.complete() 看到数学表达式
   返回 {"action": "calculator", ...}

5. Agent.run() 调用 tools.call("calculator", {"expression": "24 * (7 + 5)"})

6. ToolRegistry.call() 找到 calculator 函数并执行
   得到 observation = "288"

7. Agent.run() 把 observation 放回 messages

8. MockLLM.complete() 看到用户要求保存，并且还没保存过
   返回 {"action": "save_note", ...}

9. Agent.run() 调用 save_note(...)
   写入 data/notes.jsonl

10. Agent.run() 把保存结果放回 messages

11. MockLLM.complete() 返回 {"final": "..."}

12. Agent.run() 返回 AgentResult
```

如果你能把这 12 步讲清楚，这个项目最核心的部分你就已经懂了。

## 你应该重点掌握什么

### 1. `messages` 是 agent 的记忆

在这个项目里，agent 没有数据库级别的长期记忆。它的短期记忆就是 `messages`。

每一轮都会把这些内容传给 LLM：

- 系统提示词
- 用户任务
- 之前模型做过的 action
- 之前工具返回的 observation

真实 agent 的上下文管理、记忆压缩、多轮对话，本质上都是围绕 `messages` 或类似结构展开的。

### 2. `ToolRegistry` 是模型和代码之间的桥

模型不能直接执行 Python 函数。它只能输出文字。

所以中间需要一个协议：

```json
{
  "action": "calculator",
  "action_input": {
    "expression": "1 + 2"
  }
}
```

程序解析这个 JSON，然后用 `ToolRegistry` 找到真正的 Python 函数。

这就是 tool calling 的简化版。

### 3. `max_steps` 是安全边界

agent loop 可能一直循环，比如模型反复调用同一个工具，不返回 `final`。

所以 `Agent` 里有：

```python
max_steps: int = 8
```

真实项目里还会有更多边界：

- 超时限制
- token 限制
- 工具调用权限
- 文件访问范围
- API 成本限制
- 错误重试次数

这个项目只保留了最容易理解的一个：最大步数。

### 4. `parse_json_object()` 是输出格式兜底

模型理想情况下应该只输出 JSON。

但真实模型有时会输出：

```text
Sure, here is the JSON:
{"action": "calculator", "action_input": {"expression": "1+2"}}
```

所以 `parse_json_object()` 做了一个简单兜底：如果整段文本不是 JSON，就尝试截取第一个 `{` 到最后一个 `}`。

这不是完美方案，但适合学习。真实项目里一般会用更严格的 structured output、function calling 或 schema 校验。

## 第一次动手：加一个最简单的工具

建议你先加一个 `echo` 工具。它没有业务难度，专门用来练习工具注册流程。

目标效果：

```powershell
python -m mini_agent.main --debug "echo hello agent"
```

你要做三件事：

1. 在 `mini_agent/tools.py` 里写函数：

```python
def echo(text: str) -> str:
    return text
```

2. 在 `build_default_registry()` 里注册：

```python
registry.register(
    Tool(
        name="echo",
        description="Return the input text unchanged.",
        parameters={"text": "Text to echo back"},
        func=echo,
    )
)
```

3. 在 `mini_agent/llm.py` 的 `MockLLM.complete()` 里加一个简单规则，让它在用户输入包含 `echo` 时返回：

```json
{
  "thought": "I should echo the text.",
  "action": "echo",
  "action_input": {
    "text": "hello agent"
  }
}
```

做完这个，你就会真正理解：

- 工具函数怎么写
- 工具怎么注册
- 模型怎么选择工具
- agent 怎么执行工具

## 第二次动手：加一个 todo 工具

等你熟悉 `echo` 后，再做一个稍微真实一点的扩展：

- `write_todo(item)`：写入一条待办事项到 `data/todos.jsonl`
- `read_todos()`：读取最近几条待办事项

这会练到三个真实项目常见能力：

- 文件读写
- JSONL 数据格式
- 一个 agent 使用多个相关工具

你可以参考 `save_note()` 和 `search_notes()`，它们就是很好的模板。

## 第三次动手：做一个本地资料问答雏形

如果你想往就业作品集靠，可以继续加：

- `list_files(directory)`：列出某个目录下的文件
- `read_text_file(path)`：读取指定文本文件
- `search_text_files(query)`：在文本文件里搜索关键词

这会变成一个很小的“本地资料问答 agent”。它还不是完整 RAG，但已经开始接近真实业务。

后续你可以再把它升级成：

- 支持 PDF / Markdown / Python 文件
- 支持向量检索
- 支持 FastAPI 接口
- 支持前端页面
- 支持对每次回答展示引用来源

这条线很适合作为就业项目继续扩展。

## 使用真实大模型 API

本项目使用 OpenAI-compatible chat completions 风格的接口。很多模型服务都兼容这种格式。

1. 复制 `.env.example` 为 `.env`
2. 填入你的 API Key、模型名和接口地址
3. 运行：

```powershell
python -m mini_agent.main --backend openai --debug "用三句话解释什么是 agent，然后保存成学习笔记"
```

注意：模型名、接口地址、鉴权方式会随服务商变化，请以你使用的平台文档为准。

刚开始不建议你马上接真实 API。更好的顺序是：

1. 先用 mock 跑通。
2. 自己加一个工具。
3. 能解释完整 agent loop。
4. 再切真实 API。

这样你遇到真实模型输出不稳定、JSON 解析失败、工具参数乱填时，才知道问题发生在哪一层。

## 项目结构

```text
mini_agent/
  agent.py       agent 循环和 JSON 解析
  llm.py         mock 模型和 OpenAI-compatible API 客户端
  main.py        命令行入口
  prompts.py     系统提示词
  tools.py       工具注册和示例工具
tests/
  test_agent.py
  test_tools.py
data/
  notes.jsonl    save_note 写入的学习笔记
```

建议你按这个顺序读：

```text
main.py -> tools.py -> prompts.py -> agent.py -> llm.py -> tests/
```

## 测试

运行所有测试：

```powershell
python -m unittest discover -s tests
```

测试现在主要覆盖：

- JSON 解析
- mock 模型的简单意图判断
- calculator 的安全限制
- weather 工具的基本输出

如果你新增工具，也建议加测试。比如新增 `echo` 后，可以在 `tests/test_tools.py` 里写：

```python
from mini_agent.tools import echo

def test_echo(self) -> None:
    self.assertEqual(echo("hello"), "hello")
```

写测试不是为了形式主义，而是为了确认你改工具时没有把已有功能弄坏。

## 一周学习安排

### 第 1 天：只跑，不改

目标：能看懂 `--debug` 输出。

任务：

- 跑计算保存例子。
- 跑搜索笔记例子。
- 跑天气例子。
- 把每一步 action 和工具函数对上。

### 第 2 天：读主线代码

目标：能讲清楚 `main.py -> Agent.run() -> tools.call()`。

任务：

- 读 `main.py`。
- 读 `tools.py` 的 `Tool` 和 `ToolRegistry`。
- 读 `agent.py` 的 `run()`。
- 暂时跳过 `calculator()` 内部细节。

### 第 3 天：加 echo 工具

目标：完成第一次工具扩展。

任务：

- 写 `echo()`。
- 注册 `echo`。
- 修改 `MockLLM` 让它能选择 `echo`。
- 加一个测试。

### 第 4 天：加 todo 工具

目标：做一个有状态的小功能。

任务：

- 写入 `data/todos.jsonl`。
- 读取最近几条 todo。
- 用 `--debug` 观察多步工具调用。

### 第 5 天：接真实 API

目标：理解真实模型和 mock 模型的差异。

任务：

- 配置 `.env`。
- 使用 `--backend openai`。
- 观察模型是否严格输出 JSON。
- 记录失败案例。

### 第 6-7 天：做一个小作品方向

目标：从“学习代码”过渡到“能讲的项目”。

推荐方向：

- 学习笔记 agent：保存、搜索、总结笔记。
- 代码解释 agent：读取一个文件并解释核心逻辑。
- 本地资料问答 agent：搜索文件，再基于文件内容回答。
- 任务拆解 agent：把一个目标拆成 todo，并能保存进本地文件。

## 常见卡点

### 为什么模型要输出 JSON？

因为程序需要稳定解析模型的决定。

如果模型只说“我觉得应该调用 calculator”，程序很难知道该调用哪个函数、传什么参数。

JSON 相当于模型和程序之间的接口协议。

### 为什么要有工具注册表？

因为不能让模型随便执行任意代码。

工具注册表只暴露一小组被允许的函数。模型只能在这些工具里选。这是安全边界，也是工程边界。

### 为什么工具结果还要放回 messages？

因为模型本身不知道工具执行结果。

程序调用 `calculator` 得到 `288` 后，必须把 `288` 放回上下文，模型下一轮才能根据这个结果继续决定是否保存、是否总结、是否最终回答。

### 这个项目和 LangChain、AutoGen 有什么关系？

这个项目是在手写最小 agent 循环。

LangChain、AutoGen 这类框架会帮你封装：

- prompt 模板
- tool calling
- memory
- retriever
- agent executor
- callback / tracing
- 多 agent 协作

但如果你没理解这个项目里的最小循环，直接上框架会很容易只会调 API，不知道错在哪里。

## 这个项目故意没有做的事

为了保持适合初学者，这里暂时没有引入：

- 复杂 agent 框架
- 向量数据库
- 浏览器自动化
- 多 agent 协作
- 长期记忆系统
- 权限管理
- 成本控制
- 完整 Web 服务

你先把这个最小循环吃透，再学那些框架会轻松很多。

## 你学完后应该能回答的问题

如果你能不看代码回答下面这些问题，就说明这个项目已经基本吃透：

1. 用户输入是怎么进入 `Agent.run()` 的？
2. 系统提示词在哪里拼进 messages？
3. 模型为什么要输出 JSON？
4. `action` 和 `action_input` 分别是什么？
5. `ToolRegistry.call()` 是怎么找到真实函数的？
6. 工具结果为什么叫 observation？
7. 为什么需要 `max_steps`？
8. mock LLM 和真实 LLM 的区别是什么？
9. 如果模型输出不是合法 JSON，会发生什么？
10. 如果要新增一个工具，需要改哪几个地方？

把这些讲清楚，你再去看 Google 的 5-Day Gen AI / Kaggle 课程、LangChain 文档或者更复杂的 agent 项目，会顺很多。
