

#include "RenderStageBackground.hpp"

RenderStageBackground::RenderStageBackground() : RenderStage()
{

    {
        Shader vs, fs;
        vs.loadShader("background_vs.glsl", GL_VERTEX_SHADER);
        fs.loadShader("background_fs.glsl", GL_FRAGMENT_SHADER);

        program.createProgram();
        program.addShaderToProgram(&vs);
        program.addShaderToProgram(&fs);

        program.linkProgram();

        program.useProgram();
    }

}

RenderStageBackground::~RenderStageBackground()
{
}

void RenderStageBackground::WindowViewReset(const int width_, const int height_)
{
}

void RenderStageBackground::Execute()
{
    //{
    //    program.useProgram();
    //    glBindFramebuffer(GL_FRAMEBUFFER, (*handles)["lbufferFBO"]);

    //    glClearColor(0.f, 0.f, 0.25f, 0.f);
    //    glClear(GL_COLOR_BUFFER_BIT); // clear all 3 buffers

    //    glDisable(GL_DEPTH_TEST); // disable depth test snce we are drawing a full screen quad
    //    glDisable(GL_BLEND);

    //    glEnable(GL_STENCIL_TEST);
    //    glStencilFunc(GL_EQUAL, 0, ~0); // equal to background
    //    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    //    // draw directional light
    //    glBindVertexArray(globalLightMesh.vao);
    //    glDrawElementsBaseVertex(GL_TRIANGLE_FAN,
    //        globalLightMesh.element_count,
    //        GL_UNSIGNED_INT,
    //        TGL_BUFFER_OFFSET(globalLightMesh.startElementIndex * sizeof(int)),
    //        globalLightMesh.startVerticeIndex);
    //}
}
