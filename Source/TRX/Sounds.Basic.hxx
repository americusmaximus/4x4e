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
#include "IO.Streams.hxx"

#define SOUND_BITS_8 8
#define SOUND_BITS_16 16

#define SOUND_BYTES_1 1
#define SOUND_BYTES_2 2

#define SOUND_CHANNEL_COUNT_NONE 0
#define SOUND_CHANNEL_COUNT_MONO 1
#define SOUND_CHANNEL_COUNT_STEREO 2

#define SOUND_FREQUENCY_11025 11025
#define SOUND_FREQUENCY_22050 22050
#define SOUND_FREQUENCY_44100 44100

#define MAX_SOUND_CHANNEL_COUNT 8

#define SOUND_DIRECTORY_NAME "sound"

// NOTE: klp is a legacy metadata file used in earlier games.
// The one I found in "Fly" had the following content: 1 0 0
#define SOUND_FILE_KLP_EXTENSION_NAME "klp"

// NOTE: sfx is a modern metadata file used to describe sound.
#define SOUND_FILE_SFX_EXTENSION_NAME "sfx"

namespace Sounds
{
    enum class SoundMixMode : u32
    {
        None = 0,
        Simple = 1, // Mixing
        Advanced = 2 // Mixing & Spatialization
    };

    enum class SoundLoopMode : u32
    {
        None = 0,
        Looping = 1,
        Unknown2 = 2 // TODO
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

        SoundLoopMode LoopMode;
        s32 ChannelLength[2]; // TODO

        s32 Unk106; // TODO
        s32 Unk107; // TODO
        s32 Unk108; // TODO
        s32 Unk6; // TODO, may be an enum
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

        s32 Unk6; // TODO
        s32 Unk7; // TODO, some sort of index
        s32 Length; // TODO type

        s32 AllocatedMemorySize;
        s32 Unk10;
        s32 Unl11;
        SoundDecoder* Decoder;
        s32 Unk13;

        IO::Streams::InStreamFile Stream;

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

        // NOTE, this also can be f64x3 (see ComputeSoundEffectLocationVelocity)
        f32x3* Unknown102; // TODO
        s32 Unknown103; // TODO

        f32x3 Velocity;

        // NOTE, this also can be f64x3 + int (see ComputeSoundEffectLocationVelocity)
        f32x3* Unknown104; // TODO
        s32 Unknown105; // TODO

        f32 Volume;
        f32 HZ; // TODO
        f32 RemainingDelay;
        s32 Unknown1002[2]; // TODO
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

        BOOL Unk32; // TODO enum

        SoundSample* Sample;

        f32 HZ;

        f32 Distances[MAX_SOUND_CHANNEL_COUNT];

        f32 Pans[MAX_SOUND_CHANNEL_COUNT];

        s32 Unknown1[MAX_SOUND_CHANNEL_COUNT]; // TODO

        s32 Unknown204[MAX_SOUND_CHANNEL_COUNT]; // TODO

        f32 MinimumDistance;

        f32 Unknown1001; // TODO

        f32 MaximumDistance;

        f32 Unknown1002; // TODO
        f32 Unknown1003; // TODO
        s32 Unknown1004; // TODO

        BOOL DebugMode;

        s32 Unknown1006; // TODO

        f64 Position;

        s32 Unknown104; // TODO

        u32 Options; // TODO: flags
    };
}