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
#include "Settings.hxx"
#include "Textures.hxx"

#include <stdio.h>

#define DAFAULT_TEXTURE_CACHE_TRANSPARENT_X32_CAPACITY 0
#define DAFAULT_TEXTURE_CACHE_TRANSPARENT_X64_CAPACITY 512
#define DAFAULT_TEXTURE_CACHE_TRANSPARENT_X128_CAPACITY 128
#define DAFAULT_TEXTURE_CACHE_TRANSPARENT_X256_CAPACITY 32
#define DAFAULT_TEXTURE_CACHE_TRANSPARENT_X512_CAPACITY 8
#define DAFAULT_TEXTURE_CACHE_TRANSPARENT_X1024_CAPACITY 0

#define DAFAULT_TEXTURE_CACHE_OPAQUE_X32_CAPACITY 0
#define DAFAULT_TEXTURE_CACHE_OPAQUE_X64_CAPACITY 512
#define DAFAULT_TEXTURE_CACHE_OPAQUE_X128_CAPACITY 128
#define DAFAULT_TEXTURE_CACHE_OPAQUE_X256_CAPACITY 32
#define DAFAULT_TEXTURE_CACHE_OPAQUE_X512_CAPACITY 8
#define DAFAULT_TEXTURE_CACHE_OPAQUE_X1024_CAPACITY 0

#define TEXTURE_CACHE_INDEX_MASK (0xfff)

#define TEXTURE_SURFACE_X32_INDEX 5
#define TEXTURE_SURFACE_X64_INDEX 4
#define TEXTURE_SURFACE_X128_INDEX 3
#define TEXTURE_SURFACE_X256_INDEX 2
#define TEXTURE_SURFACE_X512_INDEX 1
#define TEXTURE_SURFACE_X1024_INDEX 0

#define MAX_TEXTURE_MIPMAP_LEVEL_COUNT 6

using namespace Renderer::Module;
using namespace Renderers;
using namespace Settings;

namespace DX
{
    u32 AcquireTextureMipMapCount(const u32 dimensions)
    {
        switch (dimensions)
        {
        case TEXTURE_DIMENSIONS_SIZE_16: { return TEXTURE_MIP_MAP_COUNT_4; }
        case TEXTURE_DIMENSIONS_SIZE_32: { return TEXTURE_MIP_MAP_COUNT_5; }
        case TEXTURE_DIMENSIONS_SIZE_64: { return TEXTURE_MIP_MAP_COUNT_6; }
        case TEXTURE_DIMENSIONS_SIZE_128: { return TEXTURE_MIP_MAP_COUNT_7; }
        case TEXTURE_DIMENSIONS_SIZE_256: { return TEXTURE_MIP_MAP_COUNT_8; }
        case TEXTURE_DIMENSIONS_SIZE_512: { return TEXTURE_MIP_MAP_COUNT_9; }
        case TEXTURE_DIMENSIONS_SIZE_1024: { return TEXTURE_MIP_MAP_COUNT_10; }
        }

        return MIN_TEXTURE_MIP_LEVEL_COUNT;
    }

    s32 AllocateTexture(const u32 dimensions, const BOOL opaque)
    {
        const auto dims =
            *State.InitializeArguments.AcceleratedGraphicsPortMode == RendererAcceleratedGraphicsPortMode::Auto
            ? dimensions / 2 : dimensions;

        const auto pf = opaque ? &State.DX.Surfaces.Staging.PixelFormat.Opaque : &State.DX.Surfaces.Staging.PixelFormat.Transparent;

        DDSURFACEDESC2 desc =
        {
            .dwSize = sizeof(DDSURFACEDESC2),
            .dwFlags = DDSD_TEXTURESTAGE | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS,
            .dwHeight = dims,
            .dwWidth = dims,
            .ddsCaps = {.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE }
        };

        CopyMemory(&desc.ddpfPixelFormat, pf, sizeof(DDPIXELFORMAT));

        if ((dimensions & State.Settings.MipMapTextureSize) == 0)
        {
            if (State.Settings.AutoMipMapMode == AutoMipMappingOptionsMode::Off)
            {
                desc.dwFlags = DDSD_TEXTURESTAGE | DDSD_MIPMAPCOUNT | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
                desc.dwMipMapCount = 1;
                desc.ddsCaps.dwCaps = DDSCAPS_MIPMAP | DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE | DDSCAPS_COMPLEX;
            }
        }
        else
        {
            desc.dwFlags = DDSD_TEXTURESTAGE | DDSD_MIPMAPCOUNT | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
            desc.dwMipMapCount = AcquireTextureMipMapCount(dims) - 3;
            desc.ddsCaps.dwCaps = DDSCAPS_MIPMAP | DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE | DDSCAPS_COMPLEX;
        }

        State.DX.Surfaces.Surfaces[State.DX.Surfaces.Count].IsOpaque = opaque;

        if (State.DX.DirectX.Instance->CreateSurface(&desc,
            &State.DX.Surfaces.Surfaces[State.DX.Surfaces.Count].Surface, NULL) != DD_OK)
        {
            return INVALID_TEXTURE_INDEX;
        }

        const auto result = State.DX.Surfaces.Count;

        State.DX.Surfaces.Count = State.DX.Surfaces.Count + 1;

        if (MAX_TEXTURE_COUNT < State.DX.Surfaces.Count)
        {
            Quit("Unable to allocate texture, the maximum is %d.", MAX_TEXTURE_COUNT);
        }

        return result;
    }

