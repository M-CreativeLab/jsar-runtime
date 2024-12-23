# Function to generate the header file that contains the shaders.
function(tr_generate_shader_header OUTPUT SOURCE_DIR)
    file(GLOB_RECURSE shader_files
        "${SOURCE_DIR}/*.vert"
        "${SOURCE_DIR}/*.frag"
    )

    set(header_content "#pragma once")
    set(header_content "${header_content}\n#include <unordered_map>")
    set(header_content "${header_content}\n#include <string>")
    set(header_content "${header_content}\n")

    # Start the namespace
    set(header_content "${header_content}\nnamespace builtin_scene::shaders {\n")

    # Write the shader strings
    foreach(shader_file IN LISTS shader_files)
        file(RELATIVE_PATH sub_path "${SOURCE_DIR}" "${shader_file}")
        string(REPLACE "." "_" sub_path "${sub_path}")
        string(REPLACE "/" "_" sub_path "${sub_path}")
        file(READ "${shader_file}" shader_content)

        # Escape quotes in shader content to properly include in the C++ string literal
        string(REPLACE "\"" "\\\"" shader_content "${shader_content}")
        set(header_content
            "${header_content}\nconst char* ${sub_path}_shader = R\"#TR(\n${shader_content})#TR\";\n"
        )
    endforeach()

    # Write the map that contains all the shaders
    set(header_content "${header_content}\nstatic std::unordered_map<std::string, std::string> SHADERS_STORE = {")
    foreach(shader_file IN LISTS shader_files)
        file(RELATIVE_PATH sub_path "${SOURCE_DIR}" "${shader_file}")
        string(REPLACE "." "_" shader_var "${sub_path}")
        string(REPLACE "/" "_" shader_var "${shader_var}")
        set(header_content "${header_content}\n  {\"${sub_path}\", ${shader_var}_shader},")
    endforeach()
    set(header_content "${header_content}\n};")

    # End the namespace
    set(header_content "${header_content}\n}\n")

    # Write to the output file
    file(WRITE "${OUTPUT}" "${header_content}")
    message(STATUS "Generated shader header file: ${OUTPUT}")
endfunction()

# Generate the header file that contains the shaders.
tr_generate_shader_header(${HEADER_OUTPUT} ${SHADERS_DIR})
