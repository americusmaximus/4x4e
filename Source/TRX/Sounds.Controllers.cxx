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
#include "Memory.hxx"
#include "Native.hxx"
#include "Sounds.Controllers.DirectSoundController.hxx"
#include "Sounds.Controllers.hxx"
#include "Sounds.Controllers.WaveIn.hxx"
#include "Sounds.Controllers.WaveOut.hxx"
#include "Sounds.Devices.hxx"
#include "Sounds.hxx"

using namespace Logger;
using namespace Memory;
using namespace Native;

namespace Sounds
{
    SoundDeviceControllerContainer SoundDeviceControllerState;

    // 0x0055fda0
    SoundMixMode AcquireSoundControllerMixMode(void)
    {
        return *SoundDeviceControllerState._MixMode;
    }

    // 0x0055fd90
    BOOL AcquireSoundDeviceControllerActiveState(void)
    {
        return *SoundDeviceControllerState._ActiveState;
    }

    // 0x0055fd80
    BOOL AcquireSoundDeviceControllerState(void)
    {
        return *SoundState._SoundDeviceController != NULL;
    }

    // 0x0055fe00
    BOOL InitializeSoundDeviceController(void)
    {
        if (!ReleaseSoundDeviceController()) { return FALSE; }

        SelectSoundDevice(*SoundDeviceState._SoundDeviceIndex);

        if (INVALID_SOUND_DEVICE_INDEX < *SoundDeviceState._SoundDeviceIndex)
        {
            const auto type = SoundDeviceState._SoundDevices[*SoundDeviceState._SoundDeviceIndex].Type;
            const auto index = SoundDeviceState._SoundDevices[*SoundDeviceState._SoundDeviceIndex].Index;

            switch (type)
            {
            case SoundDeviceType::Wave: { *SoundState._SoundDeviceController = InitializeSoundWaveOutDeviceController(index); break; }
            case SoundDeviceType::DirectSound: { *SoundState._SoundDeviceController = InitializeSoundDirectSoundDeviceController(index); break; }
            }

            if (*SoundState._SoundDeviceController == NULL) { return FALSE; }

            *SoundDeviceControllerState._MixMode = (*SoundState._SoundDeviceController)->Self->QueryCapabilities(*SoundState._SoundDeviceController);

            return TRUE;
        }

        return FALSE;
    }

    // 0x0055feb0
    BOOL ReleaseSoundDeviceController(void)
    {
        if (!StopSoundDeviceController()) { return FALSE; }

        for (u32 x = 0; x < 64; x++) // TODO constant
        {
            ReleaseSoundSample(&SoundState._SoundEffectSamples[x]);
        }

        if (*SoundState._SoundDeviceController != NULL)
        {
            if ((*SoundState._SoundDeviceController)->Self->Release(*SoundState._SoundDeviceController))
            {
                *SoundState._SoundDeviceController = NULL;

                return TRUE;
            }

            return FALSE;
        }

        return TRUE;
    }

    // 0x005b9f10
    void ReleaseSoundDeviceControllerMemory(void)
    {
        if (*SoundDeviceControllerState._UnknownMemory1 != NULL)
        {
            ReleaseMemory1(*SoundDeviceControllerState._UnknownMemory1);
            *SoundDeviceControllerState._UnknownMemory1 = NULL;
        }

        if (*SoundDeviceControllerState._UnknownMemory2 != NULL)
        {
            ReleaseMemory1(*SoundDeviceControllerState._UnknownMemory2);
            *SoundDeviceControllerState._UnknownMemory2 = NULL;
        }

        if (*SoundDeviceControllerState._UnknownMemory3 != NULL)
        {
            ReleaseMemory1(*SoundDeviceControllerState._UnknownMemory3);
            *SoundDeviceControllerState._UnknownMemory3 = NULL;
        }

        for (u32 x = 0; x < 8; x++) // TODO constant
        {
            SoundDeviceControllerState._UnknownArray1[x] = NULL; // TODO
            SoundDeviceControllerState._UnknownArray2[x] = NULL; // TODO
            SoundDeviceControllerState._UnknownArray3[x] = NULL; // TODO
            SoundDeviceControllerState._UnknownArray4[x] = NULL; // TODO
        }

        *SoundDeviceControllerState._Unknown1 = 0; // TODO
        *SoundDeviceControllerState._Unknown2 = 0; // TODO
        *SoundDeviceControllerState._Unknown3 = 0; // TODO
        *SoundDeviceControllerState._Unknown4 = 0; // TODO
    }

