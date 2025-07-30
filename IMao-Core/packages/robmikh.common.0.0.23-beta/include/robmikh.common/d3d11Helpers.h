#pragma once
#include <winrt/base.h>
#include <d3d11_4.h>
#include <dxgi1_2.h>

namespace robmikh::common::uwp
{
    struct D3D11DeviceLock
    {
    public:
        D3D11DeviceLock(std::nullopt_t) {}
        D3D11DeviceLock(ID3D11Multithread* pMultithread)
        {
            m_multithread.copy_from(pMultithread);
            m_multithread->Enter();
        }
        ~D3D11DeviceLock()
        {
            m_multithread->Leave();
            m_multithread = nullptr;
        }
    private:
        winrt::com_ptr<ID3D11Multithread> m_multithread;
    };

    inline auto CreateD3D11Device(D3D_DRIVER_TYPE const type, UINT flags, winrt::com_ptr<ID3D11Device>& device)
    {
        WINRT_ASSERT(!device);

        return D3D11CreateDevice(nullptr, type, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION, device.put(),
            nullptr, nullptr);
    }

    inline auto CreateD3D11Device(UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT)
    {
        winrt::com_ptr<ID3D11Device> device;
        HRESULT hr = CreateD3D11Device(D3D_DRIVER_TYPE_HARDWARE, flags, device);
        if (DXGI_ERROR_UNSUPPORTED == hr)
        {
            hr = CreateD3D11Device(D3D_DRIVER_TYPE_WARP, flags, device);
        }

        winrt::check_hresult(hr);
        return device;
    }

    inline auto CreateDXGISwapChain(winrt::com_ptr<ID3D11Device> const& device, const DXGI_SWAP_CHAIN_DESC1* desc)
    {
        auto dxgiDevice = device.as<IDXGIDevice2>();
        winrt::com_ptr<IDXGIAdapter> adapter;
        winrt::check_hresult(dxgiDevice->GetParent(winrt::guid_of<IDXGIAdapter>(), adapter.put_void()));
        winrt::com_ptr<IDXGIFactory2> factory;
        winrt::check_hresult(adapter->GetParent(winrt::guid_of<IDXGIFactory2>(), factory.put_void()));

        winrt::com_ptr<IDXGISwapChain1> swapchain;
        winrt::check_hresult(factory->CreateSwapChainForComposition(device.get(), desc, nullptr, swapchain.put()));
        return swapchain;
    }

    inline auto CreateDXGISwapChain(winrt::com_ptr<ID3D11Device> const& device,
        uint32_t width, uint32_t height, DXGI_FORMAT format, uint32_t bufferCount)
    {
        DXGI_SWAP_CHAIN_DESC1 desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.Format = format;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.BufferCount = bufferCount;
        desc.Scaling = DXGI_SCALING_STRETCH;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

        return CreateDXGISwapChain(device, &desc);
    }

    inline auto CopyD3DTexture(winrt::com_ptr<ID3D11Device> const& device, winrt::com_ptr<ID3D11Texture2D> const& texture, bool asStagingTexture)
    {
        winrt::com_ptr<ID3D11DeviceContext> context;
        device->GetImmediateContext(context.put());

        D3D11_TEXTURE2D_DESC desc = {};
        texture->GetDesc(&desc);
        // Clear flags that we don't need
        desc.Usage = asStagingTexture ? D3D11_USAGE_STAGING : D3D11_USAGE_DEFAULT;
        desc.BindFlags = asStagingTexture ? 0 : D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = asStagingTexture ? D3D11_CPU_ACCESS_READ : 0;
        desc.MiscFlags = 0;

        // Create and fill the texture copy
        winrt::com_ptr<ID3D11Texture2D> textureCopy;
        winrt::check_hresult(device->CreateTexture2D(&desc, nullptr, textureCopy.put()));
        context->CopyResource(textureCopy.get(), texture.get());

        return textureCopy;
    }