    void InitializeTextureCache(DXTextureCache* self, const u32 capacity, const u32 dimensions, const BOOL opaque)
    {
        self->IsOpaque = opaque;
        self->Capacity = capacity;
        self->Dimensions = dimensions;

        self->Count = 0;
        self->Version = 0;

        ZeroMemory(&self->Textures, MAX_TEXTURE_CACHE_ITEM_COUNT * sizeof(DXTexture));
        ZeroMemory(&self->Indexes, MAX_TEXTURE_CACHE_ITEM_COUNT * sizeof(u16));

        for (u32 x = 0; x < self->Capacity; x++)
        {
            const auto indx = AllocateTexture(dimensions, opaque);

            if (indx == INVALID_TEXTURE_INDEX) { break; }

            self->Textures[x].Index = indx;
        }
    }

    void InitializeTextureCaches(void)
    {
        u32 caps[MAX_TEXTURE_CACHE_COUNT] =
        {
            DAFAULT_TEXTURE_CACHE_TRANSPARENT_X32_CAPACITY,
            DAFAULT_TEXTURE_CACHE_TRANSPARENT_X64_CAPACITY,
            DAFAULT_TEXTURE_CACHE_TRANSPARENT_X128_CAPACITY,
            DAFAULT_TEXTURE_CACHE_TRANSPARENT_X256_CAPACITY,
            DAFAULT_TEXTURE_CACHE_TRANSPARENT_X512_CAPACITY,
            DAFAULT_TEXTURE_CACHE_TRANSPARENT_X1024_CAPACITY,

            DAFAULT_TEXTURE_CACHE_OPAQUE_X32_CAPACITY,
            DAFAULT_TEXTURE_CACHE_OPAQUE_X64_CAPACITY,
            DAFAULT_TEXTURE_CACHE_OPAQUE_X128_CAPACITY,
            DAFAULT_TEXTURE_CACHE_OPAQUE_X256_CAPACITY,
            DAFAULT_TEXTURE_CACHE_OPAQUE_X512_CAPACITY,
            DAFAULT_TEXTURE_CACHE_OPAQUE_X1024_CAPACITY
        };

        State.Settings.MipMapTextureSize = 0;

        {
            auto f = fopen(RENDERER_MODULE_OPTIONS_FILE_NAME, "rt");

            if (f == NULL)
            {
                f = fopen(RENDERER_MODULE_OPTIONS_FILE_NAME, "wt");

                if (f != NULL)
                {
                    fprintf(f, "[%s]\n", RENDERER_MODULE_OPTIONS_TEXTURES_SECTION_NAME);

                    fprintf(f, RENDERER_MODULE_OPTIONS_TEXTURES_32_COUNT_TEMPLATE_NAME, caps[TEXTURE_CACHE_TRANSPARENT_X32_INDEX]);
                    fprintf(f, RENDERER_MODULE_OPTIONS_TEXTURES_64_COUNT_TEMPLATE_NAME, caps[TEXTURE_CACHE_TRANSPARENT_X64_INDEX]);
                    fprintf(f, RENDERER_MODULE_OPTIONS_TEXTURES_128_COUNT_TEMPLATE_NAME, caps[TEXTURE_CACHE_TRANSPARENT_X128_INDEX]);
                    fprintf(f, RENDERER_MODULE_OPTIONS_TEXTURES_256_COUNT_TEMPLATE_NAME, caps[TEXTURE_CACHE_TRANSPARENT_X256_INDEX]);
                    fprintf(f, RENDERER_MODULE_OPTIONS_TEXTURES_512_COUNT_TEMPLATE_NAME, caps[TEXTURE_CACHE_TRANSPARENT_X512_INDEX]);
                    fprintf(f, RENDERER_MODULE_OPTIONS_TEXTURES_1024_COUNT_TEMPLATE_NAME, caps[TEXTURE_CACHE_TRANSPARENT_X1024_INDEX]);

                    fprintf(f, RENDERER_MODULE_OPTIONS_TEXTURES_MIP_MAP_TEXTURE_SIZE_TEMPLATE_NAME, State.Settings.MipMapTextureSize);

                    fclose(f);
                }
            }
            else
            {
                fclose(f);

                ReadValue(RENDERER_MODULE_OPTIONS_TEXTURES_SECTION_NAME,
                    RENDERER_MODULE_OPTIONS_TEXTURES_32_COUNT_NAME, &caps[TEXTURE_CACHE_TRANSPARENT_X32_INDEX]);
                ReadValue(RENDERER_MODULE_OPTIONS_TEXTURES_SECTION_NAME,
                    RENDERER_MODULE_OPTIONS_TEXTURES_64_COUNT_NAME, &caps[TEXTURE_CACHE_TRANSPARENT_X64_INDEX]);
                ReadValue(RENDERER_MODULE_OPTIONS_TEXTURES_SECTION_NAME,
                    RENDERER_MODULE_OPTIONS_TEXTURES_128_COUNT_NAME, &caps[TEXTURE_CACHE_TRANSPARENT_X128_INDEX]);
                ReadValue(RENDERER_MODULE_OPTIONS_TEXTURES_SECTION_NAME,
                    RENDERER_MODULE_OPTIONS_TEXTURES_256_COUNT_NAME, &caps[TEXTURE_CACHE_TRANSPARENT_X256_INDEX]);
                ReadValue(RENDERER_MODULE_OPTIONS_TEXTURES_SECTION_NAME,
                    RENDERER_MODULE_OPTIONS_TEXTURES_512_COUNT_NAME, &caps[TEXTURE_CACHE_TRANSPARENT_X512_INDEX]);
                ReadValue(RENDERER_MODULE_OPTIONS_TEXTURES_SECTION_NAME,
                    RENDERER_MODULE_OPTIONS_TEXTURES_1024_COUNT_NAME, &caps[TEXTURE_CACHE_TRANSPARENT_X1024_INDEX]);

                ReadValue(RENDERER_MODULE_OPTIONS_TEXTURES_SECTION_NAME,
                    RENDERER_MODULE_OPTIONS_TEXTURES_32_565_COUNT_NAME, &caps[TEXTURE_CACHE_OPAQUE_X32_INDEX]);
                ReadValue(RENDERER_MODULE_OPTIONS_TEXTURES_SECTION_NAME,
                    RENDERER_MODULE_OPTIONS_TEXTURES_64_565_COUNT_NAME, &caps[TEXTURE_CACHE_OPAQUE_X64_INDEX]);
                ReadValue(RENDERER_MODULE_OPTIONS_TEXTURES_SECTION_NAME,
                    RENDERER_MODULE_OPTIONS_TEXTURES_128_565_COUNT_NAME, &caps[TEXTURE_CACHE_OPAQUE_X128_INDEX]);
                ReadValue(RENDERER_MODULE_OPTIONS_TEXTURES_SECTION_NAME,
                    RENDERER_MODULE_OPTIONS_TEXTURES_256_565_COUNT_NAME, &caps[TEXTURE_CACHE_OPAQUE_X256_INDEX]);
                ReadValue(RENDERER_MODULE_OPTIONS_TEXTURES_SECTION_NAME,
                    RENDERER_MODULE_OPTIONS_TEXTURES_512_565_COUNT_NAME, &caps[TEXTURE_CACHE_OPAQUE_X512_INDEX]);
                ReadValue(RENDERER_MODULE_OPTIONS_TEXTURES_SECTION_NAME,
                    RENDERER_MODULE_OPTIONS_TEXTURES_1024_565_COUNT_NAME, &caps[TEXTURE_CACHE_OPAQUE_X1024_INDEX]);

                ReadValue(RENDERER_MODULE_OPTIONS_TEXTURES_SECTION_NAME,
                    RENDERER_MODULE_OPTIONS_TEXTURES_MIP_MAP_TEXTURE_SIZE_NAME, &State.Settings.MipMapTextureSize);
            }
        }

        const u32 dims[MAX_TEXTURE_CACHE_COUNT] =
        {
            TEXTURE_DIMENSIONS_SIZE_32,
            TEXTURE_DIMENSIONS_SIZE_64,
            TEXTURE_DIMENSIONS_SIZE_128,
            TEXTURE_DIMENSIONS_SIZE_256,
            TEXTURE_DIMENSIONS_SIZE_512,
            TEXTURE_DIMENSIONS_SIZE_1024,

            TEXTURE_DIMENSIONS_SIZE_32,
            TEXTURE_DIMENSIONS_SIZE_64,
            TEXTURE_DIMENSIONS_SIZE_128,
            TEXTURE_DIMENSIONS_SIZE_256,
            TEXTURE_DIMENSIONS_SIZE_512,
            TEXTURE_DIMENSIONS_SIZE_1024
        };

        const u32 indexes[MAX_TEXTURE_CACHE_COUNT] =
        {
            TEXTURE_CACHE_TRANSPARENT_X32_INDEX,
            TEXTURE_CACHE_TRANSPARENT_X64_INDEX,
            TEXTURE_CACHE_TRANSPARENT_X128_INDEX,
            TEXTURE_CACHE_TRANSPARENT_X256_INDEX,
            TEXTURE_CACHE_TRANSPARENT_X512_INDEX,
            TEXTURE_CACHE_TRANSPARENT_X1024_INDEX,

            TEXTURE_CACHE_OPAQUE_X32_INDEX,
            TEXTURE_CACHE_OPAQUE_X64_INDEX,
            TEXTURE_CACHE_OPAQUE_X128_INDEX,
            TEXTURE_CACHE_OPAQUE_X256_INDEX,
            TEXTURE_CACHE_OPAQUE_X512_INDEX,
            TEXTURE_CACHE_OPAQUE_X1024_INDEX
        };

        const BOOL opaqueness[MAX_TEXTURE_CACHE_COUNT] =
        {
            FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
            TRUE, TRUE, TRUE, TRUE, TRUE, TRUE
        };

        for (u32 x = 0; x < MAX_TEXTURE_CACHE_COUNT; x++)
        {
            if (State.DX.Caches[indexes[x]] == NULL)
            {
                State.DX.Caches[indexes[x]] = (DXTextureCache*)malloc(sizeof(DXTextureCache));

                if (State.DX.Caches[indexes[x]] == NULL)
                {
                    Quit("Unable to allocate texture cache memory.");
                }
            }

            InitializeTextureCache(State.DX.Caches[indexes[x]], caps[x], dims[x], opaqueness[x]);
        }
    }

