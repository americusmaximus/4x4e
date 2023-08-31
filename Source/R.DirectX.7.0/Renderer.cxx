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
#include "Mathematics.Basic.hxx"
#include "Module.hxx"
#include "Quit.hxx"
#include "Settings.hxx"

using namespace Mathematics;
using namespace Renderers;
using namespace Renderers::Modules;
using namespace Settings;

namespace Renderer::Module
{
    extern "C" u32 Add3DLine(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 AddParticle(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 BeginScene(void)
    {
        if (State.DX.Scene.IsActive) { return FALSE; }

        if (State.DX.DirectX.Device->BeginScene() != DD_OK) { return FALSE; }

        State.DX.Scene.IsActive = TRUE;

        return TRUE;
    }

    extern "C" u32 Clear(void)
    {
        if (!State.DX.Scene.IsActive)
        {
            DDBLTFX ltfx = { .dwSize = sizeof(DDBLTFX) };

            State.DX.Surfaces.Back->Blt(NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &ltfx);
        }
        else
        {
            RPVX items[GRAPHICS_RECTANGLE_VERTEX_COUNT] =
            {
                {.Unk03 = *State.InitializeArguments.Unk21, .Unk05 = 0, .Unk06 = 0 },
                {.Unk03 = *State.InitializeArguments.Unk21, .Unk05 = State.Window.Width * SIXTY_FIVE_THOUSAND_FIVE_HUNDRED_THIRTY_SIX, .Unk06 = 0 },
                {.Unk03 = *State.InitializeArguments.Unk21, .Unk05 = State.Window.Width * SIXTY_FIVE_THOUSAND_FIVE_HUNDRED_THIRTY_SIX, .Unk06 = State.Window.Height * SIXTY_FIVE_THOUSAND_FIVE_HUNDRED_THIRTY_SIX },
                {.Unk03 = *State.InitializeArguments.Unk21, .Unk05 = 0, .Unk06 = State.Window.Height * SIXTY_FIVE_THOUSAND_FIVE_HUNDRED_THIRTY_SIX }
            };

            *State.InitializeArguments.Unk13 = 0; // TODO constant

            DrawPolygon(&items[0], GRAPHICS_RECTANGLE_VERTEX_COUNT, RendererMode::U0x10);
        }

        return TRUE;
    }

    extern "C" u32 ClearZBox(const s32 x0, const s32 x1, const s32 y0, const s32 y1)
    {
        RECT rect =
        {
            .left = x0,
            .top = y0,
            .right = x1 + 1,
            .bottom = y1 + 1
        };

        DDBLTFX ltfx =
        {
            .dwSize = sizeof(DDBLTFX),
            .dwFillColor = GRAPHICS_COLOR_WHITE
        };

        State.DX.Surfaces.Depth->Blt(&rect, NULL, NULL, DDBLT_DEPTHFILL | DDBLT_WAIT, &ltfx);

        return TRUE;
    }

    extern "C" u32 ClearZBuffer(void)
    {
        DDBLTFX ltfx =
        {
            .dwSize = sizeof(DDBLTFX),
            .dwFillColor = GRAPHICS_COLOR_WHITE
        };

        State.DX.Surfaces.Depth->Blt(NULL, NULL, NULL, DDBLT_DEPTHFILL | DDBLT_WAIT, &ltfx);

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
                if (State.Settings.PremultiplyColorAlphaMode == PremultiplyColorAlphaOptionsMode::Off
                    || *State.InitializeArguments.BlendMode != RendererBlendMode::One)
                {
                    vertex->Color = (alpha << 24) | (State.DX.Mode.UnknownValue104 << 16) | (State.DX.Mode.UnknownValue104 << 8) | State.DX.Mode.UnknownValue104;
                }
                else
                {
                    State.DX.Mode.UnknownValue104 = (State.DX.Mode.UnknownValue104 * alpha) >> 8;
                    vertex->Color = (State.DX.Mode.UnknownValue104 | 0xffff0000) << 8 | (State.DX.Mode.UnknownValue104 << 16) | State.DX.Mode.UnknownValue104; // TODO constant
                }
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

    extern "C" u32 DotListD3DL(const RLVX*, const u32, const RendererMode) { return FALSE; }

    extern "C" u32 DotListD3DTL(const RTLVX * vertexes, const u32 vertexCount, const RendererMode mode)
    {
        DX::SelectRendererMode(mode);

        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_CULLMODE, State.DX.Mode.CullMode);

        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_CLIPPING, FALSE);
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_LIGHTING, FALSE);
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_EXTENTS, FALSE);

        const auto avc = Min<u32>(vertexCount, RENDERER_SIXTEEN_THOUSAND_ELEMENT_COUNT);

        for (u32 x = 0; x < avc; x++)
        {
            State.DX.Data.Indexes.Indexes[x] = x;
        }

