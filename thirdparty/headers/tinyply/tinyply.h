// tinyply.h - A simple PLY mesh file loader
// This is a simplified version based on https://github.com/ddiakopoulos/tinyply
// Copyright, Dimitri Diakopoulos (http://www.dimitridiakopoulos.com)
// Licensed under the Unlicense

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace tinyply
{
    enum class Type : uint8_t 
    { 
        INVALID, 
        INT8, UINT8, INT16, UINT16, 
        INT32, UINT32, FLOAT32, FLOAT64 
    };

    struct PropertyInfo
    {
        int stride;
        std::string str;
    };

    static std::map<Type, PropertyInfo> PropertyTable
    {
        { Type::INT8,    { 1, "char" } },
        { Type::UINT8,   { 1, "uchar" } },
        { Type::INT16,   { 2, "short" } },
        { Type::UINT16,  { 2, "ushort" } },
        { Type::INT32,   { 4, "int" } },
        { Type::UINT32,  { 4, "uint" } },
        { Type::FLOAT32, { 4, "float" } },
        { Type::FLOAT64, { 8, "double" } },
        { Type::INVALID, { 0, "INVALID" } }
    };

    class PlyData
    {
    public:
        std::vector<std::string> comments;
        std::string element_name;
        size_t count = 0;
        Type t = Type::INVALID;
        size_t buffer_size = 0;
        std::shared_ptr<uint8_t> buffer;

        template<typename T>
        std::vector<T> get_data()
        {
            std::vector<T> data;
            if (buffer && buffer_size > 0) {
                size_t element_count = buffer_size / sizeof(T);
                data.resize(element_count);
                std::memcpy(data.data(), buffer.get(), buffer_size);
            }
            return data;
        }
    };

    class PlyFile
    {
    public:
        PlyFile() = default;
        ~PlyFile() = default;

        // Read PLY file from stream
        void read(std::istream & is)
        {
            // Simple PLY parser implementation
            std::string line;
            bool in_header = true;
            size_t vertex_count = 0;
            std::vector<std::string> properties;
            
            while (std::getline(is, line)) {
                if (in_header) {
                    if (line == "end_header") {
                        in_header = false;
                        continue;
                    }
                    
                    if (line.find("element vertex") == 0) {
                        std::istringstream iss(line);
                        std::string element, vertex;
                        iss >> element >> vertex >> vertex_count;
                    }
                    
                    if (line.find("property") == 0) {
                        properties.push_back(line);
                    }
                } else {
                    // Parse vertex data
                    vertex_lines.push_back(line);
                }
            }
            
            element_count = vertex_count;
        }

        // Request property data
        std::shared_ptr<PlyData> request_properties_from_element(
            const std::string & elementKey, 
            const std::vector<std::string> & propertyKeys,
            const uint32_t list_size_hint = 0)
        {
            auto data = std::make_shared<PlyData>();
            data->element_name = elementKey;
            data->count = element_count;
            
            requested_properties.push_back({propertyKeys, data});
            return data;
        }

        // Get comments
        std::vector<std::string> get_comments() const
        {
            return comments;
        }

        // Get elements
        std::vector<std::string> get_elements() const
        {
            return {"vertex"};
        }

        // Parse requested data (call after read)
        void parse_vertex_data()
        {
            for (auto& req : requested_properties) {
                auto& property_keys = req.first;
                auto& data = req.second;
                
                // Parse vertex data based on requested properties
                size_t total_floats = property_keys.size() * element_count;
                size_t buffer_size = total_floats * sizeof(float);
                
                data->buffer = std::shared_ptr<uint8_t>(new uint8_t[buffer_size], std::default_delete<uint8_t[]>());
                data->buffer_size = buffer_size;
                data->t = Type::FLOAT32;
                
                float* float_buffer = reinterpret_cast<float*>(data->buffer.get());
                
                for (size_t i = 0; i < vertex_lines.size() && i < element_count; ++i) {
                    std::istringstream iss(vertex_lines[i]);
                    std::vector<float> values;
                    float value;
                    
                    while (iss >> value) {
                        values.push_back(value);
                    }
                    
                    // Map requested properties to values
                    for (size_t j = 0; j < property_keys.size(); ++j) {
                        size_t index = i * property_keys.size() + j;
                        if (index < total_floats) {
                            if (j < values.size()) {
                                float_buffer[index] = values[j];
                            } else {
                                float_buffer[index] = 0.0f; // Default value
                            }
                        }
                    }
                }
            }
        }

    private:
        std::vector<std::string> comments;
        std::vector<std::string> vertex_lines;
        std::vector<std::pair<std::vector<std::string>, std::shared_ptr<PlyData>>> requested_properties;
        size_t element_count = 0;
    };
} // namespace tinyply