#include <viewer/renderables/CarRenderable.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace viewer
{

    CarRenderable::CarRenderable(std::shared_ptr<Car> car, glm::vec3 carColor)
        : car_(std::move(car))
    {
        setColor(carColor);
    }

    CarRenderable::~CarRenderable()
    {
        // cleanup();
    }

    std::shared_ptr<Car> CarRenderable::getCar() const
    {
        return car_;
    }

    void CarRenderable::rebuildMesh() // safe public interface to trigger buffer rebuild
    {
        createBuffers();
    }

    void CarRenderable::cleanup()
    {
        if (vbo_)
        {
            glDeleteBuffers(1, &vbo_);
            vbo_ = 0;
        }
        if (vao_)
        {
            glDeleteVertexArrays(1, &vao_);
            vao_ = 0;
        }
        if (shader_)
        {
            glDeleteProgram(shader_);
            shader_ = 0;
        }

        for (auto &deviceRenderable : txRenderables_)
        {
            deviceRenderable->cleanup();
        }
        for (auto &deviceRenderable : rxRenderables_)
        {
            deviceRenderable->cleanup();
        }
        txRenderables_.clear();
        rxRenderables_.clear();
    }

    void CarRenderable::initGL()
    {
        createShader();
        createBuffers();
        // createBuffers2();

        // std::cout << "[CarRenderable] initGL done!" << std::endl;
    }

    void CarRenderable::createBuffers2()
    {
        // 🧹 Cleanup previous buffers
        if (vbo_)
            glDeleteBuffers(1, &vbo_);
        if (vao_)
            glDeleteVertexArrays(1, &vao_);

        // 📂 Load model from file

        std::string base_dir = core::ResourceLocator::getModelPath("topolino");

        std::string inputfile = base_dir + "/Topolino.obj";

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        bool ret = tinyobj::LoadObj(
            &attrib,
            &shapes,
            &materials,
            &warn,
            &err,
            inputfile.c_str(),
            base_dir.c_str(), // resolves mtl + texture paths
            true              // triangulate faces
        );

        if (!warn.empty())
            std::cerr << "tinyobjloader warning: " << warn << std::endl;
        if (!err.empty())
            std::cerr << "tinyobjloader error: " << err << std::endl;
        if (!ret)
            throw std::runtime_error("Failed to load .obj file: " + inputfile);

        std::vector<float> vertexData; // Interleaved: {x, y, z, r, g, b}

        float scale = 0.01F; // or 0.001F depending on how large the model is

        for (const auto &shape : shapes)
        {
            const auto &mesh = shape.mesh;

            size_t index_offset = 0;
            for (size_t face = 0; face < mesh.num_face_vertices.size(); ++face)
            {
                auto fv = mesh.num_face_vertices[face];
                int mat_id = (face < mesh.material_ids.size()) ? mesh.material_ids[face] : -1;

                // 🎨 Default to gray
                float r = 0.6F, g = 0.6F, b = 0.6F;
                if (mat_id >= 0 && static_cast<size_t>(mat_id) < materials.size())
                {
                    const auto &mat = materials[mat_id];
                    r = mat.diffuse[0];
                    g = mat.diffuse[1];
                    b = mat.diffuse[2];
                }

                for (unsigned int v = 0; v < fv; ++v)
                {
                    const auto &idx = mesh.indices[index_offset + v];
                    size_t vIdx = 3 * idx.vertex_index;
                    if (vIdx + 2 >= attrib.vertices.size())
                        continue;

                    float vx = attrib.vertices[vIdx + 0] * scale;
                    float vy = attrib.vertices[vIdx + 1] * scale;
                    float vz = attrib.vertices[vIdx + 2] * scale;

                    vertexData.push_back(vx);
                    vertexData.push_back(vy);
                    vertexData.push_back(vz);
                    vertexData.push_back(r);
                    vertexData.push_back(g);
                    vertexData.push_back(b);
                }

                index_offset += fv;
            }
        }

        // 📦 Save vertex count
        vertexCount_ = static_cast<GLsizei>(vertexData.size() / 6);
        std::cout << "[vertex count] " << vertexCount_ << std::endl;

        if (vertexCount_ == 0)
            throw std::runtime_error("Model contains no valid vertex data: " + inputfile);

        // 🧠 Upload to GPU
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);

        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0); // Position
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float))); // Color
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void CarRenderable::createBuffers()
    {
        if (vbo_)
            glDeleteBuffers(1, &vbo_);
        if (vao_)
            glDeleteVertexArrays(1, &vao_);

        glm::vec3 carColor_ = this->getColor();
        float r = carColor_.r;
        float g = carColor_.g;
        float b = carColor_.b;
        CarDimension dimension = car_->getDimension();
        float length = dimension.length, width = dimension.width, height = dimension.height;

        float cubeVertices[] = {
            // Front face
            -length / 2, -width / 2, -height / 2, r, g, b,
            -length / 2, -width / 2, +height / 2, r, g, b,
            +length / 2, -width / 2, +height / 2, r, g, b,
            +length / 2, -width / 2, +height / 2, r, g, b,
            +length / 2, -width / 2, -height / 2, r, g, b,
            -length / 2, -width / 2, -height / 2, r, g, b,

            // Back face
            -length / 2, width / 2, -height / 2, r, g, b,
            -length / 2, width / 2, +height / 2, r, g, b,
            +length / 2, width / 2, +height / 2, r, g, b,
            +length / 2, width / 2, +height / 2, r, g, b,
            +length / 2, width / 2, -height / 2, r, g, b,
            -length / 2, width / 2, -height / 2, r, g, b,

            // Left face
            +length / 2, +width / 2, -height / 2, r, g, b,
            +length / 2, -width / 2, -height / 2, r, g, b,
            -length / 2, -width / 2, -height / 2, r, g, b,
            -length / 2, -width / 2, -height / 2, r, g, b,
            -length / 2, +width / 2, -height / 2, r, g, b,
            +length / 2, +width / 2, -height / 2, r, g, b,

            // Right face
            +length / 2, +width / 2, height / 2, r, g, b,
            +length / 2, -width / 2, height / 2, r, g, b,
            -length / 2, -width / 2, height / 2, r, g, b,
            -length / 2, -width / 2, height / 2, r, g, b,
            -length / 2, +width / 2, height / 2, r, g, b,
            +length / 2, +width / 2, height / 2, r, g, b,

            // Top face
            length / 2, -width / 2, -height / 2, r, g, b,
            length / 2, -width / 2, +height / 2, r, g, b,
            length / 2, +width / 2, +height / 2, r, g, b,
            length / 2, +width / 2, +height / 2, r, g, b,
            length / 2, +width / 2, -height / 2, r, g, b,
            length / 2, -width / 2, -height / 2, r, g, b,

            // Bottom face
            -length / 2, -width / 2, -height / 2, r, g, b,
            -length / 2, -width / 2, +height / 2, r, g, b,
            -length / 2, +width / 2, +height / 2, r, g, b,
            -length / 2, +width / 2, +height / 2, r, g, b,
            -length / 2, +width / 2, -height / 2, r, g, b,
            -length / 2, -width / 2, -height / 2, r, g, b};

        vertexCount_ = static_cast<GLsizei>(sizeof(cubeVertices) / 6);

        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);

        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void CarRenderable::createShader()
    {
        shader_ = shader::ShaderUtils::createProgramFromFiles("car");

        // Cache uniform locations
        uniforms_.model = glGetUniformLocation(shader_, "model");
        uniforms_.view = glGetUniformLocation(shader_, "view");
        uniforms_.projection = glGetUniformLocation(shader_, "projection");
        uniforms_.useUniformColor = glGetUniformLocation(shader_, "useUniformColor");
        uniforms_.uniformColor = glGetUniformLocation(shader_, "uniformColor");
        uniforms_.alpha = glGetUniformLocation(shader_, "alpha");
    }

    void CarRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (!car_)
            return;

        // Calculate model matrix from car's transform
        glm::mat4 model = car_->getGlobalTransform().getModelMatrix();

        glUseProgram(shader_);

        glUniformMatrix4fv(uniforms_.model, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniforms_.projection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1f(uniforms_.alpha, alpha_);

        bool useUniform = false;                                // Set true for override
        glm::vec3 highlightColor = glm::vec3(1.0F, 1.0F, 0.0F); // yellow
        glUniform1i(uniforms_.useUniformColor, static_cast<GLint>(useUniform));
        glUniform3fv(uniforms_.uniformColor, 1, glm::value_ptr(highlightColor));

        glBindVertexArray(vao_);

        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexCount_));

        glBindVertexArray(0);

        // Render all devices on the car
        renderDevices(view, projection);

        glUseProgram(0);
    }

    void CarRenderable::renderDevices(const glm::mat4 &view, const glm::mat4 &projection)
    {
        for (const auto &deviceRenderable : txRenderables_)
        {
            deviceRenderable->render(view, projection);
        }

        for (const auto &deviceRenderable : rxRenderables_)
        {
            deviceRenderable->render(view, projection);
        }
    }

    glm::vec3 CarRenderable::getCenter() const
    {
        return car_->getGlobalTransform().getPosition().toGlmVec3(); // Or however you access position
    }

    void CarRenderable::setVisible(bool visible)
    {
        visible_ = visible;
    }
    bool CarRenderable::isVisible() const
    {
        return visible_;
    }

} // namespace viewer
