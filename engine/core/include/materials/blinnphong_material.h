#include "material.h"

#include "../cameras/camera.h"

namespace engine
{
	class BlinnPhongMaterial final : public Material
	{
	public:
		
		BlinnPhongMaterial(const Color& ambientColor);
		BlinnPhongMaterial(const Color& ambientColor, const Color& diffuseColor, const Color& specularColor, float shininess = 1.0f);
		BlinnPhongMaterial(const Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath = "", const std::string& normalTexPath = "", const std::string& heightTexPath = "", float shininess = 1.0f);

		MaterialType getTypeID() const override
		{
			return MaterialType::blinnphong;
		}
	};
}