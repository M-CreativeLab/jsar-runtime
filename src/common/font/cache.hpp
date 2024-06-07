#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <filesystem>
#include <assert.h>

#include <skia/include/core/SkTypeface.h>
#include <skia/include/core/SkFont.h>
#include <skia/include/core/SkFontMgr.h>
#include <skia/include/core/SkTypes.h>
#include <skia/include/ports/SkFontMgr_directory.h>
#include "debug.hpp"
#include "./parser.hpp"

using namespace std;

namespace font
{
  namespace generics
  {
#define GENERIC_FONT_FAMILY_MAP(XX) \
  XX(Serif, "serif")                \
  XX(SansSerif, "sans-serif")       \
  XX(Monospace, "monospace")

    enum class GenericFontFamily
    {
#define XX(type, name) type,
      GENERIC_FONT_FAMILY_MAP(XX)
#undef XX
          None,
    };

    static const unordered_map<string, int> SERIF_FONTS_LIST = {
        {"Arial", 1},
    };
    static const unordered_map<string, int> SANS_SERIF_FONTS_LIST = {
        {"PingFang SC", 1},
        {"NotoSans SC", 2},
        {"DroidSans", 3},
    };
    static const unordered_map<string, int> MONOSPACE_FONTS_LIST = {
        {"Consolas", 1},
        {"Monaco", 1},
    };
    // TODO: add more generic font families

    inline string idToGenericName(GenericFontFamily id)
    {
      switch (id)
      {
#define XX(type, name)          \
  case GenericFontFamily::type: \
    return name;
        GENERIC_FONT_FAMILY_MAP(XX)
#undef XX
      default:
        return "";
      }
    }

    inline GenericFontFamily genericNameToId(const string &name2)
    {
#define XX(type, name) \
  if (name == name2)   \
    return GenericFontFamily::type;
      GENERIC_FONT_FAMILY_MAP(XX)
#undef XX
      return GenericFontFamily::None;
    }
  }

  class FontCacheManager;
  class TrFontStyleSet : public SkFontStyleSet
  {
  public:
    explicit TrFontStyleSet(string name) : familyName(name)
    {
    }

  public:
    void appendTypeface(sk_sp<SkTypeface> typeface)
    {
      typefaces.emplace_back(move(typeface));
    }
    int count() override
    {
      return typefaces.size();
    }
    void getStyle(int index, SkFontStyle *style, SkString *styleName) override
    {
      assert(index < typefaces.size());
      if (style)
        *style = typefaces[index]->fontStyle();
      if (styleName)
        styleName->reset();
    }
    sk_sp<SkTypeface> createTypeface(int index) override
    {
      assert(index < typefaces.size());
      return typefaces[index];
    }
    sk_sp<SkTypeface> matchStyle(const SkFontStyle &pattern) override
    {
      return matchStyleCSS3(pattern);
    }
    string &getFamilyName()
    {
      return familyName;
    }

  private:
    string familyName;
    vector<sk_sp<SkTypeface>> typefaces;

    friend class FontCacheManager;
  };

  class FontCacheManager
  {
  public:
    FontCacheManager()
    {
      fontMgr = SkFontMgr_New_Custom_Directory("/Unknown");
#ifdef __APPLE__
      addFontsByDir("/Library/Fonts");
      addFontsByDir("/System/Library/Fonts");
#elif __ANDROID__
      fontMgr = SkFontMgr_New_Custom_Directory("/system/fonts");
      else fontMgr = SkFontMgr::RefEmpty();
#endif
      lookupGenericFamilies();
      printSummary();
    }

  public:
    sk_sp<SkTypeface> getTypeface()
    {
      string familyName = genericFontFamilies[generics::GenericFontFamily::SansSerif];
      auto fontFamily = findFontFamily(familyName);
      assert(fontFamily != nullptr);
      return fontFamily->matchStyle(SkFontStyle::Normal());
    }
    sk_sp<SkTypeface> getTypeface(FontShorthandParser &descriptor)
    {
      auto genericId = generics::genericNameToId(descriptor.family);
      if (genericId != generics::GenericFontFamily::None)
      {
        auto fontFamily = findFontFamily(genericFontFamilies[genericId]);
        if (fontFamily != nullptr)
          return fontFamily->matchStyle(descriptor.style);
      }
      else
      {
        auto fontFamily = findFontFamily(descriptor.family);
        if (fontFamily != nullptr)
          return fontFamily->matchStyle(descriptor.style);
      }
      return getTypeface();
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
            path.extension() == ".ttc")
        {
          size_t ttcIndex = 0;
          while (true)
          {
            sk_sp<SkTypeface> typeface = fontMgr->makeFromFile(path.c_str(), ttcIndex++);
            if (typeface == nullptr)
              break;

            SkString familyName;
            typeface->getFamilyName(&familyName);
            auto addTo = findFontFamily(familyName.c_str());
            if (addTo == nullptr)
            {
              addTo = new TrFontStyleSet(familyName.c_str());
              fontFamilies.push_back(sk_sp<TrFontStyleSet>(addTo));
            }
            addTo->appendTypeface(typeface);
          }
        }
      }
    }
    void lookupGenericFamilies()
    {
      assert(fontMgr != nullptr);
      auto count = fontMgr->countFamilies();
      for (auto &family : fontFamilies)
      {
        auto &name = family->getFamilyName();
        if (tryInsertGenericFontFamily(generics::GenericFontFamily::Serif, name, generics::SERIF_FONTS_LIST))
          continue;
        if (tryInsertGenericFontFamily(generics::GenericFontFamily::SansSerif, name, generics::SANS_SERIF_FONTS_LIST))
          continue;
        if (tryInsertGenericFontFamily(generics::GenericFontFamily::Monospace, name, generics::MONOSPACE_FONTS_LIST))
          continue;
      }
    }
    bool tryInsertGenericFontFamily(generics::GenericFontFamily type, string &familyName,
                                    const unordered_map<string, int> &fontsList)
    {
      const char *nameStr = familyName.c_str();
      if (fontsList.find(nameStr) != fontsList.end())
      {
        if (genericFontFamilies.find(type) == genericFontFamilies.end())
        {
          /**
           * Directly insert the font if the generic font family is not set yet.
           */
          genericFontFamilies[type] = nameStr;
          return true;
        }
        else
        {
          auto &existing = genericFontFamilies[type];
          if (fontsList.at(nameStr) < fontsList.at(existing))
          {
            /**
             * When the new font's priority number is less than the existing font's priority number, replace it.
             */
            genericFontFamilies[type] = nameStr;
            return true;
          }
        }
      }
      return false;
    }
    TrFontStyleSet *findFontFamily(const string &name)
    {
      for (auto &family : fontFamilies)
      {
        if (family->getFamilyName() == name)
          return family.get();
      }
      return nullptr;
    }
    void printSummary()
    {
      if (genericFontFamilies.size() == 0)
        DEBUG(LOG_TAG_FONT, "No generic font families found");
      else
      {
        DEBUG(LOG_TAG_FONT, "Generic font families:");
        for (auto &family : genericFontFamilies)
        {
          auto id = family.first;
          auto familyName = family.second;
          DEBUG(LOG_TAG_FONT, "%s -> \"%s\"", generics::idToGenericName(id).c_str(),
                familyName.c_str());
        }
      }
      DEBUG(LOG_TAG_FONT, "Loaded %d fonts", fontFamilies.size());
    }

  private:
    sk_sp<SkFontMgr> fontMgr;
    vector<sk_sp<TrFontStyleSet>> fontFamilies;
    map<generics::GenericFontFamily, string> genericFontFamilies;
  };
}
