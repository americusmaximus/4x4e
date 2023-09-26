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

#include "App.hxx"
#include "Native.Basic.hxx"

#define APP_WINDOW_NAME APP_NAME

namespace App::Windows
{
    struct AppWindowContainer
    {
        HWND* _HWND = (HWND*)0x00db2a88; //TODO
        const char* ClassName = APP_WINDOW_NAME; // 0x006023f4;
        const char* WindowName = APP_WINDOW_NAME; // 0x006023f8;
        HINSTANCE* _Instance = (HINSTANCE*)0x00db2a8c; //TODO

        u32* _Width = (u32*)0x005f8ac0; //TODO default value 320
        u32* _Height = (u32*)0x005f8ac4; //TODO default value 200
        u32* _BitsPerPixel = (u32*)0x005f8ac8; //TODO default value 8


        s32* _ActiveState = (s32*)0x006023fc; //TODO, default value -1
        BOOL* _ActiveMode = (BOOL*)0x00602400; //TODO default value 1
    };

    extern AppWindowContainer AppWindowState;

    void CreateAppWindow(void);
}