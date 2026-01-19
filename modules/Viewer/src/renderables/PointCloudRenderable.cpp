#include <viewer/renderables/PointCloudRenderable.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace viewer
{

    PointCloudRenderable::PointCloudRenderable(std::shared_ptr<math::PointCloud> pointCloud, glm::vec3 color)
        : pointCloud_(std::move(pointCloud))
    {
        this->setColor(color);
        this->setAlpha(0.5F);
    }

    PointCloudRenderable::~PointCloudRenderable() = default; // RAII handles cleanup!

    void PointCloudRenderable::cleanup()
    {
        // RAII: Simply reset the optionals - destructors handle OpenGL cleanup
        vao_.reset();
        vbo_.reset();
        shader_.reset();
    }

    void PointCloudRenderable::initGL()
    {
        createShader();
        createBuffers();
    }

    void PointCloudRenderable::createShader()
    {
        // RAII: Wrap the shader program
        shader_.emplace(shader::ShaderUtils::createProgramFromFiles("point_cloud"));

        uniforms_.model = shader_->getUniformLocation("model");
        uniforms_.view = shader_->getUniformLocation("view");
        uniforms_.projection = shader_->getUniformLocation("projection");
        uniforms_.color = shader_->getUniformLocation("uniformColor");
        uniforms_.alpha = shader_->getUniformLocation("alpha");
        uniforms_.pointSize = shader_->getUniformLocation("pointSize");
    }

    void PointCloudRenderable::createBuffers()
    {
        // RAII: Create VAO and VBO - constructor handles glGen* calls
        vao_.emplace();
        vbo_.emplace();

        vao_->bind();
        vbo_->bind(GL_ARRAY_BUFFER);

        // Set up vertex attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // If we have data, populate the buffer
        if (pointCloud_ && !pointCloud_->empty())
        {
            vertices_.clear();
            vertices_.reserve(pointCloud_->size() * 3);
            for (const auto &pt : pointCloud_->getPoints())
            {
                vertices_.push_back(pt.x());
                vertices_.push_back(pt.y());
                vertices_.push_back(pt.z());
            }
            glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(float), vertices_.data(), GL_DYNAMIC_DRAW);
        }
        else
        {
            // Create empty buffer that can be updated later
            glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
        }

        gl::VertexArray::unbind();
    }

    void PointCloudRenderable::updateBuffers()
    {
        if (!pointCloud_ || pointCloud_->empty() || !vbo_)
        {
            return;
        }

        // Rebuild vertex array in CPU memory
        vertices_.clear();
        vertices_.reserve(pointCloud_->size() * 3);
        for (const auto &pt : pointCloud_->getPoints())
        {
            vertices_.push_back(pt.x());
            vertices_.push_back(pt.y());
            vertices_.push_back(pt.z());
        }

        vbo_->bind(GL_ARRAY_BUFFER);
        // If capacity changed, reallocate; else use sub-data for speed
        GLsizei newSize = static_cast<GLsizei>(vertices_.size() * sizeof(float));
        GLint currentSize = 0;
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &currentSize);
        if (currentSize != newSize)
        {
            glBufferData(GL_ARRAY_BUFFER, newSize, vertices_.data(), GL_DYNAMIC_DRAW);
        }
        else if (newSize > 0)
        {
            glBufferSubData(GL_ARRAY_BUFFER, 0, newSize, vertices_.data());
        }
        gl::Buffer::unbind(GL_ARRAY_BUFFER);

        dirty_ = false;
    }

    void PointCloudRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (!shader_ || !vao_ || pointCloud_->empty())
        {
            return;
        }
        if (dirty_)
        {
            updateBuffers();
        }

        // Enable just before rendering
        glEnable(GL_PROGRAM_POINT_SIZE);

        shader_->use();

        glm::mat4 model = glm::mat4(1.0F);
        glUniformMatrix4fv(uniforms_.model, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniforms_.projection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(uniforms_.color, 1, glm::value_ptr(color_));
        glUniform1f(uniforms_.alpha, alpha_);
        glUniform1f(uniforms_.pointSize, pointSize_);

        vao_->bind();
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(pointCloud_->size()));
        gl::VertexArray::unbind();

        // Enable just before rendering
        glDisable(GL_PROGRAM_POINT_SIZE);
    }

    glm::vec3 PointCloudRenderable::getCenter() const
    {
        glm::vec3 center(0, 0, 0);
        return center;
    }

    void PointCloudRenderable::updatePointCloud(std::shared_ptr<math::PointCloud> newCloud)
    {
        pointCloud_ = std::move(newCloud);
        dirty_ = true;
    }

    void PointCloudRenderable::setPointSize(float pointSize)
    {
        pointSize_ = pointSize;
    }

}
