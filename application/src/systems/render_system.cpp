#include "render_system.h"

RenderSystem::RenderSystem(Shader *shader, GLFWwindow *window) : shader(shader), window(window)
{
}

void RenderSystem::update(std::unordered_map<unsigned int, TransformComponent> &transform_components,
                          std::unordered_map<unsigned int, RenderComponent> &render_components)
{
    TransformComponent &transform = transform_components[0];

    // view
    glm::mat4 view_matrix = glm::mat4(1.0f);
    view_matrix = glm::translate(view_matrix, glm::vec3(0.6f, 0.0f, 0.0f));
    view_matrix = glm::scale(view_matrix, glm::vec3(*transform.zoom_factor, *transform.zoom_factor, 1.0f));
    shader->SetUniformMatrix4fv("view", view_matrix);

    // model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians((*transform.eulers).x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians((*transform.eulers).y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians((*transform.eulers).z), glm::vec3(0.0f, 0.0f, 1.0f));
    shader->SetUniformMatrix4fv("model", model);

    for (const auto &[index, render] : render_components)
    {

        render.texture->Bind();
        render.vao->Bind();
        render.fbo->Bind();
        glDrawElements(GL_TRIANGLES, render.fbo->GetVertexCount(), GL_UNSIGNED_INT, nullptr);
    }
}