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

#include "Direct.Sound.hxx"
#include "Sounds.Controllers.hxx"

#define INVALID_ACTIVE_SOUND_BUFFER_POOL_INDEX 0
#define MIN_ACTIVE_SOUND_BUFFER_POOL_INDEX 1
#define MAX_ACTIVE_SOUND_BUFFER_POOL_SIZE 31

namespace Sounds
{
    struct SoundUnk0x18//TODO
    {
        IDirectSoundBuffer* Buffer;

        s32 Unk1;//TODO
        s32 Unk2;//TODO
        u32 Unk3;//TODO

        void* Audio;//TODO
        u32 AudioSize;//TODO
    };

    struct SoundUnk0x24//TODO
    {
        IDirectSoundBuffer* Buffer;
        IDirectSound3DBuffer* AdvancedBuffer;

        s32 Unk3;//TODO
        s32 Unk4;//TODO
        s32 Unk5;//TODO
        s32 Volume;
        s32 Pan;
        s32 HZ;
        s32 Unk9;//TODO
    };

    void SoundDirectSoundDeviceControllerConstructor(void);
    AbstractSoundDeviceController* InitializeSoundDirectSoundDeviceController(const s32 indx);

    BOOL ReleaseSoundDirectSoundDeviceController(AbstractSoundDeviceController* self);
    BOOL StartSoundDirectSoundDeviceController(AbstractSoundDeviceController* self);
    BOOL StopSoundDirectSoundDeviceController(AbstractSoundDeviceController* self);
    BOOL ResetSoundDirectSoundDeviceController(AbstractSoundDeviceController* self, const u32 bits, const u16 channels, const u32 hz, u32* count);
    BOOL PollSoundDirectSoundDeviceController(AbstractSoundDeviceController* self);
    SoundMixMode QuerySoundDirectSoundDeviceControllerCapabilities(AbstractSoundDeviceController* self);
    void ApplySoundDirectSoundDeviceControllerOptions(AbstractSoundDeviceController* self);
    void SelectSoundDirectSoundDeviceControllerPosition(AbstractSoundDeviceController* self, const f64 x, const f64 y, const f64 z);
    void SelectSoundDirectSoundDeviceControllerOrientation(AbstractSoundDeviceController* self, const f32 x, const f32 y, const f32 z, const f32 xt, const f32 yt, const f32 zt, const f32 xf, const f32 yf, const f32 zf);
    void SelectSoundDirectSoundDeviceControllerVelocity(AbstractSoundDeviceController* self, const f32 x, const f32 y, const f32 z);
    void SelectSoundDirectSoundDeviceControllerDistanceFactor(AbstractSoundDeviceController* self, const f32 value);
    void SelectSoundDirectSoundDeviceControllerEnvironment(AbstractSoundDeviceController* self, const f32 volume, const f32 time, const f32 damping);
    u32 AllocateSoundDirectSoundDeviceControllerSoundSample(AbstractSoundDeviceController* self, const SoundSampleDescriptor* desc);
    void ReleaseSoundDirectSoundDeviceControllerSoundSample(AbstractSoundDeviceController* self, const s32 indx);
    void* LockSoundDirectSoundDeviceControllerSoundSample(AbstractSoundDeviceController* self, const s32 indx, const s32 offset, const s32 length);
    void UnlockSoundDirectSoundDeviceControllerSoundSample(AbstractSoundDeviceController* self, const s32 indx, const s32 offset, const s32 length);
    u32 AllocateSoundDirectSoundDeviceControllerSoundEffect(AbstractSoundDeviceController* self, const s32 indx);
    BOOL SelectSoundDirectSoundDeviceControllerSoundEffectOptions(AbstractSoundDeviceController* self, SoundEffect* effect, const u32 mode);
    f64 AcquireSoundDirectSoundDeviceControllerSoundEffectPosition(AbstractSoundDeviceController* self, SoundEffect* effect);
    BOOL StartSoundDirectSoundDeviceControllerSoundEffect(AbstractSoundDeviceController* self, SoundEffect* effect);
    void ReleaseSoundDirectSoundDeviceControllerSoundEffect(AbstractSoundDeviceController* self, SoundEffect* effect);
    BOOL AcquireSoundDirectSoundDeviceControllerSoundEffectState(AbstractSoundDeviceController* self, SoundEffect* effect);