    inline auto PrepareStagingTexture(winrt::com_ptr<ID3D11Device> const& device, winrt::com_ptr<ID3D11Texture2D> const& texture)
    {
        // If our texture is already set up for staging, then use it.
        // Otherwise, create a staging texture.
        D3D11_TEXTURE2D_DESC desc = {};
        texture->GetDesc(&desc);
        if (desc.Usage == D3D11_USAGE_STAGING && desc.CPUAccessFlags & D3D11_CPU_ACCESS_READ)
        {
            return texture;
        }

        return CopyD3DTexture(device, texture, true);
    }

    inline size_t
        GetBytesPerPixel(DXGI_FORMAT pixelFormat)
    {
        switch (pixelFormat)
        {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
            return 16;
        case DXGI_FORMAT_R32G32B32_TYPELESS:
        case DXGI_FORMAT_R32G32B32_FLOAT:
        case DXGI_FORMAT_R32G32B32_UINT:
        case DXGI_FORMAT_R32G32B32_SINT:
            return 12;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_SINT:
        case DXGI_FORMAT_R32G32_TYPELESS:
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32_SINT:
        case DXGI_FORMAT_R32G8X24_TYPELESS:
            return 8;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
        case DXGI_FORMAT_R11G11B10_FLOAT:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_R16G16_TYPELESS:
        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_UNKNOWN:
        case DXGI_FORMAT_R16G16_UINT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_SINT:
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32_SINT:
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        case DXGI_FORMAT_R8G8_B8G8_UNORM:
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            return 4;
        case DXGI_FORMAT_R8G8_TYPELESS:
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R16_SINT:
        case DXGI_FORMAT_B5G6R5_UNORM:
        case DXGI_FORMAT_B5G5R5A1_UNORM:
        case DXGI_FORMAT_B4G4R4A4_UNORM:
            return 2;
        case DXGI_FORMAT_R8_TYPELESS:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_R8_SINT:
        case DXGI_FORMAT_A8_UNORM:
            return 1;
        default:
            throw winrt::hresult_invalid_argument(L"Invalid pixel format!");
        }
    }

    inline auto CopyBytesFromTexture(winrt::com_ptr<ID3D11Texture2D> const& texture, uint32_t subresource = 0)
    {
        winrt::com_ptr<ID3D11Device> device;
        texture->GetDevice(device.put());
        winrt::com_ptr<ID3D11DeviceContext> context;
        device->GetImmediateContext(context.put());

        auto stagingTexture = PrepareStagingTexture(device, texture);

        D3D11_TEXTURE2D_DESC desc = {};
        stagingTexture->GetDesc(&desc);
        auto bytesPerPixel = GetBytesPerPixel(desc.Format);

        // Copy the bits
        D3D11_MAPPED_SUBRESOURCE mapped = {};
        winrt::check_hresult(context->Map(stagingTexture.get(), subresource, D3D11_MAP_READ, 0, &mapped));

        auto bytesStride = static_cast<size_t>(desc.Width) * bytesPerPixel;
        std::vector<byte> bytes(bytesStride * static_cast<size_t>(desc.Height), 0);
        auto source = reinterpret_cast<byte*>(mapped.pData);
        auto dest = bytes.data();
        for (auto i = 0; i < (int)desc.Height; i++)
        {
            memcpy(dest, source, bytesStride);

            source += mapped.RowPitch;
            dest += bytesStride;
        }
        context->Unmap(stagingTexture.get(), 0);

        return bytes;
    }

    inline auto CreateTextureFromRawBytes(
        winrt::com_ptr<ID3D11Device> const& d3dDevice,
        uint8_t const* bytes,
        uint32_t width,
        uint32_t height,
        DXGI_FORMAT pixelFormat,
        bool renderTarget = false)
    {
        uint32_t bindFlags = D3D11_BIND_SHADER_RESOURCE;
        if (renderTarget)
        {
            bindFlags |= D3D11_BIND_RENDER_TARGET;
        }

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = pixelFormat;
        desc.BindFlags = bindFlags;
        desc.SampleDesc.Count = 1;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = bytes;
        initData.SysMemPitch = static_cast<uint32_t>(width * GetBytesPerPixel(pixelFormat));

        winrt::com_ptr<ID3D11Texture2D> texture;
        winrt::check_hresult(d3dDevice->CreateTexture2D(&desc, &initData, texture.put()));

        return texture;
    }
}