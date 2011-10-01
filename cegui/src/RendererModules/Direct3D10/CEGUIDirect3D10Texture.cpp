/***********************************************************************
    filename:   CEGUIDirect3D10Texture.cpp
    created:    Sat Mar 7 2009
    author:     Paul D Turner (parts based on code by Rajko Stojadinovic)
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2011 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include "CEGUI/RendererModules/Direct3D10/Texture.h"
#include "CEGUI/System.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/ImageCodec.h"
#include <d3d10.h>

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
// Helper utility function that copies an RGBA buffer into a region of a second
// buffer as D3DCOLOR data values
static void blitToSurface(const uint32* src, uint32* dst,
                   const Sizef& sz, size_t dest_pitch)
{
    for (uint i = 0; i < sz.d_height; ++i)
    {
        for (uint j = 0; j < sz.d_width; ++j)
        {
            const uint32 pixel = src[j];
            const uint32 tmp = pixel & 0x00FF00FF;
            dst[j] = pixel & 0xFF00FF00 | (tmp << 16) | (tmp >> 16);
        }

        dst += dest_pitch / sizeof(uint32);
        src += static_cast<uint32>(sz.d_width);
    }
}

//----------------------------------------------------------------------------//
// Helper utility function that copies a region of a buffer containing D3DCOLOR
// values into a second buffer as RGBA values.
static void blitFromSurface(const uint32* src, uint32* dst,
                     const Sizef& sz, size_t source_pitch)
{
    for (uint i = 0; i < sz.d_height; ++i)
    {
        for (uint j = 0; j < sz.d_width; ++j)
        {
            const uint32 pixel = src[j];
            const uint32 tmp = pixel & 0x00FF00FF;
            dst[j] = pixel & 0xFF00FF00 | (tmp << 16) | (tmp >> 16);
        }

        src += source_pitch / sizeof(uint32);
        dst += static_cast<uint32>(sz.d_width);
    }
}

//----------------------------------------------------------------------------//
void Direct3D10Texture::setDirect3DTexture(ID3D10Texture2D* tex)
{
    if (d_texture != tex)
    {
        cleanupDirect3D10Texture();
        d_dataSize.d_width = d_dataSize.d_height = 0;

        d_texture = tex;
        if (d_texture)
            d_texture->AddRef();
    }

    initialiseShaderResourceView();

    updateTextureSize();
    d_dataSize = d_size;
    updateCachedScaleValues();
}

//----------------------------------------------------------------------------//
ID3D10Texture2D* Direct3D10Texture::getDirect3DTexture() const
{
    return d_texture;
}

//----------------------------------------------------------------------------//
ID3D10ShaderResourceView* Direct3D10Texture::getDirect3DShaderResourceView() const
{
    return d_resourceView;
}

//----------------------------------------------------------------------------//
void Direct3D10Texture::setOriginalDataSize(const Sizef& sz)
{
    d_dataSize = sz;
    updateCachedScaleValues();
}

//----------------------------------------------------------------------------//
const String& Direct3D10Texture::getName() const
{
    return d_name;
}

//----------------------------------------------------------------------------//
const Sizef& Direct3D10Texture::getSize() const
{
    return d_size;
}

//----------------------------------------------------------------------------//
const Sizef& Direct3D10Texture::getOriginalDataSize() const
{
    return d_dataSize;
}

//----------------------------------------------------------------------------//
const Vector2f& Direct3D10Texture::getTexelScaling() const
{
    return d_texelScaling;
}

//----------------------------------------------------------------------------//
void Direct3D10Texture::loadFromFile(const String& filename,
                                     const String& resourceGroup)
{
    // get and check existence of CEGUI::System object
    System* sys = System::getSingletonPtr();
    if (!sys)
        CEGUI_THROW(RendererException("Direct3D10Texture::loadFromFile - "
            "CEGUI::System object has not been created!"));

    // load file to memory via resource provider
    RawDataContainer texFile;
    sys->getResourceProvider()->loadRawDataContainer(filename, texFile,
                                                     resourceGroup);

    Texture* res = sys->getImageCodec().load(texFile, this);

    // unload file data buffer
    sys->getResourceProvider()->unloadRawDataContainer(texFile);

    if (!res)
        // It's an error
        CEGUI_THROW(RendererException("Direct3D10Texture::loadFromFile - " +
            sys->getImageCodec().getIdentifierString() +
            " failed to load image '" + filename + "'."));
}

//----------------------------------------------------------------------------//
void Direct3D10Texture::loadFromMemory(const void* buffer,
                                       const Sizef& buffer_size,
                                       PixelFormat pixel_format)
{
    cleanupDirect3D10Texture();

    const void* img_src = buffer;
    if (pixel_format == PF_RGB)
    {
        const char* src = static_cast<const char*>(buffer);
        char* dest = new char[buffer_size.d_width * buffer_size.d_height * 4];

        for (int i = 0; i < buffer_size.d_width * buffer_size.d_height; ++i)
        {
            dest[i * 4 + 0] = src[i * 3 + 0];
            dest[i * 4 + 1] = src[i * 3 + 1];
            dest[i * 4 + 2] = src[i * 3 + 2];
            dest[i * 4 + 3] = 0xFF;
        }

        img_src = dest;
    }

    D3D10_TEXTURE2D_DESC tex_desc;
    ZeroMemory(&tex_desc, sizeof(tex_desc));
    tex_desc.Width = static_cast<UINT>(buffer_size.d_width);
    tex_desc.Height = static_cast<UINT>(buffer_size.d_height);
    tex_desc.ArraySize = 1;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    tex_desc.Usage = D3D10_USAGE_DEFAULT;
    tex_desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
    tex_desc.CPUAccessFlags = 0;
    tex_desc.MiscFlags = 0;
    tex_desc.MipLevels = 1;

    D3D10_SUBRESOURCE_DATA data;
    ZeroMemory(&data, sizeof(D3D10_SUBRESOURCE_DATA));
    data.pSysMem = img_src;
    data.SysMemPitch = 4 * tex_desc.Width;

    HRESULT hr = d_device.CreateTexture2D(&tex_desc, &data, &d_texture);

    if (pixel_format == PF_RGB)
        delete[] img_src;

    if (FAILED(hr))
        CEGUI_THROW(RendererException(
            "Direct3D10Texture::loadFromMemory: Failed to "
            "create texture from memory buffer."));

    initialiseShaderResourceView();

    d_dataSize = buffer_size;
    updateTextureSize();
    updateCachedScaleValues();
}

//----------------------------------------------------------------------------//
void Direct3D10Texture::blitFromMemory(void* sourceData, const Rectf& area)
{
    if (!d_texture)
        return;

    uint32* buff = new uint32[static_cast<size_t>(area.getWidth()) *
                              static_cast<size_t>(area.getHeight())];
    blitFromSurface(static_cast<uint32*>(sourceData), buff,
                    area.getSize(), static_cast<size_t>(area.getWidth()) * 4);

    D3D10_BOX dst_box = {static_cast<UINT>(area.left()),
                         static_cast<UINT>(area.top()),
                         0,
                         static_cast<UINT>(area.right()),
                         static_cast<UINT>(area.bottom()),
                         1};

    d_device.UpdateSubresource(d_texture, 0, &dst_box, buff,
                               static_cast<UINT>(area.getWidth()) * 4, 0);

    delete[] buff;
}

//----------------------------------------------------------------------------//
void Direct3D10Texture::blitToMemory(void* targetData)
{
    if (!d_texture)
        return;

    String exception_msg;

    D3D10_TEXTURE2D_DESC tex_desc;
    d_texture->GetDesc(&tex_desc);

    tex_desc.Usage = D3D10_USAGE_STAGING;
    tex_desc.BindFlags = 0;
    tex_desc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;

    ID3D10Texture2D* offscreen;
    if (SUCCEEDED(d_device.CreateTexture2D(&tex_desc, 0, &offscreen)))
    {
        d_device.CopyResource(offscreen, d_texture);

        D3D10_MAPPED_TEXTURE2D mapped_tex;
        if (SUCCEEDED(offscreen->Map(0, D3D10_MAP_READ, 0, &mapped_tex)))
        {
            blitFromSurface(static_cast<uint32*>(mapped_tex.pData),
                            static_cast<uint32*>(targetData),
                            Sizef(static_cast<float>(tex_desc.Width),
                                   static_cast<float>(tex_desc.Height)),
                            mapped_tex.RowPitch);

            offscreen->Unmap(0);
        }
        else
            exception_msg.assign("[Direct3D10Renderer] "
                "ID3D10Texture2D::Map failed.");

        offscreen->Release();
    }
    else
        exception_msg.assign("[Direct3D10Renderer] "
            "ID3D10Device::CreateTexture2D failed for 'offscreen'.");

    if (!exception_msg.empty())
        CEGUI_THROW(RendererException(exception_msg));
}

//----------------------------------------------------------------------------//
void Direct3D10Texture::cleanupDirect3D10Texture()
{
    if (d_resourceView)
    {
        d_resourceView->Release();
        d_resourceView = 0;
    }

    if (d_texture)
    {
        d_texture->Release();
        d_texture = 0;
    }
}

//----------------------------------------------------------------------------//
void Direct3D10Texture::updateCachedScaleValues()
{
    //
    // calculate what to use for x scale
    //
    const float orgW = d_dataSize.d_width;
    const float texW = d_size.d_width;

    // if texture and original data width are the same, scale is based
    // on the original size.
    // if texture is wider (and source data was not stretched), scale
    // is based on the size of the resulting texture.
    d_texelScaling.d_x = 1.0f / ((orgW == texW) ? orgW : texW);

    //
    // calculate what to use for y scale
    //
    const float orgH = d_dataSize.d_height;
    const float texH = d_size.d_height;

    // if texture and original data height are the same, scale is based
    // on the original size.
    // if texture is taller (and source data was not stretched), scale
    // is based on the size of the resulting texture.
    d_texelScaling.d_y = 1.0f / ((orgH == texH) ? orgH : texH);
}

//----------------------------------------------------------------------------//
void Direct3D10Texture::updateTextureSize()
{
    if (d_texture)
    {
        D3D10_TEXTURE2D_DESC surfDesc;
        d_texture->GetDesc(&surfDesc);
        d_size.d_width  = static_cast<float>(surfDesc.Width);
        d_size.d_height = static_cast<float>(surfDesc.Height);
    }
    else
        d_size.d_height = d_size.d_width = 0.0f;
}

//----------------------------------------------------------------------------//
Direct3D10Texture::Direct3D10Texture(ID3D10Device& device, const String& name) :
    d_device(device),
    d_texture(0),
    d_resourceView(0),
    d_size(0, 0),
    d_dataSize(0, 0),
    d_texelScaling(0, 0),
    d_name(name)
{
}

//----------------------------------------------------------------------------//
Direct3D10Texture::Direct3D10Texture(ID3D10Device& device, const String& name,
                                     const String& filename,
                                     const String& resourceGroup) :
    d_device(device),
    d_texture(0),
    d_resourceView(0),
    d_size(0, 0),
    d_dataSize(0, 0),
    d_texelScaling(0, 0),
    d_name(name)
{
    loadFromFile(filename, resourceGroup);
}

//----------------------------------------------------------------------------//
Direct3D10Texture::Direct3D10Texture(ID3D10Device& device, const String& name,
                                     const Sizef& sz) :
    d_device(device),
    d_texture(0),
    d_resourceView(0),
    d_size(0, 0),
    d_dataSize(0, 0),
    d_texelScaling(0, 0),
    d_name(name)
{
    D3D10_TEXTURE2D_DESC tex_desc;
    ZeroMemory(&tex_desc, sizeof(tex_desc));
    tex_desc.Width = static_cast<UINT>(sz.d_width);
    tex_desc.Height = static_cast<UINT>(sz.d_height);
    tex_desc.ArraySize = 1;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    tex_desc.Usage = D3D10_USAGE_DEFAULT;
    tex_desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
    tex_desc.CPUAccessFlags = 0;
    tex_desc.MiscFlags = 0;
    tex_desc.MipLevels = 1;

    if (FAILED(d_device.CreateTexture2D(&tex_desc, 0, &d_texture)))
        CEGUI_THROW(RendererException(
            "Direct3D10Texture: Failed to create texture with specified size."));

    initialiseShaderResourceView();

    d_dataSize = sz;
    updateTextureSize();
    updateCachedScaleValues();
}

//----------------------------------------------------------------------------//
Direct3D10Texture::Direct3D10Texture(ID3D10Device& device, const String& name,
                                     ID3D10Texture2D* tex) :
    d_device(device),
    d_texture(0),
    d_resourceView(0),
    d_size(0, 0),
    d_dataSize(0, 0),
    d_texelScaling(0, 0),
    d_name(name)
{
    setDirect3DTexture(tex);
}

//----------------------------------------------------------------------------//
Direct3D10Texture::~Direct3D10Texture()
{
    cleanupDirect3D10Texture();
}

//----------------------------------------------------------------------------//
void Direct3D10Texture::initialiseShaderResourceView()
{
    if (!d_texture)
        return;

    D3D10_TEXTURE2D_DESC tex_desc;
    d_texture->GetDesc(&tex_desc);

    ID3D10Resource* resource = 0;
    d_texture->QueryInterface(__uuidof(ID3D10Resource),
                              reinterpret_cast<LPVOID*>(&resource));

    D3D10_SHADER_RESOURCE_VIEW_DESC srvd;
    srvd.Format = tex_desc.Format;
    srvd.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    srvd.Texture2D.MostDetailedMip = 0;
    srvd.Texture2D.MipLevels = tex_desc.MipLevels;
    d_device.CreateShaderResourceView(resource, &srvd, &d_resourceView);
    resource->Release();
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
