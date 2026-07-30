#pragma once

#include "../common_defines.h"
#include "../ecs/entity.h"
#include "../ecs/component.h"
#include "../cameras/camera.h"
#include "../editor/icon_atlas.h"



#include <imgui.h>

#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include <functional>

namespace engine
{
	using SceneSetting = std::variant<bool, int, uint, ubyte, float>;

	enum class EditorIcon { undefined = 0, 
		// row 0 (16x16)
		entity_model_16x16 = 1, entity_primitive_16x16 = 2, entity_light_16x16 = 3, entity_camera_16x16 = 4, entity_particleSystem_16x16 = 5, entity_terrain_16x16 = 6,
		locked = 26, unlocked = 27, show = 28, hide = 29,
		
		// row 1 (48x48)
		entity_model_48x48 = 30, entity_primitive_48x48 = 31, entity_light_48x48 = 32, entity_camera_48x48 = 33, entity_particleSystem_48x48 = 34, entity_terrain_48x48 = 35,
		editor_translate = 37, editor_scale = 38, editor_rotate = 39,
		
		// dynamic counter
		COUNT };
	

	class EditorHelper final
	{
	public:
		static void renderStringVectorButtonListTable(const std::string& key, const std::vector<std::string>& items, const EditorProperty& property);
		static void renderStringVectorComboboxTable(const std::string& key, const std::vector<std::string>& items, const EditorProperty& property);

		static void renderDynamicProperties(std::shared_ptr<Component> component, const std::string& componentType);

		static bool drawCustomDragFloat(const char* text, const char* name, const ImVec2& position, const ImVec2& size, float rounding, float width, ImU32 backgroundColor, ImU32 foregroundColor, float* value, float step);
		static void drawCustomLabel(const char* text, const ImVec2& position, const ImVec2& size, float rounding, ImU32 backgroundColor, ImU32 foregroundColor);

		static bool renderSliderIntWithLabel(const char* label, int& value, int& lastValue, int min, int max);
		static bool renderSliderUnsignedByteWithLabel(const char* label, ubyte& value, ubyte& lastValue, ubyte min, ubyte max);
		static bool renderSliderFloatWithLabel(const char* label, float& value, float& lastValue, float min, float max, const char* format);
		
		static bool renderDragFloatWithLabel(const char* label, float& value, float& lastValue, float min, float max, float step, const char* format);
		static bool renderDragUnsignedByteWithLabel(const char* label, ubyte& value, ubyte& lastValue, ubyte min, ubyte max, float step);

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
		inline static const float FIELD_WIDTH = 120.0f;

		static const engine::IconUV& getIcon(const EditorIcon& icon);
		
		static void registerIconAtlas();
		static GLuint getIconAtlasTexture();
		static const engine::IconUV& getEntityTypeSmallIcon(const engine::EditorIcon icon); // 16x16 icons
		static const engine::IconUV& getEntityTypeMediumIcon(const engine::EditorIcon icon); // 48x48 icons
		

		static void addToolbarIconButton(const std::string& iconName, const EditorIcon& icon, std::function<void()> onClick);
		static void addDiscreetIconButton(bool& state, const std::string& icon_name, const EditorIcon& icon_off, const EditorIcon& icon_on, std::function<void()> onClick);

		static void setIconToggleState(const std::string& key, bool state);

		static void resetIconToggleStates();

		static bool beginCenteredToolbar(
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

		static void endCenteredToolbar();

		static bool collapsingHeader(const char* label, ImGuiTreeNodeFlags flags);
		static bool collapsingCheckboxHeader(const char* label, bool* p_checked, ImGuiTreeNodeFlags flags, std::function<void(bool)> onCheck);

		static ImVec4 getEntityColor(const engine::EntityType entityType);

		static engine::EditorIcon convertEntityTypeToAtlasIcon(const engine::EntityType type, unsigned int Iconsize);

	private:
		static std::unordered_map<std::string, bool> m_iconToggleStates;

		static engine::IconAtlas m_iconAtlas;
	};
}