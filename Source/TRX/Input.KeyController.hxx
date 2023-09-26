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

namespace Input
{
    struct KeyController;

    typedef const BOOL(CDECLAPI* KEYCONTROLLERACQUIRE) (KeyController* self, const s32 indx);
    typedef const BOOL(CDECLAPI* KEYCONTROLLERACQUIREONCE) (KeyController* self, const s32 indx);
    typedef const u32(CDECLAPI* KEYCONTROLLERRESET) (KeyController* self); // TODO type

    BOOL AcquireKeyController(KeyController* self, const s32 indx);
    BOOL AcquireKeyControllerOnce(KeyController* self, const s32 indx);
    u32 ResetKeyController(KeyController* self); // TODO type

    struct KeyControllerSelf
    {
        KEYCONTROLLERACQUIRE Acquire = (KEYCONTROLLERACQUIRE)&AcquireKeyController; // 0x0046fb00
        KEYCONTROLLERACQUIREONCE AcquireOnce = (KEYCONTROLLERACQUIREONCE)&AcquireKeyControllerOnce; // 0x0046fb10
        KEYCONTROLLERRESET Reset = (KEYCONTROLLERRESET)&ResetKeyController; // 0x0046fb30
    };

    // a.k.a.CKey
    struct KeyController
    {
        KeyControllerSelf* Self;
    };

    KeyController* ConstructKeyController(KeyController* self);
    void* ReleaseKeyController(KeyController* self, const Objects::ReleaseMode mode);

    struct KeyControllerContainer
    {
        KeyControllerSelf KeyControllerSelf; // 0x005ec4b4

        // 0x005ec4c0
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = (Objects::ABSTRACTOBJECTINITIALIZERINITIALIZE)&ConstructKeyController,
            .Assign = NULL,
            .Release = (Objects::ABSTRACTOBJECTINITIALIZERRELEASE)&ReleaseKeyController,
            .Size = sizeof(KeyController),
            .Name = "$CKey$$"
        };

        KeyController* _Instance = (KeyController*)0x00adbae4; // TODO statically initialized
        u8* _Keys = (u8*)0x00728850; // TODO, an array of unknown size
    };

    extern KeyControllerContainer KeyControllerState;

    BOOL AcquireKeyControllerControlState(KeyController* self);
    BOOL AcquireKeyControllerShiftState(KeyController* self);
    u32 AcquireKeyControllerUpperCaseValue(KeyController* self);
}