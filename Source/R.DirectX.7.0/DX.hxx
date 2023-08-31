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

#pragma once

#include "Basic.hxx"
#include "Native.Basic.hxx"
#include "Renderers.Basic.hxx"
#include "Renderers.Basic.Vertexes.hxx"

#define DIRECTDRAW_VERSION 0x700
#include <ddraw.h>

#define DIRECT3D_VERSION 0x700
#include <d3d.h>

#define DDGDI_NONE 0
#define DDSDM_NONE 0
#define D3DDP_NONE 0
#define DDBLT_NONE 0

#define MAX_TEXTURE_CACHE_COUNT 12

#define DEFAULT_TEXTURE_INDEX 0
#define INVALID_TEXTURE_INDEX (-1)

#define MAX_TEXTURE_COUNT 4096

#define MAX_TEXTURE_CACHE_ITEM_NAME_LENGTH 64
#define MAX_TEXTURE_CACHE_ITEM_COUNT MAX_TEXTURE_COUNT

#define MAX_PIXEL_PALETTE_BUFFER_SIZE 256
#define MAX_PIXEL_BUFFER_SIZE (1024 * 1024 + 512 * 512 + 256 * 256 + 128 * 128 + 64 * 64 + 32 * 32 + 16 * 16 + 8 * 8 + 4 * 4 + 2 * 2 + 1 * 1)

#define MAX_STATE_VARIABLE_INDEX_MASK (0xff)
#define MAX_STATE_VARIABLE_COUNT 256

#define TEXTURE_STAGE_0_INDEX 0
#define TEXTURE_STAGE_1_INDEX 1

#define MAX_STAGING_TEXTURE_COUNT 10

namespace DX
{
    struct DXDevice
    {
        GUID* Index;
        GUID ID;

        u32 DeviceID;
        u32 VendorID;

        char Driver[MAX_DEVICE_IDENTIFIER_STRING_LENGTH];
        char Description[MAX_DEVICE_IDENTIFIER_STRING_LENGTH];
    };

    OPTIONS(DXPixelFormat, u32)
    {
        None = 0,
            Transparent = 1,
            Opaque = 2
    };

    struct DXTexture
    {
        u32 Index;
        char Name[MAX_TEXTURE_CACHE_ITEM_NAME_LENGTH];
        u32 Version;
    };

    struct DXTextureCache
    {
        BOOL IsOpaque;
        u32 Capacity;
        u32 Dimensions;
        u32 Count;
        u32 Version;
        DXTexture Textures[MAX_TEXTURE_CACHE_ITEM_COUNT];
        u16 Indexes[MAX_TEXTURE_CACHE_ITEM_COUNT];
    };

    struct DXTextureSurface
    {
        IDirectDrawSurface7* Surface;
        BOOL IsOpaque;
    };

    struct DXState
    {
        struct
        {
            D3DCULL CullMode = D3DCULL::D3DCULL_CCW;
            Renderers::RendererMode Mode = Renderers::RendererMode::None;
            Renderers::RendererTextureFilterMode TextureFilterMode = Renderers::RendererTextureFilterMode::Nearest;
            Renderers::RendererBlendMode BlendMode;
            Renderers::RendererTextureMipMapFilterMode TextureMipMapFilterMode = Renderers::RendererTextureMipMapFilterMode::Complex;

            u32 Unk21;
            f32 UnknownValue101;
            u32 UnknownValue104;
            u32 UnknownValue105;
        } Mode;

        struct
        {
            Graphics::u32xRGB Color = { 0xff, 0xff, 0xff };
        } Fog;

        struct
        {
            struct
            {
                u32 Index;

                struct
                {
                    u32 Local, Total;
                } Memory;
            } Current;

            struct
            {
                u32 Count;
                DXDevice Details[MAX_DEVICE_COUNT];
            } Available;
        } Devices;

        struct
        {
            IDirectDraw7* Instance;
            IDirect3D7* DirectX;
            IDirect3DDevice7* Device;

            struct
            {
                D3DRENDERSTATETYPE States[MAX_STATE_VARIABLE_COUNT];
                DWORD Values[MAX_STATE_VARIABLE_COUNT];
            } State;
        } DirectX;

        struct
        {
            u32 Index;
            DDSURFACEDESC2 Descriptor;

            IDirectDrawSurface7* Current;
            IDirectDrawSurface7* Back;
            IDirectDrawSurface7* Main;
            IDirectDrawSurface7* Depth;

            u32 DepthBitsPerPixel;

            struct
            {
                u32 Dimensions[MAX_STAGING_TEXTURE_COUNT];

                struct
                {
                    DDPIXELFORMAT Opaque;
                    DDPIXELFORMAT Transparent;
                } PixelFormat;

                IDirectDrawSurface7* Transparent[MAX_STAGING_TEXTURE_COUNT];
                IDirectDrawSurface7* Opaque[MAX_STAGING_TEXTURE_COUNT];
            } Staging;

            struct
            {
                Graphics::u32xRGB Color;
                Graphics::u32xRGB Transparent;
                Graphics::u32xRGB Opaque;
            } Masks;

            u32 Count;
            DXTextureSurface Surfaces[MAX_TEXTURE_COUNT];
        } Surfaces;

        struct
        {
            u32* Colors;

            struct
            {
                u8* RGB;
                u16* RGBA;

                u8* Alphas;
                u8* Colors;
                u8* Indexes;

                u32 Palette[MAX_PIXEL_PALETTE_BUFFER_SIZE];
            } Palette;

            u32 Pixels[MAX_PIXEL_BUFFER_SIZE];
        } Pixels;

        struct
        {
            struct
            {
                u32 Count;
                u16 Indexes[RENDERER_SIXTEEN_THOUSAND_ELEMENT_COUNT];
            } Indexes;

            struct
            {
                u32 Count;
                Renderers::RTLVX Vertexes[RENDERER_SIXTEEN_THOUSAND_ELEMENT_COUNT];
            } Vertexes;
        } Data;

        struct
        {
            BOOL IsActive;
        } Scene;

        struct
        {
            BOOL IsActive;
            BOOL IsBackSurfaceLockActive;
        } Lock;

        DXTextureCache* Caches[MAX_TEXTURE_CACHE_COUNT];
    };

    HRESULT DXC(const HRESULT code, const char* message);

    BOOL AcquireDevice(GUID* id);
    BOOL Initialize(void);
    BOOL InitializeDirectX(void);
    BOOL InitializeRenderState(void);
    BOOL LockBackSurface(void);
    BOOL LockSurface(IDirectDrawSurface7* surface, DDSURFACEDESC2* desc);
    BOOL UnlockBackSurface(void);
    BOOL UnlockSurface(IDirectDrawSurface7* surface);
    BOOL WINAPI EnumerateDevices(GUID* id, LPSTR name, LPSTR desc, LPVOID ctx, HMONITOR monitor);
    HRESULT CALLBACK EnumerateDepthBuffers(LPDDPIXELFORMAT format, LPVOID ctx);
    HRESULT CALLBACK EnumerateTextureFormats(LPDDPIXELFORMAT format, LPVOID ctx);
    u32 AcquireColorBitsPerPixel(const u32 value);
    u32 Alpha(const f32 value);
    u32 Color(const f32 r, const f32 g, const f32 b, const f32 a);
    void AcquireRendererMode(Renderers::RendererMode* mode);
    void Release(void);
    void ReleaseAllocatedTextures(void);
    void Render(void);
    void SelectRendererMode(const Renderers::RendererMode mode);
    void SetRendererState(const D3DRENDERSTATETYPE state, const DWORD value);
    void UpdateRendererMode(void);
}