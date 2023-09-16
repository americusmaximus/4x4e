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
#include "Sounds.hxx"
#include "Sounds.Effects.hxx"

using namespace Logger;
using namespace Objects;

namespace Sounds
{
    SoundEffectDescriptorContainer SoundEffectDescriptorState;
    SoundEffectContainer SoundEffectState;

    // 0x00561ca0
    SoundEffectDescriptor* ConstructSoundEffectDescriptor(SoundEffectDescriptor* self)
    {
        InitializeSoundEffectDescriptor(self);

        return self;
    }

    // 0x00561cf0
    void* ReleaseSoundEffectDescriptor(SoundEffectDescriptor* self, const ReleaseMode mode) { return self; }

    // 0x0055d2a0
    void InitializeSoundEffectDescriptor(SoundEffectDescriptor* self)
    {
        self->NextChannelIndex = 0;

        self->Location.X = 0.0;
        self->Location.Y = 0.0;
        self->Location.Z = 0.0;

        self->Unknown102 = 0;
        self->Unknown103 = 0;
        self->Unknown104 = 0;
        self->Unknown105 = 0;

        self->Volume = 1.0f; // TODO constant
        self->HZ = 1.0f; // TODO constant

        self->Velocity.X = 0.0f;
        self->Velocity.Y = 0.0f;
        self->Velocity.Z = 0.0f;

        ZeroMemory(&self->Unknown1002, sizeof(self->Unknown1002)); // TODO

        self->Unknown1005 = 0.0;

        self->Unknown1007 = 0;
        self->Unknown1008 = 0;

        self->Unknown1001 = -1.0f; // TODO constant
    }

    // 0x00561cc0
    SoundEffect* ConstructSoundEffect(SoundEffect* self)
    {
        InitializeSoundEffectDescriptor(&self->Descriptor);

        return self;
    }

    // 0x00561ce0
    void* ReleaseSoundEffect(SoundEffect* self, const ReleaseMode mode) { return self; }

    // 0x0055e400
    // a.k.a. getSfxChannelVol
    f32 AcquireSoundEffectChannelVolume(const s32 indx)
    {
        if (indx < 0 || 31 < indx) // TODO constant
        {
            LogError("Unable to acquire sound effect channel volume, invalid index %d.", indx);
        }

        return SoundState.SFX.ChannelVolumes1[indx];
    }

    // 0x0055e340
    void ReleaseSoundEffectSamples(void)
    {
        LockSounds();

        for (u32 x = 0; x < 64; x++) // TODO constant
        {
            DisposeSoundEffect(&SoundState.Effects._Cache[x]);
        }

        UnlockSound1();
    }

    typedef const BOOL(CDECLAPI* FUN_00558AE0) (const u32); // TODO
    static FUN_00558AE0 FUN_00558ae0 = (FUN_00558AE0)0x00558ae0; // TODO

    // 0x0055c930
    void DisposeSoundEffect(SoundEffect* self)
    {
        if (*SoundState.Lock._Count < 1) { LogError("Sound effect must be locked."); } // TODO constant;

        if (self->Unknown1005 != 0) // TODO constant
        {
            // TODO FUN_00429710((int *)PTR_DAT_005f7fb0,s_Killing_sfx_%s_005e2014, self->Sample->Descriptor.Definition.Name);
        }

        if (self->UnknownIndex != 0) // TODO constant
        {
            if (*SoundState._SoundDeviceController != NULL)
            {
                (*SoundState._SoundDeviceController)->Self->ReleaseSoundEffect(*SoundState._SoundDeviceController, self);
            }

            self->UnknownIndex = 0; // TODO constant
        }

        self->Unknown1005 = 0; // TODO constant
        self->Unk32 = 0; // TODO constant

        if (self->Sample != NULL)
        {
            if (self->Sample->Descriptor.Priority < 1) // TODO constant
            {
                LogError("Unable to release sound effect %s. Reference count is out of balance.",
                    self->Sample->Descriptor.Definition.Name);
            }

            self->Sample->Descriptor.Priority = self->Sample->Descriptor.Priority - 1;

            auto sample = self->Sample;

            self->Sample = NULL;

            if (-1 < sample->Unk7) // TODO constant
            {
                if (sample->Descriptor.Priority != 0)
                {
                    LogError("Reference count for sound sample %s is greater than 0.", sample->Descriptor.Definition.Name);
                }

                if (SoundState.Effects._Cache[sample->Descriptor.Offset].Descriptor.NextChannelIndex != sample->Unk7)
                {
                    LogError("Streaming sound effect sample index mismatch on %s.", sample->Descriptor.Definition.Name);
                }

                DisposeSoundSample(sample);
            }

            FUN_00558ae0(0); // TODO constant

            sample->Unk7 = -1; // TODO constant
        }

        self->Options = 0; // TODO constant
    }

