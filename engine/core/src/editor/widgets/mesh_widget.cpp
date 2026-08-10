#include "../../../include/editor/widgets/mesh_widget.h"

#include "../../../include/editor/editor_helper.h"

#include <string>
#include <format>

void engine::MeshWidget::init()
{

}

void engine::MeshWidget::setMeshes(const std::vector<std::shared_ptr<Mesh>>& meshes)
{
    m_meshes.clear();
    m_meshes.reserve(meshes.size());

    for (const auto& m : meshes)
    {
        m_meshes.push_back(m);   // implicit shared_ptr → weak_ptr conversion
    }
}

void engine::MeshWidget::draw()
{
    const std::string header = std::format("Meshes ({})", m_meshes.size());

    ImGui::PushFont(ImGui::Spectrum::fontSmall2);

    ImGui::SetNextItemOpen(m_isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingHeader(header.c_str(), ImGuiTreeNodeFlags_None, EditorHelper::im_grey_dark))
    {
        const std::string tableUniqueID = "MeshesTable";
        
        if (ImGui::BeginTable(tableUniqueID.c_str(), 2, ImGuiTableFlags_SizingStretchSame))
        {
            ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("2", ImGuiTableColumnFlags_WidthFixed, 100);

            for (const auto& weakMesh : m_meshes)
            {
                // Lock the weak_ptr to get a shared_ptr
                if (auto sharedMesh = weakMesh.lock())
                {
                    displayMesh(sharedMesh);
                }
            }

            ImGui::EndTable();
        }
    }

    ImGui::PopFont();
}

void engine::MeshWidget::displayMesh(const std::shared_ptr<engine::Mesh>& mesh)
{
    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::Text(mesh->getName().c_str());

    ImGui::TableSetColumnIndex(1);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    ImGui::Text("%zu vertices", mesh->getVertices().size());
    ImGui::PopStyleColor();
}