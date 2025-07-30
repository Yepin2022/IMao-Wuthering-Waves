#pragma once
#include <winrt/base.h>
#include <d2d1_3.h>
#include <d3d11_4.h>

namespace robmikh::common::uwp
{
    inline auto CreateD2DDevice(winrt::com_ptr<ID2D1Factory1> const& factory, winrt::com_ptr<ID3D11Device> const& device)
    {
        winrt::com_ptr<ID2D1Device> result;
        winrt::check_hresult(factory->CreateDevice(device.as<IDXGIDevice>().get(), result.put()));
        return result;
    }

    inline auto CreateD2DFactory(D2D1_DEBUG_LEVEL debugLevel = D2D1_DEBUG_LEVEL_NONE)
    {
        D2D1_FACTORY_OPTIONS options = {};
        options.debugLevel = debugLevel;

        winrt::com_ptr<ID2D1Factory1> factory;
        winrt::check_hresult(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, factory.put()));
        return factory;
    }
}