    void ResetTextureCaches(void)
    {
        for (u32 x = 0; x < MAX_TEXTURE_CACHE_COUNT; x++)
        {
            ResetTextureCache(State.DX.Caches[x]);
        }
    }

    void ResetTextureCache(DXTextureCache* self)
    {
        for (u32 x = 0; x < self->Capacity; x++)
        {
            self->Textures[x].Name[0] = NULL;
        }

        ZeroMemory(&self->Indexes, MAX_TEXTURE_CACHE_ITEM_COUNT * sizeof(u16));

        self->Version = 0;
    }

    u32 SelectCachedTexture(const RendererTexture* tex, const BOOL transparent)
    {
        auto indx = INVALID_TEXTURE_CACHE_INDEX;

        if (transparent)
        {
            switch (*State.InitializeArguments.TextureSize)
            {
            case TEXTURE_DIMENSIONS_SIZE_32: { indx = TEXTURE_CACHE_TRANSPARENT_X32_INDEX; break; }
            case TEXTURE_DIMENSIONS_SIZE_64: { indx = TEXTURE_CACHE_TRANSPARENT_X64_INDEX; break; }
            case TEXTURE_DIMENSIONS_SIZE_128: { indx = TEXTURE_CACHE_TRANSPARENT_X128_INDEX; break; }
            case TEXTURE_DIMENSIONS_SIZE_256: { indx = TEXTURE_CACHE_TRANSPARENT_X256_INDEX; break; }
            case TEXTURE_DIMENSIONS_SIZE_512: { indx = TEXTURE_CACHE_TRANSPARENT_X512_INDEX; break; }
            case TEXTURE_DIMENSIONS_SIZE_1024: { indx = TEXTURE_CACHE_TRANSPARENT_X1024_INDEX; break; }
            }

            if (indx != INVALID_TEXTURE_CACHE_INDEX) { return CreateCachedTexture(State.DX.Caches[indx], tex); }

            return DEFAULT_TEXTURE_INDEX;
        }

        switch (*State.InitializeArguments.TextureSize)
        {
        case TEXTURE_DIMENSIONS_SIZE_32:
        {
            indx = State.DX.Caches[TEXTURE_CACHE_OPAQUE_X32_INDEX]->Capacity == DEFAULT_TEXTURE_CACHE_CAPACITY
                ? TEXTURE_CACHE_TRANSPARENT_X32_INDEX : TEXTURE_CACHE_OPAQUE_X32_INDEX; break;
        }
        case TEXTURE_DIMENSIONS_SIZE_64:
        {
            indx = State.DX.Caches[TEXTURE_CACHE_OPAQUE_X64_INDEX]->Capacity == DEFAULT_TEXTURE_CACHE_CAPACITY
                ? TEXTURE_CACHE_TRANSPARENT_X64_INDEX : TEXTURE_CACHE_OPAQUE_X64_INDEX; break;
        }
        case TEXTURE_DIMENSIONS_SIZE_128:
        {
            indx = State.DX.Caches[TEXTURE_CACHE_OPAQUE_X128_INDEX]->Capacity == DEFAULT_TEXTURE_CACHE_CAPACITY
                ? TEXTURE_CACHE_TRANSPARENT_X128_INDEX : TEXTURE_CACHE_OPAQUE_X128_INDEX; break;
        }
        case TEXTURE_DIMENSIONS_SIZE_256:
        {
            indx = State.DX.Caches[TEXTURE_CACHE_OPAQUE_X256_INDEX]->Capacity == DEFAULT_TEXTURE_CACHE_CAPACITY
                ? TEXTURE_CACHE_TRANSPARENT_X256_INDEX : TEXTURE_CACHE_OPAQUE_X256_INDEX; break;
        }
        case TEXTURE_DIMENSIONS_SIZE_512:
        {
            indx = State.DX.Caches[TEXTURE_CACHE_OPAQUE_X512_INDEX]->Capacity == DEFAULT_TEXTURE_CACHE_CAPACITY
                ? TEXTURE_CACHE_TRANSPARENT_X512_INDEX : TEXTURE_CACHE_OPAQUE_X512_INDEX; break;
        }
        case TEXTURE_DIMENSIONS_SIZE_1024:
        {
            indx = State.DX.Caches[TEXTURE_CACHE_OPAQUE_X1024_INDEX]->Capacity == DEFAULT_TEXTURE_CACHE_CAPACITY
                ? TEXTURE_CACHE_TRANSPARENT_X1024_INDEX : TEXTURE_CACHE_OPAQUE_X1024_INDEX; break; }
        }

        if (indx != INVALID_TEXTURE_CACHE_INDEX) { return CreateCachedTexture(State.DX.Caches[indx], tex); }

        return DEFAULT_TEXTURE_INDEX;
    }

