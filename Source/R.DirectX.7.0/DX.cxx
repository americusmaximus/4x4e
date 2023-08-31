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
#include "Textures.hxx"

#include <math.h>
#include <stdio.h>

using namespace Mathematics;
using namespace Renderer::Module;
using namespace Renderers;
using namespace Settings;

namespace DX
{
    BOOL AcquireDevice(GUID* id)
    {
        IDirectDraw7* instance;

        if (DirectDrawCreateEx(id, (void**)&instance, IID_IDirectDraw7, NULL) == DD_OK)
        {
            DDCAPS dev = { .dwSize = sizeof(DDCAPS) }; // Hardware Device
            DDCAPS emu = { .dwSize = sizeof(DDCAPS) }; // Hardware Emulation Layer

            if (instance->GetCaps(&dev, &emu) == DD_OK && (dev.dwCaps & DDCAPS_3D))
            {
                instance->Release();

                return TRUE;
            }
        }

        if (instance != NULL) { instance->Release(); }

        return FALSE;
    }

    BOOL WINAPI EnumerateDevices(GUID* id, LPSTR driver, LPSTR desc, LPVOID context, HMONITOR monitor)
    {
        if (!AcquireDevice(id)) { return TRUE; }

        if (id == NULL)
        {
            State.DX.Devices.Available.Details[State.DX.Devices.Available.Count].Index = NULL;
        }
        else
        {
            State.DX.Devices.Available.Details[State.DX.Devices.Available.Count].Index =
                &State.DX.Devices.Available.Details[State.DX.Devices.Available.Count].ID;

            State.DX.Devices.Available.Details[State.DX.Devices.Available.Count].ID = *id;
        }

        CopyMemory(State.DX.Devices.Available.Details[State.DX.Devices.Available.Count].Driver, driver, STANDARD_DEVICE_IDENTIFIER_STRING_LENGTH);
        State.DX.Devices.Available.Details[State.DX.Devices.Available.Count].Driver[(STANDARD_DEVICE_IDENTIFIER_STRING_LENGTH - 1)] = NULL;

        CopyMemory(State.DX.Devices.Available.Details[State.DX.Devices.Available.Count].Description, desc, STANDARD_DEVICE_IDENTIFIER_STRING_LENGTH);
        State.DX.Devices.Available.Details[State.DX.Devices.Available.Count].Description[(STANDARD_DEVICE_IDENTIFIER_STRING_LENGTH - 1)] = NULL;

        State.DX.Devices.Available.Count = State.DX.Devices.Available.Count + 1;

        return State.DX.Devices.Available.Count < MAX_DEVICE_COUNT;
    }

    BOOL InitializeDirectX(void)
    {
        return State.DX.DirectX.Instance->QueryInterface(IID_IDirect3D7, (void**)&State.DX.DirectX.DirectX) == DD_OK;
    }

    HRESULT CALLBACK EnumerateDepthBuffers(LPDDPIXELFORMAT format, LPVOID ctx)
    {
        if (format->dwFlags == DDPF_ZBUFFER)
        {
            if (State.Window.BitsPerPixel != GRAPHICS_BITS_PER_PIXEL_32)
            {
                CopyMemory(ctx, format, sizeof(DDPIXELFORMAT));

                return DDENUMRET_CANCEL;
            }

            {
                const auto context = (LPDDPIXELFORMAT)ctx;

                if (context->dwZBufferBitDepth < format->dwZBufferBitDepth)
                {
                    CopyMemory(ctx, format, sizeof(DDPIXELFORMAT));

                    return DDENUMRET_OK;
                }
            }
        }

        return DDENUMRET_OK;
    }

    u32 Alpha(const f32 value)
    {
        return ((u32)roundf(255.0f * value) & 0xff) << 24;
    }

