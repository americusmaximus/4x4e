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

#define RENDERER_MODULE_OPTIONS_FILE_NAME ".\\system\\render.ini"

#define RENDERER_MODULE_OPTIONS_TEXTURES_SECTION_NAME "Textures"

#define RENDERER_MODULE_OPTIONS_TEXTURES_MIP_MAP_TEXTURE_SIZE_NAME "mipMapFlag"

#define RENDERER_MODULE_OPTIONS_OPENGL_SECTION_NAME "OpenGL"

#define RENDERER_MODULE_OPTIONS_OPENGL_SECTION_REDUCE_BLEND_MODE_NAME "reduceBlendModes"
#define RENDERER_MODULE_OPTIONS_OPENGL_SECTION_ALLOW_FRAME_BUFFER_LOCK_NAME "allowFrameBufferLocking"

namespace Settings
{
    enum class ReduceBlendOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    void ReadValue(const char* section, const char* parameter, u32* value);
}