    void UpdateCachedTexture(const RendererTexture* tex, const BOOL transparent)
    {
        auto indx = INVALID_TEXTURE_CACHE_INDEX;

        if (transparent)
        {
            switch (*State.InitializeArguments.TextureSize)
            {
            case TEXTURE_DIMENSIONS_SIZE_32: { indx = TEXTURE_CACHE_TRANSPARENT_X32_INDEX; break; }
            case TEXTURE_DIMENSIONS_SIZE_64: { indx = TEXTURE_CACHE_TRANSPARENT_X64_INDEX; break; }
            case TEXTURE_DIMENSIONS_SIZE_128: { indx = TEXTURE_CACHE_TRANSPARENT_X128_INDEX; break; }
            case TEXTURE_DIMENSIONS_SIZE_256: { indx = TEXTURE_CACHE_TRANSPARENT_X256_INDEX; break; }
            case TEXTURE_DIMENSIONS_SIZE_512: { indx = TEXTURE_CACHE_TRANSPARENT_X512_INDEX; break; }
            case TEXTURE_DIMENSIONS_SIZE_1024: { indx = TEXTURE_CACHE_TRANSPARENT_X1024_INDEX; break; }
            }

            if (indx != INVALID_TEXTURE_CACHE_INDEX) { AcquireCachedTexture(State.DX.Caches[indx], tex); }

            return;
        }

        switch (*State.InitializeArguments.TextureSize)
        {
        case TEXTURE_DIMENSIONS_SIZE_32:
        {
            indx = State.DX.Caches[TEXTURE_CACHE_OPAQUE_X32_INDEX]->Capacity == DEFAULT_TEXTURE_CACHE_CAPACITY
                ? TEXTURE_CACHE_TRANSPARENT_X32_INDEX : TEXTURE_CACHE_OPAQUE_X32_INDEX; break;
        }
        case TEXTURE_DIMENSIONS_SIZE_64:
        {
            indx = State.DX.Caches[TEXTURE_CACHE_OPAQUE_X64_INDEX]->Capacity == DEFAULT_TEXTURE_CACHE_CAPACITY
                ? TEXTURE_CACHE_TRANSPARENT_X64_INDEX : TEXTURE_CACHE_OPAQUE_X64_INDEX; break;
        }
        case TEXTURE_DIMENSIONS_SIZE_128:
        {
            indx = State.DX.Caches[TEXTURE_CACHE_OPAQUE_X128_INDEX]->Capacity == DEFAULT_TEXTURE_CACHE_CAPACITY
                ? TEXTURE_CACHE_TRANSPARENT_X128_INDEX : TEXTURE_CACHE_OPAQUE_X128_INDEX; break;
        }
        case TEXTURE_DIMENSIONS_SIZE_256:
        {
            indx = State.DX.Caches[TEXTURE_CACHE_OPAQUE_X256_INDEX]->Capacity == DEFAULT_TEXTURE_CACHE_CAPACITY
                ? TEXTURE_CACHE_TRANSPARENT_X256_INDEX : TEXTURE_CACHE_OPAQUE_X256_INDEX; break;
        }
        case TEXTURE_DIMENSIONS_SIZE_512:
        {
            indx = State.DX.Caches[TEXTURE_CACHE_OPAQUE_X512_INDEX]->Capacity == DEFAULT_TEXTURE_CACHE_CAPACITY
                ? TEXTURE_CACHE_TRANSPARENT_X512_INDEX : TEXTURE_CACHE_OPAQUE_X512_INDEX; break;
        }
        case TEXTURE_DIMENSIONS_SIZE_1024:
        {
            indx = State.DX.Caches[TEXTURE_CACHE_OPAQUE_X1024_INDEX]->Capacity == DEFAULT_TEXTURE_CACHE_CAPACITY
                ? TEXTURE_CACHE_TRANSPARENT_X1024_INDEX : TEXTURE_CACHE_OPAQUE_X1024_INDEX; break; }
        }

        if (indx != INVALID_TEXTURE_CACHE_INDEX) { AcquireCachedTexture(State.DX.Caches[indx], tex); }
    }

