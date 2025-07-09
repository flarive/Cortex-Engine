#include "base_material.h"

#include "../cameras/camera.h"

namespace engine
{
	class BlinnPhongMaterial final : public Material
	{
	public:
		
		BlinnPhongMaterial(const Color& ambientColor);
		BlinnPhongMaterial(const Color& ambientColor, const Color& diffuseColor, float shininess = 0.0f);
		BlinnPhongMaterial(const Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath = "", const std::string& normalTexPath = "", const std::string& metallicTexPath = "", const std::string& roughnessTexPath = "", const std::string& aoTexPath = "", const std::string& heightTexPath = "", float shininess = 0.0f);
	};
}