    // 0x0055f920
    BOOL StopSoundDeviceController(void)
    {
        ReleaseSoundEffectSamples();

        if (*SoundState._SoundDeviceController != NULL)
        {
            if (!(*SoundState._SoundDeviceController)->Self->Stop(*SoundState._SoundDeviceController)) { return FALSE; }
        }

        *SoundDeviceControllerState._ActiveState = FALSE;

        ReleaseSoundDeviceControllerMemory();

        return TRUE;
    }

    // 0x005601c0
    BOOL AcquireSoundRecordingDeviceControllerState(void)
    {
        return *SoundState._SoundRecordingDeviceController != NULL;
    }

    // 0x00560120
    BOOL InitializeSoundRecordingDeviceController(void)
    {
        if (!ReleaseSoundRecordingDeviceController()) { return FALSE; }

        SelectSoundRecordingDevice(*SoundDeviceState._SoundRecordingDeviceIndex);

        if (INVALID_SOUND_RECORDING_DEVICE_INDEX < *SoundDeviceState._SoundRecordingDeviceIndex)
        {
            if (SoundDeviceState._SoundRecordingDevices[*SoundDeviceState._SoundRecordingDeviceIndex].Type == SoundDeviceType::Wave)
            {
                *SoundState._SoundRecordingDeviceController = InitializeSoundWaveInDeviceController(SoundDeviceState._SoundRecordingDevices[*SoundDeviceState._SoundRecordingDeviceIndex].Index);
            }

            if (*SoundState._SoundRecordingDeviceController != NULL) { return TRUE; }
        }

        return FALSE;
    }

    // 0x0055abf0
    void ReleaseSoundDeviceControllerSoundSample(SoundSample* self)
    {
        if (self->Descriptor.Offset == 0) { return; } // TODO constant

        if (*SoundState._SoundDeviceController != NULL)
        {
            (*SoundState._SoundDeviceController)->Self->ReleaseSoundSample(*SoundState._SoundDeviceController, self->Descriptor.Offset);
        }

        self->Descriptor.Offset = 0; // TODO constant
    }

    // 0x00560180
    BOOL ReleaseSoundRecordingDeviceController(void)
    {
        if (!StopSoundRecordingDeviceController()) { return FALSE; }

        if (*SoundState._SoundRecordingDeviceController != NULL)
        {
            if ((*SoundState._SoundRecordingDeviceController)->Self->Release(*SoundState._SoundRecordingDeviceController))
            {
                *SoundState._SoundRecordingDeviceController = NULL;

                return TRUE;
            }

            return FALSE;
        }

        return TRUE;
    }

    // 0x005603b0
    BOOL StopSoundRecordingDeviceController(void)
    {
        LockSounds();

        if (*SoundState._SoundRecordingDeviceController != NULL)
        {
            auto controller = *SoundState._SoundRecordingDeviceController;

            if (!controller->Self->Stop(controller))
            {
                UnlockSound1();

                return FALSE;
            }
        }

        UnlockSound1();

        return TRUE;
    }

    // 0x005607f0
    void PollSoundDeviceController(void)
    {
        if (*SoundState._SoundDeviceController == NULL) { return; }

        if (AcquireSoundDeviceControllerActiveState())
        {
            if (WaitMutex2(*SoundState.Lock._Mutex, *SoundState.Thread._TimeValue * 3.0)) // TODO constant
            {
                *SoundState.Lock._Count = *SoundState.Lock._Count + 1;

                (*SoundState._SoundDeviceController)->Self->Poll(*SoundState._SoundDeviceController);

                PollSoundDeviceControllerSoundSamples(FALSE);

                *SoundState.Lock._Count = *SoundState.Lock._Count + -1;

                DisposeMutex(*SoundState.Lock._Mutex);
            }
        }
    }

    typedef const BOOL(CDECLAPI* FUN_0055AF70) (SoundSample*, const f32, const f32); // TODO
    static FUN_0055AF70 FUN_0055af70 = (FUN_0055AF70)0x0055af70; // TODO

    // 0x00561880
    void PollSoundDeviceControllerSoundSamples(const BOOL mode)
    {
        LockSounds();

        f32 start = mode ? 1.8f : 1.7f; // TODO constant
        f32 end = mode ? 2.0f : 1.9f; // TODO constant

        for (u32 x = 0; x < 64; x++) // TODO constant
        {
            FUN_0055af70(&SoundState._SoundEffectSamples[x], start, end);
        }

        UnlockSound1();
    }

