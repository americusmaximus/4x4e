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
#include "Direct.Draw.hxx"

namespace Graphics
{
    struct GraphicsContainer
    {
        BOOL* _IsActive = (BOOL*)0x00daf298; // TODO

        struct
        {
            s32* _Width = (s32*)0x0071ba3c; // TODO
            s32* _Height = (s32*)0x0071ba40; // TODO
        } View;

        struct
        {
            IDirectDraw** _Instance = (IDirectDraw**)0x00daf280; // TODO

            struct
            {
                u32** _Frame = (u32**)0x00606010; // TODO type, name
                u32** _Depth = (u32**)0x00daf290; // TODO
                u32** _DepthAligned = (u32**)0x006072d4; // TODO
            } Buffers;

            struct
            {
                IDirectDrawSurface** _Main = (IDirectDrawSurface**)0x00daf284; // TODO
                IDirectDrawSurface** _Back = (IDirectDrawSurface**)0x00daf288; // TODO

                BOOL* _IsActive = (BOOL*)0x00daf29c; // TODO, wrong name. IsSubOptimal?

                u32* _X = (u32*)0x00daf2ac; // TODO
                u32* _Y = (u32*)0x00daf2b0; // TODO

                u32* _Width = (u32*)0x00daf2a0; // TODO
                u32* _Height = (u32*)0x00daf2a4; // TODO
                u32* _BitsPerPixel = (u32*)0x00daf2a8; // TODO
            } Surfaces;

            IDirectDrawPalette** _Palette = (IDirectDrawPalette**)0x00daf28c; // TODO
        } DX;

        struct
        {
            u32** _Colors = (u32**)0x0071ba4c; // TODO
            u32** _Depth = (u32**)0x0071cd0c; // TODO
        } Pixels;

        BOOL* _Unknown1 = (BOOL*)0x00daf294; //TODO
    };

    extern GraphicsContainer GraphicsState;

    BOOL ConstructGraphics(void);
    BOOL InitializeGraphics(const u32 width, const u32 height, const u32 bpp);
    void LockGraphicsScreen(void);
    void ReleaseGraphics(void);
    void RestoreGraphicsVideoMode(void);
    void SelectGraphicsDimensions(const u32 x, const u32 y, const u32 width, const u32 height);
    void UnlockGraphicsScreen(void);

    typedef const void(CDECLAPI* FUN_00597700) (void); // TODO
    static FUN_00597700 FUN_00597700a = (FUN_00597700)0x00597700;//TODO

    typedef const void(CDECLAPI* FUN_00597830) (void); // TODO
    static FUN_00597830 FUN_00597830a = (FUN_00597830)0x00597830; // TODO

    typedef const void(CDECLAPI* FUN_00481720) (f32, f32, f32, f32); // TODO
    static FUN_00481720 FUN_00481720a = (FUN_00481720)0x00481720;//TODO
}