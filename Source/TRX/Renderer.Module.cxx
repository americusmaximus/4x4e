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
#include "Editor.hxx"
#include "Native.hxx"
#include "Renderer.hxx"
#include "Renderer.Module.hxx"

using namespace App::Options;
using namespace Editor;
using namespace Native;
using namespace Renderer;
using namespace Renderers::Modules;
using namespace Renderers;

namespace Renderer::Module
{
    RendererModuleContainer RendererModule;

    // 0x005664c0
    BOOL InitializeRendererModule(const HWND hwnd)
    {
        BOOL invalid = FALSE;

        if (*RendererModule.State._HWND != hwnd)
        {
            *RendererModule.State._HWND = hwnd;
        }

        if (!*RendererState.Options._IsUsingModule) { return FALSE; }

        *RendererModule.State._Handle = InitializeModule(RendererState.Options._ModuleFileName);

        if (*RendererModule.State._Handle == NULL)
        {
            *RendererState.Options._IsUsingModule = FALSE;

            return FALSE;
        }

        *RendererModule._Init = (RENDERERINIT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_INIT_NAME);
        if (*RendererModule._Init == NULL) { invalid = TRUE; }

        *RendererModule._Kill = (RENDERERKILL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_KILL_NAME);
        if (*RendererModule._Kill == NULL) { invalid = TRUE; }

        *RendererModule._Toggle = (RENDERERTOGGLE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_TOGGLE_NAME);
        if (*RendererModule._Toggle == NULL) { invalid = TRUE; }

        *RendererModule._SelectVideoMode = (RENDERERSELECTVIDEOMODE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_VIDEO_MODE_NAME);
        if (*RendererModule._SelectVideoMode == NULL) { invalid = TRUE; }

        *RendererModule._SelectVideoMode2 = (RENDERERSELECTVIDEOMODE2)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_VIDEO_MODE2_NAME);
        if (*RendererModule._SelectVideoMode2 == NULL) { invalid = TRUE; }

        *RendererModule._RestoreVideoMode = (RENDERERRESTOREVIDEOMODE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_RESTORE_VIDEO_MODE_NAME);
        if (*RendererModule._RestoreVideoMode == NULL) { invalid = TRUE; }

        *RendererModule._BeginScene = (RENDERERBEGINSCENE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_BEGIN_SCENE_NAME);
        if (*RendererModule._BeginScene == NULL) { invalid = TRUE; }

        *RendererModule._EndScene = (RENDERERENDSCENE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_END_SCENE_NAME);
        if (*RendererModule._EndScene == NULL) { invalid = TRUE; }

