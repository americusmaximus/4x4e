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

#include "Assets.Sounds.hxx"
#include "Native.Basic.hxx"
#include "Sounds.Controllers.hxx"
#include "Sounds.Disk.hxx"

#define MIN_SOUND_EFFECT_CHANNEL_COUNT 0
#define MAX_SOUND_EFFECT_CHANNEL_COUNT 32

#define MIN_SOUND_LOCK_COUNT 0
#define DEFAULT_SOUND_LOCK_COUNT 1
#define MAX_SOUND_LOCK_COUNT 100

#define MAX_SOUND_THREAD_ACTION_ITERATION_COUNT 100

#define MIN_SOUND_VOLUME (0.0f)
#define MAX_SOUND_VOLUME (1.0f)

#define MAX_SOUND_UNKNOWN_ALLOCATION_SIZE (2 * 1024 * 1024)
#define MAX_SOUND_SAMPLE_ALLOCATION_SIZE (4 * 1024 * 1024)

namespace Sounds
{
    struct SoundContainer
    {
        SoundMixMode MixMode = SoundMixMode::Advanced; // 0x005ff570

        struct
        {
            s32* _Count = (s32*)0x00d621a4; // TODO
            HANDLE Mutex; // 0x00d621b4 
        } Lock;

        BOOL* _SoundChannelStates = (BOOL*)0x00d616d8; // TODO ARRAY of 32

        struct
        {
            u32* _Bits = (u32*)0x005ff558; // TODO, default value SOUND_BITS_16
            u32* _Channels = (u32*)0x005ff55c; // TODO, default value SOUND_CHANNEL_COUNT_STEREO
            u32* _HZ = (u32*)0x005ff560; // TODO, default value SOUND_FREQUENCY_22050

            f32 MaximumSoftWareLatency = DEFAULT_SOUND_LATENCY; // 0x005ff584
        } Options;

        struct
        {
            f32* _TimeValue = (f32*)0x00d621a8;//TODO
            BOOL* _IsActive = (BOOL*)0x00d621ac;//TODO
            BOOL* _IsQuit = (BOOL*)0x00d621b0;//TODO
        } Thread;

        struct
        {
            SoundEffect* _Cache = (SoundEffect*)0x00d51c70; // TODO array of 64

            u32 Index; // 0x00d61650

            struct
            {
                f32 X[2]; // 0x00d615f0 // TODO
                f32 Y[2]; // 0x00d615f8 // TODO
                f32 Z[2]; // 0x00d61600 // TODO
            } Velocity;

            struct
            {
                struct
                {
                    f32* _X = (f32*)0x00d61608; // TODO array of 2
                    f32* _Y = (f32*)0x00d61610; // TODO array of 2
                    f32* _Z = (f32*)0x00d61618; // TODO array of 2
                } XYZ;

                struct
                {
                    f32 X[2]; // 0x00d61620 // TODO
                    f32 Y[2]; // 0x00d61628 // TODO
                    f32 Z[2]; // 0x00d61630 // TODO
                } Top;

                struct
                {
                    f32 X[2]; // 0x00d61638 // TODO
                    f32 Y[2]; // 0x00d61640 // TODO
                    f32 Z[2]; // 0x00d61648 // TODO
                } Front;
            } Orientation;

            struct
            {
                f64 X[2]; // 0x00d615c0 // TODO
                f64 Y[2]; // 0x00d615d0 // TODO
                f64 Z[2]; // 0x00d615e0 // TODO
            } Position;

            struct
            {
                struct
                {
                    f64 X[MAX_SOUND_CHANNEL_COUNT]; // 0x00d620d8
                    f64 Y[MAX_SOUND_CHANNEL_COUNT]; // 0x00d62118
                    f64 Z[MAX_SOUND_CHANNEL_COUNT]; // 0x00d62158
                } Position; // TODO move around

                // NOTE: used from somewhere unknown
                f32* _Volumes = (f32*)0x00d61658; // TODO MAX_SOUND_SFX_CHANNEL_COUNT  (32)

                u32* _Count = (u32*)0x005ff554; // TODO, default value MAX_SOUND_EFFECT_CHANNEL_COUNT, move to Options?
            } Channels;
        } Effects;

        struct
        {
            f32 Volume; // 0x00d62198
            f32 Time; // 0x00d6219c
            f32 Damping; // 0x00d621a0
        } Environment;

        s32* _SoundTime1 = (s32*)0x00d621b8; //TODO
        s32 UnknownSoundCount1 = 1; // 0x00d61654

        AbstractSoundDeviceController** _SoundDeviceController = (AbstractSoundDeviceController**)0x00d62018; // TODO
        AbstractSoundRecordingDeviceController** _SoundRecordingDeviceController = (AbstractSoundRecordingDeviceController**)0x00d62a00; // TODO

        SoundSample* _SoundEffectSamples = (SoundSample*)0x00d569bc; // TODO 64 elements, todo name
        s32* _SoundEffectIndex = (s32*)0x00d569b8; // TODO
        f32* _UnknownSoundEffectValue1 = (f32*)0x00d51c60; // TODO, looks like it is read-only and always zero

        SoundEffectDescriptor* _SoundEffectDescriptors = (SoundEffectDescriptor*)0x00d56678; // TOOD array of 8
        s32* _SoundEffectDescriptorIndex = (s32*)0x00d56670;

        u32 UnknownAllocationSize = MAX_SOUND_UNKNOWN_ALLOCATION_SIZE; // 0x005ff57c
        u32 MaximumSoundSampleAllocationSize = MAX_SOUND_SAMPLE_ALLOCATION_SIZE; // 0x005ff580
    };

    extern SoundContainer SoundState;

    BOOL RestartSoundThread(const f32 value);
    BOOL StopSoundThread(void);
    DWORD WINAPI StartSoundThread(LPVOID);
    f32 AcquireMaximumSoftWareSoundLatency(void);
    f64 AcquireUnknownSoundValue101(const f64 value);
    SoundMixMode AcquireSoundMixMode(void);
    u32 AcquireSoundChannelCount(void);
    u32 AcquireSoundOutputBitsOptions(void);
    u32 AcquireSoundOutputFrequencyOptions(void);
    void AcquireSoundOutputOptions(u32* bits, u32* channels, u32* hz);
    void LockSounds(void);
    void ReleaseSounds(void);
    void SelectMaximumSoftWareLatency(const f32 value);
    void SelectSoundAllocation(const u32 unk, const u32 samples);
    void SelectSoundMixMode(const SoundMixMode mode);
    void UnlockSound1(void);
    void UnlockSound2(void);
    void UnlockSound3(void);
    void UnlockSounds(const s32 value);

    typedef const void(CDECLAPI* FUN_0055CAD0) (SoundEffect*); // TODO
    static FUN_0055CAD0 FUN_0055cad0 = (FUN_0055CAD0)0x0055cad0;//TODO

    typedef const BOOL(CDECLAPI* FUN_004925A0) (SoundDecoder*, s32); // TODO
    static FUN_004925A0 FUN_004925a0 = (FUN_004925A0)0x004925a0;//TODO
}