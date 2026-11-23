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

void engine::MaterialWidget::draw(glm::mat4& projection, glm::mat4& view)
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
                std::shared_ptr<PBRMaterial> pbrMaterial = std::static_pointer_cast<PBRMaterial>(material);
                if (pbrMaterial)
                {
                    displayColor(pbrMaterial->getBaseColorFactor(), "Base Color Factor");
                    displayColor(pbrMaterial->getEmissiveColorFactor(), "Emissive Color Factor");

                    displayPBRTexture(TextureManager::getTextureData(pbrMaterial->getDiffuseTexPath()), "Diffuse", TextureSlot::Diffuse, pbrMaterial);
                    displayPBRTexture(TextureManager::getTextureData(pbrMaterial->getNormalTexPath()), "Normal", TextureSlot::Normal, pbrMaterial);

                    if (pbrMaterial->hasArmMap())
                    {
                        displayPBRTexture(TextureManager::getTextureData(pbrMaterial->getArmTexPath()), "AO + Roughness + Metallic", TextureSlot::ARM, pbrMaterial);

                    }
                    else if (pbrMaterial->hasRmMap())
                    {
                        displayPBRTexture(TextureManager::getTextureData(pbrMaterial->getAoTexPath()), "Ambient Occlusion", TextureSlot::AO, pbrMaterial);
                        displayPBRTexture(TextureManager::getTextureData(pbrMaterial->getRmTexPath()), "Roughness + Metallic", TextureSlot::RM, pbrMaterial);
                    }
                    else
                    {
                        displayPBRTexture(TextureManager::getTextureData(pbrMaterial->getAoTexPath()), "Ambient Occlusion", TextureSlot::AO, pbrMaterial);
                        displayPBRTexture(TextureManager::getTextureData(pbrMaterial->getRoughnessTexPath()), "Roughness", TextureSlot::Roughness, pbrMaterial);
                        displayPBRTexture(TextureManager::getTextureData(pbrMaterial->getMetallicTexPath()), "Metallic", TextureSlot::Metallic, pbrMaterial);
                    }

                    displayPBRTexture(TextureManager::getTextureData(pbrMaterial->getHeightTexPath()), "Height", TextureSlot::Height, pbrMaterial);
                    displayPBRTexture(TextureManager::getTextureData(pbrMaterial->getEmissiveTexPath()), "Emissive", TextureSlot::Emissive, pbrMaterial);
                    displayPBRTexture(TextureManager::getTextureData(pbrMaterial->getOpacityTexPath()), "Opacity", TextureSlot::Opacity, pbrMaterial);
                }
            }
            else
            {
                std::shared_ptr<BlinnPhongMaterial> blinnPhongMaterial = std::static_pointer_cast<BlinnPhongMaterial>(material);
                if (blinnPhongMaterial)
                {
                    // BlinnPhong or Phong
                    //displayColor(blinnPhongMaterial->getAmbientColor(), "Ambient color");
                    displayColor(blinnPhongMaterial->getDiffuseColor(), "Diffuse color");
                    displayColor(blinnPhongMaterial->getSpecularColor(), "Specular color");

                    displayBlinnPhongTexture(TextureManager::getTextureData(blinnPhongMaterial->getDiffuseTexPath()), "Diffuse", TextureSlot::Diffuse, blinnPhongMaterial);
                    displayBlinnPhongTexture(TextureManager::getTextureData(blinnPhongMaterial->getSpecularTexPath()), "Specular", TextureSlot::Specular, blinnPhongMaterial);
                    displayBlinnPhongTexture(TextureManager::getTextureData(blinnPhongMaterial->getNormalTexPath()), "Normal", TextureSlot::Normal, blinnPhongMaterial);
                    displayBlinnPhongTexture(TextureManager::getTextureData(blinnPhongMaterial->getHeightTexPath()), "Height", TextureSlot::Height, blinnPhongMaterial);
                    displayBlinnPhongTexture(TextureManager::getTextureData(blinnPhongMaterial->getEmissiveTexPath()), "Emissive", TextureSlot::Emissive, blinnPhongMaterial);
                    displayBlinnPhongTexture(TextureManager::getTextureData(blinnPhongMaterial->getOpacityTexPath()), "Opacity", TextureSlot::Opacity, blinnPhongMaterial);
                }
            }

            ImGui::EndTable();
        }
    }
}

