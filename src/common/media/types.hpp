#pragma once

namespace media_comm
{
  enum class MediaContentType
  {
    Audio = 0,
    Video,
    Video3D,
  };

  enum class CanPlayTypeResult
  {
    No = 0,
    Probably = 1,
    Maybe = 2
  };
}
