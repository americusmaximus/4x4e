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
#include "Assets.Sounds.hxx"

namespace Sounds
{
    enum class SoundMixMode : u32
    {
        None = 0,
        Simple = 1, // Mixing
        Advanced = 2 // Mixing & Spatialization
    };

    struct SoundDecoder
    {
        // TODO
    };

    // a.k.a. SampleInfo
    struct SoundSampleDescriptor
    {
        Assets::Sounds::SoundSampleDefinition Definition;

        f32 ReferenceDistance;
        f32 MinimumDistance;
        f32 MaximumDistance;

        void* AllocatedMemory1;

        u32 LoopMode; // TODO Enum
        s32 ChannelLength[2]; // TODO

        s32 Unk106; // TODO
        s32 Unk107; // TODO
        s32 Unk108; // TODO
        s32 Unk6; // TODO
        s32 Unk109; // TODO
        s32 Unk110; // TODO
        s32 Unk111; // TODO
        s32 Unk112; // TODO

        Assets::Sounds::SoundCacheMode CacheControl;

        s32 Priority; // TODO name ReferenceCount
        u32 Offset; // TODO: name Index
    };

    // a.k.a. SfxSample
    struct SoundSample
    {
        SoundSampleDescriptor Descriptor;

        //s32 Index;
        //u32 BufferIndex;
        //s32 StreamIndex;

        //s32 Unk5; // 



        s32 Unk6; // TODO
        s32 Unk7; // TODO, some sort of index
        s32 Length; // TODO type

        char Unk[320];


        ////s32 AllocatedMemorySize; // TODO better name!
        ////s32 Unk10; // TODO
        ////s32 Unk11; // TODO
        ////SoundDecoder* Decoder;
        ////s32 Unk13; // TODO
        ////s32 LockOffset; // TODO
        ////s32 LockLength; // TODO
        ////s32 Unk16; // TODO

        struct
        {
            s32 Offset; // TODO type
            s32 Length; // TODO type
        } Lock;
    };

    struct SoundEffectDescriptor
    {
        s32 NextChannelIndex; // TODO better name
        s32 Unknown101; // TODO
        f64x3 Location;
        s32 Unknown102; // TODO
        s32 Unknown103; // TODO
        f32x3 Velocity;
        s32 Unknown104; // TODO
        s32 Unknown105; // TODO
        f32 Volume;
        f32 HZ; // TODO
        s32 Unknown1001; // TODO
        s32 Unknown1002; // TODO
        s32 Unknown1003; // TODO
        u32 Unk30;// TODO enum
        s32 Unknown1004; // TODO
        f64 Unknown1005; // TODO
        s32 Unknown1007; // TODO
        s32 Unknown1008; // TODO
    };

    // a.k.a. SfxSlot
    struct SoundEffect
    {
        SoundEffectDescriptor Descriptor;

        s32 UnknownIndex; // TODO

        s32 Unk32; // TODO check the possible values. So far it looks either as Index or an Active/Inactive flag.

        SoundSample* Sample;

        f32 HZ;

        f32 Distances[8]; // TODO constant

        f32 Pans[8]; // TODO constant

        s32 Unknown1[8]; // TODO constant

        s32 Unknown204[8]; // TODO constant

        f32 MinimumDistance;

        s32 Unknown1001; // TODO

        f32 MaximumDistance;

        s32 Unknown1002; // TODO
        s32 Unknown1003; // TODO
        s32 Unknown1004; // TODO
        s32 Unknown1005; // TODO
        s32 Unknown1006; // TODO

        f64 Position;

        s32 Unknown104; // TODO

        u32 Options; // TODO: flags
    };
}