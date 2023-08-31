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

#define FLY_OPTIONS_FILE_NAME ".\\system\\fly.ini"

#define RENDERER_MODULE_OPTIONS_FILE_NAME ".\\system\\render.ini"

#define RENDERER_MODULE_OPTIONS_GRAPHICS_SECTION_NAME "Graphics"

#define RENDERER_MODULE_OPTIONS_GRAPHICS_MASTER_DEPTH_BUFFER_COUNT_NAME "masterZBufferCount"
#define RENDERER_MODULE_OPTIONS_GRAPHICS_HOLD_BUFFER_MODE_NAME "useHoldBuffer"
#define RENDERER_MODULE_OPTIONS_GRAPHICS_PREMULTIPLY_COLOR_ALPHA_NAME "premultiplyColorAndAlpha"
#define RENDERER_MODULE_OPTIONS_GRAPHICS_TEXTURE_MODE_NAME "directTextureFlag"
#define RENDERER_MODULE_OPTIONS_GRAPHICS_AUTO_MIP_MAPPING_NAME "allowAutoMipMapping"

#define RENDERER_MODULE_OPTIONS_TEXTURES_SECTION_NAME "Textures"

#define RENDERER_MODULE_OPTIONS_TEXTURES_32_COUNT_NAME "maxTextures32"
#define RENDERER_MODULE_OPTIONS_TEXTURES_64_COUNT_NAME "maxTextures64"
#define RENDERER_MODULE_OPTIONS_TEXTURES_128_COUNT_NAME "maxTextures128"
#define RENDERER_MODULE_OPTIONS_TEXTURES_256_COUNT_NAME "maxTextures256"
#define RENDERER_MODULE_OPTIONS_TEXTURES_512_COUNT_NAME "maxTextures512"
#define RENDERER_MODULE_OPTIONS_TEXTURES_1024_COUNT_NAME "maxTextures1024"

#define RENDERER_MODULE_OPTIONS_TEXTURES_32_COUNT_TEMPLATE_NAME "maxTextures32=%d\n"
#define RENDERER_MODULE_OPTIONS_TEXTURES_64_COUNT_TEMPLATE_NAME "maxTextures64=%d\n"
#define RENDERER_MODULE_OPTIONS_TEXTURES_128_COUNT_TEMPLATE_NAME "maxTextures128=%d\n"
#define RENDERER_MODULE_OPTIONS_TEXTURES_256_COUNT_TEMPLATE_NAME "maxTextures256=%d\n"
#define RENDERER_MODULE_OPTIONS_TEXTURES_512_COUNT_TEMPLATE_NAME "maxTextures512=%d\n"
#define RENDERER_MODULE_OPTIONS_TEXTURES_1024_COUNT_TEMPLATE_NAME "maxTextures1024=%d\n"

#define RENDERER_MODULE_OPTIONS_TEXTURES_32_565_COUNT_NAME "maxTextures32_565"
#define RENDERER_MODULE_OPTIONS_TEXTURES_64_565_COUNT_NAME "maxTextures64_565"
#define RENDERER_MODULE_OPTIONS_TEXTURES_128_565_COUNT_NAME "maxTextures128_565"
#define RENDERER_MODULE_OPTIONS_TEXTURES_256_565_COUNT_NAME "maxTextures256_565"
#define RENDERER_MODULE_OPTIONS_TEXTURES_512_565_COUNT_NAME "maxTextures512_565"
#define RENDERER_MODULE_OPTIONS_TEXTURES_1024_565_COUNT_NAME "maxTextures1024_565"

#define RENDERER_MODULE_OPTIONS_TEXTURES_MIP_MAP_TEXTURE_SIZE_NAME "mipMapFlag"
#define RENDERER_MODULE_OPTIONS_TEXTURES_MIP_MAP_TEXTURE_SIZE_TEMPLATE_NAME "mipMapFlag=%d\n"

namespace Settings
{
    enum class AutoMipMappingOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class PremultiplyColorAlphaOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    void ReadValue(const char* section, const char* parameter, u32* value);
}