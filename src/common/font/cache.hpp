#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <filesystem>
#include <assert.h>

#include <skia/include/core/SkData.h>
#include <skia/include/core/SkTypeface.h>
#include <skia/include/core/SkFont.h>
#include <skia/include/core/SkFontMgr.h>
#include <skia/include/core/SkStream.h>
#include <skia/include/core/SkTypes.h>
#include <skia/include/ports/SkFontMgr_directory.h>
#include <skia/modules/skparagraph/include/FontCollection.h>
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
#ifdef __ANDROID__
        {"Noto Serif", 1},
        {"Noto Serif CJK SC", 1},
#endif
    };
    static const unordered_map<string, int> SANS_SERIF_FONTS_LIST = {
        {"PingFang SC", 1},
        {"Alibaba PuHuiTi 3.0", 1},
#ifdef __ANDROID__
        {"Noto Sans CJK SC", 2},
        {"DroidSans", 3},
#endif
    };
    static const unordered_map<string, int> MONOSPACE_FONTS_LIST = {
        {"Consolas", 1},
        {"Monaco", 1},
#ifdef __ANDROID__
        {"Noto Sans Mono CJK SC", 2},
        {"Droid Sans Mono", 3},
#endif
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

  class MutipleDirectoriesFontMgr : public SkFontMgr
  {
  public:
    MutipleDirectoriesFontMgr() = default;

  public:
    bool addFontsAt(std::string path)
    {
      if (!filesystem::exists(path) || !filesystem::is_directory(path))
        return false;

      auto fontMgr = SkFontMgr_New_Custom_Directory(path.c_str());
      if (fontMgr == nullptr)
        return false;

      fontMgrsByPath_[path] = std::move(fontMgr);
      return true;
    }

  protected:
    int onCountFamilies() const override
    {
      int count = 0;
      for (const auto &pair : fontMgrsByPath_)
        count += pair.second->countFamilies();
      return count;
    }
    void onGetFamilyName(int index, SkString *familyName) const override
    {
      for (const auto &pair : fontMgrsByPath_)
      {
        int count = pair.second->countFamilies();
        if (index < count)
        {
          pair.second->getFamilyName(index, familyName);
          return;
        }
        index -= count;
      }
    }
    sk_sp<SkFontStyleSet> onCreateStyleSet(int index) const override
    {
      for (auto &pair : fontMgrsByPath_)
      {
        auto mgr = pair.second;
        int families = mgr->countFamilies();
        if (index < families)
          return mgr->createStyleSet(index);
        index -= families;
      }
      return nullptr;
    }
    sk_sp<SkFontStyleSet> onMatchFamily(const char familyName[]) const override
    {
      for (const auto &pair : fontMgrsByPath_)
      {
        auto styleSet = pair.second->matchFamily(familyName);
        if (styleSet)
          return styleSet;
      }
      return nullptr;
    }
    sk_sp<SkTypeface> onMatchFamilyStyle(const char familyName[], const SkFontStyle &style) const override
    {
      for (const auto &pair : fontMgrsByPath_)
      {
        auto typeface = pair.second->matchFamilyStyle(familyName, style);
        if (typeface)
          return typeface;
      }
      return nullptr;
    }
    sk_sp<SkTypeface> onMatchFamilyStyleCharacter(const char familyName[], const SkFontStyle &style,
                                                  const char *bcp47[], int bcp47Count,
                                                  SkUnichar character) const override
    {
      for (const auto &pair : fontMgrsByPath_)
      {
        auto typeface = pair.second->matchFamilyStyleCharacter(familyName, style, bcp47, bcp47Count, character);
        if (typeface)
          return typeface;
      }
      return nullptr;
    }
    sk_sp<SkTypeface> onMakeFromData(sk_sp<SkData> data, int ttcIndex) const override
    {
      for (auto &pair : fontMgrsByPath_)
      {
        auto typeface = pair.second->makeFromData(data, ttcIndex);
        if (typeface)
          return typeface;
      }
      return nullptr;
    }
    sk_sp<SkTypeface> onMakeFromStreamIndex(std::unique_ptr<SkStreamAsset> stream, int ttcIndex) const override
    {
      for (auto &pair : fontMgrsByPath_)
      {
        auto typeface = pair.second->makeFromStream(std::move(stream), ttcIndex);
        if (typeface)
          return typeface;
      }
      return nullptr;
    }
    sk_sp<SkTypeface> onMakeFromStreamArgs(std::unique_ptr<SkStreamAsset> stream, const SkFontArguments &args) const override
    {
      for (auto &pair : fontMgrsByPath_)
      {
        auto typeface = pair.second->makeFromStream(std::move(stream), args);
        if (typeface)
          return typeface;
      }
      return nullptr;
    }
    sk_sp<SkTypeface> onMakeFromFile(const char path[], int ttcIndex) const override
    {
      for (auto &pair : fontMgrsByPath_)
      {
        auto typeface = pair.second->makeFromFile(path, ttcIndex);
        if (typeface)
          return typeface;
      }
      return nullptr;
    }
    sk_sp<SkTypeface> onLegacyMakeTypeface(const char familyName[], SkFontStyle style) const override
    {
      for (auto &pair : fontMgrsByPath_)
      {
        auto typeface = pair.second->legacyMakeTypeface(familyName, style);
        if (typeface)
          return typeface;
      }
      return nullptr;
    }

  private:
    std::unordered_map<std::string, sk_sp<SkFontMgr>> fontMgrsByPath_;
  };

  class FontCacheManager
  {
  public:
    FontCacheManager()
        : fontMgr_(sk_make_sp<MutipleDirectoriesFontMgr>()),
          fontCollection_(sk_make_sp<skia::textlayout::FontCollection>())
    {
#ifdef __APPLE__
      addFontsAt("/Library/Fonts");
      addFontsAt("/System/Library/Fonts");
#elif __ANDROID__
      addFontsAt("/system/fonts");
      addFontsAt("/usr/share/fonts");
      addFontsAt("/usr/local/share/fonts");
#endif
      fontCollection_->setDefaultFontManager(fontMgr_);
      printSummary();
    }

  public:
    /**
     * @returns the default typeface
     */
    inline sk_sp<SkTypeface> getTypeface()
    {
      return fontCollection_->defaultFallback();
    }
    /**
     * Get the typeface for the given family name and style.
     * 
     * @param familyName the family name
     * @param fontStyle the font style
     * @returns the typeface
     */
    inline sk_sp<SkTypeface> getTypeface(string familyName, const SkFontStyle fontStyle = SkFontStyle::Normal())
    {
      return fontCollection_->findTypefaces({SkString(familyName.c_str())}, fontStyle).front();
    }
    /**
     * Get the typeface for the given font shorthand descriptor.
     * 
     * @param descriptor the font shorthand descriptor
     * @returns the typeface
     */
    inline sk_sp<SkTypeface> getTypeface(FontShorthandParser &descriptor)
    {
      return getTypeface(descriptor.family, descriptor.style);
    }

  public:
    // implicit conversion to SkFontMgr
    operator sk_sp<SkFontMgr>() const { return fontMgr_; }
    // implicit conversion to skia::textlayout::FontCollection
    operator sk_sp<skia::textlayout::FontCollection>() const { return fontCollection_; }

  private:
    void addFontsAt(std::string root)
    {
      if (!fontMgr_->addFontsAt(root))
        return;
    }
    void printSummary()
    {
      DEBUG(LOG_TAG_FONT, "Loaded %d fonts:", fontMgr_->countFamilies());
    }

  private:
    sk_sp<MutipleDirectoriesFontMgr> fontMgr_;
    sk_sp<skia::textlayout::FontCollection> fontCollection_;
  };
}
