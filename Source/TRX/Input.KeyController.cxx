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

#include "Input.hxx"
#include "Input.KeyController.hxx"

#include <ctype.h>

using namespace Objects;

namespace Input
{
    KeyControllerContainer KeyControllerState;

    // 0x0046fae0
    KeyController* ConstructKeyController(KeyController* self)
    {
        self->Self = &KeyControllerState.KeyControllerSelf;

        return self;
    }

    // 0x0046faf0
    void* ReleaseKeyController(KeyController* self, const ReleaseMode mode)
    {
        self->Self = &KeyControllerState.KeyControllerSelf;

        return self;
    }

    // 0x0046fb00
    BOOL AcquireKeyController(KeyController* self, const s32 indx)
    {
        return KeyControllerState._Keys[indx];
    }

    // 0x0046fb10
    BOOL AcquireKeyControllerOnce(KeyController* self, const s32 indx)
    {
        const auto result = KeyControllerState._Keys[indx];

        KeyControllerState._Keys[indx] = FALSE;

        return result;
    }

    // 0x0046fb30
    u32 ResetKeyController(KeyController* self)
    {
        return ResetInput();
    }

    // 0x0046fbb0
    BOOL AcquireKeyControllerControlState(KeyController* self)
    {
        return KeyControllerState._Keys[INPUT_KEY_LEFT_CTRL] || KeyControllerState._Keys[INPUT_KEY_RIGHT_CTRL];
    }

    // 0x0046fbd0
    BOOL AcquireKeyControllerShiftState(KeyController* self)
    {
        return KeyControllerState._Keys[INPUT_KEY_LEFT_SHIFT] || KeyControllerState._Keys[INPUT_KEY_RIGHT_SHIFT];
    }

    // 0x0046fb50
    u32 AcquireKeyControllerUpperCaseValue(KeyController* self)
    {
        return toupper(AcquireUnknownArray1Value());
    }
}