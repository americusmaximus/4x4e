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

#include "DX.hxx"
#include "Module.hxx"
#include "Mathematics.Basic.hxx"
#include "Quit.hxx"

#include <math.h>

using namespace Mathematics;
using namespace Renderers::Modules;
using namespace Renderers;
using namespace Settings;

namespace Renderer::Module
{
    extern "C" u32 Add3DLine(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 AddParticle(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 BeginScene(void)
    {
        if (State.DX.Scene.IsActive) { return FALSE; }

        if (State.DX.DirectX.Device->BeginScene() != D3D_OK) { return FALSE; }

        State.DX.Scene.IsActive = TRUE;

        return TRUE;
    }

    extern "C" u32 Clear(void)
    {
        if (State.DX.DirectX.Device != NULL)
        {
            DX::DXC(State.DX.DirectX.Device->Clear(0, NULL, D3DCLEAR_TARGET, GRAPHICS_COLOR_BLACK, 0.0f, 0), "Unable to clear.");
        }

        return TRUE;
    }

    extern "C" u32 ClearZBox(const s32, const s32, const s32, const s32)
    {
        if (State.DX.DirectX.Device != NULL)
        {
            DX::DXC(State.DX.DirectX.Device->Clear(0, NULL, D3DCLEAR_ZBUFFER, GRAPHICS_COLOR_BLACK, 1.0f, 0), "Unable to clear depth buffer.");
        }

        return TRUE;
    }

    extern "C" u32 ClearZBuffer(void)
    {
        if (State.DX.DirectX.Device != NULL)
        {
            DX::DXC(State.DX.DirectX.Device->Clear(0, NULL, D3DCLEAR_ZBUFFER, GRAPHICS_COLOR_BLACK, 1.0f, 0), "Unable to clear depth buffer.");
        }

        return TRUE;
    }

    void ConvertVertex(const RPVX* input, RTLVX* vertex, const RendererMode mode, const s32 max)
    {
        vertex->XYZ.X = input->Unk05 * (1.0f / (f32)SIXTY_FIVE_THOUSAND_FIVE_HUNDRED_THIRTY_SIX);
        vertex->XYZ.Y = input->Unk06 * (1.0f / (f32)SIXTY_FIVE_THOUSAND_FIVE_HUNDRED_THIRTY_SIX);

        vertex->RHW = (f32)max / (f32)input->Unk03;

        const u32 specular = ((mode & RendererMode::U0x8) != RendererMode::None)
            ? (0xff - (input->Unk12 >> 8)) : 0xff; // TODO constants

        if ((mode & RendererMode::U0x4) != RendererMode::None)
        {
            if ((mode & RendererMode::U0x200) == RendererMode::None)
            {
                State.DX.Mode.UnknownValue105 = 0;
                State.DX.Mode.UnknownValue104 = (input->Unk09 - 256) >> 4; // TODO constant

                if (255 < State.DX.Mode.UnknownValue104) // TODO constant
                {
                    State.DX.Mode.UnknownValue105 = State.DX.Mode.UnknownValue104 - 256; // TODO constant

                    if (255 < State.DX.Mode.UnknownValue105) // TODO constant
                    {
                        State.DX.Mode.UnknownValue105 = 255; // TODO constant
                    }

                    State.DX.Mode.UnknownValue104 = 255; // TODO constant
                }
            }
            else
            {
                State.DX.Mode.UnknownValue105 = 0; // TODO constant
                State.DX.Mode.UnknownValue104 = 255; // TODO constant
            }
        }

        if ((mode & RendererMode::U0x1) == RendererMode::None)
        {
            u32 color = GRAPHICS_COLOR_BLACK;

            if ((mode & RendererMode::U0x200) == RendererMode::None)
            {
                const auto indx = ((mode & RendererMode::U0x4) == RendererMode::None)
                    ? (*State.InitializeArguments.Unk13 & 0xff) : ((input->Unk07 & 0xff0000U) >> 16);

                const u32 r = State.DX.Pixels.Palette.RGB[indx * 3 + 0];
                const u32 g = State.DX.Pixels.Palette.RGB[indx * 3 + 1];
                const u32 b = State.DX.Pixels.Palette.RGB[indx * 3 + 2];

                color = 0xff000000 | (r << 16) | (g << 8) | b; // TODO constant
            }
            else
            {
                color = 0xff000000 | ((input->Unk09 & 0xffffff00U) << 8) | (input->Unk10 & 0xffffff00U) | (input->Unk11 >> 8); // TODO constant
            }

            vertex->Color = color;
            vertex->Specular = specular << 24;
        }
        else
        {
            vertex->Specular = (specular << 24)
                | (State.DX.Mode.UnknownValue105 << 16) | (State.DX.Mode.UnknownValue105 << 8) | (State.DX.Mode.UnknownValue105 << 0);

            const auto alpha = ((mode & RendererMode::U0x100) == RendererMode::None)
                ? *State.InitializeArguments.Unk12 : (input->Unk12 >> 8);

            if ((mode & RendererMode::U0x200) == RendererMode::None)
            {
                vertex->Color = (alpha << 24) | (State.DX.Mode.UnknownValue104 << 16) | (State.DX.Mode.UnknownValue104 << 8) | State.DX.Mode.UnknownValue104;
            }
            else
            {
                vertex->Color = (alpha << 24) | ((input->Unk09 & 0xffffff00U) << 8) | (input->Unk10 & 0xffffff00U) | (input->Unk11 >> 8); // TODO constant
            }
        }

        if (*State.InitializeArguments.Unk27 == 0) // TODO constant
        {
            vertex->XYZ.Z = 1.0f - 1.0f / Clamp<f32>(State.DX.Mode.UnknownValue101 * (f32)input->Unk03, 1.0f, 256.0f); // TODO constants
        }
        else
        {
            vertex->XYZ.Z = Min<f32>(1.0f, State.DX.Mode.UnknownValue101 * (f32)input->Unk03);
        }

        vertex->UV.X = (f32)input->Unk07 * (1.0f / (f32)SIXTEEN_MILLION_SEVEN_HUNDRED_SEVENTY_SEVEN_THOUSAND_TWO_HUNDRED_SIXTEEN);
        vertex->UV.Y = (f32)input->Unk08 * (1.0f / (f32)SIXTEEN_MILLION_SEVEN_HUNDRED_SEVENTY_SEVEN_THOUSAND_TWO_HUNDRED_SIXTEEN);
    }

    u32 DotLineListD3DTL(const RTLVX* vertexes, const u32 vertexCount, const RendererMode mode, const D3DPRIMITIVETYPE primitive, const s32 primitiveCount)
    {
        DX::SelectRendererMode(mode);

        {
            RTLVX* buffer;
            DX::DXC(State.DX.Buffers.RTLVX->Lock(0, vertexCount * sizeof(RTLVX),
                (void**)&buffer, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK), "Unable to lock RTLVX vertex buffer.");

            for (u32 x = 0; x < vertexCount; x++)
            {
                const auto input = vertexes[x];
                auto vertex = buffer[x];

                vertex.XYZ.X = input.XYZ.X;
                vertex.XYZ.Y = input.XYZ.Y;
                vertex.XYZ.Z = 1.0f - input.RHW;

                vertex.RHW = input.RHW;

                vertex.Color = input.Color;
                vertex.Specular = input.Specular;

                vertex.UV.X = input.UV.X;
                vertex.UV.Y = input.UV.Y;
            }

            DX::DXC(State.DX.Buffers.RTLVX->Unlock(), "Unable to unlock RTLVX vertex buffer.");
        }

        {
            u16* buffer;
            DX::DXC(State.DX.Buffers.Index->Lock(0, vertexCount * GRAPHICS_LINE_VERTEX_COUNT,
                (void**)&buffer, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK), "Unable to lock index buffer.");

            for (u32 x = 0; x < vertexCount; x++)
            {
                buffer[x] = x;
            }

            DX::DXC(State.DX.Buffers.Index->Unlock(), "Unable to unlock index buffer.");
        }

        DX::DXC(State.DX.DirectX.Device->SetStreamSource(STREAM_SOURCE_0_INDEX, State.DX.Buffers.RTLVX, 0, sizeof(RTLVX)),
            "Unable to set RTLVX stream source.");

        DX::DXC(State.DX.DirectX.Device->SetIndices(State.DX.Buffers.Index), "Unable to set index source.");

        DX::SetShader(D3DFVF_TEX1 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW);

        DX::DXC(State.DX.DirectX.Device->DrawIndexedPrimitive(primitive, 0, 0, vertexCount, 0, primitiveCount),
            "Unable to draw indexed primitive.");

        return TRUE;
    }

    extern "C" u32 DrawPolygon(const RPVX * vertexes, const u32 vertexCount, const RendererMode mode)
    {
        const RPVX* items[MAX_RENDERER_POLYGON_VERTEX_COUNT];

        for (u32 x = 0; x < vertexCount; x++)
        {
            items[x] = &vertexes[x];
        }

        return DrawPolygon2(&items[0], vertexCount, mode);
    }

    extern "C" u32 DrawPolygon2(const RPVX * *vertexes, const u32 vertexCount, const RendererMode mode)
    {
        if (!State.DX.Scene.IsActive) { return FALSE; }

        auto am = mode;

        DX::AcquireRendererMode(&am);
        DX::SelectRendererMode(am);

        s32 max = S32_MIN; // TODO type

        for (u32 x = 0; x < vertexCount; x++)
        {
            if (max < vertexes[x]->Unk03) { max = vertexes[x]->Unk03; }
        }

        for (u32 x = 0; x < vertexCount; x++)
        {
            ConvertVertex(vertexes[x], &State.DX.Data.RTLVX.Vertexes[State.DX.Data.RTLVX.Count + x], am, max);
        }

        {
            auto indexes = &State.DX.Data.Indexes.Indexes[State.DX.Data.Indexes.Count];

            for (u32 x = 0; x < vertexCount - 2; x++)
            {
                indexes[x * 3 + 0] = State.DX.Data.RTLVX.Count + 0;
                indexes[x * 3 + 1] = State.DX.Data.RTLVX.Count + x + 1;
                indexes[x * 3 + 2] = State.DX.Data.RTLVX.Count + x + 2;
            }
        }

        State.DX.Data.Indexes.Count = State.DX.Data.Indexes.Count + (vertexCount - 2) * GRAPHICS_TRIANGLE_VERTEX_COUNT;
        State.DX.Data.RTLVX.Count = State.DX.Data.RTLVX.Count + vertexCount;

        if (RENDERER_ALMOST_SIXTEEN_THOUSAND_ELEMENT_COUNT < State.DX.Data.RTLVX.Count) { DX::Render(); }
        if (RENDERER_ALMOST_SIXTEEN_THOUSAND_ELEMENT_COUNT < State.DX.Data.Indexes.Count) { DX::Render(); }

        return TRUE;
    }

    extern "C" u32 DrawPolyList(const RPVX*, const u16**, const u32, const RendererMode) { return TRUE; } // NOTE: Referecenced by the application, but never being called.

    extern "C" u32 DrawPolyList2(const RPVX*, const u16**, const u32, const RendererMode) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 DrawPolyList3(const RPVX*, const u16*, const u32, const RendererMode) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 DotListD3DTL(const RTLVX * vertexes, const u32 vertexCount, const RendererMode mode)
    {
        return DotLineListD3DTL(vertexes, vertexCount, mode, D3DPRIMITIVETYPE::D3DPT_POINTLIST, vertexCount);
    }

    extern "C" u32 EnableCulling(const RendererCullMode mode)
    {
        switch (mode)
        {
        case RendererCullMode::CCW: { State.DX.Mode.Cull = D3DCULL::D3DCULL_CCW; break; }
        case RendererCullMode::CW: { State.DX.Mode.Cull = D3DCULL::D3DCULL_CW; break; }
        default: { State.DX.Mode.Cull = D3DCULL::D3DCULL_NONE; break; }
        }

        return TRUE;
    }

    extern "C" u32 EndScene(void)
    {
        if (!State.DX.Scene.IsActive) { return FALSE; }

        State.DX.Scene.IsActive = FALSE;

        return State.DX.DirectX.Device->EndScene() == D3D_OK;
    }

    extern "C" u32 FlushLineList(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 FlushParticleList(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 LineListD3DTL(const Renderers::RTLVX * vertexes, const u32 vertexCount, const Renderers::RendererMode mode)
    {
        return DotLineListD3DTL(vertexes, vertexCount, mode, D3DPRIMITIVETYPE::D3DPT_LINELIST, vertexCount - 1);
    }

    extern "C" u32 LockFrame(void)
    {
        if (State.DX.Scene.IsActive) { EndScene(); }

        if (!State.DX.Lock.IsActive)
        {
            State.DX.DirectX.Device->GetRenderTarget(RENDER_TARGET_0_INDEX, &State.DX.Surfaces.Lock);

            D3DLOCKED_RECT rect;

            if (State.DX.Surfaces.Lock->LockRect(&rect, NULL, D3DLOCK_NOSYSLOCK) == D3D_OK)
            {
                u32 offset = 0;

                for (u32 x = 0; x < State.Window.Height; x++)
                {
                    State.Pixels.Pixels[x] = (u32*)((addr)rect.pBits + offset);

                    offset = offset + rect.Pitch;
                }

                State.DX.Lock.IsActive = TRUE;

                return TRUE;
            }

            return FALSE;
        }

        return TRUE;
    }

    extern "C" u32 LockFrameX(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 LockHoldBuffer(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 MasterZBuffer(const s32) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 PolyListTL(const RCVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        if (!State.DX.Scene.IsActive) { return FALSE; }

        DX::SelectRendererMode(mode);

        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_CULLMODE, State.DX.Mode.Cull);
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_LIGHTING, FALSE);

        for (u32 x = 0; x < vertexCount; x++)
        {
            const auto input = &vertexes[x];
            auto vertex = &State.DX.Data.RTLVX.Vertexes[x];

            vertex->XYZ.X = input->Unk06;
            vertex->XYZ.Y = input->Unk07;
            vertex->XYZ.Z = 1.0f - input->Unk04;

            vertex->RHW = input->Unk04;

            vertex->Color = DX::Color(input->Unk12, input->Unk11, input->Unk10, input->Unk13);
            vertex->Specular = DX::Alpha(1.0f - input->Unk14);

            if ((mode & RendererMode::U0x400) == RendererMode::None)
            {
                vertex->UV.X = input->Unk08;
                vertex->UV.Y = input->Unk09;
            }
            else
            {
                vertex->RHW = vertexes->Unk15 * vertexes->Unk04;

                vertex->UV.X = (1.0f / vertexes->Unk15) * vertexes->Unk08 * 0.5f + 0.5f;
                vertex->UV.Y = (1.0f / vertexes->Unk15) * vertexes->Unk09 * -0.5f + 0.5f;
            }
        }

        DX::SetShader(D3DFVF_TEX1 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW);

        DX::DXC(State.DX.DirectX.Device->DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST,
            0, vertexCount, indexCount, indexes, D3DFORMAT::D3DFMT_INDEX16,
            State.DX.Data.RTLVX.Vertexes, sizeof(RTLVX)), "Unable to draw indexed primitive.");

        return TRUE;
    }

    extern "C" u32 RestoreZBuffer(const s32, const s32, const s32, const s32, const s32) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 SetColorTable(u8 * rgb, u16 * rgba)
    {
        if (State.Window.BitsPerPixel == GRAPHICS_BITS_PER_PIXEL_16)
        {
            *State.InitializeArguments.R.Display = 11;
            *State.InitializeArguments.R.Font = 8;
            *State.InitializeArguments.R.BitsPerPixel = 3;

            *State.InitializeArguments.G.Display = 5;
            *State.InitializeArguments.G.Font = 4;
            *State.InitializeArguments.G.BitsPerPixel = 2;

            *State.InitializeArguments.B.Display = 0;
            *State.InitializeArguments.B.Font = 8;
            *State.InitializeArguments.B.BitsPerPixel = 3;
        }
        else
        {
            *State.InitializeArguments.R.Display = 16;
            *State.InitializeArguments.R.Font = 1;
            *State.InitializeArguments.R.BitsPerPixel = 0;

            *State.InitializeArguments.G.Display = 8;
            *State.InitializeArguments.G.Font = 1;
            *State.InitializeArguments.G.BitsPerPixel = 0;

            *State.InitializeArguments.B.Display = 0;
            *State.InitializeArguments.B.Font = 1;
            *State.InitializeArguments.B.BitsPerPixel = 0;
        }

        {
            const auto rf = *State.InitializeArguments.R.Font;
            const auto rd = *State.InitializeArguments.R.Display;

            const auto gf = *State.InitializeArguments.G.Font;
            const auto gd = *State.InitializeArguments.G.Display;

            const auto bf = *State.InitializeArguments.B.Font;
            const auto bd = *State.InitializeArguments.B.Display;

            for (u32 x = 0; x < MAX_COLOR_TABLE_BUFFER_SIZE; x++)
            {
                auto r = rgb[x * 3 + 0] / rf << (rd & 0x1f);
                auto g = rgb[x * 3 + 1] / gf << (gd & 0x1f);
                auto b = rgb[x * 3 + 2] / bf << (bd & 0x1f);

                rgba[x] = (u16)(r | g | b);
            }
        }

        return TRUE;
    }

    extern "C" u32 SetFog(const f32 r, const f32 g, const f32 b, const f32 start, const f32 end)
    {
        const u32 rv = Clamp<u32>((u32)Max<f32>(0.0f, (r * 255.0f)), 0, 255);
        const u32 gv = Clamp<u32>((u32)Max<f32>(0.0f, (g * 255.0f)), 0, 255);
        const u32 bv = Clamp<u32>((u32)Max<f32>(0.0f, (b * 255.0f)), 0, 255);

        const u32 color = (rv << 16) | (gv << 8) | bv;

        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_FOGCOLOR, color);

        // Depth at which pixel or vertex fog effects begin for linear fog mode.The default value is 0.0f.
        // Depth is specified in world space for vertex fog and either device space[0.0, 1.0] or world space for pixel fog.
        // For pixel fog, these values are in device space when the system uses z for fog calculations and world - world space when the system is using eye - relative fog(w - fog).
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_FOGSTART, *((DWORD*)(&start)));

        // Depth at which pixel or vertex fog effects end for linear fog mode. The default value is 1.0f.
        // Depth is specified in world space for vertex fog and either device space [0.0, 1.0] or world space for pixel fog.
        // For pixel fog, these values are in device space when the system uses z for fog calculations and in world space when the system is using eye-relative fog (w-fog).
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_FOGEND, *((DWORD*)(&end)));

        // Fog density for pixel or vertex fog used in the exponential fog modes (D3DFOG_EXP and D3DFOG_EXP2). Valid density values range from 0.0 through 1.0.
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_FOGDENSITY, 0);

