#include "../../../include/editor/widgets/material_widget.h"

#include "../../../include/editor/editor_helper.h"


void engine::MaterialWidget::init()
{
}

void engine::MaterialWidget::setMaterial(std::shared_ptr<Material> material)
{
    m_material = material;

    // This avoids subtle bugs when the caller accidentally passes a null pointer
    if (!material)
    {
        m_material.reset();
        return;
    }
}

void engine::MaterialWidget::draw()
{
    auto mat = m_material.lock();
    if (!mat)
        return;
    

    std::string header = "Material";
    if (mat->getTypeID() == MaterialType::PBR)
        header = "PBR Material";
    else if (mat->getTypeID() == MaterialType::blinnphong)
        header = "BlinnPhong Material";

    ImGui::SetNextItemOpen(m_isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingHeader(header.c_str(), ImGuiTreeNodeFlags_None))
    {
        // Diffuse map
        if (mat->hasDiffuseMap())
        {
            TextureData textData = TextureManager::getTextureData(mat->getDiffuseTexPath());
            displayTexture(textData);
        }
    }
}

void engine::MaterialWidget::displayTexture(const TextureData& textData)
{
    const std::string tableUniqueID = std::format("TextureTable_{}", textData.id);
    
    if (ImGui::BeginTable(tableUniqueID.c_str(), 2, ImGuiTableFlags_SizingStretchSame))
    {
        ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_WidthFixed, 100);
        ImGui::TableSetupColumn("2", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, textData.thumbnailLevel);
        ImGui::Image((ImTextureID)textData.id, ImVec2(TARGET_THUMB_SIZE, TARGET_THUMB_SIZE));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);

        ImGui::TableSetColumnIndex(1);
        ImGui::Text(textData.filePath.c_str());

        ImGui::EndTable();
    }
}
