import type { Anthropic } from '@anthropic-ai/sdk';
import OpenAI from 'openai';

export function convertToOpenAiMessages(
  anthropicMessages: Anthropic.Messages.MessageParam[],
): OpenAI.Chat.ChatCompletionMessageParam[] {
  const openAiMessages: OpenAI.Chat.ChatCompletionMessageParam[] = [];

  for (const anthropicMessage of anthropicMessages) {
    if (typeof anthropicMessage.content === 'string') {
      openAiMessages.push({
        role: anthropicMessage.role,
        content: anthropicMessage.content,
      });
    } else {
      if (anthropicMessage.role === 'user') {
        const { nonToolMessages, toolMessages } = anthropicMessage.content.reduce<{
          nonToolMessages: (Anthropic.TextBlockParam | Anthropic.ImageBlockParam)[];
          toolMessages: Anthropic.ToolResultBlockParam[];
        }>(
          (acc, part) => {
            if (part.type === 'tool_result') {
              acc.toolMessages.push(part);
            } else if (part.type === 'text' || part.type === 'image') {
              acc.nonToolMessages.push(part);
            } // user cannot send tool_use messages
            return acc;
          },
          { nonToolMessages: [], toolMessages: [] },
        );

        // Process tool result messages FIRST since they must follow the tool use messages
        const toolResultImages: Anthropic.Messages.ImageBlockParam[] = [];
        toolMessages.forEach((toolMessage) => {
          // The Anthropic SDK allows tool results to be a string or an array of text and image blocks, enabling rich and structured content. In contrast, the OpenAI SDK only supports tool results as a single string, so we map the Anthropic tool result parts into one concatenated string to maintain compatibility.
          let content: string;

          if (typeof toolMessage.content === 'string') {
            content = toolMessage.content;
          } else {
            content =
              toolMessage.content
                ?.map((part) => {
                  if (part.type === 'image') {
                    toolResultImages.push(part);
                    return '(see following user message for image)';
                  }
                  return part.text;
                })
                .join('\n') ?? '';
          }
          openAiMessages.push({
            role: 'tool',
            tool_call_id: toolMessage.tool_use_id,
            content: content,
          });
        });

        // Process non-tool messages
        if (nonToolMessages.length > 0) {
          openAiMessages.push({
            role: 'user',
            content: nonToolMessages.map((part) => {
              if (part.type === 'image') {
                return {
                  type: 'image_url',
                  image_url: {
                    url: `data:${part.source.media_type};base64,${part.source.data}`,
                  },
                };
              }
              return { type: 'text', text: part.text };
            }),
          });
        }
      } else if (anthropicMessage.role === 'assistant') {
        const { nonToolMessages, toolMessages } = anthropicMessage.content.reduce<{
          nonToolMessages: (Anthropic.TextBlockParam | Anthropic.ImageBlockParam)[];
          toolMessages: Anthropic.ToolUseBlockParam[];
        }>(
          (acc, part) => {
            if (part.type === 'tool_use') {
              acc.toolMessages.push(part);
            } else if (part.type === 'text' || part.type === 'image') {
              acc.nonToolMessages.push(part);
            } // assistant cannot send tool_result messages
            return acc;
          },
          { nonToolMessages: [], toolMessages: [] },
        );

        // Process non-tool messages
        let content: string | undefined;
        if (nonToolMessages.length > 0) {
          content = nonToolMessages
            .map((part) => {
              if (part.type === 'image') {
                return ''; // impossible as the assistant cannot send images
              }
              return part.text;
            })
            .join('\n');
        }

        // Process tool use messages
        const tool_calls: OpenAI.Chat.ChatCompletionMessageToolCall[] = toolMessages.map((toolMessage) => ({
          id: toolMessage.id,
          type: 'function',
          function: {
            name: toolMessage.name,
            // json string
            arguments: JSON.stringify(toolMessage.input),
          },
        }));

        openAiMessages.push({
          role: 'assistant',
          content,
          // Cannot be an empty array. API expects an array with minimum length 1, and will respond with an error if it's empty
          tool_calls: tool_calls.length > 0 ? tool_calls : undefined,
        });
      }
    }
  }

  return openAiMessages;
}
