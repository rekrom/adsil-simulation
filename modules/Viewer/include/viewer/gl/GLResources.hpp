#pragma once

#include <glad/glad.h>
#include <utility>

namespace viewer::gl
{

    /**
     * @brief RAII wrapper for OpenGL Vertex Array Object (VAO)
     *
     * Automatically creates VAO on construction and deletes on destruction.
     * Move-only (cannot be copied to prevent double-free).
     */
    class VertexArray
    {
    public:
        VertexArray() { glGenVertexArrays(1, &id_); }

        ~VertexArray()
        {
            if (id_ != 0)
            {
                glDeleteVertexArrays(1, &id_);
            }
        }

        // Move constructor
        VertexArray(VertexArray &&other) noexcept : id_(other.id_)
        {
            other.id_ = 0;
        }

        // Move assignment
        VertexArray &operator=(VertexArray &&other) noexcept
        {
            if (this != &other)
            {
                if (id_ != 0)
                {
                    glDeleteVertexArrays(1, &id_);
                }
                id_ = other.id_;
                other.id_ = 0;
            }
            return *this;
        }

        // Delete copy operations
        VertexArray(const VertexArray &) = delete;
        VertexArray &operator=(const VertexArray &) = delete;

        // Accessors
        GLuint get() const { return id_; }
        operator GLuint() const { return id_; }

        void bind() const { glBindVertexArray(id_); }
        static void unbind() { glBindVertexArray(0); }

        bool isValid() const { return id_ != 0; }

    private:
        GLuint id_ = 0;
    };

    /**
     * @brief RAII wrapper for OpenGL Buffer Object (VBO, EBO, etc.)
     *
     * Automatically creates buffer on construction and deletes on destruction.
     * Move-only (cannot be copied to prevent double-free).
     */
    class Buffer
    {
    public:
        Buffer() { glGenBuffers(1, &id_); }

        ~Buffer()
        {
            if (id_ != 0)
            {
                glDeleteBuffers(1, &id_);
            }
        }

        // Move constructor
        Buffer(Buffer &&other) noexcept : id_(other.id_)
        {
            other.id_ = 0;
        }

        // Move assignment
        Buffer &operator=(Buffer &&other) noexcept
        {
            if (this != &other)
            {
                if (id_ != 0)
                {
                    glDeleteBuffers(1, &id_);
                }
                id_ = other.id_;
                other.id_ = 0;
            }
            return *this;
        }

        // Delete copy operations
        Buffer(const Buffer &) = delete;
        Buffer &operator=(const Buffer &) = delete;

        // Accessors
        GLuint get() const { return id_; }
        operator GLuint() const { return id_; }

        void bind(GLenum target) const { glBindBuffer(target, id_); }
        static void unbind(GLenum target) { glBindBuffer(target, 0); }

        bool isValid() const { return id_ != 0; }

    private:
        GLuint id_ = 0;
    };

    /**
     * @brief RAII wrapper for OpenGL Shader Program
     *
     * Takes ownership of an existing shader program ID.
     * Deletes the program on destruction.
     * Move-only (cannot be copied to prevent double-free).
     */
    class ShaderProgram
    {
    public:
        // Default constructor (no program)
        ShaderProgram() = default;

        // Take ownership of existing program
        explicit ShaderProgram(GLuint programId) : id_(programId) {}

        ~ShaderProgram()
        {
            if (id_ != 0)
            {
                glDeleteProgram(id_);
            }
        }

        // Move constructor
        ShaderProgram(ShaderProgram &&other) noexcept : id_(other.id_)
        {
            other.id_ = 0;
        }

        // Move assignment
        ShaderProgram &operator=(ShaderProgram &&other) noexcept
        {
            if (this != &other)
            {
                if (id_ != 0)
                {
                    glDeleteProgram(id_);
                }
                id_ = other.id_;
                other.id_ = 0;
            }
            return *this;
        }

        // Delete copy operations
        ShaderProgram(const ShaderProgram &) = delete;
        ShaderProgram &operator=(const ShaderProgram &) = delete;

        // Accessors
        GLuint get() const { return id_; }
        operator GLuint() const { return id_; }

        void use() const { glUseProgram(id_); }
        static void unuse() { glUseProgram(0); }

        bool isValid() const { return id_ != 0; }

        // Uniform helpers
        GLint getUniformLocation(const char *name) const
        {
            return glGetUniformLocation(id_, name);
        }

    private:
        GLuint id_ = 0;
    };

    /**
     * @brief RAII wrapper for OpenGL Texture
     *
     * Automatically creates texture on construction and deletes on destruction.
     * Move-only (cannot be copied to prevent double-free).
     */
    class Texture
    {
    public:
        Texture() { glGenTextures(1, &id_); }

        ~Texture()
        {
            if (id_ != 0)
            {
                glDeleteTextures(1, &id_);
            }
        }

        // Move constructor
        Texture(Texture &&other) noexcept : id_(other.id_)
        {
            other.id_ = 0;
        }

        // Move assignment
        Texture &operator=(Texture &&other) noexcept
        {
            if (this != &other)
            {
                if (id_ != 0)
                {
                    glDeleteTextures(1, &id_);
                }
                id_ = other.id_;
                other.id_ = 0;
            }
            return *this;
        }

        // Delete copy operations
        Texture(const Texture &) = delete;
        Texture &operator=(const Texture &) = delete;

        // Accessors
        GLuint get() const { return id_; }
        operator GLuint() const { return id_; }

        void bind(GLenum target = GL_TEXTURE_2D) const { glBindTexture(target, id_); }
        static void unbind(GLenum target = GL_TEXTURE_2D) { glBindTexture(target, 0); }

        bool isValid() const { return id_ != 0; }

    private:
        GLuint id_ = 0;
    };

} // namespace viewer::gl
