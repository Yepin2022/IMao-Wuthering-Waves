#pragma once
#include <windows.h>
#include <winrt/base.h>
#include <dxgi1_6.h>
#include <vector>
#include <string>

namespace robmikh::common::desktop
{
    namespace impl
    {
        inline std::map<HMONITOR, float> BuildDisplayHandleToMaxLuminanceMap()
        {
            std::map<HMONITOR, float> maxLuminances;

            winrt::com_ptr<IDXGIFactory1> factory;
            winrt::check_hresult(CreateDXGIFactory1(winrt::guid_of<IDXGIFactory1>(), factory.put_void()));

            UINT adapterCount = 0;
            winrt::com_ptr<IDXGIAdapter1> adapter;
            while (SUCCEEDED(factory->EnumAdapters1(adapterCount, adapter.put())))
            {
                UINT outputCount = 0;
                winrt::com_ptr<IDXGIOutput> output;
                while (SUCCEEDED(adapter->EnumOutputs(outputCount, output.put())))
                {
                    auto output6 = output.as<IDXGIOutput6>();
                    DXGI_OUTPUT_DESC1 desc = {};
                    winrt::check_hresult(output6->GetDesc1(&desc));
                    if (desc.AttachedToDesktop)
                    {
                        auto displayHandle = desc.Monitor;
                        auto maxLuminance = desc.MaxLuminance;
                        maxLuminances.insert({ displayHandle, maxLuminance });
                    }

                    output = nullptr;
                    outputCount++;
                }

                adapter = nullptr;
                adapterCount++;
            }

            return maxLuminances;
        }

        inline std::vector<DISPLAYCONFIG_PATH_INFO> GetDisplayConfigPathInfos()
        {
            uint32_t numPaths = 0;
            uint32_t numModes = 0;
            winrt::check_win32(GetDisplayConfigBufferSizes(
                QDC_ONLY_ACTIVE_PATHS,
                &numPaths,
                &numModes));
            std::vector<DISPLAYCONFIG_PATH_INFO> pathInfos(numPaths, DISPLAYCONFIG_PATH_INFO{});
            std::vector<DISPLAYCONFIG_MODE_INFO> modeInfos(numModes, DISPLAYCONFIG_MODE_INFO{});
            winrt::check_win32(QueryDisplayConfig(
                QDC_ONLY_ACTIVE_PATHS,
                &numPaths,
                pathInfos.data(),
                &numModes,
                modeInfos.data(),
                nullptr));
            pathInfos.resize(numPaths);
            return pathInfos;
        }

        struct DisplayHDRInfo
        {
            bool IsHDR = false;
            // Only valid if IsHDR is true
            float SDRWhiteLevelInNits = 0.0f;
        };

        inline std::map<std::wstring, DisplayHDRInfo> BuildDeviceNameToHDRInfoMap()
        {
            auto pathInfos = GetDisplayConfigPathInfos();
            std::map<std::wstring, DisplayHDRInfo> namesToHDRInfos;
            for (auto&& pathInfo : pathInfos)
            {
                // Get the device name.
                DISPLAYCONFIG_SOURCE_DEVICE_NAME deviceName = {};
                deviceName.header.size = sizeof(deviceName);
                deviceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
                deviceName.header.adapterId = pathInfo.sourceInfo.adapterId;
                deviceName.header.id = pathInfo.sourceInfo.id;
                winrt::check_win32(DisplayConfigGetDeviceInfo(&deviceName.header));
                std::wstring name(deviceName.viewGdiDeviceName);

                // Check to see if the display is in HDR mode.
                DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO colorInfo = {};
                colorInfo.header.size = sizeof(colorInfo);
                colorInfo.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_ADVANCED_COLOR_INFO;
                colorInfo.header.adapterId = pathInfo.targetInfo.adapterId;
                colorInfo.header.id = pathInfo.targetInfo.id;
                winrt::check_win32(DisplayConfigGetDeviceInfo(&colorInfo.header));
                bool isHDR = colorInfo.advancedColorEnabled && !colorInfo.wideColorEnforced;

                // Get the SDR white level.
                float sdrWhiteLevelInNits = 0.0f;
                if (isHDR)
                {
                    DISPLAYCONFIG_SDR_WHITE_LEVEL whiteLevel = {};
                    whiteLevel.header.size = sizeof(whiteLevel);
                    whiteLevel.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SDR_WHITE_LEVEL;
                    whiteLevel.header.adapterId = pathInfo.targetInfo.adapterId;
                    whiteLevel.header.id = pathInfo.targetInfo.id;
                    winrt::check_win32(DisplayConfigGetDeviceInfo(&whiteLevel.header));
                    sdrWhiteLevelInNits = static_cast<float>((whiteLevel.SDRWhiteLevel / 1000.0) * 80.0);
                }

                namesToHDRInfos.insert({ name, { isHDR, sdrWhiteLevelInNits } });
            }

            return namesToHDRInfos;
        }
    }

