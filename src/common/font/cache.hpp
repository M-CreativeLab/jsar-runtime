#pragma once

#include <string>
#include <map>
#include <filesystem>

#include <skia/include/core/SkTypeface.h>
#include <skia/include/core/SkFont.h>
#include <skia/include/core/SkFontMgr.h>
#include <skia/include/core/SkTypes.h>
#include <skia/include/ports/SkFontMgr_directory.h>
#include "./parser.hpp"

using namespace std;

namespace font
{
#ifdef __APPLE__
  constexpr const char *DEFAULT_FONT_FAMILY = "PingFang SC";
#else
  constexpr const char *DEFAULT_FONT_FAMILY = "Arial";
#endif

  class FontCacheManager
  {
  public:
    FontCacheManager()
    {
      fontMgr = SkFontMgr_New_Custom_Directory("/System/Library/Fonts");
    }

  public:
    sk_sp<SkTypeface> getTypeface()
    {
      return fontMgr->matchFamilyStyle(DEFAULT_FONT_FAMILY, SkFontStyle::Normal());
    }
    sk_sp<SkTypeface> getTypeface(FontShorthandParser &descriptor)
    {
      auto typeface = fontMgr->matchFamilyStyle(DEFAULT_FONT_FAMILY, SkFontStyle::Normal());
      // TODO: default
      return typeface;
    }

  private:
    void addFontsByDir(string root)
    {
      if (!filesystem::exists(root) || !filesystem::is_directory(root))
        return;

      for (const auto &entry : filesystem::directory_iterator(root))
      {
        auto &path = entry.path();
        if (entry.is_directory())
        {
          addFontsByDir(path);
          continue;
        }
        if (
            path.extension() == ".ttf" ||
            path.extension() == ".ttc" ||
            path.extension() == ".otf" ||
            path.extension() == ".otc")
        {
          auto typeface = fontMgr->makeFromFile(path.c_str());
          if (typeface != nullptr)
            fprintf(stderr, "Loaded font: %s\n", path.c_str());
          else
            fprintf(stderr, "Failed to load font: %s\n", path.c_str());
        }
      }
    }

  private:
    sk_sp<SkFontMgr> fontMgr;
  };
}
