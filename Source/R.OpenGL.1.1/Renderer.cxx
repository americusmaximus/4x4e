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

#include "GL.hxx"
#include "Mathematics.Basic.hxx"
#include "Module.hxx"
#include "Quit.hxx"
#include "Settings.hxx"

using namespace Mathematics;
using namespace Renderers::Modules;
using namespace Renderers;
using namespace Settings;

namespace Renderer::Module
{
    extern "C" void AcquireDisplayContext(HDC * hdc) { } // NOTE: Not being called by the application.

    extern "C" u32 Add3DLine(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 AddParticle(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 BeginScene(void) { return TRUE; }

    extern "C" u32 Clear(void)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        return TRUE;
    }

    extern "C" u32 ClearZBox(const s32, const s32, const s32, const s32)
    {
        GL::SelectRendererMode(RendererMode::U0x80 | RendererMode::U0x40);

        glClear(GL_DEPTH_BUFFER_BIT);

        return TRUE;
    }

    extern "C" u32 ClearZBuffer(void)
    {
        GL::SelectRendererMode(RendererMode::U0x80 | RendererMode::U0x40);

        glClear(GL_DEPTH_BUFFER_BIT);

        return TRUE;
    }

    void ConvertVertex(const RPVX* input, RVX* vertex, const RendererMode mode)
    {
        vertex->XYZ.X = input->Unk01 * ONE_TWO_HUNDRED_FIFTY_SIXTH;
        vertex->XYZ.Y = input->Unk02 * ONE_TWO_HUNDRED_FIFTY_SIXTH;
        vertex->XYZ.Z = -1.0f * input->Unk03 * ONE_TWO_HUNDRED_FIFTY_SIXTH;

        if ((mode & RendererMode::U0x1) == RendererMode::None)
        {
            if ((mode & RendererMode::U0x200) == RendererMode::None)
            {
                vertex->BGRA.R = 255.0f;
                vertex->BGRA.G = 255.0f;
                vertex->BGRA.B = 255.0f;

                vertex->UV.X = (f32)input->Unk07 * (1.0f / (f32)SIXTEEN_MILLION_SEVEN_HUNDRED_SEVENTY_SEVEN_THOUSAND_TWO_HUNDRED_SIXTEEN);
                vertex->UV.Y = (f32)input->Unk08 * (1.0f / (f32)SIXTEEN_MILLION_SEVEN_HUNDRED_SEVENTY_SEVEN_THOUSAND_TWO_HUNDRED_SIXTEEN);

                return;
            }
        }
        else
        {
            if ((mode & RendererMode::U0x20) == RendererMode::None)
            {
                vertex->BGRA.A = 1.0f;
            }
            else if ((mode & RendererMode::U0x100) == RendererMode::None)
            {
                vertex->BGRA.A = *State.InitializeArguments.Unk12 * ONE_TWO_HUNDRED_FIFTY_FIFTH;
            }
            else
            {
                vertex->BGRA.A = input->Unk12 * (1.0f / SIXTY_FIVE_THOUSAND_FIVE_HUNDRED_THIRTY_FIVE);
            }

            if ((mode & RendererMode::U0x200) == RendererMode::None)
            {
                if ((mode & RendererMode::U0x4) == RendererMode::None)
                {
                    vertex->BGRA.R = 1.0f;
                    vertex->BGRA.G = 1.0f;
                    vertex->BGRA.B = 1.0f;
                }
                else
                {
                    const auto color = Min<f32>((f32)((input->Unk09 - 256) >> 4), 255.0f) * ONE_TWO_HUNDRED_FIFTY_SIXTH;

                    vertex->BGRA.R = color;
                    vertex->BGRA.G = color;
                    vertex->BGRA.B = color;
                }

                vertex->UV.X = input->Unk07 * (1.0f / (f32)SIXTEEN_MILLION_SEVEN_HUNDRED_SEVENTY_SEVEN_THOUSAND_TWO_HUNDRED_SIXTEEN);
                vertex->UV.Y = input->Unk08 * (1.0f / (f32)SIXTEEN_MILLION_SEVEN_HUNDRED_SEVENTY_SEVEN_THOUSAND_TWO_HUNDRED_SIXTEEN);

                return;
            }
        }

        vertex->BGRA.R = input->Unk09 * (1.0f / SIXTY_FIVE_THOUSAND_FIVE_HUNDRED_THIRTY_SIX);
        vertex->BGRA.G = input->Unk10 * (1.0f / SIXTY_FIVE_THOUSAND_FIVE_HUNDRED_THIRTY_SIX);
        vertex->BGRA.B = input->Unk11 * (1.0f / SIXTY_FIVE_THOUSAND_FIVE_HUNDRED_THIRTY_SIX);

        vertex->UV.X = input->Unk07 * (1.0f / (f32)SIXTEEN_MILLION_SEVEN_HUNDRED_SEVENTY_SEVEN_THOUSAND_TWO_HUNDRED_SIXTEEN);
        vertex->UV.Y = input->Unk08 * (1.0f / (f32)SIXTEEN_MILLION_SEVEN_HUNDRED_SEVENTY_SEVEN_THOUSAND_TWO_HUNDRED_SIXTEEN);
    }

    extern "C" u32 DotListD3DL(const RLVX*, const u32, const RendererMode) { return FALSE; }

    extern "C" u32 DotListD3DTL(const RTLVX*, const u32, const RendererMode) { return FALSE; }

    extern "C" u32 DrawPolygon(const RPVX * vertexes, const u32 vertexCount, const RendererMode mode)
    {
        const RPVX* items[MAX_RENDERER_POLYGON_VERTEX_COUNT];

        for (u32 x = 0; x < vertexCount; x++)
        {
            items[x] = &vertexes[x];
        }

        return DrawPolygon2(items, vertexCount, mode);
    }

    extern "C" u32 DrawPolygon2(const RPVX * *vertexes, const u32 vertexCount, const RendererMode mode)
    {
        GL::SelectRendererMode(mode);

        for (u32 x = 0; x < vertexCount; x++)
        {
            ConvertVertex(vertexes[x], &State.GL.Data.Vertexes.Vertexes[State.GL.Data.Vertexes.Count + x], mode);
        }

        {
            auto indexes = &State.GL.Data.Indexes.Indexes[State.GL.Data.Indexes.Count];

            for (u32 x = 0; x < vertexCount - 2; x++)
            {
                indexes[x * 3 + 0] = State.GL.Data.Vertexes.Count + 0;
                indexes[x * 3 + 1] = State.GL.Data.Vertexes.Count + x + 1;
                indexes[x * 3 + 2] = State.GL.Data.Vertexes.Count + x + 2;
            }
        }

        State.GL.Data.Indexes.Count = State.GL.Data.Indexes.Count + (vertexCount - 2) * GRAPHICS_TRIANGLE_VERTEX_COUNT;
        State.GL.Data.Vertexes.Count = State.GL.Data.Vertexes.Count + vertexCount;

        if (RENDERER_ALMOST_SIXTEEN_THOUSAND_ELEMENT_COUNT < State.GL.Data.Vertexes.Count) { GL::Render(); }
        if (RENDERER_ALMOST_SIXTEEN_THOUSAND_ELEMENT_COUNT < State.GL.Data.Indexes.Count) { GL::Render(); }

        return TRUE;
    }

    extern "C" u32 DrawPolyList(const RPVX*, const u16**, const u32, const RendererMode) { return TRUE; } // NOTE: Referecenced by the application, but never being called.

    extern "C" u32 DrawPolyList2(const RPVX*, const u16**, const u32, const RendererMode) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 DrawPolyList3(const RPVX*, const u16*, const u32, const RendererMode) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 EnableClipping(const RendererClippingMode) { return TRUE; }

    extern "C" u32 EnableCulling(const RendererCullMode mode)
    {
        State.GL.Mode.Cull = mode == RendererCullMode::CCW ? GL_FRONT : GL_BACK;

        return TRUE;
    }

    extern "C" u32 EndScene(void) { return TRUE; }

    extern "C" u32 FlushLineList(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 FlushParticleList(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 LineListD3DL(const RLVX * vertexes, const u32 vertexCount, const RendererMode mode) { return TRUE; } // NOTE: Referecenced by the application, but never being called.

    extern "C" u32 LineListD3DTL(const RTLVX * vertexes, const u32 vertexCount, const RendererMode mode) { return TRUE; } // NOTE: Referecenced by the application, but never being called.

    extern "C" u32 LockFrame(void) { return FALSE; }

    extern "C" u32 LockFrameX(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 LockHoldBuffer(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 MasterZBuffer(const s32) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 PolyList(void) { return FALSE; }

    extern "C" u32 PolyList2(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 PolyListD3DL(const RLVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        GL::SelectRendererMode(mode);

        for (u32 x = 0; x < vertexCount; x++)
        {
            const auto input = &vertexes[x];
            auto vertex = &State.GL.Data.Vertexes.Vertexes[x];

            vertex->BGRA.A = ONE_TWO_HUNDRED_FIFTY_FIFTH * ((input->Color >> 24) & 0xff);
            vertex->BGRA.R = ONE_TWO_HUNDRED_FIFTY_FIFTH * ((input->Color >> 16) & 0xff);
            vertex->BGRA.G = ONE_TWO_HUNDRED_FIFTY_FIFTH * ((input->Color >> 8) & 0xff);
            vertex->BGRA.B = ONE_TWO_HUNDRED_FIFTY_FIFTH * ((input->Color >> 0) & 0xff);

            if (State.Settings.ReduceBlendMode == ReduceBlendOptionsMode::On
                && *State.InitializeArguments.BlendMode == RendererBlendMode::One)
            {
                const auto alpha = vertex->BGRA.A;

                vertex->BGRA.A = 1.0f;
                vertex->BGRA.R = alpha * vertex->BGRA.R;
                vertex->BGRA.G = alpha * vertex->BGRA.G;
                vertex->BGRA.B = alpha * vertex->BGRA.B;
            }
        }

        glVertexPointer(3, GL_FLOAT, sizeof(RLVX), &vertexes[0].XYZ);
        glColorPointer(4, GL_FLOAT, sizeof(RVX), &State.GL.Data.Vertexes.Vertexes[0].BGRA);
        glTexCoordPointer(2, GL_FLOAT, sizeof(RLVX), &vertexes[0].UV);

        glDrawElements(GL_TRIANGLES, indexCount * GRAPHICS_TRIANGLE_VERTEX_COUNT, GL_UNSIGNED_SHORT, indexes);

        return TRUE;
    }

    extern "C" u32 PolyListL(const RVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        GL::SelectRendererMode(mode);

        glVertexPointer(3, GL_FLOAT, sizeof(RVX), &vertexes[0].XYZ);

        if (State.Settings.ReduceBlendMode == ReduceBlendOptionsMode::Off
            || *State.InitializeArguments.BlendMode != RendererBlendMode::One)
        {
            glColorPointer(4, GL_FLOAT, sizeof(RVX), &vertexes[0].BGRA);
        }
        else
        {
            for (u32 x = 0; x < vertexCount; x++)
            {
                const auto input = &vertexes[x];
                auto vertex = &State.GL.Data.Vertexes.Vertexes[x];

                const auto alpha = input->BGRA.A;

                vertex->BGRA.A = 1.0f;
                vertex->BGRA.R = alpha * input->BGRA.R;
                vertex->BGRA.G = alpha * input->BGRA.G;
                vertex->BGRA.B = alpha * input->BGRA.B;
            }

            glColorPointer(4, GL_FLOAT, sizeof(RVX), &State.GL.Data.Vertexes.Vertexes[0].BGRA);
        }


        glTexCoordPointer(2, GL_FLOAT, sizeof(RVX), &vertexes[0].UV);

        glDrawElements(GL_TRIANGLES, indexCount * GRAPHICS_TRIANGLE_VERTEX_COUNT, GL_UNSIGNED_SHORT, indexes);

        return TRUE;
    }

    extern "C" u32 PolyListTL(const RCVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        GL::SelectRendererMode(mode);

        for (u32 x = 0; x < vertexCount; x++)
        {
            const auto input = &vertexes[x];
            auto vertex = &State.GL.Data.Vertexes.Vertexes[x];

            vertex->XYZ.X = input->Unk01;
            vertex->XYZ.Y = input->Unk02;
            vertex->XYZ.Z = input->Unk03;

            vertex->BGRA.A = input->Unk13;
            vertex->BGRA.R = input->Unk10;
            vertex->BGRA.G = input->Unk11;
            vertex->BGRA.B = input->Unk12;

            vertex->UV.X = input->Unk08;
            vertex->UV.Y = input->Unk09;
        }

        glVertexPointer(3, GL_FLOAT, sizeof(RVX), &State.GL.Data.Vertexes.Vertexes[0].XYZ);
        glColorPointer(4, GL_FLOAT, sizeof(RVX), &State.GL.Data.Vertexes.Vertexes[0].BGRA);
        glTexCoordPointer(2, GL_FLOAT, sizeof(RVX), &State.GL.Data.Vertexes.Vertexes[0].UV);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();

        const auto clippingNear = State.GL.Mode.Clipping.Near;
        const auto clippingFar = State.GL.Mode.Clipping.Far;

        const Matrix4x4 matrix =
        {
            1.0f, 0.0f, 0.0f,                                                       0.0f,
            0.0f, 1.0f, 0.0f,                                                       0.0f,
            0.0f, 0.0f, clippingFar / (clippingFar - clippingNear),                 1.0f,
            0.0f, 0.0f, -clippingNear * clippingFar / (clippingFar - clippingNear), 0.0f
        };

        glLoadMatrixf((f32*)&matrix);

        glDrawElements(GL_TRIANGLES, indexCount * GRAPHICS_TRIANGLE_VERTEX_COUNT, GL_UNSIGNED_SHORT, indexes);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        return TRUE;
    }

    extern "C" void ReleaseDisplayContext(void) { } // NOTE: Not being called by the application.

    extern "C" u32 RestoreZBuffer(const s32, const s32, const s32, const s32, const s32) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 SetColorTable(u8*, u16*) { return TRUE; }

    extern "C" u32 SetFog(const f32 r, const f32 g, const f32 b, const f32 start, const f32 end)
    {
        const f32 color[] = { r, g, b, 0.0f };

        glFogi(GL_FOG_MODE, GL_LINEAR);
        glFogfv(GL_FOG_COLOR, color);
        glFogf(GL_FOG_START, start);
        glFogf(GL_FOG_END, end);

        return TRUE;
    }

    extern "C" u32 SetFogColor(const u32 r, const u32 g, const u32 b)
    {
        const f32 color[] =
        {
            r * ONE_TWO_HUNDRED_FIFTY_FIFTH,
            g * ONE_TWO_HUNDRED_FIFTY_FIFTH,
            b * ONE_TWO_HUNDRED_FIFTY_FIFTH,
            0.0f
        };

        glFogfv(GL_FOG_COLOR, color);

        return TRUE;
    }

    extern "C" u32 SetTextureClamp(const RendererClampMode s, const RendererClampMode t)
    {
        State.GL.Mode.Clamp.S = s;
        State.GL.Mode.Clamp.T = t;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s == RendererClampMode::Wrap ? GL_REPEAT : GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t == RendererClampMode::Wrap ? GL_REPEAT : GL_CLAMP);

        return TRUE;
    }

    extern "C" u32 SetMipMapLevel(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 SetTransform(const Matrix4x4 * matrix, const f32 zNear, const f32 zFar)
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glLoadMatrixf((f32*)matrix);

        const auto clippingNear = State.GL.Mode.Clipping.Near;
        const auto clippingFar = State.GL.Mode.Clipping.Far;

        const Matrix4x4 projection =
        {
            zNear, 0.0f, 0.0f,                                                        0.0f,
            0.0f,  zFar, 0.0f,                                                        0.0f,
            0.0f,  0.0f,  clippingFar / (clippingFar - clippingNear),                 1.0f,
            0.0f,  0.0f,  -clippingNear * clippingFar / (clippingFar - clippingNear), 0.0f
        };

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glLoadMatrixf((f32*)&projection);

        return TRUE;
    }

    extern "C" u32 SetViewPort(const f32 x0, const f32 y0, const f32 x1, const f32 y1, const f32 zNear, const f32 zFar)
    {
        glViewport((s32)x0, (s32)y0, (s32)(x1 - x0), (s32)(y1 - y0));

        State.GL.Mode.Clipping.Near = zNear;
        State.GL.Mode.Clipping.Far = zFar;

        return TRUE;
    }

    extern "C" u32 Sync(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 Toggle(void)
    {
        SwapBuffers(State.Window.DeviceContext);

        return TRUE;
    }

    extern "C" u32 ToggleX(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 UnlockFrame(void) { return FALSE; }

    extern "C" u32 UnlockFrameX(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 UnlockHoldBuffer(void) { return TRUE; }
}