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

#include "Sounds.Samples.hxx"
#include "Sounds.hxx"
#include "Sounds.Effects.hxx"
#include "Mathematics.Basic.hxx"
#include "Logger.hxx"

#include <math.h>
#include <stdio.h>

using namespace Logger;
using namespace Mathematics;
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

        self->Unknown102.Single = NULL;
        self->Unknown102.Type = SoundEffectDescriptorUnknownType::None;

        self->Unknown104.Single = NULL;
        self->Unknown104.Type = SoundEffectDescriptorUnknownType::None;

        self->Volume = MAX_SOUND_VOLUME;
        self->HZ = 1.0f; // TODO constant

        self->Velocity.X = 0.0f;
        self->Velocity.Y = 0.0f;
        self->Velocity.Z = 0.0f;

        ZeroMemory(&self->UserData, MAX_SOUND_USER_DATA_COUNT * sizeof(void*)); // TODO

        self->Position = 0.0;

        self->Unknown1007 = 0;
        self->Unknown1008 = 0;

        self->RemainingDelay = -1.0f; // TODO constant
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
        if (indx < MIN_SOUND_EFFECT_CHANNEL_COUNT || (MAX_SOUND_EFFECT_CHANNEL_COUNT - 1) < indx)
        {
            LogError("Unable to acquire sound effect channel volume, invalid index %d.", indx);
        }

        return SoundState.Effects.Channels._Volumes[indx];
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
        if (*SoundState.Lock._Count < DEFAULT_SOUND_LOCK_COUNT) { LogError("Sound effect must be locked."); }

        if (self->DebugMode)
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

        self->DebugMode = FALSE;
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

    // 0x0055cbf0
    // a.k.a. updatePlaybackPos
    void UpdateSoundEffectPosition(SoundEffect* self, const f64 position)
    {
        if (self->Sample == NULL) { LogError("Unable to update sound effect playback position, sound sample is missing."); }

        if (position < 0.0 || self->Sample->Length < position) { LogError("Unable to update sound effect playback position, invalid position provided."); }

        auto delta = position - self->Position;

        if (delta < 0.0) { delta = self->Sample->Length + delta; }

        if (delta < 0.0 || (self->Sample->Length + 0.001 < delta))
        {
            LogError("Unable to update sound effect playback position, invalid step: %f - %f = %f, sample = %d (%s).",
                position, self->Position, delta, self->Sample->Length, self->Sample->Descriptor.Definition.Name);
        }

        if (self->Descriptor.Position != self->Position || position < self->Descriptor.Position)
        {
            self->Descriptor.Position = self->Descriptor.Position + delta;
        }
        else
        {
            self->Descriptor.Position = position;
        }

        if (self->Descriptor.Position < 0.0) { self->Descriptor.Position = 0.0; }

        if (-1 < self->Sample->Descriptor.Definition.Length
            && self->Sample->Descriptor.Definition.Length <= self->Descriptor.Position)
        {
            if (AcquireUnknownSoundSampleDescriptorValue1(&self->Sample->Descriptor) == 0) // TODO constant
            {
                self->Descriptor.Position = self->Sample->Descriptor.Definition.Length;
            }
            else
            {
                self->Descriptor.Position = self->Descriptor.Position
                    - AcquireUnknownSoundValue101(self->Descriptor.Position / self->Sample->Descriptor.Definition.Length) * self->Sample->Descriptor.Definition.Length;
            }
        }

        self->Position = position;
    }

    // 0x0055d740
    // a.k.a. popSfxOptions
    void PopCurrentSoundEffectDescriptor(void)
    {
        *SoundState._SoundEffectDescriptorIndex = *SoundState._SoundEffectDescriptorIndex - 1;

        if (*SoundState._SoundEffectDescriptorIndex < 0) { LogError("Unable to pop sound effect descriptor, the stack is empty."); }
    }

    // 0x0055d6c0
    // a.k.a. pushSfxOptions
    void PushCurrentSoundEffectDescriptor(void)
    {
        *SoundState._SoundEffectDescriptorIndex = *SoundState._SoundEffectDescriptorIndex + 1;

        if (7 < *SoundState._SoundEffectDescriptorIndex) { LogError("Unable to push sound effect descriptor, the stack is full."); } // TODO constant

        CopyMemory(&SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex],
            &SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex - 1], sizeof(SoundEffectDescriptor));
    }

    // 0x0055a520
    SoundEffect* AcquireSoundEffect(const u32 indx, const BOOL mode)
    {
        const auto todo = indx >> 6; // TODO constant
        const auto index = indx & 0x3f; // TODO constant

        if (todo != 0 && todo < 0xffffff && index < 64) // TODO constant
        {
            auto effect = &SoundState.Effects._Cache[index];

            if (effect->Unk32 == todo)
            {
                if (mode)
                {
                    LockSounds();

                    if (effect->Unk32 != todo || effect->Sample == NULL)
                    {
                        UnlockSound1();

                        return NULL;
                    }

                    if (effect->UnknownIndex != 0)
                    {
                        if (*SoundState._SoundDeviceController == NULL)
                        {
                            DisposeSoundEffect(effect);
                            UnlockSound1();

                            return NULL;
                        }

                        if (effect->Options & 0x40000000) { return effect; } // TODO constant

                        if (!(*SoundState._SoundDeviceController)->Self->AcquireSoundEffectState(*SoundState._SoundDeviceController, effect))
                        {
                            DisposeSoundEffect(effect);
                            UnlockSound1();

                            return NULL;
                        }
                    }
                }

                return effect;
            }
        }

        return NULL;
    }

    // 0x0055e190
    BOOL SelectSoundEffectVolume(const u32 indx, const f32 volume)
    {
        auto effect = AcquireSoundEffect(indx, TRUE);

        if (effect == NULL) { return FALSE; }

        effect->Descriptor.Volume = volume;
        effect->Options = effect->Options | 8; // TODO constant

        UnlockSound1();

        return TRUE;
    }

    // 0x0055e1c0
    BOOL SelectSoundEffectFrequency(const u32 indx, const f32 hz)
    {
        auto effect = AcquireSoundEffect(indx, TRUE);

        if (effect == NULL) { return FALSE; }

        effect->Descriptor.HZ = hz;
        effect->Options = effect->Options | 0x10; // TODO constant

        if (effect->UnknownIndex != 0 && *SoundState._SoundDeviceController != NULL)
        {
            (*SoundState._SoundDeviceController)->Self->SelectSoundEffectOptions(*SoundState._SoundDeviceController, effect, 0x10); // TODO constant
        }

        UnlockSound1();

        return TRUE;
    }

    // 0x0055e6f0
    void SelectSoundEffectIndex(const s32 indx)
    {
        LockSounds();

        SoundState.Effects.Index = indx;

        UnlockSound1();
    }

    // 0x0055b6e0
    void ComputeSoundEffectChannelsPosition(SoundEffect* self) // TODO: Distance?
    {
        for (u32 x = 0; x < *SoundState.Options._Channels; x++)
        {
            const auto dx = self->Descriptor.Location.X - SoundState.Effects.Channels.Position.X[x];
            const auto dy = self->Descriptor.Location.Y - SoundState.Effects.Channels.Position.Y[x];
            const auto dz = self->Descriptor.Location.Z - SoundState.Effects.Channels.Position.Z[x];

            self->Distances[x] = (f32)sqrt(dx * dx + dy * dy + dz * dz);
        }
    }

    // 0x0055d560
    // a.k.a. setNextSfxChannel
    void SelectNextSoundEffectDescriptorChannel(const s32 indx)
    {
        if (indx < MIN_SOUND_EFFECT_CHANNEL_COUNT || (MAX_SOUND_EFFECT_CHANNEL_COUNT - 1) < indx) { LogError("Invalid sound channel index %d.", indx); } // TODO constnats

        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].NextChannelIndex = indx;
    }

    // 0x0055d4c0
    void SelectCurrentSoundEffectDescriptorVolume(const f32 volume)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Volume = volume;
    }

    // 0x0055d600
    void UpdateCurrentSoundEffectDescriptorUnk30(const u32 mode) // TODO name, enum
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unk30 = SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unk30 | mode;
    }

    // 0x0055d5e0
    void SelectCurrentSoundEffectDescriptorUnk30(const u32 mode) // TODO name, enum
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unk30 = mode;
    }

    // 0x0055d620
    void RemoveCurrentSoundEffectDescriptorUnk30(const u32 mode) // TODO name, enum
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unk30 = SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unk30 & (~mode);
    }

    // 0x0055d690
    void InitializeCurrentSoundEffectDescriptor(void)
    {
        InitializeSoundEffectDescriptor(&SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex]);
    }

    // 0x0055d340
    void SelectCurrentSoundEffectDescriptorLocation(const f64 x, const f64 y, const f64 z)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Location.X = x;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Location.Y = y;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Location.Z = z;

        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown102.Single = NULL;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown104.Type = SoundEffectDescriptorUnknownType::None;
    }

    // 0x0055d410
    void SelectCurrentSoundEffectDescriptorVelocity(const f32 x, const f32 y, const f32 z)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Velocity.X = x;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Velocity.Y = y;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Velocity.Z = z;

        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown104.Single = NULL;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown104.Type = SoundEffectDescriptorUnknownType::None;
    }

    // 0x0055d3e0
    void SelectCurrentSoundDescriptorUnknownValues102103(f64x3* value)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown102.Type = SoundEffectDescriptorUnknownType::Double;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown102.Double = value;
    }

    // 0x0055d3b0
    void SelectCurrentSoundDescriptorUnknownValues103102(f32x3* value)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown102.Type = SoundEffectDescriptorUnknownType::Single;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown102.Single = value;
    }

    // 0x0055d460
    void SelectCurrentSoundDescriptorUnknownValues105104(f32x3* value)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown104.Type = SoundEffectDescriptorUnknownType::Single;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown104.Single = value;
    }

    // 0x0055d490
    void SelectCurrentSoundDescriptorUnknownValues104105(f64x3* value)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown104.Type = SoundEffectDescriptorUnknownType::Double;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown104.Double = value;
    }

    // 0x0055d4e0
    void SelectCurrentSoundEffectDescriptorFrequency(const f32 value)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].HZ = value;
    }

    // 0x0055d5c0
    void SelectCurrentSoundEffectDescriptorRemainingDelay(const f32 value)
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].RemainingDelay = value;
    }

    // 0x0055d500
    // a.k.a. setNextSfxUserData
    void SelectCurrentSoundEffectDescriptorUserData(const s32 indx, void* value)
    {
        if (indx < MIN_SOUND_USER_DATA_COUNT || (MAX_SOUND_USER_DATA_COUNT - 1) < indx) { LogError("Unable to set sound effect user data, invalid index %d.", indx); } // TODO constants

        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].UserData[indx] = value;
    }

    // 0x0055d650
    void SelectCurrentSoundDescriptorPosition(const f64 value, const s32 mode) // TODO params, types, names
    {
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Position = value;
        SoundState._SoundEffectDescriptors[*SoundState._SoundEffectDescriptorIndex].Unknown1007 = mode;
    }

    // 0x0055e450
    // a.k.a. setNumberOfSfxChannels
    void SelectSoundEffectChannelCount(const u32 value)
    {
        if ((MAX_SOUND_EFFECT_CHANNEL_COUNT + 1) <= value) { LogError("Unable to set sound effect channel count, invalid value %d.", value); }

        *SoundState.Effects.Channels._Count = value;
    }

    // 0x0055bac0
    // a.k.a. compute
    BOOL ComputeSoundEffect(SoundEffect* self, const f32 volume)
    {
        if (*SoundState.Lock._Count < DEFAULT_SOUND_LOCK_COUNT) { LogError("Sound effect must be locked."); }

        if (self->Sample == NULL) { return FALSE; }

        if (self->DebugMode)
        {
            // TODO FUN_00429710(PTR_DAT_005f7fb0, "[SOUND] [DBG] Sample: %s\n", self->Sample->Descriptor.Definition.Name);
        }

        if (self->Unk32 == 0 || self->Descriptor.Position < 0.0)
        {
            // TODO FUN_00429710((int *)PTR_DAT_005f7fb0, "[SOUND] Releasing sound effect %s\n", self->Sample->Descriptor.Definition.Name);

            DisposeSoundEffect(self);

            return FALSE;
        }

        if (MIN_SOUND_VOLUME < volume && MIN_SOUND_VOLUME <= self->Volume)
        {
            if (self->Volume <= volume)
            {
                if ((self->Unknown1004 & 0x7fffffffU) != 0) // TODO constant
                {
                    DisposeSoundEffect(self);

                    return FALSE;
                }

                self->Descriptor.Volume = self->Unknown1002;
            }
            else
            {
                self->Descriptor.Volume = self->Descriptor.Volume + (volume / self->Volume) * (self->Unknown1002 - self->Descriptor.Volume);

                self->Volume = self->Volume - volume;
            }

            self->Options = self->Options | 8; // TODO constant
        }

        ComputeSoundEffectLocationVelocity(self);

        SoundState.Effects.Index = UpdateSoundEffectPositionCount(self->Descriptor.Location.X, self->Descriptor.Location.Y, self->Descriptor.Location.Z);

        if (SoundState.UnknownSoundCount1 != 0) { ComputeSoundEffectsPositions(); }

        if (self->Descriptor.RemainingDelay == -1.0f) // TODO constant
        {
            if (self->DebugMode)
            {
                // TODO FUN_00429710((int*)PTR_DAT_005f7fb0, s__auto_computing_delay..._005e1ec0);
            }

            AutoCalculateSoundEffectRemainingDelay(self);
        }

        if (self->DebugMode)
        {
            // TODO FUN_00429710((int*)PTR_DAT_005f7fb0, s__delayRemaining_ = _ % 7.2fs_005e1edb, self->Descriptor.Unknown1001);
        }

        self->HZ = self->Descriptor.HZ;

        if (self->DebugMode)
        {
            // TODO FUN_00429710((int*)PTR_DAT_005f7fb0, s__freq_ = _ % 5.2f_005e1ef6, self->HZ);
        }

        if ((self->Descriptor.Unk30 & 1) == 0) // TODO constant
        {
            const auto dx = self->Descriptor.Location.X - SoundState.Effects.Position.X[SoundState.Effects.Index];
            const auto dy = self->Descriptor.Location.Y - SoundState.Effects.Position.Y[SoundState.Effects.Index];
            const auto dz = self->Descriptor.Location.Z - SoundState.Effects.Position.Z[SoundState.Effects.Index];

            auto distance = sqrt(dx * dx + dy * dy + dz * dz);

            if (self->DebugMode)
            {
                // TODO FUN_00429710((int *)PTR_DAT_005f7fb0, "  distToEar = %7.2fs\n", distance);
            }

            const auto vx = SoundState.Effects.Velocity.X[SoundState.Effects.Index] - self->Descriptor.Velocity.X;
            const auto vy = SoundState.Effects.Velocity.Y[SoundState.Effects.Index] - self->Descriptor.Velocity.Y;
            const auto vz = SoundState.Effects.Velocity.Z[SoundState.Effects.Index] - self->Descriptor.Velocity.Z;

            const auto interim = distance <= 0 ? sqrt(vx * vx + vy * vy + vz * vz) : ((dx * vx + dy * vy + dz * vz) / distance);

            const auto doppler = (interim * SoundDeviceControllerState.DistanceFactor.Value + 1116.4) / 1116.4; // TODO constant

            if (self->DebugMode)
            {
                // TODO FUN_00429710((int *)PTR_DAT_005f7fb0, "  doppler = %5.2f\n", doppler);
            }

            const auto clamped = Clamp(doppler, 0.25, 4.0); // TODO constants

            if (self->DebugMode)
            {
                // TODO FUN_00429710((int *)PTR_DAT_005f7fb0, "  doppler (clamped) = %5.2f\n", clamped);
            }

            self->HZ = self->HZ * (f32)clamped;
        }
        else if (self->DebugMode)
        {
            // TODO FUN_00429710((int *)PTR_DAT_005f7fb0, "  non spatialized\n");
        }

        ComputeSoundEffectChannelsPosition(self);
        ComputeUnknownSoundEffectValue(self);

        if (self->DebugMode)
        {
            // TODO FUN_00429710((int*)PTR_DAT_005f7fb0, s__vol_ = _ % 5.2f_005e1f5f, self->Descriptor.Volume);
        }

        ComputeSoundEffectPans(self);

        if (self->DebugMode)
        {
            for (u32 x = 0; x < *SoundState.Options._Channels; x++)
            {
                // TODO FUN_00429710((int *)PTR_DAT_005f7fb0, "  channelVol[%d] = %5.2f\n", x, self->Sample->Descriptor.Definition.BitsPerSample * self->Pans[x]);
            }
        }

        self->HZ = Clamp(self->HZ, 0.05f, 20.0f); // TODO constants

        if (self->DebugMode)
        {
            // TODO FUN_00429710((int *)PTR_DAT_005f7fb0, "  effFreq = %5.2f\n", self->HZ);
        }

        return TRUE;
    }

    // 0x00559cf0
    void ComputeSoundEffectsPositions(void)
    {
        if (*SoundState.Options._Channels == SOUND_CHANNEL_COUNT_STEREO)
        {
            SoundState.Effects.Channels.Position.X[0] = SoundState.Effects.Position.X[SoundState.Effects.Index] - (SoundState.Effects.Orientation.XYZ._X[SoundState.Effects.Index] * 0.5f * SoundDeviceControllerState.DistanceFactor.InverseValue); // TODO constant
            SoundState.Effects.Channels.Position.Y[0] = SoundState.Effects.Position.Y[SoundState.Effects.Index] - (SoundState.Effects.Orientation.XYZ._Y[SoundState.Effects.Index] * 0.5f * SoundDeviceControllerState.DistanceFactor.InverseValue); // TODO constant
            SoundState.Effects.Channels.Position.Z[0] = SoundState.Effects.Position.Z[SoundState.Effects.Index] - (SoundState.Effects.Orientation.XYZ._Z[SoundState.Effects.Index] * 0.5f * SoundDeviceControllerState.DistanceFactor.InverseValue); // TODO constant

            SoundState.Effects.Channels.Position.X[1] = SoundState.Effects.Position.X[SoundState.Effects.Index] + (SoundState.Effects.Orientation.XYZ._X[SoundState.Effects.Index] * 0.5f * SoundDeviceControllerState.DistanceFactor.InverseValue); // TODO constant
            SoundState.Effects.Channels.Position.Y[1] = SoundState.Effects.Position.Y[SoundState.Effects.Index] + (SoundState.Effects.Orientation.XYZ._Y[SoundState.Effects.Index] * 0.5f * SoundDeviceControllerState.DistanceFactor.InverseValue); // TODO constant
            SoundState.Effects.Channels.Position.Z[1] = SoundState.Effects.Position.Z[SoundState.Effects.Index] + (SoundState.Effects.Orientation.XYZ._Z[SoundState.Effects.Index] * 0.5f * SoundDeviceControllerState.DistanceFactor.InverseValue); // TODO constant

            return;
        }

        for (u32 x = 0; x < *SoundState.Options._Channels; x++)
        {
            SoundState.Effects.Channels.Position.X[x] = SoundState.Effects.Position.X[SoundState.Effects.Index];
            SoundState.Effects.Channels.Position.Y[x] = SoundState.Effects.Position.Y[SoundState.Effects.Index];
            SoundState.Effects.Channels.Position.Z[x] = SoundState.Effects.Position.Z[SoundState.Effects.Index];
        }
    }

    // 0x0055ba20
    // a.k.a. autoCalcDelayRemaining
    void AutoCalculateSoundEffectRemainingDelay(SoundEffect* self)
    {
        if (*SoundState.Lock._Count < DEFAULT_SOUND_LOCK_COUNT) { LogError("Sound effect must be locked."); }

        if ((self->Descriptor.Unk30 & 1) != 0) // TODO constant
        {
            self->Descriptor.RemainingDelay = 0.0f;

            return;
        }

        const auto dx = self->Descriptor.Location.X - SoundState.Effects.Position.X[SoundState.Effects.Index];
        const auto dy = self->Descriptor.Location.Y - SoundState.Effects.Position.Y[SoundState.Effects.Index];
        const auto dz = self->Descriptor.Location.Z - SoundState.Effects.Position.Z[SoundState.Effects.Index];

        self->Descriptor.RemainingDelay = (f32)((1.0 / 1116.4) * sqrt(dx * dx + dy * dy + dz * dz)) * SoundDeviceControllerState.DistanceFactor.Value; // TODO constants
    }

    // 0x0055e2c0
    BOOL AttemptSoundEffectDispose(const u32 indx)
    {
        auto effect = AcquireSoundEffect(indx, TRUE);

        if (effect == NULL) { return FALSE; }

        DisposeSoundEffect(effect);
        UnlockSound1();

        return TRUE;
    }

    // 0x0055dde0
    BOOL AttemptSoundEffectAcquire(const u32 indx)
    {
        if (AcquireSoundDeviceControllerMixMode() == SoundMixMode::None) { return AcquireSoundEffect(indx, FALSE) != NULL; }

        if (AcquireSoundEffect(indx, TRUE) == NULL) { return FALSE; }

        UnlockSound1();

        return TRUE;
    }

    // 0x0055b8e0
    void ComputeSoundEffectPans(SoundEffect* self)
    {
        if (*SoundState.Lock._Count < DEFAULT_SOUND_LOCK_COUNT) { LogError("Sound effect must be locked."); }

        const auto volume = self->Descriptor.Volume * AcquireSoundEffectChannelVolume(self->Descriptor.NextChannelIndex);

        if ((self->Descriptor.Unk30 & 1) == 0)
        {
            auto distance = self->MinimumDistance;

            for (u32 x = 0; x < *SoundState.Options._Channels; x++)
            {
                auto value = Clamp(self->Distances[x], self->Unknown1001, self->MaximumDistance);

                if (distance * 0.05f < value) // TODO constant
                {
                    distance = distance / value;
                }
                else
                {
                    distance = 20.0f; // TODO constant
                }

                self->Pans[x] = volume * distance;
            }
        }
        else
        {
            for (u32 x = 0; x < *SoundState.Options._Channels; x++) { self->Pans[x] = volume; }
        }
    }

    // 0x0055b550
    void ComputeSoundEffectLocationVelocity(SoundEffect* self)
    {
        if (self->Descriptor.Unknown102.Type == SoundEffectDescriptorUnknownType::Single)
        {
            if (self->Descriptor.Unknown102.Single->X != self->Descriptor.Location.X
                || self->Descriptor.Unknown102.Single->Y != self->Descriptor.Location.Y
                || self->Descriptor.Unknown102.Single->Z != self->Descriptor.Location.Z)
            {
                self->Descriptor.Location.X = self->Descriptor.Unknown102.Single->X;
                self->Descriptor.Location.Y = self->Descriptor.Unknown102.Single->Y;
                self->Descriptor.Location.Z = self->Descriptor.Unknown102.Single->Z;

                self->Options = self->Options | 2; // TODO constant
            }
        }
        else if (self->Descriptor.Unknown102.Type == SoundEffectDescriptorUnknownType::Double)
        {
            if (self->Descriptor.Unknown102.Double->X != self->Descriptor.Location.X
                || self->Descriptor.Unknown102.Double->Y != self->Descriptor.Location.Y
                || self->Descriptor.Unknown102.Double->Z != self->Descriptor.Location.Z)
            {
                self->Descriptor.Location.X = self->Descriptor.Unknown102.Double->X;
                self->Descriptor.Location.Y = self->Descriptor.Unknown102.Double->Y;
                self->Descriptor.Location.Z = self->Descriptor.Unknown102.Double->Z;

                // TODO NOT IMPLEMENTED

                self->Options = self->Options | 2; // TODO constant
            }
        }

        if (self->Descriptor.Unknown104.Type == SoundEffectDescriptorUnknownType::Single)
        {
            if (self->Descriptor.Unknown104.Single->X != self->Descriptor.Velocity.X
                || self->Descriptor.Unknown104.Single->Y != self->Descriptor.Velocity.Y
                || self->Descriptor.Unknown104.Single->Z != self->Descriptor.Velocity.Z)
            {
                self->Descriptor.Velocity.X = (f32)self->Descriptor.Unknown104.Single->X;
                self->Descriptor.Velocity.Y = (f32)self->Descriptor.Unknown104.Single->Y;
                self->Descriptor.Velocity.Z = (f32)self->Descriptor.Unknown104.Single->Z;

                self->Options = self->Options | 4; // TODO constant
            }
        }
        else if (self->Descriptor.Unknown104.Type == SoundEffectDescriptorUnknownType::Double)
        {
            if (self->Descriptor.Unknown104.Double->X != self->Descriptor.Velocity.X
                || self->Descriptor.Unknown104.Double->Y != self->Descriptor.Velocity.Y
                || self->Descriptor.Unknown104.Double->Z != self->Descriptor.Velocity.Z)
            {
                self->Descriptor.Velocity.X = (f32)self->Descriptor.Unknown104.Double->X;
                self->Descriptor.Velocity.Y = (f32)self->Descriptor.Unknown104.Double->Y;
                self->Descriptor.Velocity.Z = (f32)self->Descriptor.Unknown104.Double->Z;

                self->Options = self->Options | 4; // TODO constant
            }
        }
    }

    // 0x0055b750
    void ComputeUnknownSoundEffectValue(SoundEffect* self)
    {
        if (*SoundState.Lock._Count < DEFAULT_SOUND_LOCK_COUNT) { LogError("Sound effect must be locked."); }

        if (*SoundState.Options._Channels == SOUND_CHANNEL_COUNT_NONE || *SoundState.Options._Channels == SOUND_CHANNEL_COUNT_MONO)
        {
            self->Unknown1[0] = 0; // TODO constant

            return;
        }

        if ((self->Descriptor.Unk30 & 1) == 0 && self->UnknownIndex == 0
            && AcquireSoundDeviceControllerMixMode() == SoundMixMode::None)
        {
            auto min = S32_MAX;

            for (u32 x = 0; x < *SoundState.Options._Channels; x++)
            {
                auto value = Max(0, (s32)round(self->Distances[x] * (1.0 / 1116.4) * SoundDeviceControllerState.DistanceFactor.Value * *SoundState.Options._HZ)); // TODO constant

                self->Unknown1[x] = value;

                if (value < min) { min = value; }
            }

            for (u32 x = 0; x < *SoundState.Options._Channels; x++)
            {
                self->Unknown1[x] = self->Unknown1[x] - min;

                while (self->Unknown1[x] <= *SoundDeviceControllerState._Unknown4)
                {
                    x = x + 1;

                    if (*SoundState.Options._Channels <= x) { return; }
                }

                self->Unknown1[x] = *SoundDeviceControllerState._Unknown4;
            }

            return;
        }

        for (u32 x = 0; x < *SoundState.Options._Channels; x++) { self->Unknown1[x] = 0; } // TODO constant
    }

    // 0x0055e850
    BOOL AcquireSoundEffectDistanceState(const f64 x, const f64 y, const f64 z, const f32 distance)
    {
        const auto dx = x - SoundState.Effects.Position.X[SoundState.Effects.Index];
        const auto dy = y - SoundState.Effects.Position.Y[SoundState.Effects.Index];
        const auto dz = z - SoundState.Effects.Position.Z[SoundState.Effects.Index];

        return (dx * dx + dy * dy + dz * dz) <= (distance * distance);
    }

    // 0x0055e510
    // a.k.a. isSfxChannelEnabled
    BOOL AcquireSoundEffectChannelState(const s32 indx)
    {
        if (indx < MIN_SOUND_EFFECT_CHANNEL_COUNT || (MAX_SOUND_EFFECT_CHANNEL_COUNT - 1) < indx)
        {
            LogError("Unable to acquire sound effect channel state, invalid index %d.", indx);
        }

        return SoundState._SoundChannelStates[indx];
    }

    // 0x0055e490
    // a.k.a. enableSfxChannel
    void SelectSoundEffectChannelState(const s32 indx, const BOOL state)
    {
        if (indx < MIN_SOUND_EFFECT_CHANNEL_COUNT || (MAX_SOUND_EFFECT_CHANNEL_COUNT - 1) < indx)
        {
            LogError("Unable to select sound effect channel state, invalid index %d.", indx);
        }

        SoundState._SoundChannelStates[indx] = state;

        if (!state)
        {
            LockSounds();

            for (u32 x = 0; x < 64; x++) // TODO constant
            {
                if (indx == SoundState.Effects._Cache[x].Descriptor.NextChannelIndex)
                {
                    DisposeSoundEffect(&SoundState.Effects._Cache[x]);
                }
            }

            UnlockSound1();
        }
    }

    // 0x0055e5f0
    void AcquireSoundEffectsState(char* buffer)
    {
        LockSounds();

        u32 index = 0;

        for (u32 x = 0; x < 64; x++) // TODO constant
        {
            const auto effect = &SoundState.Effects._Cache[x];

            if (effect->Sample != NULL && effect->Unk32 != 0) // TODO constant
            {
                PollSoundEffectStream(effect);

                index = index + sprintf(&buffer[index], "sfx slot %d: %s, pos %7.1f/%7.1f\n",
                    x, effect->Sample->Descriptor.Definition.Name,
                    effect->Descriptor.Position, (f32)effect->Sample->Descriptor.Definition.Length);
            }
        }

        UnlockSound1();

        buffer[index] = NULL;
    }

    // 0x0055e370
    // a.k.a. setSfxChannelVol
    void SelectSoundEffectChannelVolume(const s32 indx, const f32 volume)
    {
        if (indx < MIN_SOUND_EFFECT_CHANNEL_COUNT || (MAX_SOUND_EFFECT_CHANNEL_COUNT - 1) < indx)
        {
            LogError("Unable to select sound effect channel volume, invalid index %d.", indx);
        }

        SoundState.Effects.Channels._Volumes[indx] = volume;

        if (*SoundState._SoundDeviceController == NULL) { return; }

        if (AcquireSoundDeviceControllerMixMode() == SoundMixMode::None) { return; }

        LockSounds();

        for (u32 x = 0; x < 64; x++) // TODO constant
        {
            auto effect = &SoundState.Effects._Cache[x];

            if (effect->Unk32 != 0 && effect->UnknownIndex != 0
                && effect->Descriptor.NextChannelIndex == indx) // TODO constant
            {
                effect->Options = effect->Options | 8; // TODO constant
            }
        }

        UnlockSound1();
    }

    // 0x0055e8c0
    // TODO: Name
    u32 UpdateSoundEffectPositionCount(const f64 x, const f64 y, const f64 z)
    {
        if (SoundState.UnknownSoundCount1 != 0) // TODO constant
        {
            const auto dx0 = (x - SoundState.Effects.Position.X[0]) * (x - SoundState.Effects.Position.X[0]);
            const auto dy0 = (y - SoundState.Effects.Position.Y[0]) * (y - SoundState.Effects.Position.Y[0]);
            const auto dz0 = (z - SoundState.Effects.Position.Z[0]) * (z - SoundState.Effects.Position.Z[0]);

            const auto dx1 = (x - SoundState.Effects.Position.X[1]) * (x - SoundState.Effects.Position.X[1]);
            const auto dy1 = (y - SoundState.Effects.Position.Y[1]) * (y - SoundState.Effects.Position.Y[1]);
            const auto dz1 = (z - SoundState.Effects.Position.Z[1]) * (z - SoundState.Effects.Position.Z[1]);

            if ((dx1 + dy1 + dz1) <= (dx0 + dy0 + dz0)) { return 1; } // TODO constant
        }

        return 0; // TODO constant
    }

    // 0x0055bfb0
    // a.k.a. mix
    // NOTE: Originally the container is being passed by value.
    void MixSoundEffect(SoundEffect* self, SoundEffectMixContainer container) // TODO
    {
        // TODO NOT IMPLEMENTED
    }
}