    struct DisplayInfo
    {
        static std::vector<DisplayInfo> GetAllDisplays()
        {
            // Get all the display handles
            std::vector<HMONITOR> displayHandles;
            EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR hmon, HDC, LPRECT, LPARAM lparam)
            {
                auto& displayHandles = *reinterpret_cast<std::vector<HMONITOR>*>(lparam);
                displayHandles.push_back(hmon);

                return TRUE;
            }, reinterpret_cast<LPARAM>(&displayHandles));

            // We need the max luminance of the display, which we get from DXGI
            auto maxLuminances = impl::BuildDisplayHandleToMaxLuminanceMap();

            // Build a mapping of device names to HDR information
            auto namesToHDRInfos = impl::BuildDeviceNameToHDRInfoMap();
            
            // Go through each display and find the matching hdr info
            std::vector<DisplayInfo> displays;
            for (auto&& displayHandle : displayHandles)
            {
                // Get the monitor rect and device name.
                MONITORINFOEXW monitorInfo = {};
                monitorInfo.cbSize = sizeof(monitorInfo);
                winrt::check_bool(GetMonitorInfoW(displayHandle, &monitorInfo));
                std::wstring name(monitorInfo.szDevice);

                // This sample assumes that we'll find the displays we're looking for.
                // You may want to assume a display isn't HDR if you can't find its information.
                auto hdrInfo = namesToHDRInfos[name];
                auto maxLuminance = maxLuminances[displayHandle];
                displays.push_back(DisplayInfo(displayHandle, monitorInfo.rcMonitor, name, hdrInfo.IsHDR, hdrInfo.SDRWhiteLevelInNits, maxLuminance));
            }

            return displays;
        }
        DisplayInfo(HMONITOR handle, RECT rect, std::wstring const& name, bool isHDR, float sdrWhiteLevelInNits, float maxLuminance)
        {
            m_handle = handle;
            m_rect = rect;
            m_displayName = name;
            m_isHDR = isHDR;
            m_sdrWhiteLevelInNits = sdrWhiteLevelInNits;
            m_maxLuminance = maxLuminance;
        }

        HMONITOR Handle() const { return m_handle; }
        RECT const& Rect() const { return m_rect; }
        std::wstring const& DisplayName() const { return m_displayName; }
        bool IsHDR() const { return m_isHDR; }
        // Only valid if IsHDR is true
        float SDRWhiteLevelInNits() const { return m_sdrWhiteLevelInNits; }
        float MaxLuminance() const { return m_maxLuminance; }

        bool operator==(const DisplayInfo& display) { return m_handle == display.m_handle; }
        bool operator!=(const DisplayInfo& display) { return !(*this == display); }

    private:
        HMONITOR m_handle = nullptr;
        RECT m_rect = {};
        bool m_isHDR = false;
        float m_sdrWhiteLevelInNits = 0.0f;
        float m_maxLuminance = 0.0f;
        std::wstring m_displayName;
    };
}