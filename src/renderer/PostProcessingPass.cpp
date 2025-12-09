#include "renderer/PostProcessingPass.h"
#include "core/Profiler.h"
#include "core/Window.h"
#include "pch.h"
#include "renderer/GLDebug.h"

namespace Engine
{

PostProcessingPass::PostProcessingPass() : m_postProcessShader("shader/postProcessing.vs", "shader/postProcessing.fs")
{
    float quadVertices[] = {-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
                            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};

    GLCall(glGenVertexArrays(1, &m_quadVAO));
    GLCall(glGenBuffers(1, &m_quadVBO));
    GLCall(glBindVertexArray(m_quadVAO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW));

    GLCall(glEnableVertexAttribArray(0)); // position
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0));

    GLCall(glEnableVertexAttribArray(1)); // texCoord
    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))));

    GLCall(glBindVertexArray(0));
}

void PostProcessingPass::execute(const PostProcessingInputs& postProcessingInputs)
{
    Profiler::beginCPURegion("[PostProcessing] OpenGL Setup");
    GLCall(glColorMask(true, true, true, true));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GLCall(glViewport(0, 0, Window::frameBufferDimensions.x, Window::frameBufferDimensions.y));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    GLCall(glEnable(GL_DEPTH_TEST));

    m_postProcessShader.bind();

    postProcessingInputs.renderTargetTexture->bind(0);
    m_postProcessShader.setUniform1i("renderTargetTexture", 0);

    GLCall(glBindVertexArray(m_quadVAO));
    GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
    Profiler::registerDrawCall("PostFX Pass");
    GLCall(glBindVertexArray(0));

    m_postProcessShader.unbind();
    Profiler::endCPURegion("[PostProcessing] OpenGL Setup");
}

} // namespace Engine
