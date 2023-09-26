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

#include "Renderer.hxx"
#include "Renderer.Module.hxx"

using namespace Renderers;
using namespace Renderer::Module;

namespace Renderer
{
    RendererContainer RendererState;

    // 0x00567bd0
    BOOL AcquireRendererGraphicsCards(u32* count, char** apis, char** names, u32* dids, u32* vids)
    {
        if (!*RendererModule.State._IsActive) { return FALSE; }

        return (*RendererModule._AcquireGraphicsCardList)(count, apis, names, dids, vids);
    }

    // 0x00567c90
    u32 AcquireRendererTextureInformation(const u32 size)
    {
        if (!*RendererModule.State._IsActive) { return FALSE; }

        return (*RendererModule._AcquireTextureInformation)(size);
    }

    // 0x00567b80
    BOOL AcquireRendererVideoMemory(u32* local, u32* agp, u32* zero)
    {
        if (!*RendererModule.State._IsActive) { return FALSE; }

        return (*RendererModule._AcquireVideoMemory)(local, agp, zero);
    }

    // 0x00567100
    BOOL BeginRendererScene(void)
    {
        if (!*RendererState._IsActive) { return FALSE; }

        return (*RendererModule._BeginScene)();
    }

    // 0x00567830
    BOOL ClearRendererScreen(void)
    {
        if (!*RendererState._IsActive) { return FALSE; }

        return (*RendererModule._Clear)();
    }

    // 0x00567a00
    BOOL ClearRendererZBox(const u32 x0, const u32 y0, const u32 x1, const u32 y1)
    {
        if (!*RendererState._IsActive) { return FALSE; }

        return (*RendererModule._ClearZBox)(x0, y0, x1, y1);
    }

    // 0x005679e0
    BOOL ClearRendererZBuffer(void)
    {
        if (!*RendererState._IsActive) { return FALSE; }

        return (*RendererModule._ClearZBuffer)();
    }

    // 0x005673e0
    BOOL DrawRendererPolygon(RPVX* vertexes, const u32 vertexCount, const RendererMode mode)
    {
        if (!*RendererState._IsActive) { return FALSE; }

        return (*RendererModule._DrawPolygon)(vertexes, vertexCount, mode);
    }

    // 0x00567410
    BOOL DrawRendererPolygon2(const RPVX** vertexes, const u32 vertexCount, const RendererMode mode)
    {
        if (!*RendererState._IsActive) { return FALSE; }

        return (*RendererModule._DrawPolygon2)(vertexes, vertexCount, mode);
    }

    // 0x00567120
    BOOL EndRendererScene(void)
    {
        if (!*RendererState._IsActive) { return FALSE; }

        return (*RendererModule._EndScene)();
    }

    // 0x00567040
    BOOL ReleaseRenderer(void)
    {
        if (*RendererModule.State._IsActive)
        {
            (*RendererModule._Kill)();

            ReleaseRendererModule();
        }

        return TRUE;
    }

    // 0x005673b0
    BOOL RestoreRendererVideoMode(void)
    {
        if (*RendererState._IsActive)
        {
            *RendererState._IsActive = FALSE;

            return (*RendererModule._RestoreVideoMode)();
        }

        return FALSE;
    }

    // 0x00567bb0
    BOOL SelectRendererGraphicsCard(const u32 indx)
    {
        if (!*RendererModule.State._IsActive) { return FALSE; }

        *RendererModule.State._SelectedDeviceIndex = indx;

        return (*RendererModule._SelectGraphicsCard)(indx);
    }
}