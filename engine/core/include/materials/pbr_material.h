#include "base_material.h"

#include "../cameras/camera.h"

namespace engine
{
	class PBRMaterial final : public Material
	{
	public:

		PBRMaterial(const Color& ambientColor);
		PBRMaterial(const Color& ambientColor, const Color& diffuseColor, float shininess = 0.0f);
		PBRMaterial(const Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath = "", const std::string& normalTexPath = "", const std::string& metallicTexPath = "", const std::string& roughnessTexPath = "", const std::string& aoTexPath = "", const std::string& heightTexPath = "", float shininess = 0.0f);
	};
}