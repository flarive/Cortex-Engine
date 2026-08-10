#include "../../../include/editor/widgets/mesh_widget.h"

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
    const std::string txt = std::format("Meshes: {}", m_meshes.size());

    ImGui::Text("%s", txt.c_str());
}