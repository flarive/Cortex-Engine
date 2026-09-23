#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h> // Will drag system OpenGL headers

namespace engine
{
    /// <summary>
    /// OpenGL render state helper
    /// </summary>
    class RenderState final
    {
    public:

        // Face Culling
        static void enableFaceCulling();
        static void disableFaceCulling();
        static void setCullFace(GLenum mode);
        static bool isFaceCullingEnabled();

        // Depth Testing
        static void enableDepthTest();
        static void disableDepthTest();
        static void setDepthFunc(GLenum func);
        static void setDepthWrite(bool enabled);

        // Stencil Testing
        static void enableStencilTest();
        static void disableStencilTest();

        // Blending
        static void enableBlending();
        static void disableBlending();
        static void setBlendFunc(GLenum srcFactor, GLenum dstFactor);

        // Wireframe / Fill
        static void enableWireframe();
        static void disableWireframe();

        // Front Face
        static void setFrontFace(GLenum winding);

        // Common presets
        static void setOpaque();
        static void setTransparent();
    };
}