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

#include "Objects.hxx"

namespace Editor
{
    // a.k.a. CEditorTools
    // NOTE: Originally a struct of 1 byte.
    struct Editor
    {
        s32 Unknown; // TODO
    };

    Editor* ConstructEditor(Editor* self);
    void* ReleaseEditor(Editor* self, const Objects::ReleaseMode mode);

    struct EditorContainer
    {
        // 0x005ebcb0
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = (Objects::ABSTRACTOBJECTINITIALIZERINITIALIZE)&ConstructEditor,
            .Assign = NULL,
            .Release = (Objects::ABSTRACTOBJECTINITIALIZERRELEASE)&ReleaseEditor,
            .Size = sizeof(Editor),
            .Name = "$CEditorTools$$"
        };

        Editor* _Instance = (Editor*)0x0071af94; // TODO
        Editor** _InstancePointer = (Editor**)0x005f81c0; // TODO, points to above (^^) where this should be located?

        struct
        {
            s32* _Active = (s32*)0x0071ba10; // TODO type, better name
        } Controls;

        struct
        {
            s32* _Count = (s32*)0x0071af9c; // TODO

            struct
            {
                s32* _Unk005 = (s32*)0x00726f08; // TODO
                s32* _Unk006 = (s32*)0x00726f0c; // TODO
                s32* _Unk007 = (s32*)0x00726f10; // TODO
                s32* _Unk008 = (s32*)0x00726f14; // TODO

                s32* _Unk013 = (s32*)0x00726ef8; // TODO
                s32* _Unk014 = (s32*)0x00726efc; // TODO
                s32* _Unk015 = (s32*)0x00726f00; // TODO
                s32* _Unk016 = (s32*)0x00726f04; // TODO
                s32* _Unk017 = (s32*)0x00726f18; // TODO
                s32* _Unk018 = (s32*)0x00726f1c; // TODO
            } Active;
        } Windows;

        u32* _Colors = (u32*)0x0071b988; // TODO array of 32
    };

    extern EditorContainer EditorState;

    void EditorConstructor(void);

    void PopEditorWindow(Editor* self);
    void SelectEditorColors(Editor* self);
}