#include "PostProcessingPass.h"
#include "Profiler.h"
#include "Scene.h"
#include "Window.h"
#include "pch.h"

namespace Engine
{

PostProcessingPass::PostProcessingPass() : m_postProcessShader("shader/postProcessing.vs", "shader/postProcessing.fs")
{
    initQuad();
}

void PostProcessingPass::initQuad()
{
    // clang-format off
        float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
    
            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };
    // clang-format on

    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1); // texCoord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void PostProcessingPass::execute(Scene& scene, const Ref<Texture>& renderTargetTexture)
{
    glColorMask(true, true, true, true);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::frameBufferDimensions.x, Window::frameBufferDimensions.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_postProcessShader.bind();

    renderTargetTexture->bind(0);
    m_postProcessShader.setUniform1i("renderTargetTexture", 0);

    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    Profiler::registerDrawCall("PostFX Pass");
    glBindVertexArray(0);

    m_postProcessShader.unbind();
}

} // namespace Engine