    void AcquireCachedTexture(DXTextureCache* self, const RendererTexture* tex)
    {
        u32 indx = self->Indexes[tex->Index & TEXTURE_CACHE_INDEX_MASK];

        if (strcmpi(self->Textures[indx].Name, tex->Name) == 0)
        {
            if (self->Count == indx)
            {
                CreateCachedTexture(self, tex);

                return;
            }
        }
        else
        {
            if (self->Count == 0)
            {
                CreateCachedTexture(self, tex);

                return;
            }

            indx = 0;

            for (u32 x = 0; x < self->Count; x++)
            {
                if (strcmpi(self->Textures[x].Name, tex->Name) == 0) { break; }

                indx = indx + 1;
            }

            if (self->Count == indx)
            {
                CreateCachedTexture(self, tex);

                return;
            }
        }

        InfuseCachedTexture(self->Textures[indx].Index);
    }

    u32* AcquireTexturePixels(const u32 dimensions)
    {
        u32 offset = 0;

        auto dims = *State.InitializeArguments.TextureSize;

        if (dims == dimensions) { return &State.DX.Pixels.Pixels[0]; }

        while (dimensions != dims)
        {
            offset = offset + dims * dims;

            dims = dims / 2;

            if (dims == 0)
            {
                Quit("Unable to acquire texture source size %d x %d.", dimensions, *State.InitializeArguments.TextureSize);
            }
        }

        return &State.DX.Pixels.Pixels[offset];
    }

    u32 CreateCachedTexture(DXTextureCache* self, const RendererTexture* tex)
    {
        u32 indx = self->Indexes[tex->Index & TEXTURE_CACHE_INDEX_MASK];

        if (strcmpi(self->Textures[indx].Name, tex->Name) != 0)
        {
            indx = 0;

            for (u32 x = 0; x < self->Count; x++)
            {
                if (strcmpi(self->Textures[x].Name, tex->Name) == 0) { break; }

                indx = indx + 1;
            }
        }

        if (indx < self->Count)
        {
            self->Textures[indx].Version = self->Version;
            self->Version = self->Version + 1;

            return self->Textures[indx].Index;
        }

        if (self->Count < self->Capacity)
        {
            self->Count = self->Count + 1;
        }
        else
        {
            auto version = U32_MAX;

            for (u32 x = 0; x < self->Capacity; x++)
            {
                if (self->Textures[x].Version < version)
                {
                    indx = x;
                    version = self->Textures[x].Version;
                }
            }
        }

        self->Indexes[tex->Index] = indx;

        InfuseCachedTexture(self->Textures[indx].Index);

        self->Textures[indx].Version = self->Version;

        strcpy(self->Textures[indx].Name, tex->Name);

        self->Version = self->Version + 1;

        return self->Textures[indx].Index;
    }

