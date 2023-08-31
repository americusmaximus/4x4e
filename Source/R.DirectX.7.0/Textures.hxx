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
#include "DX.hxx"
#include "Renderers.Basic.hxx"

#define DEFAULT_TEXTURE_HANDLE 0
#define DEFAULT_TEXTURE_CACHE_CAPACITY 0

#define INVALID_TEXTURE_CACHE_INDEX (-1)

#define TEXTURE_CACHE_TRANSPARENT_X32_INDEX 0
#define TEXTURE_CACHE_TRANSPARENT_X64_INDEX 1
#define TEXTURE_CACHE_TRANSPARENT_X128_INDEX 2
#define TEXTURE_CACHE_TRANSPARENT_X256_INDEX 3
#define TEXTURE_CACHE_TRANSPARENT_X512_INDEX 4
#define TEXTURE_CACHE_TRANSPARENT_X1024_INDEX 5

#define TEXTURE_CACHE_OPAQUE_X32_INDEX 6
#define TEXTURE_CACHE_OPAQUE_X64_INDEX 7
#define TEXTURE_CACHE_OPAQUE_X128_INDEX 8
#define TEXTURE_CACHE_OPAQUE_X256_INDEX 9
#define TEXTURE_CACHE_OPAQUE_X512_INDEX 10
#define TEXTURE_CACHE_OPAQUE_X1024_INDEX 11

#define MIN_TEXTURE_MIP_LEVEL_COUNT 1

namespace DX
{
    BOOL AllocateStageTextures(void);
    s32 AllocateTexture(const u32 dimensions, const BOOL transparent);
    u32 AcquireTextureMipMapCount(const u32 dimensions);
    u32 CreateCachedTexture(DXTextureCache* self, const Renderers::RendererTexture* tex);
    u32 SelectCachedTexture(const Renderers::RendererTexture* tex, const BOOL transparent);
    u32* AcquireTexturePixels(const u32 dimensions);
    void AcquireCachedTexture(DXTextureCache* self, const Renderers::RendererTexture* tex);
    void ConvertTexture(void);
    void CopyTexture16BitOpaque(u16* pixels, const u32 pitch, const u32 dimensions);
    void CopyTexture16BitTransparent(u16* pixels, const u32 pitch, const u32 dimensions);
    void CopyTexture32Bit(u32* pixels, const u32 pitch, const u32 dimensions);
    void InfuseCachedTexture(const u32 indx);
    void InfuseTextureSurface(void);
    void InitializeTextureCache(DXTextureCache* self, const u32 capacity, const u32 dimensions, const BOOL transparent);
    void InitializeTextureCaches(void);
    void ResetTextureCache(DXTextureCache* self);
    void ResetTextureCaches(void);
    void UpdateCachedTexture(const Renderers::RendererTexture* tex, const BOOL transparent);
}