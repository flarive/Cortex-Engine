#include "../../include/widgets/material_widget.h"

#include "../../../core/include/managers/filesystem_manager.h"
#include "../../../core/include/managers/log_manager.h"

engine::MaterialWidget::MaterialWidget() : ImGuiElement(Category::Widget, "MaterialSubComponentWidget")
{
    logger.trace("MaterialWidget constructor called");
}

void engine::MaterialWidget::init()
{
}

void engine::MaterialWidget::setMaterials(std::vector<std::shared_ptr<Material>>& materials)
{
    m_materials.clear();

    for (const auto& material : materials) {
        m_materials.push_back(material);
    }
}

void engine::MaterialWidget::draw()
{
    const std::string header = std::format("Materials ({})", m_materials.size());

    ImGui::PushFont(ImGui::Spectrum::fontSmall2);
    
    ImGui::SetNextItemOpen(m_isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingHeader(header.c_str(), ImGuiTreeNodeFlags_None, EditorHelper::im_grey_dark))
    {
        for (size_t i = 0; i < m_materials.size(); ++i)
        {
            const auto& weakMaterial = m_materials[i];

            if (auto sharedMaterial = weakMaterial.lock())
            {
                displayMaterial(sharedMaterial, i);
            }
        }
    }

    ImGui::PopFont();
}

void engine::MaterialWidget::displayMaterial(const std::shared_ptr<Material>& material, size_t index)
{
    std::string header = std::format("Unnamed material ({})", index);

    std::string materialName = material->getName();
    if (!materialName.empty())
    {
        header = materialName;
        
        if (material->getTypeID() == MaterialType::PBR)
            header = std::format("{} ({})##{}", header, "PBR", index);
        else if (material->getTypeID() == MaterialType::blinnphong)
            header = std::format("{} ({})##{}", header, "BlinnPhong", index);
    }
    else
    {
        if (material->getTypeID() == MaterialType::PBR)
            header = std::format("Unnamed material (PBR)##{}", index);
        else if (material->getTypeID() == MaterialType::blinnphong)
            header = std::format("Unnamed material (BlinnPhong)##{}", index);
    }
    
    ImGui::SetNextItemOpen(m_isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingHeader(header.c_str(), ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_Bullet, EditorHelper::im_grey_trans))
    {
        const std::string tableUniqueID = std::format("TexturesTable_{}", index);

        if (ImGui::BeginTable(tableUniqueID.c_str(), 2, ImGuiTableFlags_SizingStretchSame))
        {
            ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_WidthFixed, 34);
            ImGui::TableSetupColumn("2", ImGuiTableColumnFlags_WidthStretch);

            if (material->getTypeID() == MaterialType::PBR)
            {
                // PBR
                displayColor(material->getBaseColorFactor(), "Base Color Factor");

                displayTexture(TextureManager::getTextureData(material->getDiffuseTexPath()), "Diffuse", TextureSlot::Diffuse, material);
                displayTexture(TextureManager::getTextureData(material->getNormalTexPath()), "Normal", TextureSlot::Normal, material);
                
                if (material->hasArmMap())
                {
                    displayTexture(TextureManager::getTextureData(material->getArmTexPath()), "AO + Roughness + Metallic", TextureSlot::ARM, material);

                }
                else if (material->hasRmMap())
                {
                    displayTexture(TextureManager::getTextureData(material->getAoTexPath()), "Ambient Occlusion", TextureSlot::AO, material);
                    displayTexture(TextureManager::getTextureData(material->getRmTexPath()), "Roughness + Metallic", TextureSlot::RM, material);
                }
                else
                {
                    displayTexture(TextureManager::getTextureData(material->getAoTexPath()), "Ambient Occlusion", TextureSlot::AO, material);
                    displayTexture(TextureManager::getTextureData(material->getRoughnessTexPath()), "Roughness", TextureSlot::Roughness, material);
                    displayTexture(TextureManager::getTextureData(material->getMetallicTexPath()), "Metallic", TextureSlot::Metallic, material);
                }

                displayTexture(TextureManager::getTextureData(material->getHeightTexPath()), "Height", TextureSlot::Height, material);
                displayTexture(TextureManager::getTextureData(material->getEmissiveTexPath()), "Emissive", TextureSlot::Emissive, material);
                displayTexture(TextureManager::getTextureData(material->getOpacityTexPath()), "Opacity", TextureSlot::Opacity, material);
            }
            else
            {
                // BlinnPhong or Phong
                displayColor(material->getAmbientColor(), "Ambient color");
                displayColor(material->getDiffuseColor(), "Diffuse color");
                displayColor(material->getSpecularColor(), "Specular color");

                displayTexture(TextureManager::getTextureData(material->getDiffuseTexPath()), "Diffuse", TextureSlot::Diffuse, material);
                displayTexture(TextureManager::getTextureData(material->getSpecularTexPath()), "Specular", TextureSlot::Specular, material);
                displayTexture(TextureManager::getTextureData(material->getNormalTexPath()), "Normal", TextureSlot::Normal, material);
                displayTexture(TextureManager::getTextureData(material->getHeightTexPath()), "Height", TextureSlot::Height, material);
                displayTexture(TextureManager::getTextureData(material->getEmissiveTexPath()), "Emissive", TextureSlot::Emissive, material);
                displayTexture(TextureManager::getTextureData(material->getOpacityTexPath()), "Opacity", TextureSlot::Opacity, material);
            }

            ImGui::EndTable();
        }
    }
}

