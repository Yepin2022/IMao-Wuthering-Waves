#include "..\..\pch.h"
#include "CaptureSnapshot.h"
using namespace std;

using namespace cv;
namespace winrt
{
    using namespace Windows;
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Metadata;
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
    using namespace robmikh::common::desktop;
    using namespace robmikh::common::uwp;
}

void CaptureSnapshot::StartCaptureFromItem(winrt::GraphicsCaptureItem item)
{
    m_capture = std::make_unique<SimpleCapture>(m_device, m_dirtyRegionVisualizer, item, m_pixelFormat);
    auto surface = m_capture->CreateSurface(m_compositor);
    m_brush.Surface(surface);
    m_capture->IsCursorEnabled(false);
    m_capture->IsBorderRequired(false);
    m_capture->StartCapture();
}

winrt::GraphicsCaptureItem CaptureSnapshot::TryStartCaptureFromWindowHandle(HWND hwnd)
{
    winrt::GraphicsCaptureItem item{ nullptr };
    try
    {
        item = util::CreateCaptureItemForWindow(hwnd);
        StartCaptureFromItem(item);
    }
    catch (winrt::hresult_error const& error)
    {
        MessageBoxW(NULL,
            error.message().c_str(),
            L"Win32CaptureSample",
            MB_OK | MB_ICONERROR);
    }
    return item;
}

void CaptureSnapshot::CaptureInit(HWND hwnd) {
    // Create the DispatcherQueue that the compositor needs to run
    auto controller = util::CreateDispatcherQueueControllerForCurrentThread();
    // Initialize Composition
    auto compositor = winrt::Compositor();
    auto root = compositor.CreateContainerVisual();
    root.RelativeSizeAdjustment({ 1.0f, 1.0f });
    root.Size({ -220.0f, 0.0f });
    root.Offset({ 220.0f, 0.0f, 0.0f });

    m_mainThread = winrt::DispatcherQueue::GetForCurrentThread();
    WINRT_VERIFY(m_mainThread != nullptr);

    m_compositor = root.Compositor();
    m_root = m_compositor.CreateContainerVisual();
    m_content = m_compositor.CreateSpriteVisual();
    m_brush = m_compositor.CreateSurfaceBrush();

    m_root.RelativeSizeAdjustment({ 1, 1 });
    root.Children().InsertAtTop(m_root);

    m_content.AnchorPoint({ 0.5f, 0.5f });
    m_content.RelativeOffsetAdjustment({ 0.5f, 0.5f, 0 });
    m_content.RelativeSizeAdjustment({ 1, 1 });
    m_content.Size({ -80, -80 });
    m_content.Brush(m_brush);
    m_brush.HorizontalAlignmentRatio(0.5f);
    m_brush.VerticalAlignmentRatio(0.5f);
    m_brush.Stretch(winrt::CompositionStretch::Uniform);
    auto shadow = m_compositor.CreateDropShadow();
    shadow.Mask(m_brush);
    m_content.Shadow(shadow);
    m_root.Children().InsertAtTop(m_content);

    auto d3dDevice = util::CreateD3D11Device();
    auto dxgiDevice = d3dDevice.as<IDXGIDevice>();
    m_device = CreateDirect3DDevice(dxgiDevice.get());
    // Don't bother with a D2D device if we can't use dirty regions
    if (winrt::ApiInformation::IsPropertyPresent(winrt::name_of<winrt::GraphicsCaptureSession>(), L"DirtyRegionMode"))
    {
        m_dirtyRegionVisualizer = std::make_shared<DirtyRegionVisualizer>(d3dDevice);
    }
    auto item = TryStartCaptureFromWindowHandle(hwnd);
}

wil::task<winrt::com_ptr<ID3D11Texture2D>>
CaptureSnapshot::TakeAsync(winrt::IDirect3DDevice const& device, winrt::GraphicsCaptureItem const& item, winrt::DirectXPixelFormat const& pixelFormat)
{
    // Grab the apartment context so we can return to it.
    winrt::apartment_context context;

    auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(device);
    winrt::com_ptr<ID3D11DeviceContext> d3dContext;
    d3dDevice->GetImmediateContext(d3dContext.put());

    // Creating our frame pool with CreateFreeThreaded means that we 
    // will be called back from the frame pool's internal worker thread
    // instead of the thread we are currently on. It also disables the
    // DispatcherQueue requirement.
    auto framePool = winrt::Direct3D11CaptureFramePool::CreateFreeThreaded(
        device,
        pixelFormat,
        1,
        item.Size());
    auto session = framePool.CreateCaptureSession(item);

    wil::shared_event captureEvent(wil::EventOptions::ManualReset);
    winrt::Direct3D11CaptureFrame frame{ nullptr };
    framePool.FrameArrived([&frame, captureEvent](auto& framePool, auto&)
    {
        frame = framePool.TryGetNextFrame();

        // Complete the operation
        captureEvent.SetEvent();
    });

    session.StartCapture();
    co_await winrt::resume_on_signal(captureEvent.get());
    co_await context;

    // End the capture
    session.Close();
    framePool.Close();

    auto texture = GetDXGIInterfaceFromObject<ID3D11Texture2D>(frame.Surface());
    auto result = util::CopyD3DTexture(d3dDevice, texture, true);

    co_return result;
}


winrt::IAsyncOperation<winrt::StorageFile> CaptureSnapshot::TakeSnapshotAsync() {
    // Use what we're currently capturing
    if (m_capture == nullptr)
    {
        co_return nullptr;
    }
    auto item = m_capture->CaptureItem();

    // Take the snapshot
    auto texture = co_await CaptureSnapshot::TakeAsync(m_device, item, winrt::DirectXPixelFormat::B8G8R8A8UIntNormalized);

    // Encode the image
    D3D11_TEXTURE2D_DESC desc = {};
    texture->GetDesc(&desc);
    auto bytes = util::CopyBytesFromTexture(texture);

    Mat image(desc.Height, desc.Width, CV_8UC4, bytes.data());

    Mat bgrImage;
    cvtColor(image, bgrImage, cv::IMREAD_COLOR);

    captureResult = bgrImage;
}