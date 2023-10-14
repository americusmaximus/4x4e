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

#include "Basic.hxx"
#include "IO.Ini.hxx"
#include "Renderers.Basic.hxx"

#define OPTIONS_INI_FILE_NAME ".\\system\\metal.ini"

#define OPTIONS_INI_FILE_GAME_SECTION_NAME "Game"

#define MAX_APPLICATION_OPTIONS_CONTROLLER_BUTTON_COUNT 32
#define MAX_APPLICATION_OPTIONS_KEY_COUNT 44

namespace App::Options
{
    enum class ApplicationGroundShadowsOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationAirShadowsOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationReflectionOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationTruckReflectionOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationWaterWakeOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationLensFlareOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationHeadLightOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationRandomWeatherOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationDetailTextureOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationShowDemoOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationDetailLevelOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationPartsModeOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationViewPanModeOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationSmokeEffectModeOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationTireTrackModeOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationStickyViewModeOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationShowInformationOverlayModeOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationTruckDetailModeOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationFirstTimeModeOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationUnitModeOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationMapRotationModeOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationRedBookAudioModeOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationSwapBrakeThrottleModeOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationThrottleModeOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationGearShifterModeOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    struct ApplicationWeatherOptions
    {
        u32 WeatherMode; // TODO type
        u32 Time; // TODO type
        u32 Unknown1; // TODO
    };

    struct ApplicationOptions
    {
        struct
        {
            ApplicationGroundShadowsOptionsMode* _GroundShadowsMode = (ApplicationGroundShadowsOptionsMode*)0x005f9854; // TODO, default value
            ApplicationAirShadowsOptionsMode* _AirShadowMode = (ApplicationAirShadowsOptionsMode*)0x005f9850; // TODO, default value
            u32* _Width = (u32*)0x005f9424; // TODO, def value
            u32* _Height = (u32*)0x005f9428; // TODO, def value
            u32* _BitsPerPixel = (u32*)0x005f942c; // TODO, def value
            ApplicationReflectionOptionsMode* _ReflectionMode = (ApplicationReflectionOptionsMode*)0x005f98ac; // TODO, def value
            ApplicationTruckReflectionOptionsMode* _TruckReflectionMode = (ApplicationTruckReflectionOptionsMode*)0x005f98b8; // TODO, def value
            ApplicationWaterWakeOptionsMode* _WaterWakeMode = (ApplicationWaterWakeOptionsMode*)0x005f98bc; // TODO, def value
            ApplicationLensFlareOptionsMode* _LensFlareMode = (ApplicationLensFlareOptionsMode*)0x005f98b0; // TODO, def value
            ApplicationHeadLightOptionsMode* _HeadLightMode = (ApplicationHeadLightOptionsMode*)0x005f98b4; // TODO, def value
            ApplicationRandomWeatherOptionsMode* _RandomWeatherMode = (ApplicationRandomWeatherOptionsMode*)0x0078e004; // TODO
            Renderers::RendererTextureFilterMode* _TextureFilterMode = (Renderers::RendererTextureFilterMode*)0x00601850; // TODO, def value
            Renderers::RendererDitherMode* _DitherMode = (Renderers::RendererDitherMode*)0x00601854; // TODO, def value
            u32* _MaximumTextureCount = (u32*)0x00601858; // TODO: def value
            Renderers::RendererSyncRetraceMode* _SyncRetraceMode = (Renderers::RendererSyncRetraceMode*)0x00d62c28; // TODO
            Renderers::RendererAcceleratedGraphicsPortMode* _AcceleratedGraphicsPort = (Renderers::RendererAcceleratedGraphicsPortMode*)0x00d62c2c; // TODO
            ApplicationDetailTextureOptionsMode* _DetailTextureMode = (ApplicationDetailTextureOptionsMode*)0x005f98e0; // TODO, def value
        } Graphics;

