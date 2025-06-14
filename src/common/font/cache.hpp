#pragma once

#include <string>
#include <map>
#include <iostream>
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

#ifdef __ANDROID__
#include <skia/include/ports/SkFontMgr_android.h>
#endif

using namespace std;

namespace font
{
  class FontCacheManager;
  class TrSimpleFontStyleSet : public SkFontStyleSet
  {
  public:
    explicit TrSimpleFontStyleSet(sk_sp<SkTypeface> defaultTypeface)
    {
      typefaces_.emplace_back(std::move(defaultTypeface));
    }

  public:
    void appendTypeface(sk_sp<SkTypeface> typeface)
    {
      typefaces_.emplace_back(std::move(typeface));
    }
    int count() override
    {
      return typefaces_.size();
    }
    void getStyle(int index, SkFontStyle *style, SkString *styleName) override
    {
      assert(index < typefaces_.size());
      if (style)
        *style = typefaces_[index]->fontStyle();
      if (styleName)
        styleName->reset();
    }
    sk_sp<SkTypeface> createTypeface(int index) override
    {
      assert(index < typefaces_.size());
      return typefaces_[index];
    }
    sk_sp<SkTypeface> matchStyle(const SkFontStyle &pattern) override
    {
      return matchStyleCSS3(pattern);
    }

  private:
    std::vector<sk_sp<SkTypeface>> typefaces_;
  };

  class MutipleDirectoriesFontMgr : public SkFontMgr
  {
  public:
    MutipleDirectoriesFontMgr()
    {
#ifdef __ANDROID__
      systemFontMgr_ = SkFontMgr_New_Android(nullptr);
#endif
      if (systemFontMgr_ != nullptr)
        std::cout << "System fonts: " << systemFontMgr_->countFamilies() << std::endl;
    }

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

      // fallback to system font manager
      if (systemFontMgr_ != nullptr)
        systemFontMgr_->getFamilyName(index, familyName);
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

      // fallback to system font manager
      if (systemFontMgr_ != nullptr)
        return systemFontMgr_->createStyleSet(index);
      return nullptr;
    }
    sk_sp<SkFontStyleSet> onMatchFamily(const char familyName[]) const override
    {
      if (systemFontMgr_ != nullptr)
      {
        sk_sp<SkFontStyleSet> systemStyleSet = systemFontMgr_->matchFamily(familyName);
        if (systemStyleSet)
          return systemStyleSet;
      }

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
      if (systemFontMgr_ != nullptr)
      {
        sk_sp<SkTypeface> systemTypeface = systemFontMgr_->matchFamilyStyle(familyName, style);
        if (systemTypeface)
          return systemTypeface;
      }

      for (const auto &pair : fontMgrsByPath_)
      {
        auto typeface = pair.second->matchFamilyStyle(familyName, style);
        if (typeface)
          return typeface;
      }
      return nullptr;
    }
    sk_sp<SkTypeface> onMatchFamilyStyleCharacter(const char familyName[], const SkFontStyle &style, const char *bcp47[], int bcp47Count, SkUnichar character) const override
    {
      if (systemFontMgr_ != nullptr)
      {
        sk_sp<SkTypeface> systemTypeface = systemFontMgr_->matchFamilyStyleCharacter(familyName, style, bcp47, bcp47Count, character);
        if (systemTypeface)
          return systemTypeface;
      }

      for (const auto &pair : fontMgrsByPath_) // check custom font managers
      {
        auto typeface = pair.second->matchFamilyStyleCharacter(familyName, style, bcp47, bcp47Count, character);
        if (typeface)
          return typeface;
      }
      return nullptr;
    }
    sk_sp<SkTypeface> onMakeFromData(sk_sp<SkData> data, int ttcIndex) const override
    {
      if (systemFontMgr_ != nullptr)
      {
        sk_sp<SkTypeface> systemTypeface = systemFontMgr_->makeFromData(data, ttcIndex);
        if (systemTypeface)
          return systemTypeface;
      }

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
      if (systemFontMgr_ != nullptr)
      {
        sk_sp<SkTypeface> systemTypeface = systemFontMgr_->makeFromStream(std::move(stream), ttcIndex);
        if (systemTypeface)
          return systemTypeface;
      }

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
      if (systemFontMgr_ != nullptr)
      {
        sk_sp<SkTypeface> systemTypeface = systemFontMgr_->makeFromStream(std::move(stream), args);
        if (systemTypeface)
          return systemTypeface;
      }

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
      if (systemFontMgr_ != nullptr)
      {
        sk_sp<SkTypeface> systemTypeface = systemFontMgr_->makeFromFile(path, ttcIndex);
        if (systemTypeface)
          return systemTypeface;
      }

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
      if (systemFontMgr_ != nullptr)
      {
        sk_sp<SkTypeface> systemTypeface = systemFontMgr_->legacyMakeTypeface(familyName, style);
        if (systemTypeface)
          return systemTypeface;
      }

      for (auto &pair : fontMgrsByPath_)
      {
        auto typeface = pair.second->legacyMakeTypeface(familyName, style);
        if (typeface)
          return typeface;
      }
      return nullptr;
    }

  private:
    sk_sp<SkFontMgr> systemFontMgr_ = nullptr;
    std::unordered_map<std::string, sk_sp<SkFontMgr>> fontMgrsByPath_;
  };

  class FontCacheManager
  {
  public:
    FontCacheManager()
        : fontMgr_(sk_make_sp<MutipleDirectoriesFontMgr>())
        , fontCollection_(sk_make_sp<skia::textlayout::FontCollection>())
    {
#ifdef __APPLE__
      addFontsAt("/Library/Fonts");
      addFontsAt("/System/Library/Fonts");
#elif __ANDROID__
      addFontsAt("/system/fonts");
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
    operator sk_sp<SkFontMgr>() const
    {
      return fontMgr_;
    }
    // implicit conversion to skia::textlayout::FontCollection
    operator sk_sp<skia::textlayout::FontCollection>() const
    {
      return fontCollection_;
    }

  private:
    void addFontsAt(std::string root)
    {
      if (!fontMgr_->addFontsAt(root))
        DEBUG(LOG_TAG_FONT, "Failed to load fonts from: %s", root.c_str());
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