void engine::MaterialWidget::displayTextureControl(TextureSlot slot, const std::shared_ptr<Material>& material)
{
    switch (slot)
    {
        //uniform float uNormalStrength;     // 0..2
        //uniform float uRoughnessScale;     // 0..2
        //uniform float uRoughnessBias;      // -1..1
        //uniform float uMetallicScale;      // 0..2
        //uniform float uAOStrength;         // 0..1
        //uniform float uHeightScale;        // parallax
        //uniform float uEmissiveIntensity;  // 0..∞

        //float roughness = texture(uRoughnessMap, uv).r;
        //roughness = clamp(
        //    roughness * uRoughnessScale + uRoughnessBias,
        //    0.04, 1.0
        //);

        //float metallic = clamp(
        //    texture(uMetallicMap, uv).r * uMetallicScale,
        //    0.0, 1.0
        //);

        //float ao = mix(
        //    1.0,
        //    texture(uAOMap, uv).r,
        //    uAOStrength
        //);

        case TextureSlot::Normal:
        {
            if (material->hasNormalMap())
            {
                float& value = material->getNormalIntensity();
                if (EditorHelper::renderSliderFloat("##NormalIntensity", value, value, 0.0f, 10.0f, 80.0f, "%.1f")) {
                    emit(UIEventType::MaterialPropertyChanged, "material_normal_intensity", value);
                }
            }
            break;
        }
        case TextureSlot::Height:
        {
            if (material->hasHeightMap())
            {
                float& value = material->getHeightIntensity();
                if (EditorHelper::renderSliderFloat("##HeightIntensity", value, value, 0.0f, 10.0f, 80.0f, "%.1f")) {
                    emit(UIEventType::MaterialPropertyChanged, "material_height_intensity", value);
                }
            }
            break;
        }
        case TextureSlot::Emissive:
        {
            if (material->hasEmissiveMap())
            {
                float& value = material->getEmissiveIntensity();
                if (EditorHelper::renderSliderFloat("##EmissiveIntensity", value, value, 0.0f, 100.0f, 80.0f, "%.1f")) {
                    emit(UIEventType::MaterialPropertyChanged, "material_emissive_intensity", value);
                }
            }
            break;
        }

        default:
            break;
    }
}

void engine::MaterialWidget::displayColor(const Color& color, const std::string& textType)
{
    ImGui::TableNextRow(ImGuiTableRowFlags_None, TARGET_THUMB_SIZE + 4);

    ImGui::TableSetColumnIndex(0);
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size(TARGET_THUMB_SIZE, TARGET_THUMB_SIZE);
    ImVec2 p0 = pos;
    ImVec2 p1 = ImVec2(pos.x + size.x, pos.y + size.y);
    ImDrawList* draw = ImGui::GetWindowDrawList();
    draw->AddRectFilled(p0, p1, IM_COL32(color.r * 255, color.g * 255, color.b * 255, 255));

    ImGui::TableSetColumnIndex(1);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0)); // remove text padding

    ImGui::PushFont(ImGui::Spectrum::fontSmall2);

    if (ImGui::BeginTable("##table", 2, ImGuiTableFlags_None))
    {
        // Remove table cell padding
        ImGui::TableSetupColumn("col1", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("col2", ImGuiTableColumnFlags_WidthFixed, 50);

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));

        float rowHeight = 12.0f;

        // --- Row 1 ---
        ImGui::TableNextRow(ImGuiTableRowFlags_None, rowHeight);
        {
            ImGui::TableNextColumn();
            {
                ImGui::Text(textType.c_str());
            }

            ImGui::TableNextColumn();
            {

            }
        }

        // --- Row 2 ---
        ImGui::TableNextRow(ImGuiTableRowFlags_None, rowHeight);
        {
            ImGui::TableNextColumn();
            {

            }

            ImGui::TableNextColumn();
            {

            }
        }

        ImGui::PopStyleVar(); // CellPadding
        ImGui::EndTable();
    }

    ImGui::PopFont();
    ImGui::PopStyleVar(2); // ItemSpacing + FramePadding
}