    // INHERITANCE: AbstractSoundDeviceControllerSelf
    struct SoundDirectSoundDeviceControllerSelf
    {
        ABSTRACTSOUNDDEVICECONTROLLERRELEASE Release = (ABSTRACTSOUNDDEVICECONTROLLERRELEASE)&ReleaseSoundDirectSoundDeviceController; // 0x005554c0
        ABSTRACTSOUNDDEVICECONTROLLERSTART Start = (ABSTRACTSOUNDDEVICECONTROLLERSTART)&StartSoundDirectSoundDeviceController; // 0x00555590
        ABSTRACTSOUNDDEVICECONTROLLERSTOP Stop = (ABSTRACTSOUNDDEVICECONTROLLERSTOP)&StopSoundDirectSoundDeviceController; // 0x00555700
        ABSTRACTSOUNDDEVICECONTROLLERRESET Reset = (ABSTRACTSOUNDDEVICECONTROLLERRESET)&ResetSoundDirectSoundDeviceController; // 0x005559c0
        ABSTRACTSOUNDDEVICECONTROLLERPOLL Poll = (ABSTRACTSOUNDDEVICECONTROLLERPOLL)&PollSoundDirectSoundDeviceController; // 0x00555ee0
        ABSTRACTSOUNDDEVICECONTROLLERQUERYCAPABILITIES QueryCapabilities = (ABSTRACTSOUNDDEVICECONTROLLERQUERYCAPABILITIES)&QuerySoundDirectSoundDeviceControllerCapabilities; // 0x00555fc0
        ABSTRACTSOUNDDEVICECONTROLLERAPPLYOPTIONS ApplyOptions = (ABSTRACTSOUNDDEVICECONTROLLERAPPLYOPTIONS)&ApplySoundDirectSoundDeviceControllerOptions; // 0x00557c60
        ABSTRACTSOUNDDEVICECONTROLLERSELECTPOSITION SelectPosition = (ABSTRACTSOUNDDEVICECONTROLLERSELECTPOSITION)&SelectSoundDirectSoundDeviceControllerPosition; // 0x00556080
        ABSTRACTSOUNDDEVICECONTROLLERSELECTORIENTATION SelectOrientation = (ABSTRACTSOUNDDEVICECONTROLLERSELECTORIENTATION)SelectSoundDirectSoundDeviceControllerOrientation; // 0x005560c0
        ABSTRACTSOUNDDEVICECONTROLLERSELECTVELOCITY SelectVelocity = (ABSTRACTSOUNDDEVICECONTROLLERSELECTVELOCITY)&SelectSoundDirectSoundDeviceControllerVelocity; // 0x005560f0
        ABSTRACTSOUNDDEVICECONTROLLERSELECTDISTANCEFACTOR SelectDistanceFactor = (ABSTRACTSOUNDDEVICECONTROLLERSELECTDISTANCEFACTOR)&SelectSoundDirectSoundDeviceControllerDistanceFactor; // 0x00556120
        ABSTRACTSOUNDDEVICECONTROLLERSELECTENVIRONMENT SelectEnvironment = (ABSTRACTSOUNDDEVICECONTROLLERSELECTENVIRONMENT)&SelectSoundDirectSoundDeviceControllerEnvironment; // 0x00557c10
        ABSTRACTSOUNDDEVICECONTROLLERALLOCATESOUNDSAMPLE AllocateSoundSample = (ABSTRACTSOUNDDEVICECONTROLLERALLOCATESOUNDSAMPLE)&AllocateSoundDirectSoundDeviceControllerSoundSample; // 0x00556150
        ABSTRACTSOUNDDEVICECONTROLLERRELEASESOUNDSAMPLE ReleaseSoundSample = (ABSTRACTSOUNDDEVICECONTROLLERRELEASESOUNDSAMPLE)&ReleaseSoundDirectSoundDeviceControllerSoundSample; // 0x00556460
        ABSTRACTSOUNDDEVICECONTROLLERLOCKSOUNDSAMPLE LockSoundSample = (ABSTRACTSOUNDDEVICECONTROLLERLOCKSOUNDSAMPLE)&LockSoundDirectSoundDeviceControllerSoundSample; // 0x005564c0
        ABSTRACTSOUNDDEVICECONTROLLERUNLOCKSOUNDSAMPLE UnlockSoundSample = (ABSTRACTSOUNDDEVICECONTROLLERUNLOCKSOUNDSAMPLE)&UnlockSoundDirectSoundDeviceControllerSoundSample; // 0x00556620
        ABSTRACTSOUNDDEVICECONTROLLERALLOCATESOUNDEFFECT AllocateSoundEffect = (ABSTRACTSOUNDDEVICECONTROLLERALLOCATESOUNDEFFECT)&AllocateSoundDirectSoundDeviceControllerSoundEffect; // 0x00556700
        ABSTRACTSOUNDDEVICECONTROLLERSELECTSOUNDEFFECTOPTIONS SelectSoundEffectOptions = (ABSTRACTSOUNDDEVICECONTROLLERSELECTSOUNDEFFECTOPTIONS)&SelectSoundDirectSoundDeviceControllerSoundEffectOptions; // 0x005569c0
        ABSTRACTSOUNDDEVICECONTROLLERACQUIRESOUNDEFFECTPOSITION AcquireSoundEffectPosition = (ABSTRACTSOUNDDEVICECONTROLLERACQUIRESOUNDEFFECTPOSITION)&AcquireSoundDirectSoundDeviceControllerSoundEffectPosition; // 0x005573f0
        ABSTRACTSOUNDDEVICECONTROLLERSTARTSOUNDEFFECT StartSoundEffect = (ABSTRACTSOUNDDEVICECONTROLLERSTARTSOUNDEFFECT)&StartSoundDirectSoundDeviceControllerSoundEffect; // 0x00557510
        ABSTRACTSOUNDDEVICECONTROLLERRELEASESOUNDEFFECT ReleaseSoundEffect = (ABSTRACTSOUNDDEVICECONTROLLERRELEASESOUNDEFFECT)&ReleaseSoundDirectSoundDeviceControllerSoundEffect; // 0x00557970
        ABSTRACTSOUNDDEVICECONTROLLERACQUIRESOUNDEFFECTSTATE AcquireSoundEffectState = (ABSTRACTSOUNDDEVICECONTROLLERACQUIRESOUNDEFFECTSTATE)&AcquireSoundDirectSoundDeviceControllerSoundEffectState; // 0x00557340
    };

