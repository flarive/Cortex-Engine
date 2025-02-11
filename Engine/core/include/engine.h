#pragma once

#include "common_defines.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "themes/imgui_spectrum.h"


#include "vertex.h"
#include "shader.h"
#include "materials/material.h"
#include "texture.h"
#include "model.h"

#include "camera.h"




#include "lights/point_light.h"
#include "lights/directional_light.h"
#include "lights/spot_light.h"
#include "ui/text.h"

#include "primitives/primitive.h"
#include "primitives/cube.h"
#include "primitives/sphere.h"
#include "primitives/plane.h"
#include "primitives/billboard.h"

#include "primitives/skybox.h"

namespace engine
{

	class Engine
	{
	public:
		Engine();
		virtual ~Engine() = default;

		virtual bool initEngine();

		virtual void mainLoop();

		virtual void customMainLoop() = 0;

		virtual bool desinitEngine();
	};
}





