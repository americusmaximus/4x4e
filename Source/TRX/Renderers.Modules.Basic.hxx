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

#include "Renderers.Basic.hxx"

#define RENDERER_MODULE_AUTHOR "Americus Maximus"

#define MAX_RENDERER_MODULE_INFORMATION_NAME_LENGTH 256
#define MAX_RENDERER_MODULE_INFORMATION_AUTHOR_LENGTH 256
#define MAX_RENDERER_MODULE_INFORMATION_UNKNOWN_LENGTH 7098

namespace Renderers::Modules
{
    struct RendererModuleInitializeColorOptions
    {
        u32* Display;	  // TODO names
        u32* Font;		  // TODO names
        u32* BitsPerPixel;// TODO names
    };

    struct RendererModuleInitializeArguments
    {
        RendererModuleInitializeColorOptions R;
        RendererModuleInitializeColorOptions G;
        RendererModuleInitializeColorOptions B;
        RendererBlendMode* BlendMode;
        u32* Unk11; //TODO type
        u32* Unk12; //TODO type
        u32* Unk13; //TODO
        s32* Unk14; //TODO
        s32* Unk15; //TODO
        s32* Unk16; //TODO
        s32* Unk17; //TODO
        void* Unk18; //TODO
        u32* TextureSize;
        void* Unk20; //TODO
        s32* Unk21; //TODO type
        RendererDitherMode* DitherMode;
        u32* MaximumTextureCount;
        RendererSyncRetraceMode* SyncRetraceMode;
        RendererTextureFilterMode* TextureFilterMode;
        RendererAcceleratedGraphicsPortMode* AcceleratedGraphicsPortMode;
        u32* Unk27; //TODO type
        RendererTextureMipMapFilterMode* TextureMipMapFilterMode;
        u32 Unk29; // TODO
        u32 Unk30; // TODO
        u32 Unk31; // TODO
        u32 Unk32; // TODO
        u32 Unk33; // TODO
        u32 Unk34; // TODO
        u32 Unk35; // TODO
        void* Unk36; //TODO
    };

    struct RendererModuleInitializeInformation
    {
        char Name[MAX_RENDERER_MODULE_INFORMATION_NAME_LENGTH];
        char Author[MAX_RENDERER_MODULE_INFORMATION_AUTHOR_LENGTH];

        u16 Unknown1;
        u16 Unknown2;
        u16 Unknown3;
        u16 Unknown4;
        u32 Unknown5;
        u16 Unknown6;

        char Unknown[MAX_RENDERER_MODULE_INFORMATION_UNKNOWN_LENGTH];
    };
}