# Setup the android toolchain environment.

export TOOLCHAIN=$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/darwin-x86_64
export PATH=$TOOLCHAIN/bin:$PATH
export SYSROOT=$TOOLCHAIN/sysroot

export CC=$TOOLCHAIN/bin/clang
export CPP="$TOOLCHAIN/bin/clang -E"
export CXX=$TOOLCHAIN/bin/clang++

export CFLAGS="-fPIC -Os"
export CXXFLAGS="-fPIC -Os"

export AR=$TOOLCHAIN/bin/llvm-ar
export RANLIB=$TOOLCHAIN/bin/llvm-ranlib
export OBJCOPY=$TOOLCHAIN/bin/llvm-objcopy
export YASM=$TOOLCHAIN/bin/yasm
export STRIP=$TOOLCHAIN/bin/llvm-strip
export LIBCLANG_PATH=$TOOLCHAIN/lib64
