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
#include "Renderers.Basic.Vertexes.hxx"
#include "Renderers.Basic.Textures.hxx"

#define MAX_RENDERER_MODULE_FILE_NAME_LENGTH 64
#define MAX_RENDERER_MODULE_NAME_LENGTH 256

namespace Renderer
{
    enum class RendererTextureMipMapOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    struct RendererContainer
    {
        BOOL* _IsActive = (BOOL*)0x00728848; // TODO

        struct
        {
            BOOL* _IsUsingModule = (BOOL*)0x00d62c20; // TODO

            char ModuleName[MAX_RENDERER_MODULE_NAME_LENGTH];

            char* _ModuleFileName = (char*)0x00601750; // TODO array of [MAX_RENDERER_MODULE_FILE_NAME_LENGTH], default value

            u32* _SelectedDeviceIndex = (u32*)0x0078e03c; // TODO

            RendererTextureMipMapOptionsMode* _TextureMipMapMode = (RendererTextureMipMapOptionsMode*)0x00ae222c; // TODO
        } Options;
    };

    extern RendererContainer RendererState;

    BOOL AcquireRendererGraphicsCards(u32* count, char** apis, char** names, u32* dids, u32* vids);
    BOOL AcquireRendererVideoMemory(u32* local, u32* agp, u32* zero);
    BOOL BeginRendererScene(void);
    BOOL ClearRendererScreen(void);
    BOOL ClearRendererZBox(const u32 x0, const u32 y0, const u32 x1, const u32 y1);
    BOOL ClearRendererZBuffer(void);
    BOOL DrawRendererPolygon(Renderers::RPVX* vertexes, const u32 vertexCount, const Renderers::RendererMode mode);
    BOOL DrawRendererPolygon2(const Renderers::RPVX** vertexes, const u32 vertexCount, const Renderers::RendererMode mode);
    BOOL EndRendererScene(void);
    BOOL ReleaseRenderer(void);
    BOOL RestoreRendererVideoMode(void);
    BOOL SelectRendererGraphicsCard(const u32 indx);
    u32 AcquireRendererTextureInformation(const u32 size);
}