        State.DX.DirectX.Device->DrawIndexedPrimitive(D3DPRIMITIVETYPE::D3DPT_POINTLIST, D3DFVF_TLVERTEX,
            (void*)&vertexes, avc, State.DX.Data.Indexes.Indexes, avc, D3DDP_NONE);

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
            ConvertVertex(vertexes[x], &State.DX.Data.Vertexes.Vertexes[State.DX.Data.Vertexes.Count + x], am, max);
        }

        {
            auto indexes = &State.DX.Data.Indexes.Indexes[State.DX.Data.Indexes.Count];

            for (u32 x = 0; x < vertexCount - 2; x++)
            {
                indexes[x * 3 + 0] = State.DX.Data.Vertexes.Count + 0;
                indexes[x * 3 + 1] = State.DX.Data.Vertexes.Count + x + 1;
                indexes[x * 3 + 2] = State.DX.Data.Vertexes.Count + x + 2;
            }
        }

        State.DX.Data.Indexes.Count = State.DX.Data.Indexes.Count + (vertexCount - 2) * GRAPHICS_TRIANGLE_VERTEX_COUNT;
        State.DX.Data.Vertexes.Count = State.DX.Data.Vertexes.Count + vertexCount;

        if (RENDERER_ALMOST_SIXTEEN_THOUSAND_ELEMENT_COUNT < State.DX.Data.Vertexes.Count) { DX::Render(); }
        if (RENDERER_ALMOST_SIXTEEN_THOUSAND_ELEMENT_COUNT < State.DX.Data.Indexes.Count) { DX::Render(); }

        return TRUE;
    }

    extern "C" u32 DrawPolyList(const RPVX*, const u16**, const u32, const RendererMode) { return TRUE; } // NOTE: Referecenced by the application, but never being called.

    extern "C" u32 DrawPolyList2(const RPVX*, const u16**, const u32, const RendererMode) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 DrawPolyList3(const RPVX*, const u16*, const u32, const RendererMode) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 EnableCulling(const RendererCullMode mode)
    {
        State.DX.Mode.CullMode = mode == RendererCullMode::CCW ? D3DCULL::D3DCULL_CCW : D3DCULL::D3DCULL_CW;

        return TRUE;
    }

    extern "C" u32 EnableClipping(const RendererClippingMode) { return TRUE; }

    extern "C" u32 EndScene(void)
    {
        if (!State.DX.Scene.IsActive) { return FALSE; }

        DX::UpdateRendererMode();

        DX::Render();

        State.DX.Scene.IsActive = FALSE;

        return State.DX.DirectX.Device->EndScene() == DD_OK;
    }

    extern "C" u32 FlushLineList(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 FlushParticleList(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 LineListD3DTL(const RTLVX * vertexes, const u32 vertexCount, const RendererMode mode) { return TRUE; } // NOTE: Referecenced by the application, but never being called.

    extern "C" u32 LockFrame(void)
    {
        if (State.DX.Scene.IsActive) { EndScene(); }

        if (!DX::LockBackSurface()) { return FALSE; }

        State.DX.Lock.IsActive = TRUE;

        return TRUE;
    }

    extern "C" u32 LockFrameX(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 LockHoldBuffer(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 MasterZBuffer(const s32) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 PolyListTL(const RCVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        DX::SelectRendererMode(mode);

        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_CULLMODE, State.DX.Mode.CullMode);

        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_CLIPPING, FALSE);
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_LIGHTING, FALSE);
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_EXTENTS, FALSE);

        for (u32 x = 0; x < vertexCount; x++)
        {
            const auto input = &vertexes[x];
            auto vertex = &State.DX.Data.Vertexes.Vertexes[x];

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

        State.DX.DirectX.Device->DrawIndexedPrimitive(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX,
            &State.DX.Data.Vertexes.Vertexes, vertexCount, (LPWORD)indexes, indexCount * GRAPHICS_TRIANGLE_VERTEX_COUNT, D3DDP_NONE);

        return TRUE;
    }

    extern "C" u32 RestoreZBuffer(const s32, const s32, const s32, const s32, const s32) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 SetColorTable(u8 * rgb, u16 * rgba)
    {
        State.DX.Pixels.Palette.RGB = rgb;
        State.DX.Pixels.Palette.RGBA = rgba;

        DDSURFACEDESC2 desc = { .dwSize = sizeof(DDSURFACEDESC2) };

        if (!DX::LockSurface(State.DX.Surfaces.Back, &desc)) { Quit("Unable to lock back buffer."); }
        if (!DX::UnlockSurface(State.DX.Surfaces.Back)) { Quit("Unable to unlock back buffer."); }

        {
            u32 count = 0;

            for (u32 x = desc.ddpfPixelFormat.dwRBitMask; (x & 1) == 0; x = x >> 1)
            {
                count = count + 1;
            }

            *State.InitializeArguments.R.Display = count;
            *State.InitializeArguments.R.Font = (0xff / (desc.ddpfPixelFormat.dwRBitMask >> (count & 0x1f)));
            *State.InitializeArguments.R.BitsPerPixel = DX::AcquireColorBitsPerPixel(*State.InitializeArguments.R.Font);
        }

        {
            u32 count = 0;

            for (u32 x = desc.ddpfPixelFormat.dwGBitMask; (x & 1) == 0; x = x >> 1)
            {
                count = count + 1;
            }

            *State.InitializeArguments.G.Display = count;
            *State.InitializeArguments.G.Font = (0xff / (desc.ddpfPixelFormat.dwGBitMask >> (count & 0x1f)));
            *State.InitializeArguments.G.BitsPerPixel = DX::AcquireColorBitsPerPixel(*State.InitializeArguments.G.Font);
        }

        {
            u32 count = 0;

            for (u32 x = desc.ddpfPixelFormat.dwBBitMask; (x & 1) == 0; x = x >> 1)
            {
                count = count + 1;
            }

            *State.InitializeArguments.B.Display = count;
            *State.InitializeArguments.B.Font = (0xff / (desc.ddpfPixelFormat.dwBBitMask >> (count & 0x1f)));
            *State.InitializeArguments.B.BitsPerPixel = DX::AcquireColorBitsPerPixel(*State.InitializeArguments.B.Font);
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

        const u32 color = (rv << 16) | (gv << 8) | (bv << 0);

        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_FOGCOLOR, color);

        // Depth at which pixel or vertex fog effects begin for linear fog mode.The default value is 0.0f.
        // Depth is specified in world space for vertex fog and either device space[0.0, 1.0] or world space for pixel fog.
        // For pixel fog, these values are in device space when the system uses z for fog calculations and world - world space when the system is using eye - relative fog(w - fog).
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_FOGSTART, *((DWORD*)(&start)));

        // Depth at which pixel or vertex fog effects end for linear fog mode. The default value is 1.0f.
        // Depth is specified in world space for vertex fog and either device space [0.0, 1.0] or world space for pixel fog.
        // For pixel fog, these values are in device space when the system uses z for fog calculations and in world space when the system is using eye-relative fog (w-fog).
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_FOGEND, *((DWORD*)(&end)));

        // Fog density for pixel or vertex fog used in the exponential fog modes (D3DFOG_EXP and D3DFOG_EXP2). Valid density values range from 0.0 through 1.0.
        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_FOGDENSITY, 0);

        return TRUE;
    }

    extern "C" u32 SetFogColor(const u32 r, const u32 g, const u32 b)
    {
        State.DX.Fog.Color.R = r;
        State.DX.Fog.Color.G = g;
        State.DX.Fog.Color.B = b;

        if (!State.DX.Scene.IsActive) { BeginScene(); }

        DX::SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_FOGCOLOR,
            (State.DX.Fog.Color.R << 16) | (State.DX.Fog.Color.G << 8) | (State.DX.Fog.Color.B << 0));

        if (State.DX.Scene.IsActive) { EndScene(); }

        return TRUE;
    }

    extern "C" u32 SetMipMapLevel(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 SetTextureClamp(const RendererClampMode u, const RendererClampMode v)
    {
        State.DX.DirectX.Device->SetTextureStageState(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ADDRESS, D3DTEXTUREADDRESS::D3DTADDRESS_WRAP);

        State.DX.DirectX.Device->SetTextureStageState(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ADDRESSU,
            u == RendererClampMode::Wrap ? D3DTEXTUREADDRESS::D3DTADDRESS_WRAP : D3DTEXTUREADDRESS::D3DTADDRESS_CLAMP);

        State.DX.DirectX.Device->SetTextureStageState(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ADDRESSV,
            v == RendererClampMode::Wrap ? D3DTEXTUREADDRESS::D3DTADDRESS_WRAP : D3DTEXTUREADDRESS::D3DTADDRESS_CLAMP);

        return TRUE;
    }

    extern "C" u32 SetTransform(const Mathematics::Matrix4x4*, const f32, const f32) { return FALSE; }

    extern "C" u32 SetViewPort(const f32, const f32, const f32, const f32, const f32, const f32) { return TRUE; }

    extern "C" u32 Sync(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 Toggle(void)
    {
        if (State.DX.Surfaces.Main != NULL)
        {
            State.DX.Surfaces.Main->Flip(NULL, DDFLIP_NOVSYNC | DDFLIP_WAIT);
        }

        return TRUE;
    }

    extern "C" u32 ToggleX(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 UnlockFrame(void)
    {
        if (!State.DX.Lock.IsActive) { return FALSE; }

        State.DX.Lock.IsActive = FALSE;

        if (!DX::UnlockBackSurface()) { Quit("Unable to unlock back buffer."); }

        return TRUE;
    }

    extern "C" u32 UnlockFrameX(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 UnlockHoldBuffer(void) { return TRUE; } // NOTE: Not being called by the application.
}