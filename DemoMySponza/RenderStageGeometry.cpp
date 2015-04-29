

#include "RenderStageGeometry.hpp"

RenderStageGeometry::RenderStageGeometry() : RenderStage()
{

    {
        Shader vs, fs;
        vs.loadShader("firstpass_vs.glsl", GL_VERTEX_SHADER);
        fs.loadShader("firstpass_fs.glsl", GL_FRAGMENT_SHADER);

        program.createProgram();
        program.addShaderToProgram(&vs);
        program.addShaderToProgram(&fs);

        // set the channels of the output for this one to be sure
        glBindFragDataLocation(program.getProgramID(), 0, "position");
        glBindFragDataLocation(program.getProgramID(), 1, "normal");
        glBindFragDataLocation(program.getProgramID(), 2, "material");

        program.linkProgram();

        program.useProgram();
    }

}

RenderStageGeometry::~RenderStageGeometry()
{
}

void RenderStageGeometry::WindowViewReset(const int width_, const int height_)
{
    {
        // gbuffer position texture
        glBindTexture(GL_TEXTURE_RECTANGLE, (*handles)["gbufferPositionTO"]);
        glTexImage2D(
            GL_TEXTURE_RECTANGLE,
            0,
            GL_RGB32F,
            width_,
            height_,
            0,
            GL_RGB,
            GL_FLOAT,
            NULL
            );
        glBindTexture(GL_TEXTURE_RECTANGLE, 0);

        // gbuffer normal texture
        glBindTexture(GL_TEXTURE_RECTANGLE, (*handles)["gbufferNormalTO"]);
        glTexImage2D(
            GL_TEXTURE_RECTANGLE,
            0,
            GL_RGB32F,
            width_,
            height_,
            0,
            GL_RGB,
            GL_FLOAT,
            NULL
            );
        glBindTexture(GL_TEXTURE_RECTANGLE, 0);

        // gbuffer material texture
        glBindTexture(GL_TEXTURE_RECTANGLE, (*handles)["gbufferMaterialTO"]);
        glTexImage2D(
            GL_TEXTURE_RECTANGLE,
            0,
            GL_RGBA32F,
            width_,
            height_,
            0,
            GL_RGBA,
            GL_FLOAT,
            NULL
            );
        glBindTexture(GL_TEXTURE_RECTANGLE, 0);

        // gbuffer depth stencil buffer
        glBindRenderbuffer(GL_RENDERBUFFER, (*handles)["depthStencilRBO"]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        GLenum gbuffer_status = 0;
        glBindFramebuffer(GL_FRAMEBUFFER, (*handles)["gbufferFBO"]);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, (*handles)["depthStencilRBO"]); // attach depth stencil buffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, (*handles)["gbufferPositionTO"], 0); // attach position buffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, (*handles)["gbufferNormalTO"], 0); // attach normal buffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_RECTANGLE, (*handles)["gbufferMaterialTO"], 0); // attach material buffer

        gbuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (gbuffer_status != GL_FRAMEBUFFER_COMPLETE)
        {
            tglDebugMessage(GL_DEBUG_SEVERITY_HIGH, "gbuffer not complete");
        }

        GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, buffers);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void RenderStageGeometry::Execute()
{
    // set up the depth and stencil buffers, we are not writing to the onscreen framebuffer, we are filling the relevant data for the light render
    //{
    //    program.useProgram();
    //    glBindFramebuffer(GL_FRAMEBUFFER, (*handles)["gbufferFBO"]);

    //    glClearColor(0.f, 0.f, 0.25f, 0.f);
    //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear all 3 buffers

    //    glEnable(GL_DEPTH_TEST);
    //    glDepthMask(GL_TRUE);

    //    // not using these so disable them
    //    glDisable(GL_BLEND);

    //    glEnable(GL_STENCIL_TEST);
    //    glStencilFunc(GL_ALWAYS, 127, ~0); // we are writing 1 to all pixels that the geometry draws into
    //    glStencilOp(GL_ZERO, GL_KEEP, GL_REPLACE);

    //    // the lights are tagged onto the end of the meshes
    //    for (unsigned int i = 0; i < loadedMeshes.size(); ++i)
    //    {
    //        glBindVertexArray(loadedMeshes[i].vao);
    //        glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
    //            loadedMeshes[i].element_count,
    //            GL_UNSIGNED_INT,
    //            TGL_BUFFER_OFFSET(loadedMeshes[i].startElementIndex * sizeof(int)),
    //            vboInstances[i].GetSize(),
    //            loadedMeshes[i].startVerticeIndex);
    //    }

    //    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //}
}
