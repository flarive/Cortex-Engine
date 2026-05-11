#pragma once

#include "../misc/noncopyable.h"
#include "mesh.h"
#include "../shader.h"
#include "../transform.h"
#include "shared_model.h"

#include <glm/glm.hpp>

#include <assimp/importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include <string>
#include <vector>
#include <mutex>

namespace engine
{
    class Model : public SharedModel
    {
    public:
        Model() = default;
		~Model();

        // constructor, expects a filepath to a 3D model.
        Model(const std::string& _path, bool _gamma = false, bool _flipUVs = false, const glm::vec3& _position = glm::vec3());

        Model(const std::string& _path, const std::shared_ptr<Material>& _material, bool _gamma = false, bool _flipUVs = false, const glm::vec3& _position = glm::vec3());

        // constructor, expects a shared model to be displayed multiple times and loaded only one time.
        Model(const std::shared_ptr<SharedModel>& _shared_model, bool _gamma = false, bool _flipUVs = false, const glm::vec3& _position = glm::vec3());

        ModelType getTypeID() const
        {
            return ModelType::model;
        }




        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& transformMatrix, Transform& localTransform);



        void clean();

        unsigned int getMeshCount() const;

        glm::vec3& getPosition() { return m_position; }
        glm::vec3& getRotation() { return m_rotation; }
        glm::vec3& getScale() { return m_scale; }

        void setPosition(const glm::vec3& position) { m_position = position; }
        void setRotation(const glm::vec3& rotation) { m_rotation = rotation; }
        void setScale(const glm::vec3& scale) { m_scale = scale; }

        void setTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
            m_position = position;
            m_rotation = rotation;
            m_scale = scale;
        }

		bool isEnabled() const { return m_isEnabled; }
		void setEnabled(bool enabled) { m_isEnabled = enabled; }

    private:

        glm::vec3 m_position{};
        glm::vec3 m_rotation{};
        glm::vec3 m_scale{};

		bool m_isEnabled{ true };

        // for shared model only (loaded one time, drawn multiple times)
        std::shared_ptr<SharedModel> m_shared_model{};
    };
}
