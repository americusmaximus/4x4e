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

#include "App.Windows.hxx"
#include "Editor.hxx"
#include "Graphics.hxx"
#include "Logger.hxx"
#include "Memory.hxx"
#include "Renderer.hxx"
#include "Renderer.Module.hxx"
#include "Strings.hxx"

#define GRAPHICS_BUFFER_ALIGNMENT_VALUE 16
#define GRAPHICS_BUFFER_ALIGNMENT_MASK (~(GRAPHICS_BUFFER_ALIGNMENT_VALUE-1))

using namespace App::Windows;
using namespace Editor;
using namespace Logger;
using namespace Memory;
using namespace Renderer::Module;
using namespace Renderer;
using namespace Strings;

namespace Graphics
{
    GraphicsContainer GraphicsState;

    // 0x00596880
    BOOL ConstructGraphics(void)
    {
        *GraphicsState.DX.Buffers._Frame = (u32*)AllocateMemory(*AppWindowState._Width * *AppWindowState._Height * (*AppWindowState._BitsPerPixel >> 3));

        if (*GraphicsState.DX.Buffers._Frame == NULL) { LogError("Unable to allocate frame buffer."); }

        {
            *GraphicsState.DX.Buffers._Depth = (u32*)AllocateMemory(*AppWindowState._Width * *AppWindowState._Height * sizeof(u32) + GRAPHICS_BUFFER_ALIGNMENT_VALUE * sizeof(u32));

            if (*GraphicsState.DX.Buffers._Depth == NULL) { LogError("Unable to allocate depth buffer memory."); }

            *GraphicsState.DX.Buffers._DepthAligned = (u32*)(((addr)*GraphicsState.DX.Buffers._Depth + GRAPHICS_BUFFER_ALIGNMENT_VALUE) & GRAPHICS_BUFFER_ALIGNMENT_MASK);
        }

        if (*GraphicsState.DX._Palette != NULL)
        {
            (*GraphicsState.DX._Palette)->Release();
            *GraphicsState.DX._Palette = NULL;
        }

        if (*GraphicsState.DX.Surfaces._Back != NULL)
        {
            (*GraphicsState.DX.Surfaces._Back)->Release();
            *GraphicsState.DX.Surfaces._Back = NULL;
        }

        if (*GraphicsState.DX.Surfaces._Main != NULL)
        {
            (*GraphicsState.DX.Surfaces._Main)->Release();
            *GraphicsState.DX.Surfaces._Main = NULL;
        }

        if (*GraphicsState.DX._Instance != NULL)
        {
            (*GraphicsState.DX._Instance)->RestoreDisplayMode();
            (*GraphicsState.DX._Instance)->Release();

            *GraphicsState.DX._Instance = NULL;
        }

        if (DirectDrawCreate(NULL, GraphicsState.DX._Instance, NULL) == DD_OK)
        {
            (*GraphicsState.DX._Instance)->SetCooperativeLevel(*AppWindowState._HWND, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
        }

        InitializeRendererModule(*AppWindowState._HWND);

        return TRUE;
    }

    // 0x00596b40
    BOOL InitializeGraphics(const u32 width, const u32 height, const u32 bpp)
    {
        if (*GraphicsState.DX._Palette != NULL)
        {
            (*GraphicsState.DX._Palette)->Release();
            *GraphicsState.DX._Palette = NULL;
        }

        if (*GraphicsState.DX.Surfaces._Back != NULL)
        {
            (*GraphicsState.DX.Surfaces._Back)->Release();
            *GraphicsState.DX.Surfaces._Back = NULL;
        }

        if (*GraphicsState.DX.Surfaces._Main != NULL)
        {
            (*GraphicsState.DX.Surfaces._Main)->Release();
            *GraphicsState.DX.Surfaces._Main = NULL;
        }

        if (bpp != GRAPHICS_BITS_PER_PIXEL_8 && bpp != GRAPHICS_BITS_PER_PIXEL_16 && bpp != GRAPHICS_BITS_PER_PIXEL_32) { return FALSE; }

        *AppWindowState._Width = width;
        *AppWindowState._Height = height;
        *AppWindowState._BitsPerPixel = bpp;

        *GraphicsState.DX.Surfaces._IsActive = FALSE;

        *GraphicsState.DX.Surfaces._X = 0;
        *GraphicsState.DX.Surfaces._Y = 0;
        *GraphicsState.DX.Surfaces._Width = width;
        *GraphicsState.DX.Surfaces._Height = height;
        *GraphicsState.DX.Surfaces._BitsPerPixel = bpp;

        if (*GraphicsState.DX.Buffers._Frame != NULL)
        {
            ReleaseMemory1(*GraphicsState.DX.Buffers._Frame);
            *GraphicsState.DX.Buffers._Frame = NULL;
        }

        if (*GraphicsState.DX.Buffers._Depth != NULL)
        {
            ReleaseMemory1(*GraphicsState.DX.Buffers._Depth);

            *GraphicsState.DX.Buffers._Depth = NULL;
            *GraphicsState.DX.Buffers._DepthAligned = NULL;
        }

        {
            *GraphicsState.DX.Buffers._Frame = (u32*)AllocateMemory(*AppWindowState._Width * *AppWindowState._Height * (*AppWindowState._BitsPerPixel >> 3));

            if (*GraphicsState.DX.Buffers._Frame == NULL) { LogError("Unable to allocate frame buffer memory."); }
        }

        {
            *GraphicsState.DX.Buffers._Depth = (u32*)AllocateMemory(*AppWindowState._Width * *AppWindowState._Height * sizeof(u32) + GRAPHICS_BUFFER_ALIGNMENT_VALUE);

            if (*GraphicsState.DX.Buffers._Depth == NULL) { LogError("Unable to allocate depth buffer memory."); }

            *GraphicsState.DX.Buffers._DepthAligned = (u32*)(((addr)*GraphicsState.DX.Buffers._Depth + GRAPHICS_BUFFER_ALIGNMENT_VALUE) & GRAPHICS_BUFFER_ALIGNMENT_MASK);
        }

        if ((*GraphicsState.DX._Instance)->SetDisplayMode(*AppWindowState._Width, *AppWindowState._Height, bpp) != DD_OK) { return FALSE; }

        {
            DDSURFACEDESC desc =
            {
                .dwSize = sizeof(DDSURFACEDESC),
                .dwFlags = DDSD_BACKBUFFERCOUNT | DDSD_CAPS,
                .dwBackBufferCount = 2,
                .ddsCaps = {.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_3DDEVICE | DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX }
            };

            if ((*GraphicsState.DX._Instance)->CreateSurface(&desc, GraphicsState.DX.Surfaces._Main, NULL) != DD_OK)
            {
                desc.dwFlags = DDSD_CAPS;

                if ((*GraphicsState.DX._Instance)->CreateSurface(&desc, GraphicsState.DX.Surfaces._Main, NULL) != DD_OK)
                {
                    desc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

                    if ((*GraphicsState.DX._Instance)->CreateSurface(&desc, GraphicsState.DX.Surfaces._Main, NULL) != DD_OK) { return FALSE; }
                }
            }
        }

        {
            DDSCAPS caps = { .dwCaps = DDSCAPS_BACKBUFFER };

            if ((*GraphicsState.DX.Surfaces._Main)->GetAttachedSurface(&caps, GraphicsState.DX.Surfaces._Back) != DD_OK) { LogError("Unable to create back buffer."); }
        }

        for (u32 x = 0; x < *AppWindowState._Height; x++)
        {
            const auto coffset = x * *AppWindowState._Width * (*AppWindowState._BitsPerPixel >> 3);
            GraphicsState.Pixels._Colors[x] = (u32*)(((addr)*GraphicsState.DX.Buffers._Frame) + coffset);

            const auto doffset = x * *AppWindowState._Width * sizeof(u32);
            GraphicsState.Pixels._Depth[x] = (u32*)(((addr)*GraphicsState.DX.Buffers._DepthAligned) + doffset);
        }

        SelectGraphicsDimensions(0, 0, *AppWindowState._Width - 1, *AppWindowState._Height - 1);

        FUN_00597700a();

        {
            DDSURFACEDESC desc = { .dwSize = sizeof(DDSURFACEDESC) };

            if ((*GraphicsState.DX.Surfaces._Back)->Lock(NULL, &desc, DDLOCK_WAIT, NULL) != DD_OK) { LogError("Unable to lock back buffer."); }

            const auto length = *GraphicsState.DX.Surfaces._Width * (*GraphicsState.DX.Surfaces._BitsPerPixel >> 3);

            for (u32 x = 0; x < *GraphicsState.DX.Surfaces._Height; x++)
            {
                ZeroMemory((void*)((addr)desc.lpSurface + x * desc.lPitch), length);
            }

            if ((*GraphicsState.DX.Surfaces._Back)->Unlock(NULL) != DD_OK) { LogError("Unable to unlock back buffer."); }
        }

        *GraphicsState._Unknown1 = TRUE;

        FUN_00597830a();

        return TRUE;
    }

    // 0x00597350
    // a.k.a. openScreenDevice
    void LockGraphicsScreen(void)
    {
        if (*GraphicsState.DX.Surfaces._IsActive || *RendererState._IsActive) { return; }

        DDSURFACEDESC desc = { .dwSize = sizeof(DDSURFACEDESC) };

        if ((*GraphicsState.DX.Surfaces._Back)->Lock(NULL, &desc, DDLOCK_WAIT, NULL) != DD_OK) { LogError("Unable to lock the screen."); }

        auto offset = 0U;

        if (!*GraphicsState.DX.Surfaces._IsActive)
        {
            offset = *GraphicsState.DX.Surfaces._X * (*GraphicsState.DX.Surfaces._BitsPerPixel >> 3);
        }

        for (u32 x = 0; x < *AppWindowState._Height; x++)
        {
            GraphicsState.Pixels._Colors[x] = (u32*)(((addr)desc.lpSurface) + x * desc.lPitch + offset);
        }
    }

    // 0x005969f0
    void ReleaseGraphics(void)
    {
        if (*GraphicsState.DX.Buffers._Frame != NULL)
        {
            ReleaseMemory1(*GraphicsState.DX.Buffers._Frame);
            *GraphicsState.DX.Buffers._Frame = NULL;
        }

        if (*GraphicsState.DX.Buffers._Depth != NULL)
        {
            ReleaseMemory1(*GraphicsState.DX.Buffers._Depth);
            *GraphicsState.DX.Buffers._Depth = NULL;
        }

        ReleaseRenderer();

        if (*GraphicsState.DX._Palette != NULL)
        {
            (*GraphicsState.DX._Palette)->Release();
            *GraphicsState.DX._Palette = NULL;
        }

        if (*GraphicsState.DX.Surfaces._Back != NULL)
        {
            (*GraphicsState.DX.Surfaces._Back)->Release();
            *GraphicsState.DX.Surfaces._Back = NULL;
        }

        if (*GraphicsState.DX.Surfaces._Main != NULL)
        {
            (*GraphicsState.DX.Surfaces._Main)->Release();
            *GraphicsState.DX.Surfaces._Main = NULL;
        }

        if (*GraphicsState.DX._Instance != NULL)
        {
            (*GraphicsState.DX._Instance)->RestoreDisplayMode();
            (*GraphicsState.DX._Instance)->Release();

            *GraphicsState.DX._Instance = NULL;
        }
    }

    // 0x00597ab0
    void RestoreGraphicsVideoMode(void)
    {
        if (*RendererState.Options._IsUsingModule && *GraphicsState._IsActive)
        {
            if (*RendererModule._VideoSave != NULL)
            {
                (*RendererModule._VideoSave)();

                *RendererState._IsActive = FALSE;

                return;
            }

            if (!EqualStrings(RendererState.Options._ModuleFileName, LEGACY_OPENGL_RENDERER_MODULE_NAME))
            {
                RestoreRendererVideoMode();
            }

            ShowWindow(*AppWindowState._HWND, SW_MINIMIZE);
        }
    }

    // 0x00401790
    void SelectGraphicsDimensions(const u32 x, const u32 y, const u32 width, const u32 height)
    {
        *EditorState.Windows.Active._Unk005 = x;
        *EditorState.Windows.Active._Unk006 = y;
        *EditorState.Windows.Active._Unk007 = width;
        *EditorState.Windows.Active._Unk008 = height;

        *EditorState.Windows.Active._Unk017 = (width - x) + 1;
        *EditorState.Windows.Active._Unk018 = (height - y) + 1;

        const auto uw = (*EditorState.Windows.Active._Unk017 / 2 & 0xffffU) - 1; // TODO
        const auto uh = (*EditorState.Windows.Active._Unk018 / 2 & 0xffffU) - 1; // TODO

        *EditorState.Windows.Active._Unk013 = (uw + x + 1) * 0x10000;            // TODO
        *EditorState.Windows.Active._Unk014 = (uh + y + 1) * 0x10000;            // TODO
        *EditorState.Windows.Active._Unk015 = uw * 0x10000;                      // TODO
        *EditorState.Windows.Active._Unk016 = uh * -0x10000;                     // TODO
    }

    // 0x00597440
    // a.k.a. closeScreenDevice
    void UnlockGraphicsScreen(void)
    {
        if (*GraphicsState.DX.Surfaces._IsActive) { return; }

        if (*RendererState._IsActive)
        {
            *GraphicsState._Unknown1 = TRUE;

            return;
        }

        for (u32 x = 0; x < *AppWindowState._Height; x++)
        {
            const auto coffset = x * *AppWindowState._Width * (*AppWindowState._BitsPerPixel >> 3);
            GraphicsState.Pixels._Colors[x] = (u32*)(((addr)*GraphicsState.DX.Buffers._Frame) + coffset);

            const auto doffset = x * *AppWindowState._Width * sizeof(u32);
            GraphicsState.Pixels._Depth[x] = (u32*)(((addr)*GraphicsState.DX.Buffers._DepthAligned) + doffset);
        }

        if ((*GraphicsState.DX.Surfaces._Back)->Unlock(NULL) != DD_OK) { LogError("Unable to unlock the screen."); }

        *GraphicsState._Unknown1 = TRUE;
    }
}