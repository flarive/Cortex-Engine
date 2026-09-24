#include "../../include/tools/render_state.h"

namespace engine
{
    //--------------------------------------------------------------------------
    // Face Culling
    //--------------------------------------------------------------------------

    void RenderState::enableFaceCulling()
    {
        glEnable(GL_CULL_FACE);
    }

    void RenderState::disableFaceCulling()
    {
        glDisable(GL_CULL_FACE);
    }

    void RenderState::setCullFace(GLenum mode)
    {
        glCullFace(mode);
    }

    bool RenderState::isFaceCullingEnabled()
    {
        GLboolean cullingEnabled = glIsEnabled(GL_CULL_FACE);
        return static_cast<bool>(cullingEnabled);
    }

    //--------------------------------------------------------------------------
    // Depth Testing
    //--------------------------------------------------------------------------

    void RenderState::enableDepthTest()
    {
        glEnable(GL_DEPTH_TEST);
    }

    void RenderState::disableDepthTest()
    {
        glDisable(GL_DEPTH_TEST);
    }

    void RenderState::setDepthFunc(GLenum func)
    {
        glDepthFunc(func);
    }

    void RenderState::setDepthWrite(bool enabled)
    {
        glDepthMask(enabled ? GL_TRUE : GL_FALSE);
    }

    //--------------------------------------------------------------------------
    // Depth Mask
    //--------------------------------------------------------------------------

    void RenderState::enableDepthMask()
    {
        glDepthMask(GL_TRUE);
    }
    
    void RenderState::disableDepthMask()
    {
        glDepthMask(GL_FALSE);
    }

    //--------------------------------------------------------------------------
    // Stencil Testing
    //--------------------------------------------------------------------------

    void RenderState::enableStencilTest()
    {
        glEnable(GL_STENCIL_TEST);
    }

    void RenderState::disableStencilTest()
    {
        glDisable(GL_STENCIL_TEST);
    }

    //--------------------------------------------------------------------------
    // Blending
    //--------------------------------------------------------------------------

    void RenderState::enableBlending()
    {
        glEnable(GL_BLEND);
    }

    void RenderState::disableBlending()
    {
        glDisable(GL_BLEND);
    }

    void RenderState::setBlendFunc(GLenum srcFactor, GLenum dstFactor)
    {
        glBlendFunc(srcFactor, dstFactor);
    }

    //--------------------------------------------------------------------------
    // Polygon Mode
    //--------------------------------------------------------------------------

    void RenderState::enableWireframe()
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void RenderState::disableWireframe()
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void RenderState::setPolygonMode(GLint value)
    {
        glPolygonMode(GL_FRONT_AND_BACK, value);
    }

    //--------------------------------------------------------------------------
    // Front Face
    //--------------------------------------------------------------------------

    void RenderState::setFrontFace(GLenum winding)
    {
        glFrontFace(winding);
    }

    //--------------------------------------------------------------------------
    // Presets
    //--------------------------------------------------------------------------

    void RenderState::setOpaque()
    {
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
    }

    void RenderState::setTransparent()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
    }

    //--------------------------------------------------------------------------
    // Cubemap
    //--------------------------------------------------------------------------

    void RenderState::enableCubeMapSeamless()
    {
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    }

    void RenderState::disableCubeMapSeamless()
    {
        glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    }
}