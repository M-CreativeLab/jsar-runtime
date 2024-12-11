if [ "$(uname -s)" == "Darwin" ]; then
    HOST_OS="darwin"
else
    HOST_OS="linux"
fi
HOST_ARCH="x86_64"

TOOLCHAIN=$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/$HOST_OS-$HOST_ARCH
export PATH=$TOOLCHAIN/bin:$PATH
echo "export PATH=$PATH"

cmake \
    -G "Unix Makefiles" \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_STL=c++_static \
    -DANDROID_PLATFORM=android-26 \
    -DCMAKE_BUILD_TYPE=Release \
    -B targets/android \
    ..