    // 0x005605f0
    void SelectSoundDeviceControllerOptions(void)
    {
        if (*SoundState._SoundDeviceController == NULL) { return; }

        if (AcquireSoundControllerMixMode() == SoundMixMode::None) { return; }

        for (u32 x = 0; x < 64; x++) // TODO constant
        {
            auto effect = &SoundState.Effects._Cache[x];

            if (effect->UnknownIndex == 0) { continue; } // TODO constant

            if (*SoundDeviceControllerState._MixMode == SoundMixMode::Simple)
            {
                effect->Options = effect->Options | *SoundDirectSoundSoundControllerState._Options;
            }

            if (effect->Descriptor.Unk30 & 1) // TODO constant
            {
                effect->Options = effect->Options & 0xdffffff9; // TODO constant
            }

            if (effect->Options == 0) { continue; } // TODO constant

            FUN_0055bac0(effect, 0.0f); // TODO constant

            if (!(*SoundState._SoundDeviceController)->Self->SelectSoundEffectOptions(*SoundState._SoundDeviceController, effect, effect->Options))
            {
                ReleaseSoundEffect(effect);
            }
            else if (effect->Options & 0x40000000) // TODO constant
            {
                if ((*SoundState._SoundDeviceController)->Self->StartSoundEffect(*SoundState._SoundDeviceController, effect))
                {
                    effect->Options = effect->Options & 0xbfffffff; // TODO constant
                }
                else
                {
                    ReleaseSoundEffect(effect);
                }
            }

        }

        if ((*SoundDirectSoundSoundControllerState._Options & 2) != 0) // TODO constant
        {
            (*SoundState._SoundDeviceController)->Self->SelectPosition(*SoundState._SoundDeviceController,
                SoundState.Effects.Position._X[*SoundState.Effects._Index],
                SoundState.Effects.Position._Y[*SoundState.Effects._Index],
                SoundState.Effects.Position._Z[*SoundState.Effects._Index]);
        }

        if ((*SoundDirectSoundSoundControllerState._Options & 0x20000000) != 0) // TODO constant
        {
            (*SoundState._SoundDeviceController)->Self->SelectOrientation(*SoundState._SoundDeviceController,
                SoundState.Effects.Orientation.XYZ._X[*SoundState.Effects._Index],
                SoundState.Effects.Orientation.XYZ._Y[*SoundState.Effects._Index],
                SoundState.Effects.Orientation.XYZ._Z[*SoundState.Effects._Index],
                SoundState.Effects.Orientation.Top._X[*SoundState.Effects._Index],
                SoundState.Effects.Orientation.Top._Y[*SoundState.Effects._Index],
                SoundState.Effects.Orientation.Top._Z[*SoundState.Effects._Index],
                SoundState.Effects.Orientation.Front._X[*SoundState.Effects._Index],
                SoundState.Effects.Orientation.Front._Y[*SoundState.Effects._Index],
                SoundState.Effects.Orientation.Front._Z[*SoundState.Effects._Index]);
        }

        if ((*SoundDirectSoundSoundControllerState._Options & 4) != 0) // TODO constant
        {
            (*SoundState._SoundDeviceController)->Self->SelectVelocity(*SoundState._SoundDeviceController,
                SoundState.Effects.Velocity._X[*SoundState.Effects._Index],
                SoundState.Effects.Velocity._Y[*SoundState.Effects._Index],
                SoundState.Effects.Velocity._Z[*SoundState.Effects._Index]);
        }

        *SoundDirectSoundSoundControllerState._Options = 0;  // TODO constant

        (*SoundState._SoundDeviceController)->Self->ApplyOptions(*SoundState._SoundDeviceController);
    }

    typedef const void(CDECLAPI* FUN_00559F80) (u32, u32); // TODO
    static FUN_00559F80 FUN_00559f80 = (FUN_00559F80)0x00559f80; // TODO

    // 0x0055f960
    // a.k.a. setSoundOutputMode
    BOOL SelectSoundDeviceControllerSoundMode(const u32 bits, const u32 channels, const u32 hz)
    {
        if (AcquireSoundDeviceControllerActiveState())
        {
            LogError("Unable to select sound output mode, the device is already active.");
        }

        *SoundState.Options._Bits = bits;
        *SoundState.Options._ChannelCount = channels;
        *SoundState.Options._HZ = hz;

        if (*SoundState._SoundDeviceController != NULL)
        {
            u32 count = 0; // TODO constant

            if (!(*SoundState._SoundDeviceController)->Self->Reset(*SoundState._SoundDeviceController, bits, channels, hz, &count)) { return FALSE; }

            if (count != 0) // TODO constant
            {
                FUN_00559f80(count, 4); // TODO constant
            }
        }

        return TRUE;
    }
}