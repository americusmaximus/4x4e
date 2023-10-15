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

#define MAX_SOUND_VEHICLE_SOUND_EFFECT_COUNT 10

namespace Vehicles
{
    // a.k.a. CVehicleSfx
    struct VehicleSoundEffect
    {
        s32 Unk[44]; // TODO
    };

    VehicleSoundEffect* ConstructVehicleSoundEffect(VehicleSoundEffect* self);
    void* ReleaseVehicleSoundEffect(VehicleSoundEffect* self);

    struct VehicleSoundEffectContainer
    {
        VehicleSoundEffect* _Items = (VehicleSoundEffect*)0x00d6d198; // TODO Array of MAX_SOUND_VEHICLE_SOUND_EFFECT_COUNT

        // 9x005ef110
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = (Objects::ABSTRACTOBJECTINITIALIZERINITIALIZE)&ConstructVehicleSoundEffect,
            .Assign = NULL,
            .Release = (Objects::ABSTRACTOBJECTINITIALIZERRELEASE)&ReleaseVehicleSoundEffect,
            .Size = sizeof(VehicleSoundEffect),
            .Name = "$CVehicleSfx$$"
        };
    };

    extern VehicleSoundEffectContainer VehicleSoundEffectState;
}