#pragma once

#include "imgui_ui_manager.h"

#include "../ecs/entity.h"
#include "../app/scene_settings.h"

#include "../lights/light.h"

#include "../misc/event.h"

#include "editor_helper.h"
#include "icon_atlas.h"

#include <variant>

#include <imgui.h>

#if EDITOR_MODE
namespace engine
{
	class ImGuiEditor final
	{
	public:
		ImGuiEditor() = default;
		~ImGuiEditor() = default;

		void init();

		void setScene(std::shared_ptr<Entity> rootEntity);

		void initEditor();

		void renderEditor(bool show, glm::mat4& projection, glm::mat4& view, const bool displayObjectTransformGuizmo);

		// Let parent register a callback
		void setOnSelectionChanged(std::function<void(std::shared_ptr<Entity>)> callback) {
			m_onSelectionChanged = std::move(callback);
		}

		// Let parent register a callback
		void setOnSceneSettingChanged(std::function<void(const std::string&, SceneSetting)> callback) {
			m_onSceneSettingChanged = std::move(callback);
		}


		void initRenderGuizmo(const std::shared_ptr<Camera> camera);

		
		

		void renderGuizmo(const ImGuiID& dockspace_id, glm::mat4& projection, glm::mat4& view, const bool displayObjectTransformGuizmo);

		void editTransform(const float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition, std::shared_ptr<Entity> entity);

		void renderViewGuizmo(glm::mat4& projection, glm::mat4& view, bool displayViewTransformGuizmo);


		


	private:
		std::shared_ptr<Entity> m_rootEntity{};

		std::shared_ptr<Entity> m_selectedEntity{};

		std::function<void(std::shared_ptr<Entity>)> m_onSelectionChanged; // << callback

		std::function<void(std::string, SceneSetting)> m_onSceneSettingChanged; // << callback

		




		template<typename T>
		bool any_is(const std::any& a)
		{
			return a.type() == typeid(T);
		}
	
		

		void onEditorUIEvent(const UIEvent& evt);

		

	protected:
		// guizmo
		float viewWidth{ 10.0f }; // for orthographic
		const float camYAngle{ 165.f / 180.f * 3.14159f };
		const float camXAngle{ 32.f / 180.f * 3.14159f };
		float camDistance{};
		int gizmoCount{ 1 };

		bool firstFrame{ true };
		int lastUsing{};


		ImGuiUIManager m_ui;

		std::shared_ptr<Camera> m_guizmoCamera{};
	};
}
#endif