void engine::MaterialWidget::displayBlinnPhongTextureControl(TextureSlot slot, const std::shared_ptr<BlinnPhongMaterial>& material)
{
    switch (slot)
    {
         case TextureSlot::Specular:
        {
            if (material->hasSpecularMap())
            {
                float& intensity = material->getSpecularIntensity();
                if (EditorHelper::renderSliderFloat("##SpecularIntensity", intensity, intensity, 0.0f, 10.0f, "%.1f", 0.0f, 80.0f)) {
                    emit(UIEventType::MaterialPropertyChanged, "material_specular_intensity", intensity);
                }
            }
            break;
        }
        case TextureSlot::Normal:
        {
            if (material->hasNormalMap())
            {
                float& intensity = material->getNormalIntensity();
                if (EditorHelper::renderSliderFloat("##NormalIntensity", intensity, intensity, 0.0f, 10.0f, "%.1f", 0.0f, 80.0f)) {
                    emit(UIEventType::MaterialPropertyChanged, "material_normal_intensity", intensity);
                }
            }
            break;
        }
        case TextureSlot::Height:
        {
            if (material->hasHeightMap())
            {
                float& intensity = material->getHeightIntensity();
                if (EditorHelper::renderSliderFloat("##HeightIntensity", intensity, intensity, 0.0f, 10.0f, "%.1f", 0.0f, 80.0f)) {
                    emit(UIEventType::MaterialPropertyChanged, "material_height_intensity", intensity);
                }
            }
            break;
        }
        case TextureSlot::Emissive:
        {
            if (material->hasEmissiveMap())
            {
                float& intensity = material->getEmissiveStrength();
                if (EditorHelper::renderSliderFloat("##EmissiveStrength", intensity, intensity, 0.0f, 100.0f, "%.1f", 0.0f, 80.0f)) {
                    emit(UIEventType::MaterialPropertyChanged, "material_emissive_strength", intensity);
                }
            }
            break;
        }

        default:
            break;
    }
}

void engine::MaterialWidget::displayPBRTextureControl(TextureSlot slot, const std::shared_ptr<PBRMaterial>& material)
{
    switch (slot)
    {
    case TextureSlot::Normal:
    {
        if (material->hasNormalMap())
        {
            float& intensity = material->getNormalIntensity();
            if (EditorHelper::renderSliderFloat("##NormalIntensity", intensity, intensity, 0.0f, 10.0f, "%.1f", 0.0f, 80.0f)) {
                emit(UIEventType::MaterialPropertyChanged, "material_normal_intensity", intensity);
            }
        }
        break;
    }
    case TextureSlot::Roughness:
    {
        if (material->hasRoughnessMap())
        {
            float& intensity = material->getRougnessIntensity();
            if (EditorHelper::renderSliderFloat("##RoughnesslIntensity", intensity, intensity, 0.0f, 1.0f, "%.1f", 0.0f, 80.0f)) {
                emit(UIEventType::MaterialPropertyChanged, "material_roughness_intensity", intensity);
            }
        }
        break;
    }
    case TextureSlot::Metallic:
    {
        if (material->hasMetallicMap())
        {
            float& intensity = material->getMetallicIntensity();
            if (EditorHelper::renderSliderFloat("##MetallicIntensity", intensity, intensity, 0.0f, 1.0f, "%.1f", 0.0f, 80.0f)) {
                emit(UIEventType::MaterialPropertyChanged, "material_metallic_intensity", intensity);
            }
        }
        break;
    }
    case TextureSlot::AO:
    {
        if (material->hasAoMap())
        {
            float& intensity = material->getAmbientOcclusionIntensity();
            if (EditorHelper::renderSliderFloat("##AmbientOcclusionIntensity", intensity, intensity, 0.0f, 1.0f, "%.1f", 0.0f, 80.0f)) {
                emit(UIEventType::MaterialPropertyChanged, "material_ao_intensity", intensity);
            }
        }
        break;
    }
    case TextureSlot::Height:
    {
        if (material->hasHeightMap())
        {
            float& intensity = material->getHeightIntensity();
            if (EditorHelper::renderSliderFloat("##HeightIntensity", intensity, intensity, 0.0f, 10.0f, "%.1f", 0.0f, 80.0f)) {
                emit(UIEventType::MaterialPropertyChanged, "material_height_intensity", intensity);
            }
        }
        break;
    }
    case TextureSlot::Emissive:
    {
        if (material->hasEmissiveMap())
        {
            float& intensity = material->getEmissiveStrength();
            if (EditorHelper::renderSliderFloat("##EmissiveStrength", intensity, intensity, 0.0f, 100.0f, "%.1f", 0.0f, 80.0f)) {
                emit(UIEventType::MaterialPropertyChanged, "material_emissive_strength", intensity);
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

void engine::MaterialWidget::displayBlinnPhongTexture(const TextureData* textData, const std::string& label, TextureSlot slot, const std::shared_ptr<BlinnPhongMaterial>& material)
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

                displayBlinnPhongTextureControl(slot, material);
            }
        }

        ImGui::PopStyleVar(); // CellPadding
        ImGui::EndTable();
    }

    ImGui::PopFont();
    ImGui::PopStyleVar(2); // ItemSpacing + FramePadding
}

void engine::MaterialWidget::displayPBRTexture(const TextureData* textData, const std::string& label, TextureSlot slot, const std::shared_ptr<PBRMaterial>& material)
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

                displayPBRTextureControl(slot, material);
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