    void ConvertTexture(void)
    {
        if (State.DX.Surfaces.Staging.PixelFormat.Transparent.dwRGBBitCount == GRAPHICS_BITS_PER_PIXEL_32)
        {
            State.DX.Surfaces.Masks.Color.R = State.DX.Surfaces.Masks.Transparent.R;
            State.DX.Surfaces.Masks.Color.G = State.DX.Surfaces.Masks.Transparent.G;
            State.DX.Surfaces.Masks.Color.B = State.DX.Surfaces.Masks.Transparent.B;
        }
        else
        {
            State.DX.Surfaces.Masks.Color.R = 16;// TODO constant
            State.DX.Surfaces.Masks.Color.G = 8; // TODO constant
            State.DX.Surfaces.Masks.Color.B = 0; // TODO constant
        }

        const auto size = *State.InitializeArguments.TextureSize;
        const auto length = size * size;

        if (State.DX.Pixels.Colors == NULL)
        {
            const auto mr = (State.DX.Surfaces.Masks.Color.R & 0x1f);
            const auto mg = (State.DX.Surfaces.Masks.Color.G & 0x1f);
            const auto mb = (State.DX.Surfaces.Masks.Color.B & 0x1f);

            for (u32 x = 0; x < MAX_PIXEL_PALETTE_BUFFER_SIZE; x++)
            {
                const u32 r = State.DX.Pixels.Palette.Colors[x * 3 + 0];
                const u32 g = State.DX.Pixels.Palette.Colors[x * 3 + 1];
                const u32 b = State.DX.Pixels.Palette.Colors[x * 3 + 2];

                State.DX.Pixels.Palette.Palette[x] = (r << mr) | (g << mg) | (b << mb);
            }

            if (State.DX.Pixels.Palette.Alphas == NULL)
            {
                for (u32 x = 0; x < length; x++)
                {
                    const auto indx = State.DX.Pixels.Palette.Indexes[x];

                    auto color = State.DX.Pixels.Palette.Palette[indx];

                    if (color != GRAPHICS_COLOR_BLACK) { color = 0xff000000 | color; }

                    State.DX.Pixels.Pixels[x] = color;
                }
            }
            else
            {
                for (u32 x = 0; x < length; x++)
                {
                    const auto indx = State.DX.Pixels.Palette.Indexes[x];
                    const auto alpha = ((u32)State.DX.Pixels.Palette.Alphas[x]) << 24;

                    State.DX.Pixels.Pixels[x] = alpha | State.DX.Pixels.Palette.Palette[indx];
                }
            }
        }
        else
        {
            for (u32 x = 0; x < length; x++)
            {
                State.DX.Pixels.Pixels[x] = State.DX.Pixels.Colors[x];
            }
        }

        // Generate down-sized texture pixels.
        if ((*State.InitializeArguments.TextureSize & State.Settings.MipMapTextureSize) != 0
            || *State.InitializeArguments.AcceleratedGraphicsPortMode == RendererAcceleratedGraphicsPortMode::Auto)
        {
            auto pixels = &State.DX.Pixels.Pixels[0];

            for (u32 dims = *State.InitializeArguments.TextureSize; 1 < dims; dims = dims / 2)
            {
                auto level = &pixels[dims * dims];
                const auto next = dims / 2;

                for (u32 x = 0; x < dims; x = x + 2)
                {
                    for (u32 xx = 0; xx < dims; xx = xx + 2)
                    {
                        const auto c00 = pixels[x * dims + xx];
                        const auto c01 = pixels[x * dims + (xx + 1)];
                        const auto c10 = pixels[(x + 1) * dims + xx];
                        const auto c11 = pixels[(x + 1) * dims + (xx + 1)];

                        const auto ca = ((c00 >> 24) + (c01 >> 24) + (c10 >> 24) + (c11 >> 24)) / 4;
                        const auto cr = ((c00 >> 16 & 0xff) + (c01 >> 16 & 0xff) + (c10 >> 16 & 0xff) + (c11 >> 16 & 0xff)) / 4;
                        const auto cg = ((c00 >> 8 & 0xff) + (c01 >> 8 & 0xff) + (c10 >> 8 & 0xff) + (c11 >> 8 & 0xff)) / 4;
                        const auto cb = ((c00 >> 0 & 0xff) + (c01 >> 0 & 0xff) + (c10 >> 0 & 0xff) + (c11 >> 0 & 0xff)) / 4;

                        level[((x * next) + xx) / 2] = (ca << 24) | (cr << 16) | (cg << 8) | (cb << 0);
                    }
                }

                pixels = level;
            }
        }
    }

    void CopyTexture16BitOpaque(u16* pixels, const u32 pitch, const u32 dimensions)
    {
        const auto stride = pitch / sizeof(u16);
        const auto px = AcquireTexturePixels(dimensions);

        for (u32 x = 0; x < dimensions; x++)
        {
            for (u32 xx = 0; xx < dimensions; xx++)
            {
                const auto color = px[x * dimensions + xx];

                const auto r = ((color >> 19) & 0x1f) << (State.DX.Surfaces.Masks.Opaque.R & 0x1f);
                const auto g = ((color >> 10) & 0x3f) << (State.DX.Surfaces.Masks.Opaque.G & 0x1f);
                const auto b = ((color >> 3) & 0x1f) << (State.DX.Surfaces.Masks.Opaque.B & 0x1f);

                pixels[x * stride + xx] = (u16)(r | g | b);
            }
        }
    }

