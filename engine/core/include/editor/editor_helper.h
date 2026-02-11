#pragma once

#include "../common_defines.h"
#include "../ecs/entity.h"
#include "../ecs/component.h"
#include "../cameras/camera.h"

#include "../ecs/entity.h"

#include <imgui.h>

#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include <functional>

namespace engine
{
	using SceneSetting = std::variant<bool, int, unsigned int, float>;

	class EditorHelper final
	{
	public:
		static void renderStringVectorButtonListTable(const std::vector<std::string>& items, const EditorProperty& property);
		static void renderStringVectorComboboxTable(const std::vector<std::string>& items, const EditorProperty& property);

		static void renderDynamicProperties(std::shared_ptr<Component> component, const std::string& componentType);

		static bool drawCustomDragFloat(const char* text, const char* name, const ImVec2& position, const ImVec2& size, float rounding, float width, ImU32 backgroundColor, ImU32 foregroundColor, float* value, float step);
		static void drawCustomLabel(const char* text, const ImVec2& position, const ImVec2& size, float rounding, ImU32 backgroundColor, ImU32 foregroundColor);

		static void renderSliderIntWithLabel(const char* label, const char* key, int& value, int& lastValue, int min, int max, std::function<void(std::string, SceneSetting)> sceneSettingChanged);
		static void renderSliderFloatWithLabel(const char* label, const char* key, float& value, float& lastValue, float min, float max, const char* format, std::function<void(std::string, SceneSetting)> sceneSettingChanged);
		static void renderDragFloatWithLabel(const char* label, const char* key, float& value, float& lastValue, float min, float max, float step, const char* format, std::function<void(std::string, SceneSetting)> sceneSettingChanged);

		inline static const ImVec4 im_white{ 0.882f, 0.882f, 0.882f, 1.0f };
		inline static const ImVec4 im_gray{ 0.502f, 0.502f, 0.502f, 1.0f };
		inline static const ImVec4 im_dark{ 0.0f, 0.0f, 0.0f, 0.2f };
		inline static const ImVec4 im_light{ 1.0f, 1.0f, 1.0f, 0.2f };

		inline static auto green = IM_COL32(0, 151, 51, 255);
		inline static auto blue = IM_COL32(44, 143, 255, 255);
		inline static auto red = IM_COL32(255, 54, 83, 255);
		inline static auto white = IM_COL32(255, 255, 255, 255);

		inline static const float ROUNDING = 3.0f;
		inline static const ImVec2 SIZE{ 21.0f, 21.0f };
		inline static const float ITEM_LABEL_WIDTH{ 100.0f }; // pixels

		static GLuint getIcon(const std::string& key);
		static GLuint getEntityTypeSmallIcon(const EntityType entityType);
		static GLuint getEntityTypeMediumIcon(const EntityType entityType);

		static void addToolbarIconButton(const std::string& icon, std::function<void()> onClick);
		static void addDiscreetIconButton(bool& state, const std::string& icon_off, const std::string& icon_on, std::function<void()> onClick);

		static GLuint getIconTexture(const std::string& key, const std::string& prefix, const std::string& folder);
		static void setIconToggleState(const std::string& key, bool state);

		static void resetIconToggleStates();

		static bool BeginCenteredToolbar(
			int   iconCount,
			float iconSize,
			float iconSpacing = -1.0f,              // <0 => use style.ItemSpacing.x
			float bgPaddingX = 8.0f,
			float bgPaddingY = 6.0f,
			float bgRounding = 10.0f,
			ImU32 bgColor = IM_COL32(37, 37, 37, 255),
			ImVec2* outBgMin = nullptr,
			ImVec2* outBgMax = nullptr,
			ImVec2* outStartCursorLocal = nullptr);

		static void EndCenteredToolbar();

		static bool collapsingCheckboxHeader(const char* label, bool* p_checked, ImGuiTreeNodeFlags flags, std::function<void(bool)> onCheck);

	private:
		static std::unordered_map<std::string, GLuint> m_iconTextureCache;
		static std::unordered_map<std::string, bool> m_iconToggleStates;

		static std::unordered_map<std::string, GLuint> m_iconActionTextureCache;

		static std::unordered_map<EntityType, GLuint> m_iconSmallTextureCache;
		static std::unordered_map<EntityType, GLuint> m_iconMediumTextureCache;

		
	};
}