        struct
        {
            ApplicationFirstTimeModeOptionsMode* _FirstTimeMode = (ApplicationFirstTimeModeOptionsMode*)0x0078e060; // TODO
            ApplicationShowDemoOptionsMode* _ShowDemoMode = (ApplicationShowDemoOptionsMode*)0x005f9844; // TODO, def value
            ApplicationDetailLevelOptionsMode* _DetailLevelMode = (ApplicationDetailLevelOptionsMode*)0x005f98c8; // TODO, def value
            u32* _DefaultOpponentCount = (u32*)0x005f98dc; // TODO, def value
            ApplicationPartsModeOptionsMode* _PartsMode = (ApplicationPartsModeOptionsMode*)0x005f9890; // TODO, def value
            ApplicationViewPanModeOptionsMode* _ViewPanMode = (ApplicationViewPanModeOptionsMode*)0x005f98d8; // TODO, def value
            ApplicationSmokeEffectModeOptionsMode* _SmokeEffectMode = (ApplicationSmokeEffectModeOptionsMode*)0x005f98a4; // TODO, def value
            ApplicationTireTrackModeOptionsMode* _TireTrackMode = (ApplicationTireTrackModeOptionsMode*)0x005f98a8; // TODO def value
            ApplicationStickyViewModeOptionsMode* _StickyViewMode = (ApplicationStickyViewModeOptionsMode*)0x0078dff8; // TODO
            ApplicationStickyViewModeOptionsMode* _StickyViewPMode = (ApplicationStickyViewModeOptionsMode*)0x0078dffc; // TODO
            ApplicationStickyViewModeOptionsMode* _StickyViewHMode = (ApplicationStickyViewModeOptionsMode*)0x0078e000; // TODO
            ApplicationShowInformationOverlayModeOptionsMode* _ShowInformationOverlay = (ApplicationShowInformationOverlayModeOptionsMode*)0x005f9420; // TODO def value
            ApplicationUnitModeOptionsMode* _UnitMode = (ApplicationUnitModeOptionsMode*)0x0078e01c; // TODO
            ApplicationMapRotationModeOptionsMode* _MapRotationMode = (ApplicationMapRotationModeOptionsMode*)0x005f98c0; // TODO def value
            u32* _ComputerDriverCount = (u32*)0x005f98c4; // TODO def value
            ApplicationRedBookAudioModeOptionsMode* _RedBookAudioMode = (ApplicationRedBookAudioModeOptionsMode*)0x005f98e8; // TODO def value
            ApplicationTruckDetailModeOptionsMode* _TruckDetailMode = (ApplicationTruckDetailModeOptionsMode*)0x005f98e4; // TODO, def value
        } Game;

        struct
        {
            u32* _NullZone = (u32*)0x0078e010; // TODO
            ApplicationSwapBrakeThrottleModeOptionsMode* _SwapBrakeThrottle = (ApplicationSwapBrakeThrottleModeOptionsMode*)0x0078e014; // TODO
            ApplicationThrottleModeOptionsMode* _ThrottleMode = (ApplicationThrottleModeOptionsMode*)0x0078e018; // TODO
            ApplicationGearShifterModeOptionsMode* _GearShifterMode = (ApplicationGearShifterModeOptionsMode*)0x00adbae8; // TODO
        } Control;

        u32* _Keys = (u32*)0x005f5bc4; // TODO [MAX_APPLICATION_OPTIONS_KEY_COUNT], def values

        // TODO ApplicationWeatherOptions* _Weather = (ApplicationWeatherOptions*)0x00da1d6c; // TODO
    };

    struct ApplicationOptionsContainer
    {
        ApplicationOptions Options;
    };

    extern ApplicationOptionsContainer AppOptionsState;

    void LoadAppOptions(void);
    void SaveAppOptions(void);

    void LoadSoundOptions(IO::Ini::AbstractIni* ini);
    void SaveSoundOptions(IO::Ini::AbstractIni* ini);
}