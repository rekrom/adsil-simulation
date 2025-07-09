#pragma once

#include <string>
#include <iostream>
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <core/ResourceLocator.hpp>

namespace viewer
{
    namespace shader
    {
        class ShaderUtils
        {
        public:
            static std::string loadShaderSource(const std::string &filePath)
            {
                std::ifstream file(filePath);
                if (!file.is_open())
                {
                    std::cerr << "[ShaderUtils] Failed to open shader file: " << filePath << std::endl;
                    return "";
                }

                std::stringstream buffer;
                buffer << file.rdbuf();
                return buffer.str();
            }

            static GLuint compileShader(GLenum type, const char *source)
            {
                GLuint shader = glCreateShader(type);
                glShaderSource(shader, 1, &source, nullptr);
                glCompileShader(shader);

                GLint success;
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    char infoLog[512];
                    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
                    std::cerr << "[ShaderUtils] Shader compilation failed:\n"
                              << infoLog << std::endl;
                }

                return shader;
            }

            static GLuint createProgram(const char *vertexSrc, const char *fragmentSrc)
            {
                GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
                GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

                GLuint program = glCreateProgram();
                glAttachShader(program, vertexShader);
                glAttachShader(program, fragmentShader);
                glLinkProgram(program);

                GLint success;
                glGetProgramiv(program, GL_LINK_STATUS, &success);
                if (!success)
                {
                    char infoLog[512];
                    glGetProgramInfoLog(program, 512, nullptr, infoLog);
                    std::cerr << "[ShaderUtils] Program linking failed:\n"
                              << infoLog << std::endl;
                }

                glDeleteShader(vertexShader);
                glDeleteShader(fragmentShader);

                return program;
            }

            static GLuint createProgramFromFiles(const std::string &baseName)
            {
                std::string vertPath = core::ResourceLocator::getShaderPath(baseName, "vert");
                std::string fragPath = core::ResourceLocator::getShaderPath(baseName, "frag");

                std::string vertexCode = loadShaderSource(vertPath);
                std::string fragmentCode = loadShaderSource(fragPath);

                if (vertexCode.empty() || fragmentCode.empty())
                {
                    std::cerr << "[ShaderUtils] Shader source code is empty. Aborting shader program creation." << std::endl;
                    return 0;
                }

                return createProgram(vertexCode.c_str(), fragmentCode.c_str());
            }
        };
    }
}