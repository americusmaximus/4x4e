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

#include "Renderers.Modules.Import.hxx"

#define LEGACY_OPENGL_RENDERER_MODULE_NAME "trigl.dll"

namespace Renderer::Module
{
    struct RendererModuleContainer
    {
        struct
        {
            HWND* _HWND = (HWND*)0x00d62d80; // TODO
            BOOL* _IsActive = (BOOL*)0x00d62d6c; // TODO
            HMODULE* _Handle = (HMODULE*)0x00d62d70; // TODO

            u32* _SelectedDeviceIndex = (u32*)0x00d62c24; // TODO
        } State;

        Renderers::Modules::RENDERERACQUIREMODULENAME AcquireModuleName;

        Renderers::Modules::RENDERERINIT* _Init = (Renderers::Modules::RENDERERINIT*)0x00d62c38; // TODO
        Renderers::Modules::RENDERERKILL* _Kill = (Renderers::Modules::RENDERERKILL*)0x00d62c3c; // TODO
        Renderers::Modules::RENDERERTOGGLE* _Toggle = (Renderers::Modules::RENDERERTOGGLE*)0x00d62c40; // TODO
        Renderers::Modules::RENDERERSELECTVIDEOMODE* _SelectVideoMode = (Renderers::Modules::RENDERERSELECTVIDEOMODE*)0x00d62c44; // TODO
        Renderers::Modules::RENDERERSELECTVIDEOMODE2* _SelectVideoMode2 = (Renderers::Modules::RENDERERSELECTVIDEOMODE2*)0x00d62c48; // TODO
        Renderers::Modules::RENDERERRESTOREVIDEOMODE* _RestoreVideoMode = (Renderers::Modules::RENDERERRESTOREVIDEOMODE*)0x00d62c4c; // TODO
        Renderers::Modules::RENDERERBEGINSCENE* _BeginScene = (Renderers::Modules::RENDERERBEGINSCENE*)0x00d62c50; // TODO
        Renderers::Modules::RENDERERENDSCENE* _EndScene = (Renderers::Modules::RENDERERENDSCENE*)0x00d62c54; // TODO
        Renderers::Modules::RENDERERLOCKFRAME* _LockFrame = (Renderers::Modules::RENDERERLOCKFRAME*)0x00d62c58; // TODO
        Renderers::Modules::RENDERERUNLOCKFRAME* _UnlockFrame = (Renderers::Modules::RENDERERUNLOCKFRAME*)0x00d62c5c; // TODO
        Renderers::Modules::RENDERERSELECTTEXTURE* _SelectTexture = (Renderers::Modules::RENDERERSELECTTEXTURE*)0x00d62c60; // TODO
        Renderers::Modules::RENDERERUPDATETEXTURE* _UpdateTexture = (Renderers::Modules::RENDERERUPDATETEXTURE*)0x00d62c64; // TODO
        Renderers::Modules::RENDERERSELECTTEXTUREBGRA* _SelectTextureBGRA = (Renderers::Modules::RENDERERSELECTTEXTUREBGRA*)0x00d62c68; // TODO
        Renderers::Modules::RENDERERUPDATETEXTUREBGRA* _UpdateTextureBGRA = (Renderers::Modules::RENDERERUPDATETEXTUREBGRA*)0x00d62c6c; // TODO
        Renderers::Modules::RENDERERSETMIPMAPLEVEL* _SetMipMapLevel = (Renderers::Modules::RENDERERSETMIPMAPLEVEL*)0x00d62c70; // TODO
        Renderers::Modules::RENDERERDRAWPOLYGON* _DrawPolygon = (Renderers::Modules::RENDERERDRAWPOLYGON*)0x00d62c74; // TODO
        Renderers::Modules::RENDERERDRAWPOLYGON2* _DrawPolygon2 = (Renderers::Modules::RENDERERDRAWPOLYGON2*)0x00d62c78; // TODO
        Renderers::Modules::RENDERERDRAWPOLYLIST* _DrawPolyList = (Renderers::Modules::RENDERERDRAWPOLYLIST*)0x00d62c7c; // TODO
        Renderers::Modules::RENDERERDRAWPOLYLIST2* _DrawPolyList2 = (Renderers::Modules::RENDERERDRAWPOLYLIST2*)0x00d62c80; // TODO
        Renderers::Modules::RENDERERDRAWPOLYLIST3* _DrawPolyList3 = (Renderers::Modules::RENDERERDRAWPOLYLIST3*)0x00d62c84; // TODO
        Renderers::Modules::RENDERERADDPARTICLE* _AddParticle = (Renderers::Modules::RENDERERADDPARTICLE*)0x00d62c88; // TODO
        Renderers::Modules::RENDERERFLUSHPARTICLELIST* _FlushParticleList = (Renderers::Modules::RENDERERFLUSHPARTICLELIST*)0x00d62c8c; // TODO
        Renderers::Modules::RENDERERADD3DLINE* _Add3DLine = (Renderers::Modules::RENDERERADD3DLINE*)0x00d62c90; // TODO
        Renderers::Modules::RENDERERFLUSHLINELIST* _FlushLineList = (Renderers::Modules::RENDERERFLUSHLINELIST*)0x00d62c94; // TODO
        Renderers::Modules::RENDERERCLEAR* _Clear = (Renderers::Modules::RENDERERCLEAR*)0x00d62c98; // TODO
        Renderers::Modules::RENDERERSETFOGCOLOR* _SetFogColor = (Renderers::Modules::RENDERERSETFOGCOLOR*)0x00d62c9c; // TODO
        Renderers::Modules::RENDERERSYNC* _Sync = (Renderers::Modules::RENDERERSYNC*)0x00d62ca0; // TODO
        Renderers::Modules::RENDERERCLEARZBUFFER* _ClearZBuffer = (Renderers::Modules::RENDERERCLEARZBUFFER*)0x00d62ca4; // TODO
        Renderers::Modules::RENDERERCLEARZBOX* _ClearZBox = (Renderers::Modules::RENDERERCLEARZBOX*)0x00d62ca8; // TODO
        Renderers::Modules::RENDERERSETCOLORTABLE* _SetColorTable = (Renderers::Modules::RENDERERSETCOLORTABLE*)0x00d62cac; // TODO
        Renderers::Modules::RENDERERACQUIREDISPLAYCONTEXT* _AcquireDisplayContext = (Renderers::Modules::RENDERERACQUIREDISPLAYCONTEXT*)0x00d62cb0; // TODO
        Renderers::Modules::RENDERERRELEASEDISPLAYCONTEXT* _ReleaseDisplayContext = (Renderers::Modules::RENDERERRELEASEDISPLAYCONTEXT*)0x00d62cb4; // TODO
        Renderers::Modules::RENDERERMASTERZBUFFER* _MasterZBuffer = (Renderers::Modules::RENDERERMASTERZBUFFER*)0x00d62cb8; // TODO
        Renderers::Modules::RENDERERRESTOREZBUFFER* _RestoreZBuffer = (Renderers::Modules::RENDERERRESTOREZBUFFER*)0x00d62cbc; // TODO
        Renderers::Modules::RENDERERACQUIREVIDEOMEMORY* _AcquireVideoMemory = (Renderers::Modules::RENDERERACQUIREVIDEOMEMORY*)0x00d62cc0; // TODO
        Renderers::Modules::RENDERERSELECTGRAPHICSCARD* _SelectGraphicsCard = (Renderers::Modules::RENDERERSELECTGRAPHICSCARD*)0x00d62cc4; // TODO
        Renderers::Modules::RENDERERACQUIREGRAPHICSCARDLIST* _AcquireGraphicsCardList = (Renderers::Modules::RENDERERACQUIREGRAPHICSCARDLIST*)0x00d62cc8; // TODO
        Renderers::Modules::RENDERERLOCKHOLDBUFFER* _LockHoldBuffer = (Renderers::Modules::RENDERERLOCKHOLDBUFFER*)0x00d62ccc; // TODO
        Renderers::Modules::RENDERERUNLOCKHOLDBUFFER* _UnlockHoldBuffer = (Renderers::Modules::RENDERERUNLOCKHOLDBUFFER*)0x00d62cd0; // TODO
        Renderers::Modules::RENDERERLOCKTEXTURE* _LockTexture = (Renderers::Modules::RENDERERLOCKTEXTURE*)0x00d62cd4; // TODO
        Renderers::Modules::RENDERERUNLOCKTEXTURE* _UnlockTexture = (Renderers::Modules::RENDERERUNLOCKTEXTURE*)0x00d62cd8; // TODO
        Renderers::Modules::RENDERERACQUIRETEXTUREINFORMATION* _AcquireTextureInformation = (Renderers::Modules::RENDERERACQUIRETEXTUREINFORMATION*)0x00d62cdc; // TODO
        Renderers::Modules::RENDERERSELECTVIDEOMODEX* _SelectVideoModeX = (Renderers::Modules::RENDERERSELECTVIDEOMODEX*)0x00d62ce0; // TODO
        Renderers::Modules::RENDERERRESTOREVIDEOMODEX* _RestoreVideoModeX = (Renderers::Modules::RENDERERRESTOREVIDEOMODEX*)0x00d62ce4; // TODO
        Renderers::Modules::RENDERERLOCKFRAMEX* _LockFrameX = (Renderers::Modules::RENDERERLOCKFRAMEX*)0x00d62ce8; // TODO
        Renderers::Modules::RENDERERUNLOCKFRAMEX* _UnlockFrameX = (Renderers::Modules::RENDERERUNLOCKFRAMEX*)0x00d62cec; // TODO
        Renderers::Modules::RENDERERTOGGLEX* _ToggleX = (Renderers::Modules::RENDERERTOGGLEX*)0x00d62cf0; // TODO
        Renderers::Modules::RENDERERSETVIEWPORT* _SetViewPort = (Renderers::Modules::RENDERERSETVIEWPORT*)0x00d62cf4; // TODO
        Renderers::Modules::RENDERERSETLIGHTCONSTANTS* _SetLightConstants = (Renderers::Modules::RENDERERSETLIGHTCONSTANTS*)0x00d62cf8; // TODO
        Renderers::Modules::RENDERERSETLIGHTVECTOR* _SetLightVector = (Renderers::Modules::RENDERERSETLIGHTVECTOR*)0x00d62cfc; // TODO
        Renderers::Modules::RENDERERSETAMBIENTLIGHT* _SetAmbientLight = (Renderers::Modules::RENDERERSETAMBIENTLIGHT*)0x00d62d00; // TODO
        Renderers::Modules::RENDERERSETTRANSFORM* _SetTransform = (Renderers::Modules::RENDERERSETTRANSFORM*)0x00d62d04; // TODO
        Renderers::Modules::RENDERERSETTRANSFORMW2L* _SetTransformW2L = (Renderers::Modules::RENDERERSETTRANSFORMW2L*)0x00d62d08; // TODO
        Renderers::Modules::RENDERERSETTRANSFORMO2W* _SetTransformO2W = (Renderers::Modules::RENDERERSETTRANSFORMO2W*)0x00d62d0c; // TODO
        Renderers::Modules::RENDERERPOLYLIST* _PolyList = (Renderers::Modules::RENDERERPOLYLIST*)0x00d62d10; // TODO
        Renderers::Modules::RENDERERPOLYLIST2* _PolyList2 = (Renderers::Modules::RENDERERPOLYLIST2*)0x00d62d14; // TODO
        Renderers::Modules::RENDERERPOLYLISTL* _PolyListL = (Renderers::Modules::RENDERERPOLYLISTL*)0x00d62d18; // TODO
        Renderers::Modules::RENDERERPOLYLISTD3DL* _PolyListD3DL = (Renderers::Modules::RENDERERPOLYLISTD3DL*)0x00d62d1c; // TODO
        Renderers::Modules::RENDERERPOLYLISTD3DTL* _PolyListD3DTL = (Renderers::Modules::RENDERERPOLYLISTD3DTL*)0x00d62d20; // TODO
        Renderers::Modules::RENDERERPOLYLISTTL* _PolyListTL = (Renderers::Modules::RENDERERPOLYLISTTL*)0x00d62d24; // TODO
        Renderers::Modules::RENDERERDOTLISTD3DL* _DotListD3DL = (Renderers::Modules::RENDERERDOTLISTD3DL*)0x00d62d28; // TODO
        Renderers::Modules::RENDERERDOTLISTD3DTL* _DotListD3DTL = (Renderers::Modules::RENDERERDOTLISTD3DTL*)0x00d62d2c; // TODO
        Renderers::Modules::RENDERERLINELISTD3DL* _LineListD3DL = (Renderers::Modules::RENDERERLINELISTD3DL*)0x00d62d30; // TODO
        Renderers::Modules::RENDERERLINELISTD3DTL* _LineListD3DTL = (Renderers::Modules::RENDERERLINELISTD3DTL*)0x00d62d34; // TODO
        Renderers::Modules::RENDERERACQUIRETEXTUREHANDLE* _AcquireTextureHandle = (Renderers::Modules::RENDERERACQUIRETEXTUREHANDLE*)0x00d62d38; // TODO
        Renderers::Modules::RENDERERSELECTTEXTUREBYHANDLE* _SelectTextureByHandle = (Renderers::Modules::RENDERERSELECTTEXTUREBYHANDLE*)0x00d62d3c; // TODO
        Renderers::Modules::RENDERERRESETTEXTURECACHE* _ResetTextureCache = (Renderers::Modules::RENDERERRESETTEXTURECACHE*)0x00d62d40; // TODO
        Renderers::Modules::RENDERERENABLECULLING* _EnableCulling = (Renderers::Modules::RENDERERENABLECULLING*)0x00d62d44; // TODO
        Renderers::Modules::RENDERERENABLECLIPPING* _EnableClipping = (Renderers::Modules::RENDERERENABLECLIPPING*)0x00d62d48; // TODO
        Renderers::Modules::RENDERERSETTEXTURECLAMP* _SetTextureClamp = (Renderers::Modules::RENDERERSETTEXTURECLAMP*)0x00d62d4c; // TODO
        Renderers::Modules::RENDERERSETFOG* _SetFog = (Renderers::Modules::RENDERERSETFOG*)0x00d62d50; // TODO
        Renderers::Modules::RENDERERRESETDYNAMICLIGHT* _ResetDynamicLight = (Renderers::Modules::RENDERERRESETDYNAMICLIGHT*)0x00d62d54; // TODO
        Renderers::Modules::RENDERERADDDYNAMICLIGHT* _AddDynamicLight = (Renderers::Modules::RENDERERADDDYNAMICLIGHT*)0x00d62d58; // TODO
        Renderers::Modules::RENDERERDYNAMICLIGHTHINT* _DynamicLightHint = (Renderers::Modules::RENDERERDYNAMICLIGHTHINT*)0x00d62d5c; // TODO
        Renderers::Modules::RENDERERDRAWPIXELS* _DrawPixels = (Renderers::Modules::RENDERERDRAWPIXELS*)0x00d62d60; // TODO
        Renderers::Modules::RENDERERVIDEOSAVE* _VideoSave = (Renderers::Modules::RENDERERVIDEOSAVE*)0x00d62d64; // TODO
        Renderers::Modules::RENDERERVIDEORESTORE* _VideoRestore = (Renderers::Modules::RENDERERVIDEORESTORE*)0x00d62d68; // TODO
    };

    extern RendererModuleContainer RendererModule;

    BOOL InitializeRendererModule(const HWND hwnd);
    void ReleaseRendererModule(void);
}