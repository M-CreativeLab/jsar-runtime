#!/bin/bash

DEFAULT_TARGET_CPU="arm64"
DEFAULT_OUT_DIR="out/release-darwin-${DEFAULT_TARGET_CPU}"

# Function to generate output directory based on target OS and CPU
generate_out_dir() {
  local is_android=$1
  local target_cpu=$2
  local out_dir=$3

  if [ -z "$out_dir" ]; then
    if [ "$is_android" = true ]; then
      out_dir="out/release-android-${target_cpu}"
    else
      out_dir="out/release-darwin-${target_cpu}"
    fi
  fi

  # Check if out_dir starts with --
  if [[ "$out_dir" == --* ]]; then
    echo "Error: Output directory '$out_dir' starts with '--'. Please place arguments before the command."
    echo "Usage: $0 [--android] [--target_cpu=<cpu>] {configure|build} [out_dir]"
    exit 1
  fi

  echo "$out_dir"
}

configure() {
  local out_dir=$1
  local is_android=$2
  local target_cpu=$3

  out_dir=$(generate_out_dir "$is_android" "$target_cpu" "$out_dir")

  # Base build arguments
  export BUILD_ARGS=$(
    echo \
      "target_cpu=\"${target_cpu}\"" \
      "is_official_build=true" \
      "is_component_build=false" \
      "is_debug=false" \
      "skia_canvaskit_enable_alias_font=false" \
      "skia_canvaskit_enable_canvas_bindings=false" \
      "skia_canvaskit_enable_debugger=false" \
      "skia_canvaskit_enable_effects_deserialization=false" \
      "skia_canvaskit_enable_embedded_font=false" \
      "skia_canvaskit_enable_font=false" \
      "skia_canvaskit_enable_matrix_helper=false" \
      "skia_canvaskit_enable_paragraph=false" \
      "skia_canvaskit_enable_pathops=false" \
      "skia_canvaskit_enable_rt_shader=false" \
      "skia_canvaskit_enable_skp_serialization=false" \
      "skia_canvaskit_enable_sksl_trace=false" \
      "skia_canvaskit_enable_webgl=false" \
      "skia_canvaskit_enable_webgpu=false" \
      "skia_canvaskit_force_tracing=false" \
      "skia_canvaskit_include_viewer=false" \
      "skia_canvaskit_legacy_draw_vertices_blend_mode=false" \
      "skia_canvaskit_profile_build=false" \
      "skia_compile_modules=false" \
      "skia_compile_sksl_tests=false" \
      "skia_disable_tracing=true" \
      "skia_enable_fontmgr_FontConfigInterface=false" \
      "skia_enable_fontmgr_android=false" \
      "skia_enable_fontmgr_custom_directory=true" \
      "skia_enable_fontmgr_custom_embedded=false" \
      "skia_enable_fontmgr_custom_empty=true" \
      "skia_enable_fontmgr_empty=true" \
      "skia_enable_fontmgr_fontconfig=false" \
      "skia_enable_fontmgr_fuchsia=false" \
      "skia_enable_fontmgr_win=false" \
      "skia_enable_fontmgr_win_gdi=false" \
      "skia_enable_ganesh=true" \
      "skia_enable_pdf=false" \
      "skia_enable_skottie=false" \
      "skia_enable_skparagraph=true" \
      "skia_enable_skshaper=true" \
      "skia_enable_skshaper_tests=false" \
      "skia_enable_skunicode=true" \
      "skia_enable_svg=true" \
      "skia_use_perfetto=false" \
      "skia_use_xps=false" \
      "skia_use_icu=true" \
      "skia_use_libwebp_decode=true" \
      "skia_use_libwebp_encode=true" \
      "skia_use_harfbuzz=true" \
      "skia_use_zlib=true" \
      "skia_use_system_icu=false" \
      "skia_use_system_libjpeg_turbo=false" \
      "skia_use_system_libpng=false" \
      "skia_use_system_libwebp=false" \
      "skia_use_system_harfbuzz=false" \
      "skia_use_system_zlib=false" \
      "skia_use_runtime_icu=true" \
      "skia_use_system_freetype2=false" \
      "skia_use_freetype=true" \
      "skia_use_freetype_svg=true" \
      "skia_use_freetype_woff2=true"
  )

  # Android-specific configuration
  if [ "$is_android" = true ]; then
    if [ -z "$ANDROID_NDK_HOME" ]; then
      echo "Error: ANDROID_NDK_HOME environment variable is not set."
      exit 1
    fi

    export BUILD_ARGS="$BUILD_ARGS $(echo \
      "target_os=\"android\"" \
      "ndk=\"$ANDROID_NDK_HOME\"" \
      "skia_use_expat=true" \
      "skia_use_system_expat=false"
    )"
  fi

  echo "Configuring build with output directory: $out_dir"
  echo "Build arguments: $BUILD_ARGS"
  bin/gn gen "$out_dir" --args="${BUILD_ARGS}"
}

build() {
  local out_dir=$1
  local is_android=$2
  local target_cpu=$3

  out_dir=$(generate_out_dir "$is_android" "$target_cpu" "$out_dir")

  echo "Building with output directory: $out_dir"
  ninja -C "$out_dir"
}

main() {
  local command=$1
  local out_dir=$2
  local is_android=false
  local target_cpu=$DEFAULT_TARGET_CPU

  # Parse arguments
  while [[ $# -gt 0 ]]; do
    case $1 in
      --android)
        is_android=true
        shift
        ;;
      --target_cpu=*)
        target_cpu="${1#*=}"
        shift
        ;;
      *)
        command=$1
        out_dir=$2
        shift
        ;;
    esac
  done

  case $command in
  configure)
    configure "$out_dir" "$is_android" "$target_cpu"
    ;;
  build)
    build "$out_dir" "$is_android" "$target_cpu"
    ;;
  rebuild)
    configure "$out_dir" "$is_android" "$target_cpu"
    build "$out_dir" "$is_android" "$target_cpu"
    ;;
  *)
    echo "Usage: $0 [--android] [--target_cpu=<cpu>] {configure|build} [out_dir]"
    echo "  --android: Enable Android-specific build configuration."
    echo "  --target_cpu: Set target CPU (default: arm64)."
    echo "  configure: Configure the build with optional output directory."
    echo "  build: Build the project with optional output directory."
    exit 1
    ;;
  esac
}

if [ $# -eq 0 ]; then
  echo "Error: No arguments provided."
  echo "Usage: $0 [--android] [--target_cpu=<cpu>] {configure|build} [out_dir]"
  exit 1
else
  main "$@"
fi
