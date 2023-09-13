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
#include "IO.Streams.hxx"
#include "Logger.hxx"
#include "Memory.hxx"
#include "Sounds.hxx"
#include "Sounds.Samples.hxx"

using namespace Assets::Sounds;
using namespace IO::Streams;
using namespace Logger;
using namespace Memory;
using namespace Objects;

namespace Sounds
{
    SoundSampleContainer SoundSampleState;

    // 0x0055a920
    SoundSample* ConstructSoundSample(SoundSample* self)
    {
        ZeroMemory(self, 0x150); // TODO, why 0x150 if sizeof(SoundSampleDescriptor) == 0x15c ??? and then set cachecontrol, priority and offset separately?

        self->Descriptor.ReferenceDistance = 20.0f;

        self->Descriptor.MinimumDistance = *SoundState._UnknownSoundEffectValue1;
        self->Descriptor.MaximumDistance = 10000.0f;

        ConstructInStreamFile(&self->File);

        self->Descriptor.AllocatedMemory1 = NULL;

        self->Descriptor.CacheControl = SoundCacheMode::Normal;
        self->Descriptor.Priority = 0;
        self->Descriptor.Offset = 0;

        self->Unk6 = 0;
        self->Unk7 = -1; // TODO constant
        self->Unk12 = 0;

        self->File.File.Handle = NULL;

        self->Length = 0;
        self->Unk9 = 0;

        return self;
    }

    // 0x0055a990
    void* ReleaseSoundSample(SoundSample* self, const ReleaseMode mode)
    {
        DisposeSoundSample(self);

        ReleaseInStreamFile(&self->File, ReleaseMode::None);

        return self;
    }

    // 0x0055ab30
    // a.k.a. freeMemory
    void DisposeSoundSample(SoundSample* self)
    {
        if (self->Descriptor.Priority != 0)
        {
            LogError("Unable to release sound effect sample, it is currently in use.");
        }

        UnlockSoundSample(self);
        ReleaseSoundSampleMemory(self);
        ReleaseSoundDeviceControllerSoundSample(self);

        // TODO: NOT IMPLEMENTED
        // TODO: NOT IMPLEMENTED
        // TODO: NOT IMPLEMENTED
        // TODO: NOT IMPLEMENTED
        // TODO: NOT IMPLEMENTED

        self->Unk7 = -1; // TODO constant
    }

    // 0x0055ac40
    void ReleaseSoundSampleMemory(SoundSample* self)
    {
        if (self->Descriptor.AllocatedMemory1 == NULL) { return; }

        ReleaseMemory1(self->Descriptor.AllocatedMemory1);

        self->Descriptor.AllocatedMemory1 = NULL;
    }

    // 0x0055ac70
    // a.k.a. lock
    void* LockSoundSample(SoundSample* self, const s32 offset, const s32 length)
    {
        if (self->Lock.Length != 0)
        {
            LogError("Unable to lock sound sample, it is already locked.");
        }

        if (offset < 0 || (self->Length < offset + length))
        {
            LogError("Unable to lock sound sample, provided region is invalid.");
        }

        void* result = NULL;

        if (self->Descriptor.Offset == 0)
        {
            if (self->Lock.Length == 0)
            {
                LogError("Unable to lock sound sample, memory buffer is not allocated.");
            }

            result = (void*)((addr)self->Descriptor.AllocatedMemory1 + (addr)AcquireSoundSampleOffset(self, offset));
        }
        else
        {
            if (*SoundState._SoundDeviceController == NULL) { return NULL; }

            result = (*SoundState._SoundDeviceController)->Self->LockSoundSample(*SoundState._SoundDeviceController,
                self->Descriptor.Offset, offset, length);
        }

        if (result != NULL)
        {
            self->Lock.Offset = offset;
            self->Lock.Length = length;
        }

        return result;
    }

    // 0x0055ad70
    void UnlockSoundSample(SoundSample* self)
    {
        if (self->Lock.Length == NULL) { return; }

        if (self->Descriptor.Offset != 0 && *SoundState._SoundDeviceController != NULL)
        {
            (*SoundState._SoundDeviceController)->Self->UnlockSoundSample(*SoundState._SoundDeviceController,
                self->Descriptor.Offset, self->Lock.Offset, self->Lock.Length);
        }

        self->Lock.Offset = 0;
        self->Lock.Length = 0;
    }

    // 0x0055cfc0
    s32 AcquireSoundSampleOffset(SoundSample* self, const s32 offset)
    {
        const s32 value = offset * self->Descriptor.Definition.BitsPerSample * self->Descriptor.Definition.Channels + 7; // TODO constant

        const auto sign = value >> 0x1f; // TODO constants

        return (s32)((value + sign * -8) - (u32)(sign << 2 < 0)) >> 3; // TODO constants
    }

    // 0x0055d240
    u32 AcquireUnknownSoundSampleValue1(SoundSample* self)
    {
        if (self->Descriptor.LoopMode < 2
            && self->Descriptor.Definition.Length == self->Descriptor.ChannelLength[0]) // TODO constant
        {
            if (self->Descriptor.Unk6 < 1) // TODO constant
            {
                if (self->Descriptor.Unk6 == 0) { return 1; } // TODO constant

                return 0; // TODO constant
            }
        }

        return 2; // TODO constant
    }
}