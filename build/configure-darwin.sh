
cmake \
    -G "Unix Makefiles" \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11 \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
    -B targets/darwin \
    ..