    // 0x0055cb60
    // a.k.a. pollStream
    BOOL PollSoundEffectStream(SoundEffect* self)
    {
        if (self->UnknownIndex != 0 && *SoundState._SoundDeviceController != NULL && self->Sample != NULL)
        {
            const auto position = (*SoundState._SoundDeviceController)->Self->AcquireSoundEffectPosition(*SoundState._SoundDeviceController, self);

            if (position < 0.0)
            {
                LogMessage("[ERROR] [SOUND] Unable to poll hardware playback position %s.", self->Sample->Descriptor.Definition.Name);

                return FALSE;
            }

            UpdateSoundEffectPosition(self, position);

            return TRUE;
        }

        return FALSE;
    }

    // 0x005bc9f0
    // a.k.a. updatePlaybackPos
    void UpdateSoundEffectPosition(SoundEffect* self, const f64 position)
    {
        if (self->Sample == NULL)
        {
            LogError("Unable to update sound effect playback position, sound sample is missing.");
        }

        if (position < 0.0 || self->Sample->Length < position)
        {
            LogError("Unable to update sound effect playback position, invalid position provided.");
        }

        auto delta = position - self->Position;

        if (delta < 0.0) { delta = self->Sample->Length + delta; }

        if (delta < 0.0 || (self->Sample->Length + 0.001 < delta))
        {
            LogError("Unable to update sound effect playback position, invalid step: %f - %f = %f, sample = %d (%s).",
                position, self->Position, delta, self->Sample->Length, self->Sample->Descriptor.Definition.Name);
        }

        if (self->Descriptor.Unknown1005 != self->Position
            || position < self->Descriptor.Unknown1005)
        {
            self->Descriptor.Unknown1005 = self->Descriptor.Unknown1005 + delta;
        }
        else
        {
            self->Descriptor.Unknown1005 = position;
        }

        if (self->Descriptor.Unknown1005 < 0.0)
        {
            self->Descriptor.Unknown1005 = 0.0;
        }

        if (-1 < self->Sample->Descriptor.Definition.Length
            && self->Sample->Descriptor.Definition.Length <= self->Descriptor.Unknown1005)
        {
            if (AcquireUnknownSoundSampleValue1(self->Sample) == 0) // TODO constant
            {
                self->Descriptor.Unknown1005 = self->Sample->Descriptor.Definition.Length;
            }
            else
            {
                self->Descriptor.Unknown1005 = self->Descriptor.Unknown1005
                    - AcquireUnknownSoundValue101(self->Descriptor.Unknown1005 / self->Sample->Descriptor.Definition.Length) * self->Sample->Descriptor.Definition.Length;
            }
        }

        self->Position = position;
    }

    // 0x0055d740
    // a.k.a. popSfxOptions
    void PopSoundEffectDescriptor(void)
    {
        *SoundState._SoundEffectDescriptorIndex = *SoundState._SoundEffectDescriptorIndex - 1;

        if (*SoundState._SoundEffectDescriptorIndex < 0)
        {
            LogError("Unable to pop sound effect descriptor, the stack is empty.");
        }
    }

    // 0x0055d6c0
    // a.k.a. pushSfxOptions
    void PushSoundEffectDescriptor(void)
    {
        *SoundState._SoundEffectDescriptorIndex = *SoundState._SoundEffectDescriptorIndex + 1;

        if (7 < *SoundState._SoundEffectDescriptorIndex) // TODO constant
        {
            LogError("Unable to push sound effect descriptor, the stack is full.");
        }

        CopyMemory(&SoundState._SoundEffectDescriptor[*SoundState._SoundEffectDescriptorIndex],
            &SoundState._SoundEffectDescriptor[*SoundState._SoundEffectDescriptorIndex - 1], sizeof(SoundEffectDescriptor));
    }
}