void engine::MaterialWidget::displayTexture(const TextureData* textData, const std::string& label, TextureSlot slot, const std::shared_ptr<Material>& material)
{
	if (!textData || textData->filePath.empty())
	{
		return; // Skip if the texture path is empty
	}
    
    ImGui::TableNextRow();


    // get thumbnail from mipmaps on GPU side
    ImGui::TableSetColumnIndex(0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, textData->thumbnailLevel);
    ImGui::Image((ImTextureID)textData->id, ImVec2(TARGET_THUMB_SIZE, TARGET_THUMB_SIZE));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);


    ImGui::TableSetColumnIndex(1);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0)); // remove text padding

    ImGui::PushFont(ImGui::Spectrum::fontSmall2);

    if (ImGui::BeginTable("##table", 2, ImGuiTableFlags_None))
    {
        // Remove table cell padding
        ImGui::TableSetupColumn("col1", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("col2", ImGuiTableColumnFlags_WidthFixed, 104);

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));

        // --- Row 1 ---
        ImGui::TableNextRow(ImGuiTableRowFlags_None, ROW_HEIGHT);
        {
            ImGui::TableNextColumn();
            {
                ImGui::Text("%s", label.c_str());
            }

            ImGui::TableNextColumn();
            {
                std::string resolution = std::format("ID:{} {}x{}", textData->id, textData->width, textData->height);
				//auto tagColors = getImageSizeTagColor(textData->width, textData->height);
                //EditorHelper::drawTagRightAligned(resolution.c_str(), tagColors.bg, tagColors.fg, ROW_HEIGHT);
                //ImGui::Text("%s", resolution.c_str());
                EditorHelper::drawTextRightAlign(resolution.c_str(), GREY_TEXT_COLOR);
            }
        }

        // --- Row 2 ---
        ImGui::TableNextRow(ImGuiTableRowFlags_None, ROW_HEIGHT);
        {
            ImGui::TableNextColumn();
            {
                ImGui::PushStyleColor(ImGuiCol_Text, GREY_TEXT_COLOR);
                ImGui::Text(FileSystemManager::getFileName(textData->filePath).c_str());
                ImGui::PopStyleColor();
            }

            ImGui::TableNextColumn();
            {
                //auto tagColors = getImageSizeTagColor(textData->width, textData->height);
                //EditorHelper::drawTagRightAligned(resolution.c_str(), tagColors.bg, tagColors.fg, ROW_HEIGHT);
                //ImGui::Text("%s", openGLTexID.c_str());
                
                //std::string openGLTexID = std::format("ID {}", textData->id);
                //EditorHelper::drawTextRightAlign(openGLTexID.c_str(), GREY_TEXT_COLOR);

                displayTextureControl(slot, material);
            }
        }

        ImGui::PopStyleVar(); // CellPadding
        ImGui::EndTable();
    }

    ImGui::PopFont();
    ImGui::PopStyleVar(2); // ItemSpacing + FramePadding
}

engine::TagColors engine::MaterialWidget::getImageSizeTagColor(int width, int height)
{
    engine::TagColors tagColors;
    tagColors.bg = IM_COL32(0, 0, 0, 0);
    tagColors.fg = IM_COL32(255, 255, 255, 255);

	if (width >= 2048 || height >= 2048)
	{
        tagColors.bg = IM_COL32(255, 0, 255, 255); // Purple for large textures
	}
	else if (width >= 1024 || height >= 1024)
	{
        tagColors.bg = IM_COL32(0, 0, 255, 255); // Blue for medium textures
	}
	else
	{
        tagColors.bg = IM_COL32(255, 165, 0, 255); // Orange for small textures
	}

    return tagColors;
}

engine::MaterialWidget::~MaterialWidget()
{
    logger.trace("MaterialWidget destructor called");
}