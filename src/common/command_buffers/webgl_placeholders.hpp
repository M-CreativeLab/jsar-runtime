#pragma once

#include <string>

static const char *WEBGL_PLACEHOLDERS_USE_RIGHTHANDED_KEY = "_useRightHanded";
static const char *WEBGL_PLACEHOLDERS_INVERSE_MATRIX_KEY = "_inverseMatrix";
static const char *WEBGL_PLACEHOLDERS_PLACEHOLDER_ID_KEY = "_placeholderId";

#define PLACEHOLDER_ID_MAP(XX) \
  XX(ProjectionMatrix, 10)     \
  XX(ViewMatrix, 11)           \
  XX(ViewProjectionMatrix, 12) \
  XX(ViewProjectionMatrixForRightEye, 13)

static const char *WEBGL_PLACEHOLDERS_IDS[] = {
#define XX(id, _) #id,
    PLACEHOLDER_ID_MAP(XX)
#undef XX
};

enum class WebGLMatrixPlaceholderId
{
#define XX(id, v) id = v,
  PLACEHOLDER_ID_MAP(XX)
#undef XX
  NotSet = 0,
};

inline bool isValidPlaceholderId(uint32_t placeholderId)
{
  switch (placeholderId)
  {
#define XX(_, v) \
  case v:        \
    return true;
    PLACEHOLDER_ID_MAP(XX)
#undef XX
  default:
    return false;
  }
}