        *RendererModule._LockFrame = (RENDERERLOCKFRAME)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_LOCK_FRAME_NAME);
        if (*RendererModule._LockFrame == NULL) { invalid = TRUE; }

        *RendererModule._UnlockFrame = (RENDERERUNLOCKFRAME)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_UNLOCK_FRAME_NAME);
        if (*RendererModule._UnlockFrame == NULL) { invalid = TRUE; }

        *RendererModule._SelectTexture = (RENDERERSELECTTEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_TEXTURE_NAME);
        if (*RendererModule._SelectTexture == NULL) { invalid = TRUE; }

        *RendererModule._UpdateTexture = (RENDERERUPDATETEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_UPDATE_TEXTURE_NAME);
        if (*RendererModule._UpdateTexture == NULL) { invalid = TRUE; }

        *RendererModule._SelectTextureBGRA = (RENDERERSELECTTEXTUREBGRA)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_TEXTURE_BGRA_NAME);
        *RendererModule._UpdateTextureBGRA = (RENDERERUPDATETEXTUREBGRA)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_UPDATE_TEXTURE_BGRA_NAME);

        *RendererModule._SetMipMapLevel = (RENDERERSETMIPMAPLEVEL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_MIP_MAP_LEVEL_NAME);
        if (*RendererModule._SetMipMapLevel == NULL) { invalid = TRUE; }

        *RendererModule._DrawPolygon = (RENDERERDRAWPOLYGON)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_DRAW_POLYGON_NAME);
        if (*RendererModule._DrawPolygon == NULL) { invalid = TRUE; }

        *RendererModule._DrawPolygon2 = (RENDERERDRAWPOLYGON2)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_DRAW_POLYGON2_NAME);
        if (*RendererModule._DrawPolygon2 == NULL) { invalid = TRUE; }

        *RendererModule._DrawPolyList = (RENDERERDRAWPOLYLIST)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_DRAW_POLY_LIST_NAME);
        *RendererModule._DrawPolyList2 = (RENDERERDRAWPOLYLIST2)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_DRAW_POLY_LIST2_NAME);
        *RendererModule._DrawPolyList3 = (RENDERERDRAWPOLYLIST3)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_DRAW_POLY_LIST3_NAME);

        *RendererModule._AddParticle = (RENDERERADDPARTICLE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ADD_PARTICLE_NAME);
        *RendererModule._FlushParticleList = (RENDERERFLUSHPARTICLELIST)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_FLUSH_PARTICLE_LIST_NAME);
        *RendererModule._Add3DLine = (RENDERERADD3DLINE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ADD_3D_LINE_NAME);
        *RendererModule._FlushLineList = (RENDERERFLUSHLINELIST)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_FLUSH_LINE_LIST_NAME);

        *RendererModule._Clear = (RENDERERCLEAR)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_CLEAR_NAME);
        if (*RendererModule._Clear == NULL) { invalid = TRUE; }

        *RendererModule._SetFogColor = (RENDERERSETFOGCOLOR)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_FOG_COLOR_NAME);
        if (*RendererModule._SetFogColor == NULL) { invalid = TRUE; }

        *RendererModule._Sync = (RENDERERSYNC)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SYNC_NAME);

        *RendererModule._ClearZBuffer = (RENDERERCLEARZBUFFER)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_CLEAR_Z_BUFFER_NAME);
        if (*RendererModule._ClearZBuffer == NULL) { invalid = TRUE; }

        *RendererModule._ClearZBox = (RENDERERCLEARZBOX)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_CLEAR_Z_BOX_NAME);
        if (*RendererModule._ClearZBox == NULL) { invalid = TRUE; }

        *RendererModule._SetColorTable = (RENDERERSETCOLORTABLE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_COLOR_TABLE_NAME);
        if (*RendererModule._SetColorTable == NULL) { invalid = TRUE; }

        *RendererModule._AcquireDisplayContext = (RENDERERACQUIREDISPLAYCONTEXT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ACQUIRE_DISPLAY_CONTEXT_NAME);
        *RendererModule._ReleaseDisplayContext = (RENDERERRELEASEDISPLAYCONTEXT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_RELEASE_DISPLAY_CONTEXT_NAME);
        *RendererModule._MasterZBuffer = (RENDERERMASTERZBUFFER)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_MASTER_Z_BUFFER_NAME);
        *RendererModule._RestoreZBuffer = (RENDERERRESTOREZBUFFER)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_RESTORE_Z_BUFFER_NAME);

        *RendererModule._AcquireVideoMemory = (RENDERERACQUIREVIDEOMEMORY)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ACQUIRE_VIDEO_MEMORY_NAME);
        if (*RendererModule._AcquireVideoMemory == NULL) { invalid = TRUE; }

        *RendererModule._SelectGraphicsCard = (RENDERERSELECTGRAPHICSCARD)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_GRAPHICS_CARD_NAME);
        if (*RendererModule._SelectGraphicsCard == NULL) { invalid = TRUE; }

        *RendererModule._AcquireGraphicsCardList = (RENDERERACQUIREGRAPHICSCARDLIST)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_CREATE_GRAPHICS_CARD_LIST_NAME);
        if (*RendererModule._AcquireGraphicsCardList == NULL) { invalid = TRUE; }

        *RendererModule._LockHoldBuffer = (RENDERERLOCKHOLDBUFFER)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_LOCK_HOLD_BUFFER_NAME);
        *RendererModule._UnlockHoldBuffer = (RENDERERUNLOCKHOLDBUFFER)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_UNLOCK_HOLD_BUFFER_NAME);
        *RendererModule._LockTexture = (RENDERERLOCKTEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_LOCK_TEXTURE_NAME);
        *RendererModule._UnlockTexture = (RENDERERUNLOCKTEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_UNLOCK_TEXTURE_NAME);

        *RendererModule._AcquireTextureInformation = (RENDERERACQUIRETEXTUREINFORMATION)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ACQUIRE_TEXTURE_INFORMATION_NAME);
        if (*RendererModule._AcquireTextureInformation == NULL) { invalid = TRUE; }

        RendererModule.AcquireModuleName = (RENDERERACQUIREMODULENAME)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ACQUIRE_MODULE_NAME_NAME);

        *RendererModule._SelectVideoModeX = (RENDERERSELECTVIDEOMODEX)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_VIDEO_MODEX_NAME);
        *RendererModule._RestoreVideoModeX = (RENDERERRESTOREVIDEOMODEX)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_RESTORE_VIDEO_MODEX_NAME);
        *RendererModule._LockFrameX = (RENDERERLOCKFRAMEX)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_LOCK_FRAMEX_NAME);
        *RendererModule._UnlockFrameX = (RENDERERUNLOCKFRAMEX)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_UNLOCK_FRAMEX_NAME);
        *RendererModule._ToggleX = (RENDERERTOGGLEX)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_TOGGLEX_NAME);
        *RendererModule._SetViewPort = (RENDERERSETVIEWPORT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_VIEWPORT_NAME);
        *RendererModule._SetLightConstants = (RENDERERSETLIGHTCONSTANTS)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_LIGHT_CONSTANTS_NAME);
        *RendererModule._SetLightVector = (RENDERERSETLIGHTVECTOR)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_LIGHT_VECTOR_NAME);
        *RendererModule._SetAmbientLight = (RENDERERSETAMBIENTLIGHT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_AMBIENT_LIGHT_NAME);
        *RendererModule._SetTransform = (RENDERERSETTRANSFORM)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_TRANSFORM_NAME);
        *RendererModule._SetTransformW2L = (RENDERERSETTRANSFORMW2L)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_TRANSFORM_W2L_NAME);
        *RendererModule._SetTransformO2W = (RENDERERSETTRANSFORMO2W)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_TRANSFORM_O2W_NAME);
        *RendererModule._PolyList = (RENDERERPOLYLIST)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_POLY_LIST_NAME);
        *RendererModule._PolyList2 = (RENDERERPOLYLIST2)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_POLY_LIST2_NAME);
        *RendererModule._PolyListL = (RENDERERPOLYLISTL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_POLY_LISTL_NAME);
        *RendererModule._PolyListD3DL = (RENDERERPOLYLISTD3DL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_POLY_LIST_D3DL_NAME);
        *RendererModule._PolyListD3DTL = (RENDERERPOLYLISTD3DTL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_POLY_LIST_D3DTL_NAME);
        *RendererModule._PolyListTL = (RENDERERPOLYLISTTL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_POLY_LIST_TL_NAME);
        *RendererModule._DotListD3DL = (RENDERERDOTLISTD3DL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_DOT_LIST_D3DL_NAME);
        *RendererModule._DotListD3DTL = (RENDERERDOTLISTD3DTL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_DOT_LIST_D3DTL_NAME);
        *RendererModule._LineListD3DL = (RENDERERLINELISTD3DL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_LINE_LIST_D3DL_NAME);
        *RendererModule._LineListD3DTL = (RENDERERLINELISTD3DTL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_LINE_LIST_D3DTL_NAME);
        *RendererModule._AcquireTextureHandle = (RENDERERACQUIRETEXTUREHANDLE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ACQUIRE_TEXTURE_HANDLE_NAME);
        *RendererModule._SelectTextureByHandle = (RENDERERSELECTTEXTUREBYHANDLE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ACQUIRE_TEXTURE_HANDLE_NAME);
        *RendererModule._ResetTextureCache = (RENDERERRESETTEXTURECACHE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_RESET_TEXTURE_CACHE_NAME);
        *RendererModule._EnableCulling = (RENDERERENABLECULLING)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ENABLE_CULLING_NAME);
        *RendererModule._EnableClipping = (RENDERERENABLECLIPPING)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ENABLE_CLIPPING_NAME);
        *RendererModule._SetTextureClamp = (RENDERERSETTEXTURECLAMP)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_TEXTURE_CLAMP_NAME);
        *RendererModule._SetFog = (RENDERERSETFOG)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_FOG_NAME);
        *RendererModule._ResetDynamicLight = (RENDERERRESETDYNAMICLIGHT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_RESET_DYNAMIC_LIGHT_NAME);
        *RendererModule._AddDynamicLight = (RENDERERADDDYNAMICLIGHT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ADD_DYNAMIC_LIGHT_NAME);
        *RendererModule._DynamicLightHint = (RENDERERDYNAMICLIGHTHINT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_DYNAMIC_LIGHT_HINT_NAME);
        *RendererModule._DrawPixels = (RENDERERDRAWPIXELS)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_DRAW_PIXELS_NAME);
        *RendererModule._VideoSave = (RENDERERVIDEOSAVE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_VIDEO_SAVE_NAME);
        *RendererModule._VideoRestore = (RENDERERVIDEORESTORE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_VIDEO_RESTORE_NAME);

        if (!invalid)
        {
            *RendererModule.State._IsActive = TRUE;

            const RendererModuleInitializeArguments args =
            {
                .R =
                {
                    .Display = (u32*)0x00726bd4, // TODO
                    .Font = (u32*)0x00726bd8, // TODO
                    .BitsPerPixel = (u32*)0x00726bdc, // TODO
                },
                .G =
                {
                    .Display = (u32*)0x00726be0, // TODO
                    .Font = (u32*)0x00726be4, // TODO
                    .BitsPerPixel = (u32*)0x00726be8, // TODO
                },
                .B =
                {
                    .Display = (u32*)0x00726bec, // TODO
                    .Font = (u32*)0x00726bf0, // TODO
                    .BitsPerPixel = (u32*)0x00726bf4, // TODO
                },
                .BlendMode = (RendererBlendMode*)0x00728c0c, // TODO
                .Unk11 = (u32*)0x00726f24, // TODO
                .Unk12 = (u32*)0x005f8ae0, // TODO
                .Unk13 = (u32*)0x00726f20, // TODO
                .Unk14 = EditorState.Windows.Active._Unk005,
                .Unk15 = EditorState.Windows.Active._Unk006,
                .Unk16 = EditorState.Windows.Active._Unk007,
                .Unk17 = EditorState.Windows.Active._Unk008,
                .Unk18 = (void*)0x005f8acc, // TODO
                .TextureSize = (u32*)0x005f8ad0, // TODO
                .Unk20 = (void*)0x00728840, // TODO
                .Unk21 = (s32*)0x0060185c, // TODO
                .DitherMode = AppOptionsState.Options.Graphics._DitherMode,
                .MaximumTextureCount = AppOptionsState.Options.Graphics._MaximumTextureCount,
                .SyncRetraceMode = AppOptionsState.Options.Graphics._SyncRetraceMode,
                .TextureFilterMode = AppOptionsState.Options.Graphics._TextureFilterMode,
                .AcceleratedGraphicsPortMode = AppOptionsState.Options.Graphics._AcceleratedGraphicsPort,
                .Unk27 = (u32*)0x00728c10, // TODO
                .TextureMipMapFilterMode = (RendererTextureMipMapFilterMode*)0x005f8ae4, // TODO
                .Unk29 = 48, // TODO constant
                .Unk30 = 44, // TODO constant
                .Unk31 = 12, // TODO constant
                .Unk32 = 32, // TODO constant
                .Unk33 = 40, // TODO constant
                .Unk34 = 36, // TODO constant
                .Unk35 = 32, // TODO constant
                .Unk36 = (void*)0x00d62c34 // TODO
            };

            if ((*RendererModule._Init)(*RendererModule.State._HWND, &args))
            {
                if (RendererModule.AcquireModuleName != NULL)
                {
                    if (!RendererModule.AcquireModuleName(RendererState.Options.ModuleName))
                    {
                        strcpy(RendererState.Options.ModuleName, RendererState.Options._ModuleFileName);
                    }
                }
                else
                {
                    strcpy(RendererState.Options.ModuleName, RendererState.Options._ModuleFileName);
                }

                SelectRendererGraphicsCard(*RendererModule.State._SelectedDeviceIndex);

                return TRUE;
            }

            *RendererState.Options._IsUsingModule = FALSE;

            return FALSE;
        }

        ReleaseRendererModule();

        *RendererState.Options._IsUsingModule = FALSE;

        return FALSE;
    }

    // 0x00565920
    void ReleaseRendererModule()
    {
        *RendererState._IsActive = FALSE;
        *RendererModule.State._IsActive = FALSE;

        if (*RendererModule.State._Handle != NULL)
        {
            ReleaseModule(*RendererModule.State._Handle);

            *RendererModule.State._Handle = NULL;
        }

        RendererModule.AcquireModuleName = NULL;

        *RendererModule._Kill = NULL;
        *RendererModule._Toggle = NULL;
        *RendererModule._SelectVideoMode = NULL;
        *RendererModule._SelectVideoMode2 = NULL;
        *RendererModule._RestoreVideoMode = NULL;
        *RendererModule._BeginScene = NULL;
        *RendererModule._EndScene = NULL;
        *RendererModule._LockFrame = NULL;
        *RendererModule._UnlockFrame = NULL;
        *RendererModule._SelectTexture = NULL;
        *RendererModule._UpdateTexture = NULL;
        *RendererModule._SetMipMapLevel = NULL;
        *RendererModule._DrawPolygon = NULL;
        *RendererModule._DrawPolygon2 = NULL;
        *RendererModule._DrawPolyList = NULL;
        *RendererModule._DrawPolyList2 = NULL;
        *RendererModule._DrawPolyList3 = NULL;
        *RendererModule._AddParticle = NULL;
        *RendererModule._FlushParticleList = NULL;
        *RendererModule._Add3DLine = NULL;
        *RendererModule._FlushLineList = NULL;
        *RendererModule._Clear = NULL;
        *RendererModule._SetFogColor = NULL;
        *RendererModule._Sync = NULL;
        *RendererModule._ClearZBuffer = NULL;
        *RendererModule._ClearZBox = NULL;
        *RendererModule._SetColorTable = NULL;
        *RendererModule._AcquireDisplayContext = NULL;
        *RendererModule._ReleaseDisplayContext = NULL;
        *RendererModule._MasterZBuffer = NULL;
        *RendererModule._RestoreZBuffer = NULL;
        *RendererModule._AcquireVideoMemory = NULL;
        *RendererModule._SelectGraphicsCard = NULL;
        *RendererModule._AcquireGraphicsCardList = NULL;
        *RendererModule._LockHoldBuffer = NULL;
        *RendererModule._UnlockHoldBuffer = NULL;
        *RendererModule._LockTexture = NULL;
        *RendererModule._UnlockTexture = NULL;
        *RendererModule._AcquireTextureInformation = NULL;
        *RendererModule._SelectVideoModeX = NULL;
        *RendererModule._RestoreVideoModeX = NULL;
        *RendererModule._LockFrameX = NULL;
        *RendererModule._UnlockFrameX = NULL;
        *RendererModule._ToggleX = NULL;
        *RendererModule._SetViewPort = NULL;
        *RendererModule._SetLightConstants = NULL;
        *RendererModule._SetLightVector = NULL;
        *RendererModule._SetAmbientLight = NULL;
        *RendererModule._SetTransform = NULL;
        *RendererModule._SetTransformW2L = NULL;
        *RendererModule._SetTransformO2W = NULL;
        *RendererModule._PolyList = NULL;
        *RendererModule._PolyList2 = NULL;
        *RendererModule._PolyListL = NULL;
        *RendererModule._PolyListD3DL = NULL;
        *RendererModule._PolyListD3DTL = NULL;
        *RendererModule._PolyListTL = NULL;
        *RendererModule._DotListD3DL = NULL;
        *RendererModule._DotListD3DTL = NULL;
        *RendererModule._LineListD3DL = NULL;
        *RendererModule._LineListD3DTL = NULL;
        *RendererModule._AcquireTextureHandle = NULL;
        *RendererModule._SelectTextureByHandle = NULL;
        *RendererModule._ResetTextureCache = NULL;
        *RendererModule._EnableCulling = NULL;
        *RendererModule._EnableClipping = NULL;
        *RendererModule._SetTextureClamp = NULL;
        *RendererModule._SetFog = NULL;
        *RendererModule._ResetDynamicLight = NULL;
        *RendererModule._AddDynamicLight = NULL;
        *RendererModule._DynamicLightHint = NULL;
        *RendererModule._DrawPixels = NULL;
        *RendererModule._VideoSave = NULL;
        *RendererModule._VideoRestore = NULL;
        *RendererModule._Init = NULL;
    }
}