    void CopyTexture16BitTransparent(u16* pixels, const u32 pitch, const u32 dimensions)
    {
        const auto stride = pitch / sizeof(u16);
        const auto px = AcquireTexturePixels(dimensions);

        for (u32 x = 0; x < dimensions; x++)
        {
            for (u32 xx = 0; xx < dimensions; xx++)
            {
                const auto color = px[x * dimensions + xx];

                const auto a = (color >> 16) & 0xf000;
                const auto r = ((color >> 20) & 0xf) << (State.DX.Surfaces.Masks.Transparent.R & 0x1f);
                const auto g = ((color >> 12) & 0xf) << (State.DX.Surfaces.Masks.Transparent.G & 0x1f);
                const auto b = ((color >> 4) & 0xf) << (State.DX.Surfaces.Masks.Transparent.B & 0x1f);

                pixels[x * stride + xx] = a | r | g | b;
            }
        }
    }

    void CopyTexture32Bit(u32* pixels, const u32 pitch, const u32 dimensions)
    {
        const auto stride = pitch / sizeof(u32);
        const auto px = AcquireTexturePixels(dimensions);

        for (u32 x = 0; x < dimensions; x++)
        {
            for (u32 xx = 0; xx < dimensions; xx++)
            {
                pixels[x * stride + xx] = px[x * dimensions + xx];
            }
        }
    }

    void InfuseCachedTexture(const u32 indx)
    {
        State.DX.Surfaces.Index = indx;

        InfuseTextureSurface();
    }

    void InfuseTextureSurface(void)
    {
        if (State.DX.Pixels.Palette.Indexes == NULL && State.DX.Pixels.Colors == NULL) { return; }

        u32 indx = 0;

        switch (*State.InitializeArguments.TextureSize)
        {
        case TEXTURE_DIMENSIONS_SIZE_32: { indx = TEXTURE_SURFACE_X32_INDEX; break; }
        case TEXTURE_DIMENSIONS_SIZE_64: { indx = TEXTURE_SURFACE_X64_INDEX; break; }
        case TEXTURE_DIMENSIONS_SIZE_128: { indx = TEXTURE_SURFACE_X128_INDEX; break; }
        case TEXTURE_DIMENSIONS_SIZE_256: { indx = TEXTURE_SURFACE_X256_INDEX; break; }
        case TEXTURE_DIMENSIONS_SIZE_512: { indx = TEXTURE_SURFACE_X512_INDEX; break; }
        case TEXTURE_DIMENSIONS_SIZE_1024: { indx = TEXTURE_SURFACE_X1024_INDEX; break; }
        default: { Quit("Unknown texture size %d.", *State.InitializeArguments.TextureSize); }
        }

        ConvertTexture();

        auto surface = State.DX.Surfaces.Surfaces[State.DX.Surfaces.Index].Surface;

        for (u32 x = 0; x < MAX_TEXTURE_MIPMAP_LEVEL_COUNT; x++)
        {
            auto staging = State.DX.Surfaces.Surfaces[State.DX.Surfaces.Index].IsOpaque
                ? State.DX.Surfaces.Staging.Opaque[indx + x] : State.DX.Surfaces.Staging.Transparent[indx + x];

            DDSURFACEDESC2 desc = { .dwSize = sizeof(DDSURFACEDESC2) };

            if (!LockSurface(staging, &desc)) { Quit("Unable to load texture, unable to lock texture surface."); }

            if (!State.DX.Surfaces.Surfaces[State.DX.Surfaces.Index].IsOpaque)
            {
                if (State.Window.BitsPerPixel == GRAPHICS_BITS_PER_PIXEL_32)
                {
                    CopyTexture32Bit((u32*)desc.lpSurface, desc.lPitch, desc.dwWidth);
                }
                else
                {
                    CopyTexture16BitTransparent((u16*)desc.lpSurface, desc.lPitch, desc.dwWidth);
                }
            }
            else
            {
                CopyTexture16BitOpaque((u16*)desc.lpSurface, desc.lPitch, desc.dwWidth);
            }

            if (!UnlockSurface(staging)) { Quit("Unable to load texture, unable to unlock texture surface."); }

            DXC(surface->Blt(NULL, staging, NULL, DDBLT_NONE, NULL),
                "Unable to bit block transfer to upload texture surface.");

            if ((*State.InitializeArguments.TextureSize & State.Settings.MipMapTextureSize) == 0) { break; }

            surface->GetSurfaceDesc(&desc);

            if (surface->GetAttachedSurface(&desc.ddsCaps, &surface) != DD_OK) { surface = NULL; }

            if (surface == NULL) { return; }
        }
    }
}

