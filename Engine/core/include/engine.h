#pragma once

#include "common_defines.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "themes/imgui_spectrum.h"

#include "misc/noncopyable.h"
#include "misc/colors.h"
#include "misc/skybox.h"

#include "app/scene_settings.h"

#include "vertex.h"
#include "shader.h"
#include "texture.h"


#include "materials/material.h"
#include "materials/blinnphong_material.h"
#include "materials/pbr_material.h"

#include "renderers/blinnphong_renderer.h"
#include "renderers/pbr_renderer.h"

#include "ecs/entity.h"
#include "ecs/component.h"
#include "ecs/transform_component.h"
#include "ecs/camera_component.h"
#include "ecs/primitive_component.h"
#include "ecs/light_component.h"
#include "ecs/model_component.h"
#include "ecs/animator_component.h"


#include "models/model.h"
#include "models/animator.h"
#include "models/animation.h"

#include "cameras/camera.h"
#include "cameras/fly_camera.h"
#include "cameras/fps_camera.h"
#include "cameras/orbit_camera.h"



#include "lights/light.h"
#include "lights/point_light.h"
#include "lights/directional_light.h"
#include "lights/spot_light.h"
#include "lights/area_light.h"


#include "ui/text.h"
#include "ui/sprite.h"

#include "primitives/primitive.h"
#include "primitives/cube.h"
#include "primitives/sphere.h"
#include "primitives/cylinder.h"
#include "primitives/cone.h"
#include "primitives/plane.h"
#include "primitives/billboard.h"




#include "managers/log_manager.h"
#include "managers/audio_manager.h"
