add_custom_target(TransmuteProtoFiles)
add_custom_command(
    TARGET TransmuteProtoFiles
    COMMAND ${CMAKE_SOURCE_DIR}/Thirdparty/Library/Darwin/bin/protoc
    ARGS --cpp_out=${CMAKE_SOURCE_DIR}/Source/proto --csharp_out=../../TransmuteUnityFramework/Assets/UPM/Scripts/Proto/ transmute_vgom.proto
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Source/proto
    COMMENT "Generating Transmute Protobuf files"
)