    u32 Color(const f32 r, const f32 g, const f32 b, const f32 a)
    {
        auto av = a;
        auto rv = r;
        auto gv = g;
        auto bv = b;

        if (State.Settings.PremultiplyColorAlphaMode == PremultiplyColorAlphaOptionsMode::On
            && *State.InitializeArguments.BlendMode == RendererBlendMode::One)
        {
            av = 1.0f;
            rv = a * r;
            gv = a * g;
            bv = a * b;
        }

        const auto ac = (u32)roundf(255.0f * av) & 0xff;
        const auto rc = (u32)roundf(255.0f * rv) & 0xff;
        const auto gc = (u32)roundf(255.0f * gv) & 0xff;
        const auto bc = (u32)roundf(255.0f * bv) & 0xff;

        return (ac << 24) | (rc << 16) | (gc << 8) | (bc << 0);
    }

    u32 AcquireColorBitsPerPixel(const u32 value)
    {
        if (value == 0) { return GRAPHICS_BITS_PER_PIXEL_32; }

        auto v = value;
        u32 result = 0;

        if (1 < v)
        {
            do
            {
                v = v >> 1;
                result = result + 1;
            } while (1 < v);
        }

        return result;
    }

    void AcquireRendererMode(RendererMode* mode)
    {
        if (State.DX.Pixels.Palette.Alphas != NULL)
        {
            if ((*mode & RendererMode::U0x1) != RendererMode::None)
            {
                *mode = *mode | RendererMode::U0x20 | RendererMode::U0x2;
            }
        }
    }

    BOOL UnlockSurface(IDirectDrawSurface7* surface)
    {
        if (surface == NULL) { return TRUE; }

        const auto result = surface->Unlock(NULL);

        if (result != DDERR_NOTLOCKED && result != DD_OK) { return FALSE; }

        return TRUE;
    }

    BOOL LockSurface(IDirectDrawSurface7* surface, DDSURFACEDESC2* desc)
    {
        if (surface == NULL) { return TRUE; }

        ZeroMemory(desc, sizeof(DDSURFACEDESC2));

        desc->dwSize = sizeof(DDSURFACEDESC2);

        return surface->Lock(NULL, desc, DDLOCK_NOSYSLOCK, NULL) == DD_OK;
    }

    HRESULT CALLBACK EnumerateTextureFormats(LPDDPIXELFORMAT format, LPVOID ctx)
    {
        auto result = (DXPixelFormat*)ctx;

        if (format->dwRGBBitCount == GRAPHICS_BITS_PER_PIXEL_16
            && ((*result & DXPixelFormat::Opaque) == DXPixelFormat::None)
            && format->dwRGBAlphaBitMask == 0
            && (format->dwGBitMask | format->dwBBitMask | format->dwRBitMask) == 0xffff)
        {
            CopyMemory(&State.DX.Surfaces.Staging.PixelFormat.Opaque, format, sizeof(DDPIXELFORMAT));

            *result = *result | DXPixelFormat::Opaque;
        }

        if (State.Window.BitsPerPixel == GRAPHICS_BITS_PER_PIXEL_32)
        {
            if (format->dwRGBBitCount == GRAPHICS_BITS_PER_PIXEL_32
                && ((*result & DXPixelFormat::Transparent) == DXPixelFormat::None)
                && (format->dwRGBAlphaBitMask & 0xff000000))
            {
                CopyMemory(&State.DX.Surfaces.Staging.PixelFormat.Transparent, format, sizeof(DDPIXELFORMAT));

                *result = *result | DXPixelFormat::Transparent;

                return DDENUMRET_OK;
            }
        }
        else if (format->dwRGBBitCount == GRAPHICS_BITS_PER_PIXEL_16
            && ((*result & DXPixelFormat::Transparent) == DXPixelFormat::None)
            && format->dwRGBAlphaBitMask == 0xf000)
        {
            CopyMemory(&State.DX.Surfaces.Staging.PixelFormat.Transparent, format, sizeof(DDPIXELFORMAT));

            *result = *result | DXPixelFormat::Transparent;
        }

        return DDENUMRET_OK;
    }

