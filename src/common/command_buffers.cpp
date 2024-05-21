#include "command_buffers.hpp"

using namespace std;

constexpr int16_t TR_COMMAND_BUFFER_MAGIC = 0x72ff;

bool TrCommandBufferMessage::serialize(void **outData, size_t *outSize)
{
  usage = USAGE_SERIALIZE;

  size_t segmentsLength = 0;
  for (auto &segment : segments)
    segmentsLength += (sizeof(segment.size) + segment.size); // size + data
  size_t contentSize =
      sizeof(CommandBufferType) + // type
      sizeof(uint32_t) +          // id
      sizeof(size_t) +            // segments length
      sizeof(size_t) +            // segments count
      segmentsLength +            // segments
      sizeof(size_t) +            // base size
      baseSize;                   // base

  int16_t magic = TR_COMMAND_BUFFER_MAGIC;
  size_t bufferSize =
      sizeof(magic) +       // magic
      sizeof(contentSize) + // content size
      contentSize;          // content

  char *buffer = (char *)malloc(bufferSize);
  if (buffer == nullptr)
    return false; // out of memory

  size_t offset = 0;
  // Write header
  offset = writeTo(buffer, offset, &magic);
  offset = writeTo(buffer, offset, &contentSize, sizeof(contentSize));

  // Write metadata
  offset = writeTo(buffer, offset, &type, sizeof(type));
  offset = writeTo(buffer, offset, &id, sizeof(id));

  // Write segments
  size_t segmentsCount = segments.size();
  offset = writeTo(buffer, offset, &segmentsLength, sizeof(segmentsLength));
  offset = writeTo(buffer, offset, &segmentsCount, sizeof(size_t));
  for (auto &segment : segments)
  {
    offset = writeTo(buffer, offset, &segment.size, sizeof(segment.size));
    offset = writeTo(buffer, offset, segment.data, segment.size);
  }

  // Write base
  offset = writeTo(buffer, offset, &baseSize, sizeof(baseSize));
  offset = writeTo(buffer, offset, base, baseSize);

  DEBUG(LOG_TAG_CONTENT, "Assertion: offset(%d) = bufferSize(%d)",
        offset, bufferSize);
  assert(offset == bufferSize);

  *outData = buffer;
  *outSize = bufferSize;
  return true;
}

bool TrCommandBufferMessage::deserialize(TrCommandBufferReceiver *receiver, int recvTimeout)
{
  usage = USAGE_DESERIALIZE; // mark as deserialized

  int16_t magic;
  if (!receiver->tryRecvRaw(&magic, sizeof(magic), recvTimeout))
    return false;
  if (magic != TR_COMMAND_BUFFER_MAGIC)
    return false;

  size_t contentSize;
  if (!receiver->tryRecvRaw(&contentSize, sizeof(contentSize), recvTimeout))
    return false;

  auto buffer = (char *)malloc(contentSize);
  if (!receiver->tryRecvRaw(buffer, contentSize, recvTimeout))
    return false;

  int res = deserializeContent(buffer, contentSize);
  free(buffer);
  return res;
}

bool TrCommandBufferMessage::deserialize(char *buffer, size_t size)
{
  size_t offset = 0;
  int16_t magic;
  size_t contentSize;

  offset = readFrom(buffer, offset, &magic);
  offset = readFrom(buffer, offset, &contentSize);

  if (magic != TR_COMMAND_BUFFER_MAGIC)
    return false;
  return deserializeContent(buffer + offset, contentSize);
}

bool TrCommandBufferMessage::deserializeContent(char *contentBuffer, size_t contentSize)
{
  size_t offset = 0;
  offset = readFrom(contentBuffer, offset, &type);
  offset = readFrom(contentBuffer, offset, &id);

  size_t segmentsLength, segmentsCount;
  offset = readFrom(contentBuffer, offset, &segmentsLength);
  offset = readFrom(contentBuffer, offset, &segmentsCount);

  for (size_t i = 0; i < segmentsCount; i++)
  {
    size_t cSize;
    offset = readFrom(contentBuffer, offset, &cSize);
    void *cData = malloc(cSize);
    offset = readFrom(contentBuffer, offset, cData, cSize);
    segments.push_back(TrCommandBufferSegment(cSize, cData));
  }
  offset = readFrom(contentBuffer, offset, &baseSize);
  base = malloc(baseSize);
  offset = readFrom(contentBuffer, offset, base, baseSize);

  assert(offset == contentSize);
  return true;
}

bool TrCommandBufferSender::sendCommandBufferRequest(TrCommandBufferBase &req)
{
  TrCommandBufferMessage *message = nullptr;
  switch (req.type)
  {
  case COMMAND_BUFFER_WEBGL_CONTEXT_INIT_REQ:
  {
    WebGL1ContextInitCommandBufferRequest *data = dynamic_cast<WebGL1ContextInitCommandBufferRequest *>(&req);
    message = data->serialize();
    break;
  }
  default:
    break;
  };

  if (message == nullptr)
  {
    DEBUG(LOG_TAG_CONTENT, "Failed to serialize command buffer: %d", req.type);
    return false;
  }

  void *data = nullptr;
  size_t size = 0;
  if (!message->serialize(&data, &size))
    return false;

  return sendRaw(data, size);
}

bool TrCommandBufferSender::sendCommandBufferResponse(TrCommandBufferResponse &res)
{
  TrCommandBufferMessage *message = nullptr;
  switch (res.type)
  {
  case COMMAND_BUFFER_WEBGL_CONTEXT_INIT_RES:
  {
    auto data = dynamic_cast<WebGL1ContextInitCommandBufferResponse *>(&res);
    message = data->serialize();
    break;
  }
  default:
    break;
  };

  if (message == nullptr)
  {
    DEBUG(LOG_TAG_CONTENT, "Failed to serialize command buffer: %d", res.type);
    return false;
  }

  void *data = nullptr;
  size_t size = 0;
  if (!message->serialize(&data, &size))
    return false;

  return sendRaw(data, size);
}

TrCommandBufferBase *TrCommandBufferReceiver::recvCommandBufferRequest(int timeout)
{
  TrCommandBufferMessage message;
  if (!message.deserialize(this, timeout))
    return nullptr;

  if (message.type == COMMAND_BUFFER_WEBGL_CONTEXT_INIT_REQ)
  {
    return TrCommandBufferBase::CreateFromMessage<WebGL1ContextInitCommandBufferRequest>(message);
  }
  else
  {
    DEBUG(LOG_TAG_CONTENT, "Received an unknown command buffer: %d", message.type);
    return nullptr;
  }
}

TrCommandBufferResponse *TrCommandBufferReceiver::recvCommandBufferResponse(int timeout)
{
  TrCommandBufferMessage message;
  if (!message.deserialize(this, timeout))
    return nullptr;

  if (message.type == COMMAND_BUFFER_WEBGL_CONTEXT_INIT_RES)
  {
    return TrCommandBufferBase::CreateFromMessage<WebGL1ContextInitCommandBufferResponse>(message);
  }
  else
  {
    DEBUG(LOG_TAG_CONTENT, "Received an unknown command buffer: %d", message.type);
    return nullptr;
  }
}
