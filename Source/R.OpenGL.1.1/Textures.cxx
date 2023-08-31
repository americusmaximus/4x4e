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
#include "Renderers.Basic.hxx"
#include "Settings.hxx"
#include "Textures.hxx"

#include <stdlib.h>

#define MIN_TEXTURE_POOL_MINI_COUNT 1000
#define MIN_TEXTURE_POOL_MINI_REAL_COUNT 1021
#define MIN_TEXTURE_POOL_SMALL_COUNT 2000
#define MIN_TEXTURE_POOL_SMALL_REAL_COUNT 2039
#define MIN_TEXTURE_POOL_MEDIUM_COUNT 4000
#define MIN_TEXTURE_POOL_MEDIUM_REAL_COUNT 4093
#define MIN_TEXTURE_POOL_LARGE_COUNT 8000
#define MIN_TEXTURE_POOL_LARGE_REAL_COUNT 8191

#define TEXTURE_CACHE_INDEX_MULTIPLIER 127

using namespace Renderers;
using namespace Settings;
using namespace Renderer::Module;

namespace GL
{
    BOOL BindTexture(const RendererTexture* tex)
    {
        if (strcmpi(tex->Name, State.GL.Textures.Name) == 0 && !State.GL.Textures.UploadMode) { return TRUE; }

        Render();

        const auto id = AcquireTextureID(&State.GL.Textures.Cache, tex->Name);

        if (id == INVALID_TEXTURE_ID)
        {
            glGenTextures(1, &State.GL.Textures.ID);
            glBindTexture(GL_TEXTURE_2D, State.GL.Textures.ID);

            const auto priority = STANDART_TEXTURE_PRIORITY;
            glPrioritizeTextures(1, &State.GL.Textures.ID, &priority);

            strcpy(State.GL.Textures.Name, tex->Name);

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            if ((*State.InitializeArguments.TextureSize & State.Settings.MipMapTextureSize) == 0)
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    *State.InitializeArguments.TextureFilterMode == RendererTextureFilterMode::Nearest ? GL_NEAREST : GL_LINEAR);
            }
            else
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    *State.InitializeArguments.TextureFilterMode == RendererTextureFilterMode::Nearest ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_LINEAR);
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                *State.InitializeArguments.TextureFilterMode == RendererTextureFilterMode::Nearest ? GL_NEAREST : GL_LINEAR);

            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

            UploadTexture();

            State.GL.Textures.UploadMode = FALSE;

            UpdateCache(&State.GL.Textures.Cache, tex->Name, State.GL.Textures.ID);
        }
        else
        {
            State.GL.Textures.ID = id;

            strcpy(State.GL.Textures.Name, tex->Name);

            glBindTexture(GL_TEXTURE_2D, id);
        }

        if (State.GL.Textures.UploadMode)
        {
            UploadTexture();

            State.GL.Textures.UploadMode = FALSE;
        }

        return TRUE;
    }

    BOOL InitializeCache(GLTextureCache* self, const u32 count)
    {
        if (count < MIN_TEXTURE_POOL_MINI_COUNT)
        {
            self->Capacity = MIN_TEXTURE_POOL_MINI_REAL_COUNT;
            self->Textures = (GL::GLTexture*)malloc(MIN_TEXTURE_POOL_MINI_REAL_COUNT * sizeof(GL::GLTexture));

            if (self->Textures != NULL) { ZeroMemory(self->Textures, MIN_TEXTURE_POOL_MINI_REAL_COUNT * sizeof(GL::GLTexture)); }

            return self->Textures != NULL;
        }

        if (count < MIN_TEXTURE_POOL_SMALL_COUNT)
        {
            self->Capacity = MIN_TEXTURE_POOL_SMALL_REAL_COUNT;
            self->Textures = (GL::GLTexture*)malloc(MIN_TEXTURE_POOL_SMALL_REAL_COUNT * sizeof(GL::GLTexture));

            if (self->Textures != NULL) { ZeroMemory(self->Textures, MIN_TEXTURE_POOL_SMALL_REAL_COUNT * sizeof(GL::GLTexture)); }

            return self->Textures != NULL;
        }

        if (count < MIN_TEXTURE_POOL_MEDIUM_COUNT)
        {
            self->Capacity = MIN_TEXTURE_POOL_MEDIUM_REAL_COUNT;
            self->Textures = (GL::GLTexture*)malloc(MIN_TEXTURE_POOL_MEDIUM_REAL_COUNT * sizeof(GL::GLTexture));

            if (self->Textures != NULL) { ZeroMemory(self->Textures, MIN_TEXTURE_POOL_MEDIUM_REAL_COUNT * sizeof(GL::GLTexture)); }

            return self->Textures != NULL;
        }

        if (count < MIN_TEXTURE_POOL_LARGE_COUNT)
        {
            self->Capacity = MIN_TEXTURE_POOL_LARGE_REAL_COUNT;
            self->Textures = (GL::GLTexture*)malloc(MIN_TEXTURE_POOL_LARGE_REAL_COUNT * sizeof(GL::GLTexture));

            if (self->Textures != NULL) { ZeroMemory(self->Textures, MIN_TEXTURE_POOL_LARGE_REAL_COUNT * sizeof(GL::GLTexture)); }

            return self->Textures != NULL;
        }

        return FALSE;
    }

    void ReleaseCache(GLTextureCache* cache)
    {
        if (cache->Textures != NULL)
        {
            free(cache->Textures);

            cache->Textures = NULL;
        }
    }

    void UpdateCache(GLTextureCache* cache, const char* name, const u32 id)
    {
        auto indx = AcquireTextureIndex(cache, name);

        if (cache->Textures[indx].ID != INVALID_TEXTURE_ID)
        {
            BOOL found = FALSE;

            while (TRUE)
            {
                indx = indx + 1;

                if (cache->Textures[indx].ID == INVALID_TEXTURE_ID)
                {
                    cache->Textures[indx].ID = id;

                    strcpy(cache->Textures[indx].Name, name);

                    found = TRUE;
                }

                if ((cache->Capacity - 1) < indx) { indx = 0; }

                if (found) { return; }
            }
        }

        cache->Textures[indx].ID = id;

        strcpy(cache->Textures[indx].Name, name);

        return;
    }

    u32 AcquireTextureIndex(const GLTextureCache* cache, const char* name)
    {
        u32 indx = INVALID_TEXTURE_ID;

        for (u32 x = 0; x < strlen(name); x++)
        {
            indx = ((indx * TEXTURE_CACHE_INDEX_MULTIPLIER) + name[x]) % cache->Capacity;
        }

        return indx;
    }

    u32 AcquireTextureID(const GLTextureCache* cache, const char* name)
    {
        auto indx = AcquireTextureIndex(cache, name);

        if (strcmpi(cache->Textures[indx].Name, name) == 0) { return cache->Textures[indx].ID; }

        for (u32 x = indx; x < cache->Capacity; x++)
        {
            if (strcmpi(cache->Textures[x].Name, name) == 0) { return cache->Textures[x].ID; }
        }

        for (u32 x = 0; x < indx; x++)
        {
            if (strcmpi(cache->Textures[x].Name, name) == 0) { return cache->Textures[x].ID; }
        }

        return INVALID_TEXTURE_ID;
    }

    void ConvertTexture(void)
    {
        const auto size = *State.InitializeArguments.TextureSize;
        const auto length = size * size;

        if (State.GL.Pixels.Colors == NULL)
        {
            for (u32 x = 0; x < MAX_PIXEL_PALETTE_BUFFER_SIZE; x++)
            {
                State.GL.Pixels.Palette.Palette[x] =
                    ((u32)State.GL.Pixels.Palette.Colors[x * 3 + 2] << 16)
                    | ((u32)State.GL.Pixels.Palette.Colors[x * 3 + 1] << 8)
                    | ((u32)State.GL.Pixels.Palette.Colors[x * 3 + 0] << 0);
            }

            if (State.GL.Pixels.Palette.Alphas == NULL)
            {
                for (u32 x = 0; x < length; x++)
                {
                    const auto indx = State.GL.Pixels.Palette.Indexes[x];

                    auto color = State.GL.Pixels.Palette.Palette[indx];

                    if (color != GRAPHICS_COLOR_BLACK) { color = color | 0xff000000; }

                    State.GL.Pixels.Pixels[x] = color;
                }
            }
            else
            {
                for (u32 x = 0; x < length; x++)
                {
                    const auto indx = State.GL.Pixels.Palette.Indexes[x];
                    const auto alpha = ((u32)State.GL.Pixels.Palette.Alphas[x]) << 24;

                    State.GL.Pixels.Pixels[x] = alpha | State.GL.Pixels.Palette.Palette[indx];
                }
            }
        }
        else
        {
            for (u32 x = 0; x < length; x++)
            {
                const auto color = State.GL.Pixels.Colors[x];

                State.GL.Pixels.Pixels[x] = (color & 0xff00ff00) | ((color & 0xff0000) >> 16) | ((color & 0xff) << 16);
            }
        }

        // Generate down-sized texture pixels.
        if ((*State.InitializeArguments.TextureSize & State.Settings.MipMapTextureSize) != 0
            || *State.InitializeArguments.AcceleratedGraphicsPortMode == RendererAcceleratedGraphicsPortMode::Auto)
        {
            auto pixels = &State.GL.Pixels.Pixels[0];

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

    void UploadTexture(void)
    {
        auto dims = *State.InitializeArguments.TextureSize;

        ConvertTexture();

        auto pixels = &State.GL.Pixels.Pixels[0];

        if (*State.InitializeArguments.AcceleratedGraphicsPortMode == RendererAcceleratedGraphicsPortMode::Auto)
        {
            pixels = &State.GL.Pixels.Pixels[dims * dims];

            dims = dims / 2;
        }

        if ((*State.InitializeArguments.TextureSize & State.Settings.MipMapTextureSize) == 0)
        {
            glTexImage2D(GL_TEXTURE_2D, 0,
                State.Window.BitsPerPixel == GRAPHICS_BITS_PER_PIXEL_16 ? GL_RGBA4 : GL_RGBA8,
                dims, dims, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        }
        else
        {
            for (u32 x = 0; dims != 0; x++)
            {
                glTexImage2D(GL_TEXTURE_2D, x,
                    State.Window.BitsPerPixel == GRAPHICS_BITS_PER_PIXEL_16 ? GL_RGBA4 : GL_RGBA8,
                    dims, dims, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

                pixels = &pixels[dims * dims];

                dims = dims / 2;
            }
        }
    }
}

namespace Renderer::Module
{
    extern "C" u32 AcquireTextureInformation(const u32) { return DEFAULT_TEXTURE_CACHE_CAPACITY; }

    extern "C" u32 AcquireTextureHandle(void) { return DEFAULT_TEXTURE_HANDLE; } // NOTE: Not being called by the application.

    extern "C" u32 LockTexture(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 ResetTextureCache(void)
    {
        for (u32 x = 0; x < State.GL.Textures.Cache.Capacity; x++)
        {
            glDeleteTextures(1, &State.GL.Textures.Cache.Textures[x].ID);

            State.GL.Textures.Cache.Textures[x].ID = INVALID_TEXTURE_ID;
            State.GL.Textures.Cache.Textures[x].Name[0] = NULL;
        }

        return TRUE;
    }

    extern "C" u32 SelectTexture(const RendererTexture * tex, const u32 dimensions, u8 * indexes, u8 * colors, u8 * alphas)
    {
        State.GL.Pixels.Palette.Indexes = indexes;
        State.GL.Pixels.Palette.Colors = colors;
        State.GL.Pixels.Palette.Alphas = alphas;

        State.GL.Pixels.Colors = NULL;

        return GL::BindTexture(tex);
    }

    extern "C" u32 SelectTextureBGRA(const RendererTexture * tex, const u32 dimensions, u32 * colors, const BOOL transparent)
    {
        State.GL.Pixels.Palette.Indexes = NULL;
        State.GL.Pixels.Palette.Colors = NULL;
        State.GL.Pixels.Palette.Alphas = NULL;

        State.GL.Pixels.Colors = colors;

        return GL::BindTexture(tex);
    }

    extern "C" u32 SelectTextureByHandle(const u32, const u32) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 UnlockTexture(void) { return TRUE; }

    extern "C" u32 UpdateTexture(const RendererTexture * tex, const u32 dimensions, u8 * indexes, u8 * colors, u8 * alphas)
    {
        State.GL.Textures.UploadMode = TRUE;

        return SelectTexture(tex, dimensions, indexes, colors, alphas);
    }

    extern "C" u32 UpdateTextureBGRA(const RendererTexture * tex, const u32 dimensions, u32 * colors, const BOOL transparent)
    {
        State.GL.Textures.UploadMode = TRUE;

        return SelectTextureBGRA(tex, dimensions, colors, transparent);
    }
}