#pragma once
#include <memorybuffer.h>
#include <d3d11.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <wil/coroutine.h>
#include "d3d11Helpers.h"
#include "customBuffers.h"

namespace robmikh::common::uwp
{
    enum class BitmapEncoding
    {
        Bmp,
        Gif,
        Heif,
        Jpeg,
        JpegXr,
        Png,
        Tiff
    };

    namespace impl
    {
        inline auto BitmapEncodingToEncoderGuid(BitmapEncoding const& encoding)
        {
            switch (encoding)
            {
                case BitmapEncoding::Bmp:
                    return winrt::Windows::Graphics::Imaging::BitmapEncoder::BmpEncoderId();
                case BitmapEncoding::Gif:
                    return winrt::Windows::Graphics::Imaging::BitmapEncoder::GifEncoderId();
                case BitmapEncoding::Heif:
                    return winrt::Windows::Graphics::Imaging::BitmapEncoder::HeifEncoderId();
                case BitmapEncoding::Jpeg:
                    return winrt::Windows::Graphics::Imaging::BitmapEncoder::JpegEncoderId();
                case BitmapEncoding::JpegXr:
                    return winrt::Windows::Graphics::Imaging::BitmapEncoder::JpegXREncoderId();
                case BitmapEncoding::Png:
                    return winrt::Windows::Graphics::Imaging::BitmapEncoder::PngEncoderId();
                case BitmapEncoding::Tiff:
                    return winrt::Windows::Graphics::Imaging::BitmapEncoder::TiffEncoderId();
                default:
                    throw winrt::hresult_invalid_argument();
            }
        }
    }

    inline wil::task<winrt::com_ptr<ID3D11Texture2D>> LoadTextureFromStreamAsync(
        winrt::Windows::Storage::Streams::IRandomAccessStream const& stream,
        winrt::com_ptr<ID3D11Device> d3dDevice,
        bool renderTarget = false,
        bool interpretIgnoreAsStraight = true)
    {
        // Read the image
        auto decoder = co_await winrt::Windows::Graphics::Imaging::BitmapDecoder::CreateAsync(stream);
        auto frame = co_await decoder.GetFrameAsync(0);
        auto width = frame.PixelWidth();
        auto height = frame.PixelHeight();
        auto pixelFormat = frame.BitmapPixelFormat();
        auto alphaMode = frame.BitmapAlphaMode();

        if (interpretIgnoreAsStraight && alphaMode == winrt::Windows::Graphics::Imaging::BitmapAlphaMode::Ignore)
        {
            alphaMode = winrt::Windows::Graphics::Imaging::BitmapAlphaMode::Straight;
        }

        // Get the pixels
        auto pixelData = co_await frame.GetPixelDataAsync();
        auto sourceBytes = pixelData.DetachPixelData();

        // If the pixel format and alpha mode matches our desired output,
        // don't bother with the conversion.
        winrt::com_ptr<ID3D11Texture2D> texture;
        if (pixelFormat == winrt::Windows::Graphics::Imaging::BitmapPixelFormat::Bgra8 &&
            alphaMode == winrt::Windows::Graphics::Imaging::BitmapAlphaMode::Premultiplied)
        {
            texture = robmikh::common::uwp::CreateTextureFromRawBytes(
                d3dDevice, 
                sourceBytes.data(), 
                width, 
                height, 
                DXGI_FORMAT_B8G8R8A8_UNORM, 
                renderTarget);
        }
        else
        {
            // Create a SoftwareBitmap
            auto buffer = winrt::make<robmikh::common::uwp::ComArrayBuffer>(std::move(sourceBytes));
            auto sourceBitmap = winrt::Windows::Graphics::Imaging::SoftwareBitmap::CreateCopyFromBuffer(
                buffer, pixelFormat, width, height, alphaMode);
        
            // Convert our SoftwareBitmap
            auto convertedBitmap = winrt::Windows::Graphics::Imaging::SoftwareBitmap::Convert(
                sourceBitmap, 
                winrt::Windows::Graphics::Imaging::BitmapPixelFormat::Bgra8, 
                winrt::Windows::Graphics::Imaging::BitmapAlphaMode::Premultiplied);

            auto bitmapBuffer = convertedBitmap.LockBuffer(winrt::Windows::Graphics::Imaging::BitmapBufferAccessMode::Read);
            auto reference = bitmapBuffer.CreateReference();
            auto byteAccess = reference.as<::Windows::Foundation::IMemoryBufferByteAccess>();
            uint8_t* bytes = nullptr;
            uint32_t size = 0;
            winrt::check_hresult(byteAccess->GetBuffer(&bytes, &size));

            texture =  robmikh::common::uwp::CreateTextureFromRawBytes(
                d3dDevice, 
                bytes, 
                width, 
                height, 
                DXGI_FORMAT_B8G8R8A8_UNORM, 
                renderTarget);
        }

        co_return texture;
    }

    inline winrt::Windows::Foundation::IAsyncAction SaveTextureToStreamAsync(
        winrt::com_ptr<ID3D11Texture2D> const& texture,
        winrt::Windows::Storage::Streams::IRandomAccessStream const& stream,
        winrt::guid const& encoderGuid)
    {
        D3D11_TEXTURE2D_DESC desc = {};
        texture->GetDesc(&desc);
        auto bytes = robmikh::common::uwp::CopyBytesFromTexture(texture);

        auto encoder = co_await winrt::Windows::Graphics::Imaging::BitmapEncoder::CreateAsync(
            encoderGuid, 
            stream);
        encoder.SetPixelData(
            winrt::Windows::Graphics::Imaging::BitmapPixelFormat::Bgra8,
            winrt::Windows::Graphics::Imaging::BitmapAlphaMode::Premultiplied,
            desc.Width,
            desc.Height,
            1.0,
            1.0,
            bytes);
        co_await encoder.FlushAsync();

        co_return;
    }

    inline winrt::Windows::Foundation::IAsyncAction SaveTextureToStreamAsync(
        winrt::com_ptr<ID3D11Texture2D> const& texture,
        winrt::Windows::Storage::Streams::IRandomAccessStream const& stream,
        BitmapEncoding const& encoding)
    {
        auto encoderGuid = impl::BitmapEncodingToEncoderGuid(encoding);
        co_await SaveTextureToStreamAsync(texture, stream, encoderGuid);
        co_return;
    }
}