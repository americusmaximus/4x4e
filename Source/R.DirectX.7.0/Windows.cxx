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

#include "Module.hxx"
#include "Quit.hxx"
#include "Renderers.Basic.hxx"

#include <stdio.h>

using namespace Renderers::Modules;
using namespace Renderers;

namespace Renderer::Module
{
    extern "C" void AcquireDisplayContext(HDC*) { } // NOTE: Not being called by the application.

    extern "C" void ReleaseDisplayContext(void) { } // NOTE: Not being called by the application.

    extern "C" u32 RestoreVideoMode(void)
    {
        if (State.DX.Lock.IsActive)
        {
            if (!DX::UnlockBackSurface()) { Quit("Unable to unlock back buffer."); }
        }

        State.DX.DirectX.Instance->FlipToGDISurface();
        State.DX.DirectX.Instance->RestoreDisplayMode();
        State.DX.DirectX.Instance->SetCooperativeLevel(State.Window.HWND, DDSCL_NORMAL);

        DX::Release();

        SHOWCURSOR(TRUE);

        return TRUE;
    }

    extern "C" u32 RestoreVideoModeX(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 SelectVideoMode(u32 * *pixels)
    {
        State.DX.Mode.Mode = RendererMode::None;

        State.DX.Mode.TextureFilterMode = RendererTextureFilterMode::Nearest;
        State.DX.Mode.BlendMode = RendererBlendMode::OneMinusSourceAlpha;
        State.DX.Mode.TextureMipMapFilterMode = RendererTextureMipMapFilterMode::Complex;

        State.DX.Mode.Unk21 = 0; // TODO
        State.DX.Mode.UnknownValue101 = 0.0f; // TODO

        if (State.DX.Devices.Current.Memory.Total == 0)
        {
            *State.InitializeArguments.AcceleratedGraphicsPortMode = RendererAcceleratedGraphicsPortMode::Auto;
        }

        if (*State.InitializeArguments.AcceleratedGraphicsPortMode == RendererAcceleratedGraphicsPortMode::Auto)
        {
            State.DX.Surfaces.Staging.Dimensions[0] = TEXTURE_DIMENSIONS_SIZE_512;
            State.DX.Surfaces.Staging.Dimensions[1] = TEXTURE_DIMENSIONS_SIZE_256;
            State.DX.Surfaces.Staging.Dimensions[2] = TEXTURE_DIMENSIONS_SIZE_128;
            State.DX.Surfaces.Staging.Dimensions[3] = TEXTURE_DIMENSIONS_SIZE_64;
            State.DX.Surfaces.Staging.Dimensions[4] = TEXTURE_DIMENSIONS_SIZE_32;
            State.DX.Surfaces.Staging.Dimensions[5] = TEXTURE_DIMENSIONS_SIZE_16;
            State.DX.Surfaces.Staging.Dimensions[6] = TEXTURE_DIMENSIONS_SIZE_8;
            State.DX.Surfaces.Staging.Dimensions[7] = TEXTURE_DIMENSIONS_SIZE_4;
            State.DX.Surfaces.Staging.Dimensions[8] = TEXTURE_DIMENSIONS_SIZE_2;
            State.DX.Surfaces.Staging.Dimensions[9] = TEXTURE_DIMENSIONS_SIZE_1;
        }
        else
        {
            State.DX.Surfaces.Staging.Dimensions[0] = TEXTURE_DIMENSIONS_SIZE_1024;
            State.DX.Surfaces.Staging.Dimensions[1] = TEXTURE_DIMENSIONS_SIZE_512;
            State.DX.Surfaces.Staging.Dimensions[2] = TEXTURE_DIMENSIONS_SIZE_256;
            State.DX.Surfaces.Staging.Dimensions[3] = TEXTURE_DIMENSIONS_SIZE_128;
            State.DX.Surfaces.Staging.Dimensions[4] = TEXTURE_DIMENSIONS_SIZE_64;
            State.DX.Surfaces.Staging.Dimensions[5] = TEXTURE_DIMENSIONS_SIZE_32;
            State.DX.Surfaces.Staging.Dimensions[6] = TEXTURE_DIMENSIONS_SIZE_16;
            State.DX.Surfaces.Staging.Dimensions[7] = TEXTURE_DIMENSIONS_SIZE_8;
            State.DX.Surfaces.Staging.Dimensions[8] = TEXTURE_DIMENSIONS_SIZE_4;
            State.DX.Surfaces.Staging.Dimensions[9] = TEXTURE_DIMENSIONS_SIZE_2;
        }

        State.Settings.PremultiplyColorAlphaMode = Settings::PremultiplyColorAlphaOptionsMode::Off;
        Settings::ReadValue(RENDERER_MODULE_OPTIONS_GRAPHICS_SECTION_NAME,
            RENDERER_MODULE_OPTIONS_GRAPHICS_PREMULTIPLY_COLOR_ALPHA_NAME, (u32*)&State.Settings.PremultiplyColorAlphaMode);

        State.Settings.AutoMipMapMode = Settings::AutoMipMappingOptionsMode::Off;
        Settings::ReadValue(RENDERER_MODULE_OPTIONS_GRAPHICS_SECTION_NAME,
            RENDERER_MODULE_OPTIONS_GRAPHICS_AUTO_MIP_MAPPING_NAME, (u32*)&State.Settings.AutoMipMapMode);

        State.Pixels.Pixels = pixels;

        for (u32 x = 0; x < State.Window.Height; x++)
        {
            State.Pixels.Values[x] = pixels[x];
        }

        DX::Release();

        if (DX::DXC(State.DX.DirectX.Instance->SetCooperativeLevel(State.Window.HWND, DDSCL_FPUPRESERVE | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN),
            "Unable to set cooperative level.") != DD_OK)
        {
            return FALSE;
        }

        if (DX::DXC(State.DX.DirectX.Instance->SetDisplayMode(State.Window.Width, State.Window.Height, State.Window.BitsPerPixel, 0, DDSDM_NONE),
            "Unable to set display mode.") != DD_OK)
        {
            return FALSE;
        }

        ZeroMemory(&State.DX.Surfaces.Descriptor, sizeof(DDSURFACEDESC2));

        State.DX.Surfaces.Descriptor.dwSize = sizeof(DDSURFACEDESC2);
        State.DX.Surfaces.Descriptor.dwFlags = DDSD_BACKBUFFERCOUNT | DDSD_CAPS;
        State.DX.Surfaces.Descriptor.dwBackBufferCount = 2;
        State.DX.Surfaces.Descriptor.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_3DDEVICE | DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

        if (*State.InitializeArguments.AcceleratedGraphicsPortMode == RendererAcceleratedGraphicsPortMode::Auto
            || *State.InitializeArguments.AcceleratedGraphicsPortMode == RendererAcceleratedGraphicsPortMode::X1)
        {
            State.DX.Surfaces.Descriptor.dwBackBufferCount = 1;
        }

        if (State.DX.DirectX.Instance->CreateSurface(&State.DX.Surfaces.Descriptor, &State.DX.Surfaces.Main, NULL) != DD_OK)
        {
            State.DX.Surfaces.Descriptor.dwBackBufferCount = 1;

            if (State.DX.DirectX.Instance->CreateSurface(&State.DX.Surfaces.Descriptor, &State.DX.Surfaces.Main, NULL) != DD_OK)
            {
                State.DX.DirectX.Instance->RestoreDisplayMode();

                return FALSE;
            }
        }

        {
            DDSCAPS2 caps = { .dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_BACKBUFFER };

            if (State.DX.Surfaces.Main->GetAttachedSurface(&caps, &State.DX.Surfaces.Back) != DD_OK)
            {
                State.DX.DirectX.Instance->RestoreDisplayMode();

                return FALSE;
            }
        }

        if (!DX::InitializeDirectX())
        {
            State.DX.DirectX.Instance->RestoreDisplayMode();

            return FALSE;
        }

        DDPIXELFORMAT pf;

        ZeroMemory(&pf, sizeof(DDPIXELFORMAT));

        DX::DXC(State.DX.DirectX.DirectX->EnumZBufferFormats(IID_IDirect3DHALDevice, DX::EnumerateDepthBuffers, &pf),
            "Unable to enumerate depth buffer formats.");

        if (pf.dwSize != sizeof(DDPIXELFORMAT))
        {
            if (State.DX.DirectX.Instance->RestoreDisplayMode() == DD_OK)
            {
                Quit("Unable to acquire acceptable depth buffer format.");
            }

            return FALSE;
        }

        DDSURFACEDESC2 desc =
        {
            .dwSize = sizeof(DDSURFACEDESC2),
            .dwFlags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS,
            .dwHeight = State.Window.Height,
            .dwWidth = State.Window.Width,
            .ddsCaps = {.dwCaps = DDSCAPS_ZBUFFER }
        };

        CopyMemory(&desc.ddpfPixelFormat, &pf, sizeof(DDPIXELFORMAT));

        if (State.DX.DirectX.Instance->CreateSurface(&desc, &State.DX.Surfaces.Depth, NULL) == DD_OK)
        {
            State.DX.Surfaces.DepthBitsPerPixel = pf.dwZBufferBitDepth;

            if (State.DX.Surfaces.Back->AddAttachedSurface(State.DX.Surfaces.Depth) == DD_OK)
            {
                DDBLTFX ltfx = { .dwSize = sizeof(DDBLTFX) };

                State.DX.Surfaces.Main->Blt(NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &ltfx);
                State.DX.Surfaces.Back->Blt(NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &ltfx);

                if (!DX::Initialize())
                {
                    Quit("Unable to initialize Direct3D in %d x %d x %d bpp. Your video card does not support this mode.",
                        State.Window.Width, State.Window.Height, State.Window.BitsPerPixel);
                }

                SetCursorPos(State.Window.Width - 1, State.Window.Height - 1);

                SHOWCURSOR(FALSE);

                Clear();
                Toggle();

                return TRUE;
            }
            else
            {
                State.DX.DirectX.Instance->RestoreDisplayMode();

                MessageBoxA(NULL, "Unable to attach depth buffer to back buffer. Please restart the game and turn off hardware acceleration.", RENDERER_MODULE_TITLE_NAME, MB_ICONHAND | MB_OK);

                ExitProcess(MODULE_EXIT_FAILURE);
            }
        }
        else
        {
            State.DX.DirectX.Instance->RestoreDisplayMode();

            MessageBoxA(NULL, "Unable to create a depth buffer. Your video card does not support a 16-bit depth buffer. Please restart the game and turn off hardware acceleration.", RENDERER_MODULE_TITLE_NAME, MB_ICONHAND | MB_OK);

            ExitProcess(MODULE_EXIT_FAILURE);
        }

        return TRUE;
    }

    extern "C" u32 SelectVideoMode2(const u32 width, const u32 height, const u32 bpp, u32 * *pixels)
    {
        State.Window.Width = width;
        State.Window.Height = height;
        State.Window.BitsPerPixel = bpp;

        return SelectVideoMode(pixels);
    }

    extern "C" u32 SelectVideoModeX(const u32, const u32, const u32, const u32) { return FALSE; } // NOTE: Not being called by the application.
}