    BOOL Initialize(void)
    {
        if (State.DX.DirectX.DirectX->CreateDevice(IID_IDirect3DTnLHalDevice,
            State.DX.Surfaces.Back, &State.DX.DirectX.Device) != DD_OK)
        {
            if (State.DX.DirectX.DirectX->CreateDevice(IID_IDirect3DHALDevice,
                State.DX.Surfaces.Back, &State.DX.DirectX.Device) != DD_OK)
            {
                return FALSE;
            }
        }

        {
            DXPixelFormat result = DXPixelFormat::None;

            if (State.DX.DirectX.Device->EnumTextureFormats(EnumerateTextureFormats, &result) != DD_OK) { return FALSE; }

            if (result == DXPixelFormat::None) { return FALSE; }
        }

        {
            D3DVIEWPORT7 viewport =
            {
                .dwX = 0,
                .dwY = 0,
                .dwWidth = State.Window.Width,
                .dwHeight = State.Window.Height,
                .dvMinZ = 0.0f,
                .dvMaxZ = 1.0f
            };

            State.DX.DirectX.Device->SetViewport(&viewport);
        }

        if (!AllocateStageTextures()) { return FALSE; }

        if (!InitializeRenderState()) { return FALSE; }

        return TRUE;
    }

    void ReleaseAllocatedTextures(void)
    {
        for (u32 x = 0; x < MAX_STAGING_TEXTURE_COUNT; x++)
        {
            if (State.DX.Surfaces.Staging.Transparent[x] != NULL)
            {
                State.DX.Surfaces.Staging.Transparent[x]->Release();
                State.DX.Surfaces.Staging.Transparent[x] = NULL;
            }

            if (State.DX.Surfaces.Staging.Opaque[x] != NULL)
            {
                State.DX.Surfaces.Staging.Opaque[x]->Release();
                State.DX.Surfaces.Staging.Opaque[x] = NULL;
            }
        }

        for (u32 x = 0; x < MAX_TEXTURE_COUNT; x++)
        {
            if (State.DX.Surfaces.Surfaces[x].Surface != NULL)
            {
                State.DX.Surfaces.Surfaces[x].Surface->Release();
                State.DX.Surfaces.Surfaces[x].Surface = NULL;
            }

            State.DX.Surfaces.Surfaces[x].IsOpaque = FALSE;
        }
    }

    void Release(void)
    {
        ReleaseAllocatedTextures();

        if (State.DX.DirectX.Device != NULL)
        {
            State.DX.DirectX.Device->Release();
            State.DX.DirectX.Device = NULL;
        }

        if (State.DX.DirectX.DirectX != NULL)
        {
            State.DX.DirectX.DirectX->Release();
            State.DX.DirectX.DirectX = NULL;
        }

        if (State.DX.Surfaces.Depth != NULL)
        {
            State.DX.Surfaces.Depth->Release();
            State.DX.Surfaces.Depth = NULL;
        }

        if (State.DX.Surfaces.Main != NULL)
        {
            State.DX.Surfaces.Main->Release();
            State.DX.Surfaces.Main = NULL;
        }

        for (u32 x = 0; x < MAX_TEXTURE_CACHE_COUNT; x++)
        {
            if (State.DX.Caches[x] != NULL)
            {
                free(State.DX.Caches[x]);

                State.DX.Caches[x] = NULL;
            }
        }
    }

    BOOL LockBackSurface(void)
    {
        if (State.DX.Lock.IsBackSurfaceLockActive) { return TRUE; }

        if (LockSurface(State.DX.Surfaces.Back, &State.DX.Surfaces.Descriptor))
        {
            u32 offset = 0;
            const auto bits = State.DX.Surfaces.Descriptor.lpSurface;
            const auto pitch = State.DX.Surfaces.Descriptor.lPitch;

            for (u32 x = 0; x < State.Window.Height; x++)
            {
                State.Pixels.Pixels[x] = (u32*)((addr)bits + offset);

                offset = offset + pitch;
            }

            State.DX.Lock.IsBackSurfaceLockActive = TRUE;

            return TRUE;
        }

        return FALSE;
    }

