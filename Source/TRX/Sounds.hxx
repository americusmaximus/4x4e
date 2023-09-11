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

#define MAX_SOUND_LOCK_COUNT 100

#define MAX_SOUND_THREAD_ACTION_ITERATION_COUNT 100

namespace Sounds
{
    struct SoundContainer
    {
        SoundMixMode* _MixMode = (SoundMixMode*)0x0055fdb0; // TODO def value 2

        struct
        {
            s32* _Count = (s32*)0x00d621a4;//TODO
            HANDLE* _Mutex = (HANDLE*)0x00d621b4;//TODO
        } Lock;

        struct
        {
            f32* ChannelVolumes1 = (f32*)0x00d61658; // TODO MAX_SOUND_SFX_CHANNEL_COUNT  (32)
        } SFX;

        BOOL* _SoundChannelStates = (BOOL*)0x00d616d8; // TODO ARRAY of 32

        struct
        {
            u32* _Bits = (u32*)0x005ff558; // TODO, default value 16
            u32* _ChannelCount = (u32*)0x005ff55c; // TODO, default value 2
            u32* _HZ = (u32*)0x005ff560; // TODO, default value 22050

            f32* _MaximumSoftWareLatency = (f32*)0x005ff584;//todo, default value 0.5f;
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

            u32* _Index = (u32*)0x00d61650; // TODO

            struct
            {
                f32* _X = (f32*)0x00d615f0; // TODO array of 2
                f32* _Y = (f32*)0x00d615f8; // TODO array of 2
                f32* _Z = (f32*)0x00d61600; // TODO array of 2
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
                    f32* _X = (f32*)0x00d61620; // TODO array of 2
                    f32* _Y = (f32*)0x00d61628; // TODO array of 2
                    f32* _Z = (f32*)0x00d61630; // TODO array of 2
                } Top;

                struct
                {
                    f32* _X = (f32*)0x00d61638; // TODO array of 2
                    f32* _Y = (f32*)0x00d61640; // TODO array of 2
                    f32* _Z = (f32*)0x00d61648; // TODO array of 2
                } Front;
            } Orientation;

            struct
            {
                f64* _X = (f64*)0x00d615c0; // TODO array of 2
                f64* _Y = (f64*)0x00d615d0; // TODO array of 2
                f64* _Z = (f64*)0x00d43670; // TODO array of 2
            } Position;
        } Effects;

        s32* _SoundTime1 = (s32*)0x00d621b8; //TODO
        s32* _SoundTimeUnknown1 = (s32*)0x00d61654; //TODO, looks like SoundMixMode, default value 1

        AbstractSoundDeviceController** _SoundDeviceController = (AbstractSoundDeviceController**)0x00d62018; // TODO
        AbstractSoundRecordingDeviceController** _SoundRecordingDeviceController = (AbstractSoundRecordingDeviceController**)0x00d62a00; // TODO

        SoundSample* _SoundEffectSamples = (SoundSample*)0x00d569bc; // TODO 64 elements, todo name
    };

    extern SoundContainer SoundState;

    void LockSounds(void);
    void UnlockSound1(void);
    void UnlockSound2(void);
    void UnlockSound3(void);

    SoundMixMode AcquireSoundMixMode2(void);

    BOOL RestartSoundThread(const f32 value);
    DWORD WINAPI StartSoundThread(LPVOID);
    BOOL StopSoundThread(void);

    f32 AcquireMaximumSoftWareSoundLatency(void);
    void SelectMaximumSoftWareLatency(const f32 value);

    u32 AcquireSoundChannelCount(void);
    f32 AcquireSoundEffectChannelVolume(const s32 indx);
    void AcquireSoundOutputOptions(u32* bits, u32* channels, u32* hz);
    f64 AcquireUnknownSoundValue101(const f64 value);
    s32 AcquireSoundSampleDescriptorValue204(SoundSampleDescriptor* self, const s32 offset);
    BOOL AcquireSoundEffectChannelState(const s32 indx);
    void SelectSoundEffectChannelState(const s32 indx, const BOOL value);


    void ReleaseSoundSample(SoundSample* self);
    void ReleaseSoundEffectSamples(void);
    void ReleaseSoundEffect(SoundEffect* self);
    void ReleaseSoundSampleMemory(SoundSample* self);
    void SelectSoundEffectChannelVolume(const s32 indx, const f32 volume);
    void* LockSoundSample(SoundSample* self, const s32 offset, const s32 length);
    void UnlockSoundSample(SoundSample* self);
    s32 AcquireSoundSampleOffset(SoundSample* self, const s32 offset);
    BOOL PollSoundEffectStream(SoundEffect* self);
    void UpdateSoundEffectPosition(SoundEffect* self, const f64 position);
    u32 AcquireUnknownSoundSampleValue1(SoundSample* self);

    typedef const BOOL(CDECLAPI* FUN_005550E0) (void); // TODO
    static FUN_005550E0 FUN_005550e0 = (FUN_005550E0)0x005550e0;//TODO
    typedef const void(CDECLAPI* FUN_00559CF0) (void); // TODO
    static FUN_00559CF0 FUN_00559cf0 = (FUN_00559CF0)0x00559cf0;//TODO
    typedef const void(CDECLAPI* FUN_0055CAD0) (SoundEffect*); // TODO
    static FUN_0055CAD0 FUN_0055cad0 = (FUN_0055CAD0)0x0055cad0;//TODO
    typedef const BOOL(CDECLAPI* FUN_0055BAC0) (SoundEffect*, f32); // TODO
    static FUN_0055BAC0 FUN_0055bac0 = (FUN_0055BAC0)0x0055bac0;//TODO
}