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

#include "Sounds.hxx"
#include "Logger.hxx"

using namespace Logger;

namespace Sounds
{
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
            ReleaseSoundEffect(&SoundState.Effects._Cache[x]);
        }

        UnlockSound1();
    }

    typedef const BOOL(CDECLAPI* FUN_00558AE0) (const u32); // TODO
    static FUN_00558AE0 FUN_00558ae0 = (FUN_00558AE0)0x00558ae0; // TODO

    // 0x0055c930
    void ReleaseSoundEffect(SoundEffect* self)
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

                ReleaseSoundSample(sample);
            }

            FUN_00558ae0(0); // TODO constant

            sample->Unk7 = -1; // TODO constant
        }

        self->Options = 0; // TODO constant
    }
}