        return TRUE;
    }

    extern "C" u32 SetFogColor(const u32 r, const u32 g, const u32 b)
    {
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRS_FOGCOLOR, (r << 16) | (g << 8) | (b << 0));

        return TRUE;
    }

    extern "C" u32 SetMipMapLevel(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 SetTextureClamp(const RendererClampMode u, const RendererClampMode v)
    {
        if (State.DX.Mode.Clamp.U != u || State.DX.Mode.Clamp.V != v)
        {
            State.DX.Mode.Clamp.U = u;
            State.DX.Mode.Clamp.V = v;
        }

        return TRUE;
    }

    extern "C" u32 Sync(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 Toggle(void)
    {
        if (State.DX.DirectX.Device != NULL)
        {
            DX::DXC(State.DX.DirectX.Device->Present(NULL, NULL, NULL, NULL), "Unable to present back buffer.");
        }

        return TRUE;
    }

    extern "C" u32 ToggleX(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 UnlockFrame(void)
    {
        if (!State.DX.Lock.IsActive) { return FALSE; }

        State.DX.Lock.IsActive = FALSE;

        for (u32 x = 0; x < State.Window.Height; x++)
        {
            State.Pixels.Pixels[x] = State.Pixels.Values[x];
        }

        DX::DXC(State.DX.Surfaces.Lock->UnlockRect(), "Unable to unlock back buffer.");

        State.DX.Surfaces.Lock->Release();

        return TRUE;
    }

    extern "C" u32 UnlockFrameX(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 UnlockHoldBuffer(void) { return TRUE; } // NOTE: Not being called by the application.
}