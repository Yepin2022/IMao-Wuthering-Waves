#pragma once
#include <winrt/base.h>
#include <wincodec.h>

namespace robmikh::common::uwp
{
    inline auto CreateWICFactory()
    {
        return winrt::create_instance<IWICImagingFactory2>(CLSID_WICImagingFactory2);
    }
}