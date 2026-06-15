const form = document.querySelector("#chatForm");
const taskInput = document.querySelector("#task");
const backendInput = document.querySelector("#backend");
const maxStepsInput = document.querySelector("#maxSteps");
const debugInput = document.querySelector("#debug");
const submitButton = document.querySelector("#submitButton");
const answerOutput = document.querySelector("#answerOutput");
const errorOutput = document.querySelector("#errorOutput");
const resultMeta = document.querySelector("#resultMeta");
const stepBadge = document.querySelector("#stepBadge");
const apiStatus = document.querySelector("#apiStatus");

async function checkHealth() {
  try {
    const response = await fetch("/health");
    if (!response.ok) {
      throw new Error(`HTTP ${response.status}`);
    }
    apiStatus.textContent = "API online";
    apiStatus.classList.add("is-online");
    apiStatus.classList.remove("is-offline");
  } catch (error) {
    apiStatus.textContent = "API offline";
    apiStatus.classList.add("is-offline");
    apiStatus.classList.remove("is-online");
  }
}

function setLoading(isLoading) {
  submitButton.disabled = isLoading;
  submitButton.textContent = isLoading ? "Running" : "Run Agent";
}

function showError(message) {
  errorOutput.hidden = false;
  errorOutput.textContent = message;
}

function clearError() {
  errorOutput.hidden = true;
  errorOutput.textContent = "";
}

function buildRequestBody() {
  return {
    task: taskInput.value.trim(),
    backend: backendInput.value,
    max_steps: Number(maxStepsInput.value),
    debug: debugInput.checked,
  };
}

form.addEventListener("submit", async (event) => {
  event.preventDefault();
  clearError();

  const body = buildRequestBody();
  if (!body.task) {
    showError("Task 不能为空。");
    taskInput.focus();
    return;
  }

  setLoading(true);
  const startedAt = performance.now();
  answerOutput.textContent = "Running agent...";
  resultMeta.textContent = `${body.backend} / max ${body.max_steps} steps`;
  stepBadge.textContent = "running";

  try {
    const response = await fetch("/chat", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(body),
    });

    const data = await response.json();
    if (!response.ok) {
      throw new Error(data.detail || `HTTP ${response.status}`);
    }

    const elapsed = ((performance.now() - startedAt) / 1000).toFixed(2);
    answerOutput.textContent = data.answer || "";
    resultMeta.textContent = `${body.backend} / ${elapsed}s`;
    stepBadge.textContent = `${data.steps} step${data.steps === 1 ? "" : "s"}`;
  } catch (error) {
    answerOutput.textContent = "请求失败。";
    resultMeta.textContent = "Error";
    stepBadge.textContent = "0 steps";
    showError(error.message);
  } finally {
    setLoading(false);
  }
});

document.querySelectorAll("[data-example]").forEach((button) => {
  button.addEventListener("click", () => {
    taskInput.value = button.dataset.example;
    taskInput.focus();
  });
});

checkHealth();