namespace Renderer::Module
{
    extern "C" u32 AcquireTextureInformation(const u32 dimensions)
    {
        switch (dimensions)
        {
        case TEXTURE_DIMENSIONS_SIZE_32:
        {
            if (State.DX.Caches[TEXTURE_CACHE_TRANSPARENT_X32_INDEX] == NULL || State.DX.Caches[TEXTURE_CACHE_OPAQUE_X32_INDEX] == NULL) { return DEFAULT_TEXTURE_CACHE_CAPACITY; }

            return State.DX.Caches[TEXTURE_CACHE_TRANSPARENT_X32_INDEX]->Capacity + State.DX.Caches[TEXTURE_CACHE_OPAQUE_X32_INDEX]->Capacity;
        }
        case TEXTURE_DIMENSIONS_SIZE_64:
        {
            if (State.DX.Caches[TEXTURE_CACHE_TRANSPARENT_X64_INDEX] == NULL || State.DX.Caches[TEXTURE_CACHE_OPAQUE_X64_INDEX] == NULL) { return DEFAULT_TEXTURE_CACHE_CAPACITY; }

            return State.DX.Caches[TEXTURE_CACHE_TRANSPARENT_X64_INDEX]->Capacity + State.DX.Caches[TEXTURE_CACHE_OPAQUE_X64_INDEX]->Capacity;
        }
        case TEXTURE_DIMENSIONS_SIZE_128:
        {
            if (State.DX.Caches[TEXTURE_CACHE_TRANSPARENT_X128_INDEX] == NULL || State.DX.Caches[TEXTURE_CACHE_OPAQUE_X128_INDEX] == NULL) { return DEFAULT_TEXTURE_CACHE_CAPACITY; }

            return State.DX.Caches[TEXTURE_CACHE_TRANSPARENT_X128_INDEX]->Capacity + State.DX.Caches[TEXTURE_CACHE_OPAQUE_X128_INDEX]->Capacity;
        }
        case TEXTURE_DIMENSIONS_SIZE_256:
        {
            if (State.DX.Caches[TEXTURE_CACHE_TRANSPARENT_X256_INDEX] == NULL || State.DX.Caches[TEXTURE_CACHE_OPAQUE_X256_INDEX] == NULL) { return DEFAULT_TEXTURE_CACHE_CAPACITY; }

            return State.DX.Caches[TEXTURE_CACHE_TRANSPARENT_X256_INDEX]->Capacity + State.DX.Caches[TEXTURE_CACHE_OPAQUE_X256_INDEX]->Capacity;
        }
        case TEXTURE_DIMENSIONS_SIZE_512:
        {
            if (State.DX.Caches[TEXTURE_CACHE_TRANSPARENT_X512_INDEX] == NULL || State.DX.Caches[TEXTURE_CACHE_OPAQUE_X512_INDEX] == NULL) { return DEFAULT_TEXTURE_CACHE_CAPACITY; }

            return State.DX.Caches[TEXTURE_CACHE_TRANSPARENT_X512_INDEX]->Capacity + State.DX.Caches[TEXTURE_CACHE_OPAQUE_X512_INDEX]->Capacity;
        }
        case TEXTURE_DIMENSIONS_SIZE_1024:
        {
            if (State.DX.Caches[TEXTURE_CACHE_TRANSPARENT_X1024_INDEX] == NULL || State.DX.Caches[TEXTURE_CACHE_OPAQUE_X1024_INDEX] == NULL) { return DEFAULT_TEXTURE_CACHE_CAPACITY; }

            return State.DX.Caches[TEXTURE_CACHE_TRANSPARENT_X1024_INDEX]->Capacity + State.DX.Caches[TEXTURE_CACHE_OPAQUE_X1024_INDEX]->Capacity;
        }
        }

        return DEFAULT_TEXTURE_CACHE_CAPACITY;
    }

    extern "C" u32 AcquireTextureHandle(void) { return DEFAULT_TEXTURE_HANDLE; } // NOTE: Not being called by the application.

    extern "C" u32 LockTexture(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 SelectTexture(const RendererTexture * tex, const u32 dimensions, u8 * indexes, u8 * colors, u8 * alphas)
    {
        State.DX.Pixels.Palette.Alphas = alphas;
        State.DX.Pixels.Palette.Colors = colors;
        State.DX.Pixels.Palette.Indexes = indexes;

        State.DX.Pixels.Colors = NULL;

        State.DX.Surfaces.Index = DX::SelectCachedTexture(tex, alphas != NULL);

        return TRUE;
    }

    extern "C" u32 SelectTextureBGRA(const RendererTexture * tex, const u32 dimensions, u32 * colors, const BOOL transparent)
    {
        State.DX.Pixels.Palette.Colors = NULL;
        State.DX.Pixels.Palette.Alphas = NULL;
        State.DX.Pixels.Palette.Indexes = NULL;

        State.DX.Pixels.Colors = colors;

        State.DX.Surfaces.Index = DX::SelectCachedTexture(tex, transparent);

        return TRUE;
    }

    extern "C" u32 SelectTextureByHandle(const u32, const u32) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 ResetTextureCache(void)
    {
        DX::ResetTextureCaches();

        return TRUE;
    }

    extern "C" u32 UnlockTexture(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 UpdateTexture(const RendererTexture * tex, const u32 dimensions, u8 * indexes, u8 * colors, u8 * alphas)
    {
        State.DX.Pixels.Palette.Alphas = alphas;
        State.DX.Pixels.Palette.Colors = colors;
        State.DX.Pixels.Palette.Indexes = indexes;

        State.DX.Pixels.Colors = NULL;

        DX::UpdateCachedTexture(tex, alphas != NULL);

        return TRUE;
    }

    extern "C" u32 UpdateTextureBGRA(const RendererTexture * tex, const u32 dimensions, u32 * colors, const BOOL transparent)
    {
        State.DX.Pixels.Palette.Alphas = NULL;
        State.DX.Pixels.Palette.Colors = NULL;
        State.DX.Pixels.Palette.Indexes = NULL;

        State.DX.Pixels.Colors = colors;

        DX::UpdateCachedTexture(tex, transparent);

        return TRUE;
    }
}