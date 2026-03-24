#pragma once

#include <vector>
#include <map>
#include <functional>

#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

#include "../models/model.h"
#include "../models/bone.h"
#include "../models/assimp_glm_helpers.h"


namespace engine
{
	class Animation : private NonCopyable
	{
	public:
		Animation() = default;
		Animation(const std::string& animationName, const std::string& animationPath, std::shared_ptr<Model> model, float speedFactor = 1.0f);

		~Animation() = default;

		virtual std::string& getName() { return m_name; }
		virtual void setName(std::string name) { m_name = name; }

		virtual std::string& getFilepath() { return m_filepath; }
		virtual void setFilepath(const std::string& filepath) { m_filepath = filepath; }


	protected:
		std::string m_name{};
		std::string m_filepath{};
	};
}