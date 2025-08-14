#pragma once

#include "common_defines.h"

namespace engine
{
	class Transform
	{
	protected:
		//Local space information
		glm::vec3 m_pos{};
		glm::vec3 m_eulerRot{}; //In degrees
		glm::vec3 m_scale{ 1.0f };

		//Global space information concatenate in matrix
		//	m_modelMatrix = | Right.x   Up.x   Forward.x   Position.x |
		//					| Right.y   Up.y   Forward.y   Position.y |
		//					| Right.z   Up.z   Forward.z   Position.z |
		//					|   0        0        0           1 |
		//  Column 0 : Local Right axis(X - direction after rotation & scale)
		//	Column 1 : Local Up axis(Y - direction after rotation & scale)
		//	Column 2 : Local Forward axis(Z - direction after rotation & scale) — note : some engines use Backward as default forward depending on handedness
		//	Column 3 : World position of the object
		glm::mat4 m_modelMatrix = glm::mat4(1.0f);


		// Global space information
		glm::vec3 m_globalPosition{};
		

		//Dirty flag
		bool m_isDirty = true;

		glm::mat4 getLocalModelMatrix();

	public:

		Transform() = default;
		~Transform() = default;

		Transform(glm::vec3 _position, glm::vec3 _scale = glm::vec3(1.0f), glm::vec3 _rotation = glm::vec3(0.0f));

		void computeModelMatrix(const std::string& name);
		void computeModelMatrix(const std::string& name, const glm::mat4& parentGlobalModelMatrix);
		void setLocalPosition(const glm::vec3& newPosition);
		void setLocalRotation(const glm::vec3& newRotation);
		void setLocalScale(const glm::vec3& newScale);
		const glm::vec3& getGlobalPosition() const;
		glm::vec3& getLocalPosition();
		glm::vec3& getLocalRotation();
		glm::vec3& getLocalScale();
		const glm::mat4& getModelMatrix() const;
		glm::vec3 getRight() const;
		glm::vec3 getUp() const;
		glm::vec3 getBackward() const;
		glm::vec3 getForward() const;
		glm::vec3 getGlobalScale() const;
		bool isDirty() const;
		void setDirty();
		void clearDirty();
	};
}