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

#include "Logger.hxx"
#include "Sounds.Controllers.hxx"
#include "Sounds.Devices.DirectSound.hxx"
#include "Sounds.Devices.hxx"
#include "Sounds.hxx"

#include <mmeapi.h>
#include <stdio.h>

using namespace Logger;

namespace Sounds
{
    SoundDeviceContainer SoundDeviceState;

    // 0x00562b20
    BOOL AcquireSoundDeviceCapabilities(const s32 indx, SoundDevice* device)
    {
        WAVEOUTCAPSA caps;

        if (waveOutGetDevCapsA(indx, &caps, sizeof(WAVEOUTCAPSA)) != MMSYSERR_NOERROR) { return FALSE; }

        device->Type = SoundDeviceType::Wave;
        device->Index = indx;
        device->MixMode = SoundMixMode::None;
        device->Unk2 = 0; // TODO constant

        sprintf(device->Name, "WavOutWrite: %s", &caps.szPname);

        return TRUE;
    }

    // 0x0055fb00
    s32 AcquireSoundDeviceCount(void)
    {
        if (INVALID_SOUND_DEVICE_COUNT < SoundDeviceState.SoundDeviceCount) { return SoundDeviceState.SoundDeviceCount; }

        LockSounds();
        UnlockSound1();

        SoundDeviceState.SoundDeviceCount = MIN_SOUND_DEVICE_COUNT;

        for (u32 x = MIN_SOUND_DEVICE_COUNT; x < MAX_SOUND_DEVICE_COUNT; x++)
        {
            if (!AcquireSoundDeviceCapabilities(x, &SoundDeviceState.SoundDevices[x])) { break; }

            if (!EnumerateDirectSoundDevices(x, &SoundDeviceState.SoundDevices[x])) { return SoundDeviceState.SoundDeviceCount; }

            SoundDeviceState.SoundDeviceCount = SoundDeviceState.SoundDeviceCount + 1;
        }

        return SoundDeviceState.SoundDeviceCount;
    }

    // 0x0055fbf0
    s32 AcquireSoundDeviceIndex(void)
    {
        const auto count = AcquireSoundDeviceCount();

        if (count < 1) { return INVALID_SOUND_DEVICE_INDEX; }

        for (u32 x = 0; x < count; x++)
        {
            SoundDevice device;
            AcquireSoundDevice(x, &device);

            if (device.MixMode == SoundMixMode::Advanced) { return x; }
        }

        for (u32 x = 0; x < count; x++)
        {
            SoundDevice device;
            AcquireSoundDevice(x, &device);

            if (device.MixMode == SoundMixMode::Simple) { return x; }
        }

        for (u32 x = 0; x < count; x++)
        {
            SoundDevice device;
            AcquireSoundDevice(x, &device);

            if (device.Type == SoundDeviceType::DirectSound && device.IsPrimary && !device.IsEmulated) { return x; }
        }

        for (u32 x = 0; x < count; x++)
        {
            SoundDevice device;
            AcquireSoundDevice(x, &device);

            if (device.Type == SoundDeviceType::DirectSound) { return x; }
        }

        for (u32 x = 0; x < count; x++)
        {
            SoundDevice device;
            AcquireSoundDevice(x, &device);

            if (device.Type == SoundDeviceType::Wave) { return x; }
        }

        return MIN_SOUND_DEVICE_INDEX;
    }

    // 0x0055ff10
    s32 AcquireSelectedSoundDeviceIndex(void)
    {
        return SoundDeviceState.SoundDeviceIndex;
    }

    // 0x0055fb90
    // a.k.a. getSoundDeviceInfo
    void AcquireSoundDevice(const s32 indx, SoundDevice* device)
    {
        if (indx < MIN_SOUND_DEVICE_COUNT || AcquireSoundDeviceCount() <= indx)
        {
            LogError("Unable to get sound device, invalid index %d.", indx);
        }

        CopyMemory(device, &SoundDeviceState.SoundDevices[indx], sizeof(SoundDevice));
    }

