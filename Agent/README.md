# Mini Learning Agent

这是一个给初学者练手的“小型学习型 agent”项目。它不是为了做成复杂产品，而是让你看清楚 agent 开发里最核心的几件事：

- 如何把用户任务、系统提示词和历史消息组织起来
- 如何让模型用结构化 JSON 决定下一步
- 如何注册工具，并让 agent 调用工具
- 如何把工具结果重新喂给模型，形成循环
- 如何用 mock 模型先本地跑通，再切换到真实 API

## 你会学到什么

一个最小 agent 通常包含这些模块：

1. `Prompt`：告诉模型它是谁、可以用什么工具、输出格式是什么。
2. `LLM Client`：负责调用大模型 API，或者在本项目中用 mock 模型模拟。
3. `Tool Registry`：把普通函数包装成 agent 可调用的工具。
4. `Agent Loop`：让模型“思考 -> 选工具 -> 观察结果 -> 继续思考 -> 给最终答案”。
5. `Tracing`：打开 `--debug` 后，你可以看到每一步发生了什么。

## 快速运行

本项目默认使用 mock 模型，不需要 API Key。

```powershell
python -m mini_agent.main --debug "帮我算一下 24 * (7 + 5)，然后把结果保存成笔记"
```

你应该能看到 agent 调用了 `calculator` 和 `save_note` 两个工具，最后给出答案。

如果 Windows 上的 `python` 命令不可用，可以把下面所有命令里的 `python` 换成 `py`。

再试一个：

```powershell
python -m mini_agent.main --debug "查找包含 288 的笔记"
```

新增天气工具后，也可以试：

```powershell
python -m mini_agent.main --debug "北京天气怎么样"
```

## 使用真实大模型 API

本项目使用“OpenAI-compatible chat completions”风格的接口。很多模型服务都兼容这种格式。

1. 复制 `.env.example` 为 `.env`
2. 填入你的 API Key、模型名和接口地址
3. 运行：

```powershell
python -m mini_agent.main --backend openai --debug "用三句话解释什么是 agent，然后保存成学习笔记"
```

注意：模型名会随服务商变化，请以你使用的平台文档为准。

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
```

## 推荐学习路线

第 1 步：先跑 mock 模型

观察 `--debug` 输出，理解 agent 的每一步。

第 2 步：读 `mini_agent/tools.py`

新增一个自己的工具，比如：

- `weather(city)`：先返回假天气
- `read_todo()`：读取本地任务列表
- `write_todo(item)`：写入一条任务

第 3 步：读 `mini_agent/agent.py`

重点看 `run()` 方法。它就是最小 agent 的心脏。

第 4 步：切到真实 API

真实模型可能输出不稳定，所以你会开始理解为什么 agent 开发需要：

- 更严格的输出格式
- 更好的错误恢复
- 最大步数限制
- 日志和调试信息
- 工具参数校验

第 5 步：做一个自己的小扩展

推荐方向：

- 学习笔记 agent：自动保存、检索、总结笔记
- 代码解释 agent：读取一个文件并解释核心逻辑
- 任务拆解 agent：把目标拆成待办事项
- 本地资料问答 agent：先搜索文件，再回答问题

## 这个项目故意没有做的事

为了保持适合初学者，这里暂时没有引入复杂框架、向量数据库、浏览器自动化、多 agent 协作或长期记忆系统。你先把这个最小循环吃透，再学那些框架会轻松很多。
