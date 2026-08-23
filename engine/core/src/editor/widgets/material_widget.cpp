#include "../../../include/editor/widgets/material_widget.h"

#include "../../../include/editor/editor_helper.h"

#include "../../../include/managers/filesystem_manager.h"

#include "themes/imgui_spectrum.h"

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
            header = std::format("{} ({})", header, "PBR");
        else if (material->getTypeID() == MaterialType::blinnphong)
            header = std::format("{} ({})", header, "BlinnPhong");
    }
    else
    {
        if (material->getTypeID() == MaterialType::PBR)
            header = std::format("Unnamed material ({}) (PBR)", index);
        else if (material->getTypeID() == MaterialType::blinnphong)
            header = std::format("Unnamed material ({}) (BlinnPhong)", index);
    }
    
    ImGui::SetNextItemOpen(m_isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingHeader(header.c_str(), ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_Bullet, EditorHelper::im_grey_trans))
    {
        const std::string tableUniqueID = std::format("TexturesTable_{}", material->getName());

        if (ImGui::BeginTable(tableUniqueID.c_str(), 2, ImGuiTableFlags_SizingStretchSame))
        {
            ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_WidthFixed, 34);
            ImGui::TableSetupColumn("2", ImGuiTableColumnFlags_WidthStretch);

            if (material->getTypeID() == MaterialType::PBR)
            {
                // PBR
                displayColor(material->getBaseColorFactor(), "Base Color");

                displayTexture(TextureManager::getTextureData(material->getDiffuseTexPath()), "Diffuse");
                displayTexture(TextureManager::getTextureData(material->getNormalTexPath()), "Normal");
                
                if (material->hasArmMap())
                {
                    displayTexture(TextureManager::getTextureData(material->getArmTexPath()), "ARM");

                }
                else if (material->hasRmMap())
                {
                    displayTexture(TextureManager::getTextureData(material->getRmTexPath()), "RM");
                }
                else
                {
                    displayTexture(TextureManager::getTextureData(material->getAoTexPath()), "Ambient Occlusion");
                    displayTexture(TextureManager::getTextureData(material->getRoughnessTexPath()), "Roughness");
                    displayTexture(TextureManager::getTextureData(material->getMetallicTexPath()), "Metallic");
                }

                displayTexture(TextureManager::getTextureData(material->getHeightTexPath()), "Height");
                displayTexture(TextureManager::getTextureData(material->getEmissiveTexPath()), "Emissive");
            }
            else
            {
                // BlinnPhong or Phong

                displayColor(material->getAmbientColor(), "Ambient Color");
                displayColor(material->getDiffuseColor(), "Diffuse Color");
                displayColor(material->getSpecularColor(), "Specular Color");

                displayTexture(TextureManager::getTextureData(material->getDiffuseTexPath()), "Diffuse");
                displayTexture(TextureManager::getTextureData(material->getSpecularTexPath()), "Specular");
                displayTexture(TextureManager::getTextureData(material->getNormalTexPath()), "Normal");
                displayTexture(TextureManager::getTextureData(material->getHeightTexPath()), "Height");
                displayTexture(TextureManager::getTextureData(material->getEmissiveTexPath()), "Emissive");
            }

            ImGui::EndTable();
        }
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

void engine::MaterialWidget::displayTexture(const TextureData* textData, const std::string& textType)
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
                std::string resolution = std::format("{}x{}", textData->width, textData->height);
                EditorHelper::drawTagRightAligned(resolution.c_str(), IM_COL32(0, 255, 0, 255), 12.0f);
            }
        }

        // --- Row 2 ---
        ImGui::TableNextRow(ImGuiTableRowFlags_None, rowHeight);
        {
            ImGui::TableNextColumn();
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                ImGui::Text(FileSystemManager::getFileName(textData->filePath).c_str());
                ImGui::PopStyleColor();
            }

            ImGui::TableNextColumn();
            {
                std::string resolution = std::format("ID {}", textData->id);
                EditorHelper::drawTagRightAligned(resolution.c_str(), IM_COL32(0, 0, 255, 255), 12.0f);
            }
        }

        ImGui::PopStyleVar(); // CellPadding
        ImGui::EndTable();
    }

    ImGui::PopFont();
    ImGui::PopStyleVar(2); // ItemSpacing + FramePadding
}