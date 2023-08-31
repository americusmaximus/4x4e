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
#include "Graphics.Basic.hxx"
#include "Mathematics.Basic.hxx"

#define MAX_GRAPHICS_CARD_AVAILABLE_MEMORY (128 * 1024 * 1024)

#define MAX_COLOR_TABLE_BUFFER_SIZE 256

#define MAX_DEVICE_COUNT 16
#define MAX_DEVICE_IDENTIFIER_STRING_LENGTH 512
#define STANDARD_DEVICE_IDENTIFIER_STRING_LENGTH 256

#define MAX_RENDERER_POLYGON_VERTEX_COUNT 32

#define RENDERER_SIXTEEN_THOUSAND_ELEMENT_COUNT 16000
#define RENDERER_ALMOST_SIXTEEN_THOUSAND_ELEMENT_COUNT 15990

#define SIXTY_FIVE_THOUSAND_FIVE_HUNDRED_THIRTY_FIVE (65535)
#define SIXTY_FIVE_THOUSAND_FIVE_HUNDRED_THIRTY_SIX (65536)
#define SIXTEEN_MILLION_SEVEN_HUNDRED_SEVENTY_SEVEN_THOUSAND_TWO_HUNDRED_SIXTEEN (16777216)

#define DEFAULT_RENDERER_CLIPPING_NEAR (1.0f)
#define DEFAULT_RENDERER_CLIPPING_FAR (6400.0f)

namespace Renderers
{
    enum class RendererAcceleratedGraphicsPortMode : u32
    {
        Auto = 0,
        X1 = 1,
        X2 = 2,
        X4 = 3,
        X8 = 4
    };

    OPTIONS(RendererMode, u32)
    {
        None = 0x0,
        U0x1 = 0x1,
        U0x2 = 0x2,
        U0x4 = 0x4,
        U0x8 = 0x8,
        U0x10 = 0x10,
        U0x20 = 0x20,
        U0x40 = 0x40,
        U0x80 = 0x80,
        U0x100 = 0x100,
        U0x200 = 0x200,
        U0x400 = 0x400
    };

    enum class RendererClippingMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class RendererCullMode : u32
    {
        CCW = 0,
        CW = 1,
        None = 2
    };

    enum class RendererClampMode : u32
    {
        Wrap = 0,
        Clamp = 1
    };

    enum class RendererTextureFilterMode : u32
    {
        Nearest = 0,
        Linear = 1
    };

    enum class RendererTextureMipMapFilterMode : u32
    {
        Simple = 0,
        Complex = 1
    };

    enum class RendererDitherMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class RendererSyncRetraceMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class RendererBlendMode : u32
    {
        OneMinusSourceAlpha = 0,
        One = 1,
        SourceColor = 2,
        OneMinusSourceColor = 3
    };
}