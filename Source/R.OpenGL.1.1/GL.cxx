/*
Copyright (c) 2023 Americus Maximus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Basic.hxx"
#include "GL.hxx"
#include "Mathematics.Basic.hxx"
#include "Module.hxx"

using namespace Mathematics;
using namespace Renderer::Module;
using namespace Renderers::Modules;
using namespace Renderers;
using namespace Settings;

namespace GL
{
    void Initialize(void)
    {
        ResetTextureCache();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glViewport(0, 0, State.Window.Width, State.Window.Height);
        glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 1600.0);
        glDepthRange(0.0, 1.0);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glShadeModel(GL_SMOOTH);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0039215689f);
        glDisable(GL_FOG);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ZERO);
        glEnable(GL_TEXTURE_2D);

        glFrontFace(GL_CCW);
        glCullFace(GL_FRONT);
        glEnable(GL_CULL_FACE);
    }

    void Release()
    {
        wglMakeCurrent(NULL, NULL);

        if (State.Window.RenderContext != NULL)
        {
            wglDeleteContext(State.Window.RenderContext);

            State.Window.RenderContext = NULL;
        }

        if (State.Window.DeviceContext != NULL)
        {
            ReleaseDC(State.Window.HWND, State.Window.DeviceContext);

            State.Window.DeviceContext = NULL;
        }
    }

    void SelectRendererMode(const RendererMode mode)
    {
        glFrontFace(GL_CCW);
        glCullFace(State.GL.Mode.Cull);
        glEnable(GL_CULL_FACE);

        glDisable(GL_LIGHTING);
        glDisableClientState(GL_NORMAL_ARRAY);

        if ((*State.InitializeArguments.TextureSize & State.Settings.MipMapTextureSize) == 0
            || *State.InitializeArguments.TextureMipMapFilterMode == RendererTextureMipMapFilterMode::Simple)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }

        if (*State.InitializeArguments.TextureFilterMode != State.GL.Mode.TextureFilterMode)
        {
            Render();

            const auto value =
                *State.InitializeArguments.TextureFilterMode == RendererTextureFilterMode::Nearest
                ? GL_NEAREST : GL_LINEAR;

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, value);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, value);

            State.GL.Mode.TextureFilterMode = *State.InitializeArguments.TextureFilterMode;
        }

        if (((State.GL.Mode.Mode ^ mode) & RendererMode::U0x20) != RendererMode::None
            || *State.InitializeArguments.BlendMode != State.GL.Mode.BlendMode)
        {
            Render();

            auto value = ((mode & RendererMode::U0x20) == RendererMode::None) ? GL_ONE : GL_SRC_ALPHA;

            if (*State.InitializeArguments.BlendMode == RendererBlendMode::SourceColor) { value = GL_ZERO; }

            if (State.Settings.ReduceBlendMode != ReduceBlendOptionsMode::Off
                && *State.InitializeArguments.BlendMode == RendererBlendMode::One) { value = GL_ONE; }

            switch (*State.InitializeArguments.BlendMode)
            {
            case RendererBlendMode::OneMinusSourceAlpha: { glBlendFunc(value, GL_ONE_MINUS_SRC_ALPHA); break; }
            case RendererBlendMode::One: { glBlendFunc(value, GL_ONE); break; }
            case RendererBlendMode::SourceColor: { glBlendFunc(value, GL_SRC_COLOR); break; }
            case RendererBlendMode::OneMinusSourceColor: { glBlendFunc(value, GL_ONE_MINUS_SRC_COLOR); break; }
            }

            State.GL.Mode.BlendMode = *State.InitializeArguments.BlendMode;
        }

        if (((State.GL.Mode.Mode ^ mode) & RendererMode::U0x2) != RendererMode::None)
        {
            Render();

            if ((mode & RendererMode::U0x2) != RendererMode::None)
            {
                glEnable(GL_BLEND);
                glEnable(GL_ALPHA_TEST);
            }
            else
            {
                glDisable(GL_BLEND);
                glDisable(GL_ALPHA_TEST);
            }
        }

        if (((State.GL.Mode.Mode ^ mode) & RendererMode::U0x1) != RendererMode::None)
        {
            Render();

            if ((mode & RendererMode::U0x1) != RendererMode::None)
            {
                glEnable(GL_TEXTURE_2D);
            }
            else
            {
                glDisable(GL_TEXTURE_2D);
            }
        }

        if (((State.GL.Mode.Mode ^ mode) & RendererMode::U0x8) != RendererMode::None)
        {
            if ((mode & RendererMode::U0x8) != RendererMode::None)
            {
                glEnable(GL_FOG);
            }
            else
            {
                glDisable(GL_FOG);
            }
        }

        if (((State.GL.Mode.Mode ^ mode) & (RendererMode::U0x80 | RendererMode::U0x40)) != RendererMode::None)
        {
            if ((mode & RendererMode::U0x80) != RendererMode::None && (mode & RendererMode::U0x40) != RendererMode::None)
            {
                glEnable(GL_DEPTH_TEST);
                glDepthMask(GL_TRUE);
                glDepthFunc(GL_LEQUAL);
            }
            else if ((mode & RendererMode::U0x80) != RendererMode::None)
            {
                glEnable(GL_DEPTH_TEST);
                glDepthMask(GL_TRUE);
                glDepthFunc(GL_ALWAYS);
            }
            else if ((mode & RendererMode::U0x40) != RendererMode::None)
            {
                glEnable(GL_DEPTH_TEST);
                glDepthMask(GL_FALSE);
                glDepthFunc(GL_LEQUAL);
            }
            else
            {
                glDisable(GL_DEPTH_TEST);
                glDepthMask(GL_FALSE);
                glDepthFunc(GL_ALWAYS);
            }
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, State.GL.Mode.Clamp.S == RendererClampMode::Wrap ? GL_REPEAT : GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, State.GL.Mode.Clamp.T == RendererClampMode::Wrap ? GL_REPEAT : GL_CLAMP);

        State.GL.Mode.Mode = mode;
    }

    void Render(void)
    {
        if (State.GL.Data.Vertexes.Count != 0)
        {
            glVertexPointer(3, GL_FLOAT, sizeof(RVX), &State.GL.Data.Vertexes.Vertexes[0].XYZ);
            glColorPointer(4, GL_FLOAT, sizeof(RVX), &State.GL.Data.Vertexes.Vertexes[0].BGRA);
            glTexCoordPointer(2, GL_FLOAT, sizeof(RVX), &State.GL.Data.Vertexes.Vertexes[0].UV);

            glDrawElements(GL_TRIANGLES, State.GL.Data.Indexes.Count, GL_UNSIGNED_INT, &State.GL.Data.Indexes.Indexes);

            State.GL.Data.Indexes.Count = 0;
            State.GL.Data.Vertexes.Count = 0;
        }
    }
}