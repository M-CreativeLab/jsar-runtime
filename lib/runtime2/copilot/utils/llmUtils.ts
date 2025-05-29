import Anthropic from "@anthropic-ai/sdk"
import { buildApiHandler } from "../api"
import { ApiConfiguration, ApiProvider } from "../shared/api"
import { ApiStream } from "../api/transform/stream";
function ask(
  { input,
    systemPrompt,
    apiProvider,
    apiKey,
    apiModelId }:
    {
      input: string,
      systemPrompt: string,
      apiProvider: ApiProvider,
      apiKey: string,
      apiModelId: string
    }
): ApiStream {
  const config: ApiConfiguration = {
    apiProvider,
    apiModelId,
    qwenApiLine: 'china'
  }
  if (apiProvider === "doubao") {
    config.doubaoApiKey = apiKey
  } else if (apiProvider === 'qwen') {
    config.qwenApiKey = apiKey
  } else {
    console.error("Unsupported API provider:", apiProvider);
    return null;
  }

  const handler = buildApiHandler(config)
  const messages: Anthropic.Messages.MessageParam[] = [{ role: "user", content: input }];
  return handler.createMessage(systemPrompt, messages)
}

async function askAsync(
  { input,
    systemPrompt,
    apiProvider,
    apiKey,
    apiModelId }:
    {
      input: string,
      systemPrompt: string,
      apiProvider: ApiProvider,
      apiKey: string,
      apiModelId: string
    }
): Promise<string> {
  let html = ""
  const time = new Date().getTime();
  for await (const chunk of ask({ input, systemPrompt, apiProvider, apiKey, apiModelId })) {
    if (chunk.type === "text") {
      html += chunk.text
    }
  }
  html = html.replace(/```html\s*|\s*```/g, '');
  html = html.replace(/<!DOCTYPE html>/i, '');
  return html
}
export { ask, askAsync };
