#pragma once
#include "opencv2/opencv.hpp"
#include "SimpleCapture.h"
#include "DirtyRegionVisualizer.h"
using namespace cv;

namespace winrt
{
    using namespace Windows;
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Numerics;
    using namespace Windows::Graphics;
    using namespace Windows::Graphics::Capture;
    using namespace Windows::Graphics::DirectX;
    using namespace Windows::Graphics::DirectX::Direct3D11;
    using namespace Windows::Storage;
    using namespace Windows::Storage::Pickers;
    using namespace Windows::System;
    using namespace Windows::UI;
    using namespace Windows::UI::Composition;
}

namespace util
{
    using namespace robmikh::common::uwp;
    using namespace robmikh::common::desktop;
}

class CaptureSnapshot
{
public:
    static wil::task<winrt::com_ptr<ID3D11Texture2D>> TakeAsync(
        winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const& device,
        winrt::Windows::Graphics::Capture::GraphicsCaptureItem const& item,
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat const& format = winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized);
    
    winrt::IAsyncOperation<winrt::StorageFile> TakeSnapshotAsync();

    CaptureSnapshot(HWND hwnd) {
        CaptureInit(hwnd);
    }

    Mat getCaptureResult() {
        return captureResult;
    }

    std::shared_ptr<SimpleCapture> Getcapture() {
        return m_capture;
    }

private:
    std::shared_ptr<SimpleCapture> m_capture;
    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice m_device;
    std::shared_ptr<DirtyRegionVisualizer> m_dirtyRegionVisualizer;
    winrt::Windows::Graphics::DirectX::DirectXPixelFormat m_pixelFormat = winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized;
    winrt::Windows::System::DispatcherQueue m_mainThread{ nullptr };
    winrt::Windows::UI::Composition::Compositor m_compositor{ nullptr };
    winrt::Windows::UI::Composition::ContainerVisual m_root{ nullptr };
    winrt::Windows::UI::Composition::SpriteVisual m_content{ nullptr };
    winrt::Windows::UI::Composition::CompositionSurfaceBrush m_brush{ nullptr };
    Mat captureResult;

    void StartCaptureFromItem(winrt::GraphicsCaptureItem item);
    winrt::GraphicsCaptureItem TryStartCaptureFromWindowHandle(HWND hwnd);
    void CaptureInit(HWND hwnd);
};