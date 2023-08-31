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

#include "DX.hxx"
#include "Quit.hxx"

using namespace Renderer::Module;

namespace DX
{
    HRESULT DXC(const HRESULT code, const char* message)
    {
        if (code == DD_OK) { return code; }

        const char* description = NULL;

        switch (code)
        {
        case D3DERR_BADMAJORVERSION: { description = "The service you requested is unavailable in this major version of DirectX. (A \"major version\" denotes a primary release, such as DirectX 6.0.)."; break; }
        case D3DERR_BADMINORVERSION: { description = "The service you requested is available in this major version of DirectX, but not in this minor version. Get the latest version of the component runtime from Microsoft. (A \"minor version\" denotes a secondary release, such as DirectX 6.1.)."; break; }
        case D3DERR_INVALID_DEVICE: { description = "The requested device type is not valid."; break; }
        case D3DERR_INITFAILED: { description = "A rendering device could not be created because the new device could not be initialized."; break; }
        case D3DERR_DEVICEAGGREGATED: { description = "The IDirect3DDevice3::SetRenderTarget method was called on a device that was retrieved from the render target surface."; break; }
        case D3DERR_EXECUTE_CREATE_FAILED: { description = "The execute buffer could not be created. This typically occurs when no memory is available to allocate the execute buffer."; break; }
        case D3DERR_EXECUTE_DESTROY_FAILED: { description = "The memory for the execute buffer could not be deallocated."; break; }
        case D3DERR_EXECUTE_LOCK_FAILED: { description = "The execute buffer could not be locked."; break; }
        case D3DERR_EXECUTE_UNLOCK_FAILED: { description = "The execute buffer could not be unlocked."; break; }
        case D3DERR_EXECUTE_LOCKED: { description = "The operation requested by the application could not be completed because the execute buffer is locked."; break; }
        case D3DERR_EXECUTE_NOT_LOCKED: { description = "The execute buffer could not be unlocked because it is not currently locked."; break; }
        case D3DERR_EXECUTE_FAILED: { description = "The contents of the execute buffer are invalid and cannot be executed."; break; }
        case D3DERR_EXECUTE_CLIPPED_FAILED: { description = "The execute buffer could not be clipped during execution."; break; }
        case D3DERR_TEXTURE_NO_SUPPORT: { description = "The device does not support texture mapping."; break; }
        case D3DERR_TEXTURE_CREATE_FAILED: { description = "The texture handle for the texture could not be retrieved from the driver."; break; }
        case D3DERR_TEXTURE_DESTROY_FAILED: { description = "The device was unable to deallocate the texture memory."; break; }
        case D3DERR_TEXTURE_LOCK_FAILED: { description = "The texture could not be locked."; break; }
        case D3DERR_TEXTURE_UNLOCK_FAILED: { description = "The texture surface could not be unlocked."; break; }
        case D3DERR_TEXTURE_LOAD_FAILED: { description = "The texture could not be loaded."; break; }
        case D3DERR_TEXTURE_SWAP_FAILED: { description = "The texture handles could not be swapped."; break; }
        case D3DERR_TEXTURE_LOCKED: { description = "The requested operation could not be completed because the texture surface is currently locked."; break; }
        case D3DERR_TEXTURE_NOT_LOCKED: { description = "The requested operation could not be completed because the texture surface is not locked."; break; }
        case D3DERR_TEXTURE_GETSURF_FAILED: { description = "The DirectDraw surface used to create the texture could not be retrieved."; break; }
        case D3DERR_MATRIX_CREATE_FAILED: { description = "The matrix could not be created. This can occur when no memory is available to allocate for the matrix."; break; }
        case D3DERR_MATRIX_DESTROY_FAILED: { description = "The memory for the matrix could not be deallocated."; break; }
        case D3DERR_MATRIX_SETDATA_FAILED: { description = "The matrix data could not be set. This can occur when the matrix was not created by the current device."; break; }
        case D3DERR_MATRIX_GETDATA_FAILED: { description = "The matrix data could not be retrieved. This can occur when the matrix was not created by the current device."; break; }
        case D3DERR_SETVIEWPORTDATA_FAILED: { description = "The viewport parameters could not be set."; break; }
        case D3DERR_INVALIDCURRENTVIEWPORT: { description = "The currently selected viewport is not valid."; break; }
        case D3DERR_INVALIDPRIMITIVETYPE: { description = "The primitive type specified by the application is invalid."; break; }
        case D3DERR_INVALIDVERTEXTYPE: { description = "The vertex type specified by the application is invalid."; break; }
        case D3DERR_TEXTURE_BADSIZE: { description = "The dimensions of a current texture are invalid. This can occur when an application attempts to use a texture that has non-power-of-two dimensions with a device that requires them."; break; }
        case D3DERR_INVALIDRAMPTEXTURE: { description = "Ramp mode is being used and the texture handle in the current material does not match the current texture handle that is set as a render state."; break; }
        case D3DERR_MATERIAL_CREATE_FAILED: { description = "The material could not be created. This typically occurs when no memory is available to allocate for the material."; break; }
        case D3DERR_MATERIAL_DESTROY_FAILED: { description = "The memory for the material could not be deallocated."; break; }
        case D3DERR_MATERIAL_SETDATA_FAILED: { description = "The material parameters could not be set."; break; }
        case D3DERR_MATERIAL_GETDATA_FAILED: { description = "The material parameters could not be retrieved."; break; }
        case D3DERR_INVALIDPALETTE: { description = "The palette associated with a surface is invalid."; break; }
        case D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY: { description = "The requested operation could not be completed because the specified device requires system-memory depth-buffer surfaces. (Software rendering devices require system-memory depth buffers.)"; break; }
        case D3DERR_ZBUFF_NEEDS_VIDEOMEMORY: { description = "The requested operation could not be completed because the specified device requires video-memory depth-buffer surfaces. (Hardware-accelerated devices require video-memory depth buffers.)"; break; }
        case D3DERR_SURFACENOTINVIDMEM: { description = "The device could not be created because the render target surface is not located in video-memory. (Hardware-accelerated devices require video-memory render target surfaces.)"; break; }
        case D3DERR_LIGHT_SET_FAILED: { description = "The attempt to set lighting parameters for a light object failed."; break; }
        case D3DERR_LIGHTHASVIEWPORT: { description = "The requested operation failed because the light object is associated with another viewport."; break; }
        case D3DERR_LIGHTNOTINTHISVIEWPORT: { description = "The requested operation failed because the light object has not been associated with this viewport."; break; }
        case D3DERR_SCENE_IN_SCENE: { description = "Scene rendering could not begin because a previous scene was not completed by a call to the IDirect3DDevice3::EndScene method."; break; }
        case D3DERR_SCENE_NOT_IN_SCENE: { description = "Scene rendering could not be completed because a scene was not started by a previous call to the IDirect3DDevice3::BeginScene method."; break; }
        case D3DERR_SCENE_BEGIN_FAILED: { description = "Scene rendering could not begin."; break; }
        case D3DERR_SCENE_END_FAILED: { description = "Scene rendering could not be completed."; break; }
        case D3DERR_INBEGIN: { description = "The requested operation cannot be completed while scene rendering is taking place. Try again after the scene is completed and the IDirect3DDevice::EndScene method (or equivalent method) is called."; break; }
        case D3DERR_NOTINBEGIN: { description = "The requested rendering operation could not be completed because scene rendering has not begun. Call IDirect3DDevice3::BeginScene to begin rendering then try again."; break; }
        case D3DERR_NOVIEWPORTS: { description = "The requested operation failed because the device currently has no viewports associated with it."; break; }
        case D3DERR_VIEWPORTDATANOTSET: { description = "The requested operation could not be completed because viewport parameters have not yet been set. Set the viewport parameters by calling IDirect3DViewport3::SetViewport method and try again."; break; }
        case D3DERR_VIEWPORTHASNODEVICE: { description = "The requested operation could not be completed because the viewport has not yet been associated with a device. Associate the viewport with a rendering device by calling IDirect3DDevice3::AddViewport and try again."; break; }
        case D3DERR_NOCURRENTVIEWPORT: { description = "The viewport parameters could not be retrieved because none have been set."; break; }
        case D3DERR_INVALIDVERTEXFORMAT: { description = "The combination of flexible vertex format flags specified by the application is not valid."; break; }
        case D3DERR_VERTEXBUFFEROPTIMIZED: { description = "The requested operation could not be completed because the vertex buffer is optimized. (The contents of optimized vertex buffers are driver specific, and considered private.)"; break; }
        case D3DERR_COLORKEYATTACHED: { description = "The application attempted to create a texture with a surface that uses a color key for transparency."; break; }
        case D3DERR_ZBUFFER_NOTPRESENT: { description = "The requested operation could not be completed because the render target surface does not have an attached depth buffer."; break; }
        case D3DERR_VBUF_CREATE_FAILED: { description = "The vertex buffer could not be created. This can happen when there is insufficient memory to allocate a vertex buffer."; break; }
        case D3DERR_VERTEXBUFFERLOCKED: { description = "The requested operation could not be completed because the vertex buffer is locked."; break; }
        case D3DERR_STENCILBUFFER_NOTPRESENT: { description = "The requested stencil buffer operation could not be completed because there is no stencil buffer attached to the render target surface."; break; }
        case D3DERR_WRONGTEXTUREFORMAT: { description = "The pixel format of the texture surface is not valid."; break; }
        case D3DERR_UNSUPPORTEDCOLOROPERATION: { description = "The device does not support the one of the specified texture blending operations for color values."; break; }
        case D3DERR_UNSUPPORTEDCOLORARG: { description = "The device does not support the one of the specified texture blending arguments for color values."; break; }
        case D3DERR_UNSUPPORTEDALPHAOPERATION: { description = "The device does not support one of the specified texture blending operations for the alpha channel."; break; }
        case D3DERR_UNSUPPORTEDALPHAARG: { description = "The device does not support one of the specified texture blending arguments for the alpha channel."; break; }
        case D3DERR_CONFLICTINGTEXTUREFILTER: { description = "The current texture filters cannot be used together."; break; }
        case D3DERR_UNSUPPORTEDFACTORVALUE: { description = "The specified texture factor value is not supported by the device."; break; }
        case D3DERR_CONFLICTINGRENDERSTATE: { description = "The currently set render states cannot be used together."; break; }
        case D3DERR_UNSUPPORTEDTEXTUREFILTER: { description = "The specified texture filter is not supported by the device."; break; }
        case D3DERR_TOOMANYPRIMITIVES: { description = "The device is unable to render the provided quantity of primitives in a single pass."; break; }
        case D3DERR_INVALIDMATRIX: { description = "The requested operation could not be completed because the combination of the currently set world, view, and projection matrices is invalid (the determinant of the combined matrix is zero)."; break; }
        case D3DERR_CONFLICTINGTEXTUREPALETTE: { description = "The current textures cannot be used simultaneously. This generally occurs when a multi-texture device requires that all palettized textures simultaneously enabled also share the same palette."; break; }
        }

        Quit("DirectX: %s %s", message, description ? description : "An unknown error code was returned.");

        return code;
    }
}