    // a.k.a. DirectSoundDevice
    // INHERITANCE: AbstractSoundDeviceController
    struct SoundDirectSoundDeviceController
    {
        AbstractSoundDeviceControllerSelf* Self; // NOTE: Originally statically initialized.
    };

    struct SoundDirectSoundSoundControllerContainer
    {
        AbstractSoundDeviceController* _Instance = (AbstractSoundDeviceController*)0x00d3d3cc; // TODO
        SoundDirectSoundDeviceControllerSelf SoundDirectDeviceSoundControllerSelf; // 0x005eeb6c

        struct
        {
            IKsPropertySet* Instance; // 0x00d3c934
        } EAX;

        struct
        {
            IDirectSound* Instance; // 0x00d3c924
            IDirectSound3DListener* Listener; // 0x00d3c930
        } DirectSound;

        struct
        {
            struct
            {
                u32* _BitsPerSample = (u32*)0x00d3c938; // TODO
                u32* _HZ = (u32*)0x00d3c93c; // TODO
                u32* _Channels = (u32*)0x00d3c940; // TODO

                IDirectSoundBuffer* Buffer; // 0x00d3c928
            } Primary;

            struct
            {
                // TODO: these two do not go for only secondary buffer
                u32* _Unknown1 = (u32*)0x00d3c94c; // TODO, some sort of count
                u32* _Unknown2 = (u32*)0x00d3c950; // TODO some sort of count

                u32* _Count = (u32*)0x00d3c944; // TODO
                s32* _ChannelBufferSize = (s32*)0x00d3c948; // TODO, type!, name: BufferSize

                IDirectSoundBuffer** _Buffer = (IDirectSoundBuffer**)0x00d3c92c; // TODO
            } Secondary;

        } Buffers;

        u32* _Options = (u32*)0x00d621bc; // TODO enum

        SoundUnk0x18* _SoundUnk0x18Array = (SoundUnk0x18*)0x00d3c954; // TODO 65 size
        SoundUnk0x24* _SoundUnk0x24Array = (SoundUnk0x24*)0x00d3cf70; // TODO 31 size
    };

    extern SoundDirectSoundSoundControllerContainer SoundDirectSoundSoundControllerState;

    s32 AcquireSoundDirectSoundSoundControllerFrequency(const f32 value);
    BOOL StopSoundDirectSoundDeviceControllerSoundSample(const s32 indx);
    void PollSoundDirectSoundSoundControllerSoundEffect(void);
    s32 AcquireUnknownSoundValue102(const f32 value);
}