    BOOL UnlockBackSurface(void)
    {
        if (!State.DX.Lock.IsBackSurfaceLockActive) { return TRUE; }

        for (u32 x = 0; x < State.Window.Height; x++)
        {
            State.Pixels.Pixels[x] = State.Pixels.Values[x];
        }

        if (!UnlockSurface(State.DX.Surfaces.Back)) { return FALSE; }

        State.DX.Lock.IsBackSurfaceLockActive = FALSE;

        return TRUE;
    }

    void SetRendererState(const D3DRENDERSTATETYPE state, const DWORD value)
    {
        const auto indx = state & MAX_STATE_VARIABLE_INDEX_MASK;

        if (State.DX.DirectX.State.States[indx] != state || State.DX.DirectX.State.Values[indx] != value)
        {
            State.DX.DirectX.State.States[indx] = state;
            State.DX.DirectX.State.Values[indx] = value;

            Render();

            State.DX.DirectX.Device->SetRenderState(state, value);
        }
    }

    void Render(void)
    {
        if (State.DX.Scene.IsActive && State.DX.Data.Vertexes.Count == 0) { return; }

        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_CULLMODE, D3DCULL::D3DCULL_NONE);

        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_CLIPPING, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_LIGHTING, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_EXTENTS, FALSE);

        State.DX.DirectX.Device->DrawIndexedPrimitive(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX,
            State.DX.Data.Vertexes.Vertexes, State.DX.Data.Vertexes.Count,
            State.DX.Data.Indexes.Indexes, State.DX.Data.Indexes.Count, D3DDP_NONE);

        State.DX.Data.Vertexes.Count = 0;
        State.DX.Data.Indexes.Count = 0;
    }

    BOOL InitializeRenderState(void)
    {
        ZeroMemory(&State.DX.DirectX.State.States, sizeof(D3DRENDERSTATETYPE) * MAX_STATE_VARIABLE_COUNT);
        ZeroMemory(&State.DX.DirectX.State.Values, sizeof(DWORD) * MAX_STATE_VARIABLE_COUNT);

        if (State.DX.DirectX.Device->BeginScene() != DD_OK) { return FALSE; }

        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_TEXTUREHANDLE, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ANTIALIAS, D3DANTIALIASMODE::D3DANTIALIAS_NONE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_TEXTUREADDRESS, D3DTEXTUREADDRESS::D3DTADDRESS_CLAMP);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_WRAPU, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_WRAPV, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ZENABLE, D3DZBUFFERTYPE::D3DZB_FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_FILLMODE, D3DFILLMODE::D3DFILL_SOLID);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_SHADEMODE, D3DSHADEMODE::D3DSHADE_GOURAUD);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_MONOENABLE, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ZWRITEENABLE, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_LASTPIXEL, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_TEXTUREMAG,
            (*State.InitializeArguments.TextureFilterMode == RendererTextureFilterMode::Nearest) ? D3DTEXTUREFILTER::D3DFILTER_NEAREST : D3DTEXTUREFILTER::D3DFILTER_LINEAR);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_TEXTUREMIN,
            (*State.InitializeArguments.TextureFilterMode == RendererTextureFilterMode::Nearest) ? D3DTEXTUREFILTER::D3DFILTER_NEAREST : D3DTEXTUREFILTER::D3DFILTER_LINEAR);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_SRCBLEND, D3DBLEND::D3DBLEND_ONE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_DESTBLEND, D3DBLEND::D3DBLEND_INVSRCALPHA);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTEXTUREBLEND::D3DTBLEND_MODULATE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_CULLMODE, D3DCULL::D3DCULL_NONE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_DITHERENABLE, (DWORD)*State.InitializeArguments.DitherMode);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_FOGENABLE, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_FOGCOLOR, (State.DX.Fog.Color.R << 16) | (State.DX.Fog.Color.G << 8) | (State.DX.Fog.Color.B << 0));
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_FOGTABLEMODE, D3DFOGMODE::D3DFOG_NONE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_SPECULARENABLE, TRUE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ZVISIBLE, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_SUBPIXEL, TRUE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_STIPPLEDALPHA, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_STIPPLEENABLE, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ZENABLE, D3DZBUFFERTYPE::D3DZB_FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ZWRITEENABLE, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ZFUNC, D3DCMPFUNC::D3DCMP_ALWAYS);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_COLORKEYENABLE, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ALPHAREF, 0);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ALPHAFUNC, D3DCMPFUNC::D3DCMP_GREATER);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_CLIPPING, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_LIGHTING, FALSE);
        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_EXTENTS, FALSE);

        UpdateRendererMode();

        return State.DX.DirectX.Device->EndScene() == DD_OK;
    }

    void UpdateRendererMode(void)
    {
        *State.InitializeArguments.Unk12 = 0; // TODO constant
        *State.InitializeArguments.BlendMode = RendererBlendMode::One;

        SelectRendererMode(RendererMode::U0x2 | RendererMode::U0x4 | RendererMode::U0x20 | RendererMode::U0x200);

        *State.InitializeArguments.Unk12 = 0xff; // TODO constant
        *State.InitializeArguments.BlendMode = RendererBlendMode::OneMinusSourceAlpha;
    }

    void SelectRendererMode(const RendererMode mode)
    {
        if (*State.InitializeArguments.TextureFilterMode != State.DX.Mode.TextureFilterMode)
        {
            Render();

            State.DX.DirectX.Device->SetTextureStageState(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_MAGFILTER,
                (*State.InitializeArguments.TextureFilterMode == RendererTextureFilterMode::Nearest) ? D3DTEXTUREFILTER::D3DFILTER_NEAREST : D3DTEXTUREFILTER::D3DFILTER_LINEAR);
            State.DX.DirectX.Device->SetTextureStageState(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_MINFILTER,
                (*State.InitializeArguments.TextureFilterMode == RendererTextureFilterMode::Nearest) ? D3DTEXTUREFILTER::D3DFILTER_NEAREST : D3DTEXTUREFILTER::D3DFILTER_LINEAR);

            State.DX.Mode.TextureFilterMode = *State.InitializeArguments.TextureFilterMode;
        }

        SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_SRCBLEND,
            (mode & RendererMode::U0x20) == RendererMode::None ? D3DBLEND::D3DBLEND_ONE : D3DBLEND::D3DBLEND_SRCALPHA);

        if (*State.InitializeArguments.BlendMode != State.DX.Mode.BlendMode)
        {
            State.DX.Mode.BlendMode = *State.InitializeArguments.BlendMode;

            switch (State.DX.Mode.BlendMode)
            {
            case RendererBlendMode::OneMinusSourceAlpha: { SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_DESTBLEND, D3DBLEND::D3DBLEND_INVSRCALPHA); break; }
            case RendererBlendMode::One:
            {
                SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_DESTBLEND, D3DBLEND::D3DBLEND_ONE);

                if (State.Settings.PremultiplyColorAlphaMode == PremultiplyColorAlphaOptionsMode::On)
                {
                    SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_SRCBLEND, D3DBLEND::D3DBLEND_ONE);
                }

                break;
            }
            case RendererBlendMode::SourceColor: { SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_DESTBLEND, D3DBLEND::D3DBLEND_SRCCOLOR); break; }
            case RendererBlendMode::OneMinusSourceColor: { SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_DESTBLEND, D3DBLEND::D3DBLEND_INVSRCCOLOR); break; }
            }
        }

        if (((mode & RendererMode::U0x20) != RendererMode::None)
            && *State.InitializeArguments.BlendMode == RendererBlendMode::SourceColor)
        {
            SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_SRCBLEND, D3DBLEND::D3DBLEND_ZERO);
        }

        if (((State.DX.Mode.Mode ^ mode) & RendererMode::U0x1) != RendererMode::None)
        {
            Render();

            if ((mode & RendererMode::U0x1) == RendererMode::None)
            {
                State.DX.DirectX.Device->SetTextureStageState(TEXTURE_STAGE_0_INDEX,
                    D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
            }
            else
            {
                State.DX.Surfaces.Current = NULL;
            }
        }

        if (((mode & RendererMode::U0x1) != RendererMode::None) &&
            State.DX.Surfaces.Surfaces[State.DX.Surfaces.Index].Surface != State.DX.Surfaces.Current)
        {
            Render();

            State.DX.Surfaces.Current = State.DX.Surfaces.Surfaces[State.DX.Surfaces.Index].Surface;

            State.DX.DirectX.Device->SetTextureStageState(TEXTURE_STAGE_0_INDEX,
                D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_MODULATE);
            State.DX.DirectX.Device->SetTextureStageState(TEXTURE_STAGE_0_INDEX,
                D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG1, D3DTA_TEXTURE);
            State.DX.DirectX.Device->SetTextureStageState(TEXTURE_STAGE_0_INDEX,
                D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG2, D3DTA_DIFFUSE);
            State.DX.DirectX.Device->SetTextureStageState(TEXTURE_STAGE_0_INDEX,
                D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_MODULATE);
            State.DX.DirectX.Device->SetTextureStageState(TEXTURE_STAGE_0_INDEX,
                D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            State.DX.DirectX.Device->SetTextureStageState(TEXTURE_STAGE_0_INDEX,
                D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

            State.DX.DirectX.Device->SetTextureStageState(TEXTURE_STAGE_1_INDEX,
                D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
            State.DX.DirectX.Device->SetTextureStageState(TEXTURE_STAGE_1_INDEX,
                D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);

            State.DX.DirectX.Device->SetTexture(TEXTURE_STAGE_1_INDEX, NULL);

            State.DX.DirectX.Device->SetTextureStageState(TEXTURE_STAGE_0_INDEX,
                D3DTEXTURESTAGESTATETYPE::D3DTSS_MAGFILTER,
                (*State.InitializeArguments.TextureFilterMode == RendererTextureFilterMode::Nearest)
                ? D3DTEXTUREFILTER::D3DFILTER_NEAREST : D3DTEXTUREFILTER::D3DFILTER_LINEAR);
            State.DX.DirectX.Device->SetTextureStageState(TEXTURE_STAGE_0_INDEX,
                D3DTEXTURESTAGESTATETYPE::D3DTSS_MINFILTER,
                (*State.InitializeArguments.TextureFilterMode == RendererTextureFilterMode::Nearest)
                ? D3DTEXTUREFILTER::D3DFILTER_NEAREST : D3DTEXTUREFILTER::D3DFILTER_LINEAR);
            State.DX.DirectX.Device->SetTextureStageState(TEXTURE_STAGE_0_INDEX,
                D3DTEXTURESTAGESTATETYPE::D3DTSS_MIPFILTER,
                (*State.InitializeArguments.TextureMipMapFilterMode == RendererTextureMipMapFilterMode::Simple)
                ? D3DTEXTUREFILTER::D3DFILTER_NEAREST : D3DTEXTUREFILTER::D3DFILTER_MIPNEAREST);

            State.DX.DirectX.Device->SetTexture(TEXTURE_STAGE_0_INDEX, State.DX.Surfaces.Current);
        }

        if (*State.InitializeArguments.TextureMipMapFilterMode != State.DX.Mode.TextureMipMapFilterMode)
        {
            State.DX.Mode.TextureMipMapFilterMode = *State.InitializeArguments.TextureMipMapFilterMode;

            State.DX.DirectX.Device->SetTextureStageState(TEXTURE_STAGE_0_INDEX,
                D3DTEXTURESTAGESTATETYPE::D3DTSS_MIPFILTER,
                (*State.InitializeArguments.TextureMipMapFilterMode == RendererTextureMipMapFilterMode::Simple)
                ? D3DTEXTUREFILTER::D3DFILTER_NEAREST : D3DTEXTUREFILTER::D3DFILTER_MIPNEAREST);
        }

        if (((State.DX.Mode.Mode ^ mode) & RendererMode::U0x2) != RendererMode::None)
        {
            SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ALPHABLENDENABLE, (mode & RendererMode::U0x2) != RendererMode::None);
            SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ALPHATESTENABLE, (mode & RendererMode::U0x2) != RendererMode::None);
        }

        if (((State.DX.Mode.Mode ^ mode) & RendererMode::U0x8) != RendererMode::None)
        {
            if ((mode & RendererMode::U0x8) == RendererMode::None)
            {
                SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_FOGENABLE, FALSE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_FOGVERTEXMODE, D3DFOGMODE::D3DFOG_NONE);
            }
            else
            {
                SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_FOGENABLE, TRUE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_FOGVERTEXMODE, D3DFOGMODE::D3DFOG_LINEAR);
            }

            SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_FOGTABLEMODE, D3DFOGMODE::D3DFOG_NONE);
        }

        if (((State.DX.Mode.Mode ^ mode) & (RendererMode::U0x80 | RendererMode::U0x40)) != RendererMode::None)
        {
            if ((mode & RendererMode::U0x80) != RendererMode::None && (mode & RendererMode::U0x40) != RendererMode::None)
            {
                SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ZENABLE, D3DZBUFFERTYPE::D3DZB_TRUE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ZWRITEENABLE, TRUE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ZFUNC, D3DCMPFUNC::D3DCMP_LESSEQUAL);
            }
            else if ((mode & RendererMode::U0x80) != RendererMode::None)
            {
                SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ZENABLE, D3DZBUFFERTYPE::D3DZB_TRUE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ZWRITEENABLE, TRUE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ZFUNC, D3DCMPFUNC::D3DCMP_ALWAYS);
            }
            else if ((mode & RendererMode::U0x40) != RendererMode::None)
            {
                SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ZENABLE, D3DZBUFFERTYPE::D3DZB_TRUE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ZWRITEENABLE, FALSE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ZFUNC, D3DCMPFUNC::D3DCMP_LESSEQUAL);
            }
            else
            {
                SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ZENABLE, D3DZBUFFERTYPE::D3DZB_FALSE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ZWRITEENABLE, FALSE);
                SetRendererState(D3DRENDERSTATETYPE::D3DRENDERSTATE_ZFUNC, D3DCMPFUNC::D3DCMP_ALWAYS);
            }
        }

        State.DX.Mode.Mode = mode;

        if (State.DX.Mode.Unk21 != *State.InitializeArguments.Unk21)
        {
            State.DX.Mode.Unk21 = *State.InitializeArguments.Unk21;

            if (*State.InitializeArguments.Unk27 == 0) // TODO constant
            {
                State.DX.Mode.UnknownValue101 = 256.0f / (f32)(*State.InitializeArguments.Unk21); // TODO constant
            }
            else
            {
                State.DX.Mode.UnknownValue101 = 1.0f / (f32)(*State.InitializeArguments.Unk21); // TODO constant
            }
        }

        if ((mode & RendererMode::U0x10) == RendererMode::None)
        {
            State.DX.Mode.UnknownValue104 = 0xff; // TODO constant
        }
        else
        {
            State.DX.Mode.UnknownValue104 = (*State.InitializeArguments.Unk11 - 256) >> 4;

            if (0xff < State.DX.Mode.UnknownValue104) // TODO constant
            {
                State.DX.Mode.UnknownValue105 = State.DX.Mode.UnknownValue104 - 256; // TODO constant

                if (0xff < State.DX.Mode.UnknownValue105) // TODO constant
                {
                    State.DX.Mode.UnknownValue105 = 0xff; // TODO constant
                }

                State.DX.Mode.UnknownValue104 = 0xff; // TODO constant

                return;
            }
        }

        State.DX.Mode.UnknownValue105 = 0;
    }

    BOOL AllocateStageTextures(void)
    {
        ReleaseAllocatedTextures();

        DDSURFACEDESC2 desc;

        for (u32 x = 0; x < MAX_STAGING_TEXTURE_COUNT; x++)
        {
            ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

            desc.dwSize = sizeof(DDSURFACEDESC2);
            desc.dwFlags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
            desc.dwHeight = State.DX.Surfaces.Staging.Dimensions[x];
            desc.dwWidth = State.DX.Surfaces.Staging.Dimensions[x];
            desc.ddsCaps = { .dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY };

            CopyMemory(&desc.ddpfPixelFormat, &State.DX.Surfaces.Staging.PixelFormat.Transparent, sizeof(DDPIXELFORMAT));

            if (State.DX.DirectX.Instance->CreateSurface(&desc, &State.DX.Surfaces.Staging.Transparent[x], NULL) != DD_OK) { return FALSE; }
        }

        State.DX.Surfaces.Masks.Transparent.R = 0;

        for (u32 x = desc.ddpfPixelFormat.dwRBitMask; (x & 1) == 0; x = x >> 1)
        {
            State.DX.Surfaces.Masks.Transparent.R = State.DX.Surfaces.Masks.Transparent.R + 1;
        }

        State.DX.Surfaces.Masks.Transparent.G = 0;

        for (u32 x = desc.ddpfPixelFormat.dwGBitMask; (x & 1) == 0; x = x >> 1)
        {
            State.DX.Surfaces.Masks.Transparent.G = State.DX.Surfaces.Masks.Transparent.G + 1;
        }

        State.DX.Surfaces.Masks.Transparent.B = 0;

        for (u32 x = desc.ddpfPixelFormat.dwBBitMask; (x & 1) == 0; x = x >> 1)
        {
            State.DX.Surfaces.Masks.Transparent.B = State.DX.Surfaces.Masks.Transparent.B + 1;
        }

        for (u32 x = 0; x < MAX_STAGING_TEXTURE_COUNT; x++)
        {
            ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

            desc.dwSize = sizeof(DDSURFACEDESC2);
            desc.dwFlags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
            desc.dwHeight = State.DX.Surfaces.Staging.Dimensions[x];
            desc.dwWidth = State.DX.Surfaces.Staging.Dimensions[x];
            desc.ddsCaps = { .dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY };

            CopyMemory(&desc.ddpfPixelFormat, &State.DX.Surfaces.Staging.PixelFormat.Opaque, sizeof(DDPIXELFORMAT));

            if (State.DX.DirectX.Instance->CreateSurface(&desc, &State.DX.Surfaces.Staging.Opaque[x], NULL) != DD_OK) { return FALSE; }
        }

        State.DX.Surfaces.Masks.Opaque.R = 0;

        for (u32 x = desc.ddpfPixelFormat.dwRBitMask; (x & 1) == 0; x = x >> 1)
        {
            State.DX.Surfaces.Masks.Opaque.R = State.DX.Surfaces.Masks.Opaque.R + 1;
        }

        State.DX.Surfaces.Masks.Opaque.G = 0;

        for (u32 x = desc.ddpfPixelFormat.dwGBitMask; (x & 1) == 0; x = x >> 1)
        {
            State.DX.Surfaces.Masks.Opaque.G = State.DX.Surfaces.Masks.Opaque.G + 1;
        }

        State.DX.Surfaces.Masks.Opaque.B = 0;

        for (u32 x = desc.ddpfPixelFormat.dwBBitMask; (x & 1) == 0; x = x >> 1)
        {
            State.DX.Surfaces.Masks.Opaque.B = State.DX.Surfaces.Masks.Opaque.B + 1;
        }

        State.DX.Surfaces.Count = 0;

        InitializeTextureCaches();

        return TRUE;
    }
}