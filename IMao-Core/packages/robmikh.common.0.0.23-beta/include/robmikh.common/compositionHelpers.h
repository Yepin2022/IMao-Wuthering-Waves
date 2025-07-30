#pragma once
#include "composition.interop.h"
#include <d2d1_3.h>

namespace robmikh::common::uwp
{
    struct SurfaceContext
    {
    public:
        SurfaceContext(std::nullptr_t) {}
        SurfaceContext(
            winrt::Windows::UI::Composition::CompositionDrawingSurface surface)
        {
            m_surface = surface;
            m_d2dContext = SurfaceBeginDraw(m_surface);
        }
        ~SurfaceContext()
        {
            SurfaceEndDraw(m_surface);
            m_d2dContext = nullptr;
            m_surface = nullptr;
        }

        winrt::com_ptr<ID2D1DeviceContext> GetDeviceContext() { return m_d2dContext; }

    private:
        winrt::com_ptr<ID2D1DeviceContext> m_d2dContext;
        winrt::Windows::UI::Composition::CompositionDrawingSurface m_surface{ nullptr };
    };
}