    // 0x0055fd10
    // a.k.a. selectSoundDevice
    void SelectSoundDevice(const s32 indx)
    {
        if (AcquireSoundDeviceControllerState())
        {
            LogError("Unable to select sound device, the device is already open.");
        }

        const auto count = AcquireSoundDeviceCount();
        const auto index = indx < 0 ? SoundDeviceState.SoundDeviceIndex : indx;

        if (indx < count && INVALID_SOUND_RECORDING_DEVICE_INDEX < indx)
        {
            SoundDeviceState.SoundDeviceIndex = indx;
        }
        else
        {
            SoundDeviceState.SoundDeviceIndex = AcquireSoundDeviceIndex();
        }
    }

    // 0x00562c10
    BOOL AcquireSoundRecordingDeviceCapabilities(const u32 indx, SoundRecordingDevice* device)
    {
        WAVEINCAPSA caps;

        if (waveInGetDevCapsA(indx, &caps, sizeof(WAVEINCAPSA)) != MMSYSERR_NOERROR) { return FALSE; }

        device->Type = SoundDeviceType::Wave;
        device->Index = indx;

        sprintf(device->Name, "WavIn: %s", caps.szPname);

        return TRUE;
    }

    // 0x0055ff70
    s32 AcquireSoundRecordingDeviceCount(void)
    {
        if (INVALID_SOUND_RECORDING_DEVICE_COUNT < SoundDeviceState.SoundRecordingDeviceCount) { return SoundDeviceState.SoundRecordingDeviceCount; }

        LockSounds();
        UnlockSound1();

        SoundDeviceState.SoundRecordingDeviceCount = MIN_SOUND_RECORDING_DEVICE_COUNT;

        for (u32 x = MIN_SOUND_RECORDING_DEVICE_COUNT; x < MAX_SOUND_RECORDING_DEVICE_COUNT; x++)
        {
            if (!AcquireSoundRecordingDeviceCapabilities(x, &SoundDeviceState.SoundRecordingDevices[x])) { break; }

            SoundDeviceState.SoundRecordingDeviceCount = SoundDeviceState.SoundRecordingDeviceCount + 1;
        }

        return SoundDeviceState.SoundRecordingDeviceCount;
    }

    // 0x0055ffd0
    // a.k.a. getRecordingDeviceInfo
    void AcquireSoundRecordingDevice(const s32 indx, SoundRecordingDevice* device)
    {
        if (indx < MIN_SOUND_RECORDING_DEVICE_COUNT || AcquireSoundRecordingDeviceCount() <= indx)
        {
            LogError("Unable to get sound recording device, invalid index %d.", indx);
        }

        CopyMemory(device, &SoundDeviceState.SoundRecordingDevices[indx], sizeof(SoundRecordingDevice));
    }

    // 0x00560030
    s32 AcquireSoundRecordingDeviceIndex(void)
    {
        const auto count = AcquireSoundRecordingDeviceCount();

        if (count < 1) { return INVALID_SOUND_RECORDING_DEVICE_INDEX; }

        for (u32 x = 0; x < count; x++)
        {
            SoundRecordingDevice device;
            AcquireSoundRecordingDevice(x, &device);

            if (device.Type == SoundDeviceType::DirectSound) { return x; }
        }

        for (u32 x = 0; x < count; x++)
        {
            SoundRecordingDevice device;
            AcquireSoundRecordingDevice(x, &device);

            if (device.Type == SoundDeviceType::Wave) { return x; }
        }

        return MIN_SOUND_RECORDING_DEVICE_INDEX;
    }

    // 0x005601e0
    s32 AcquireSelectedSoundRecordingDeviceIndex(void)
    {
        return SoundDeviceState.SoundRecordingDeviceIndex;
    }

    // 0x005600b0
    // a.k.a. selectRecordingDevice
    void SelectSoundRecordingDevice(const s32 indx)
    {
        if (AcquireSoundRecordingDeviceControllerState())
        {
            LogError("Unable to select sound recording device, the device is already open.");
        }

        const auto count = AcquireSoundRecordingDeviceCount();
        const auto index = indx < 0 ? SoundDeviceState.SoundRecordingDeviceIndex : indx;

        SoundDeviceState.SoundRecordingDeviceIndex =
            (indx < count && INVALID_SOUND_RECORDING_DEVICE_INDEX < indx) ? indx : AcquireSoundRecordingDeviceIndex();
    }
}