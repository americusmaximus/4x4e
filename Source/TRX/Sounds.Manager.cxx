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

#include "App.Options.hxx"
#include "Logger.hxx"
#include "Sounds.Effects.hxx"
#include "Sounds.hxx"
#include "Sounds.Options.hxx"
#include "Sounds.Samples.hxx"
#include "Vehicles.Sounds.hxx"

using namespace App::Options;
using namespace IO::Ini;
using namespace Logger;
using namespace Objects;
using namespace Sounds::Options;
using namespace Vehicles;

namespace Sounds
{
    SoundManagerContainer SoundManagerState;

    // 0x00580e90
    void LoadSoundManagerOptions(SoundManager* self, const char* file, const char* section)
    {
        IniFile ini;

        InitializeIniFile(&ini, file, section);

        LoadSoundOptions((AbstractIni*)&ini);

        ReleaseIniFile(&ini, ReleaseMode::None);
    }

    // 0x00580ee0
    void SaveSoundManagerOptions(SoundManager* self, const char* file, const char* section)
    {
        IniFile ini;

        InitializeIniFile(&ini, file, section);

        SaveSoundOptions((AbstractIni*)&ini);

        ReleaseIniFile(&ini, ReleaseMode::None);
    }

    // 0x00581150
    void InitializeSoundManager(SoundManager* self)
    {
        ReleaseSoundEffectSamples();

        CloseSoundManager(self);

        ZeroMemory(VehicleSoundEffectState._Items, MAX_SOUND_VEHICLE_SOUND_EFFECT_COUNT * sizeof(VehicleSoundEffect));
    }

    // 0x00580fd0
    // a.k.a. start
    void StartSoundManager(SoundManager* self)
    {
        LogMessage("StartSoundManager {\n");

        InitializeSoundManager(self);

        if (0.69999999f < AcquireSoundEffectChannelVolume(0)) { SelectSoundEffectChannelVolume(0, 0.69999999f); } // TODO constant

        if (!*SoundManagerState._UnknownValue101) { return; }

        if (!AcquireSoundEffectChannelState(0))
        {
            if (AcquireSoundEffectChannelState(1))
            {
                ReleaseSounds();
                StartSoundDeviceController();

                if (AcquireSoundDeviceControllerActiveState())
                {
                    // TODO NOT IMPLEMENTED
                    
                    return;
                }
            }
        }
        else
        {
            // TODO NOT IMPLEMENTED
            
            StartSoundDeviceController();

            if (AcquireSoundDeviceControllerActiveState())
            {
                // TODO NOT IMPLEMENTED
                
                LockSounds();

                // TODO NOT IMPLEMENTED

                UnlockSound1();

                LogMessage("} // StartSoundManager\n");
            }
        }
    }

    // 0x005818b0
    void CloseSoundManager(SoundManager* self)
    {
        ReleaseSoundEffectSamples();
        StopSoundDeviceController();

        if (*AppOptionsState.Options.Game._RedBookAudioMode != ApplicationRedBookAudioModeOptionsMode::Off)
        {
            CloseSoundDisk(SoundDiskState._Instance);

            return;
        }

        AttemptSoundEffectDispose(*SoundManagerState._SoundEffectIndex);

        *SoundManagerState._IsActive = FALSE;
    }
}