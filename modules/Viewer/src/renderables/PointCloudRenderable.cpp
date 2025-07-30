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

    PointCloudRenderable::~PointCloudRenderable()
    {
        // cleanup();
    }

    void PointCloudRenderable::cleanup()
    {
        if (vao_ != 0)
        {
            glDeleteVertexArrays(1, &vao_);
            vao_ = 0;
        }

        if (vbo_ != 0)
        {
            glDeleteBuffers(1, &vbo_);
            vbo_ = 0;
        }

        if (shader_ != 0)
        {
            glDeleteProgram(shader_);
            shader_ = 0;
        }
    }

    void PointCloudRenderable::initGL()
    {
        createShader();
        createBuffers();
    }

    void PointCloudRenderable::createShader()
    {
        shader_ = shader::ShaderUtils::createProgramFromFiles("point_cloud");

        uniforms_.model = glGetUniformLocation(shader_, "model");
        uniforms_.view = glGetUniformLocation(shader_, "view");
        uniforms_.projection = glGetUniformLocation(shader_, "projection");
        uniforms_.color = glGetUniformLocation(shader_, "uniformColor");
        uniforms_.alpha = glGetUniformLocation(shader_, "alpha");
        uniforms_.pointSize = glGetUniformLocation(shader_, "pointSize");
    }

    void PointCloudRenderable::createBuffers()
    {
        if (!pointCloud_ || pointCloud_->empty())
            return;

        vertices_.clear();
        vertices_.reserve(pointCloud_->size() * 3);
        for (const auto &pt : pointCloud_->getPoints())
        {
            vertices_.push_back(pt.x());
            vertices_.push_back(pt.y());
            vertices_.push_back(pt.z());
        }

        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);

        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(float), vertices_.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void PointCloudRenderable::updateBuffers()
    {
        if (!pointCloud_ || pointCloud_->empty() || vbo_ == 0)
        {
            return;
        }

        vertices_.clear();
        vertices_.reserve(pointCloud_->size() * 3);
        for (const auto &pt : pointCloud_->getPoints())
        {
            vertices_.push_back(pt.x());
            vertices_.push_back(pt.y());
            vertices_.push_back(pt.z());
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_.size() * sizeof(float), vertices_.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        dirty_ = false;
    }

    void PointCloudRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
    {

        if (pointCloud_->empty())
        {
            return;
        }
        if (dirty_)
        {
            if (vao_ == 0 || vbo_ == 0)
            {
                createBuffers();
            }
            else
            {
                LOGGER_DEBUG("PointCloudRenderable::render: Updating buffers for point cloud.");
                updateBuffers();
            }
        }

        // Enable just before rendering
        glEnable(GL_PROGRAM_POINT_SIZE);

        glUseProgram(shader_);

        glm::mat4 model = glm::mat4(1.0F);
        glUniformMatrix4fv(uniforms_.model, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniforms_.projection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(uniforms_.color, 1, glm::value_ptr(color_));
        glUniform1f(uniforms_.alpha, alpha_);
        glUniform1f(uniforms_.pointSize, pointSize_);

        glBindVertexArray(vao_);
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(pointCloud_->size()));
        glBindVertexArray(0);

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
