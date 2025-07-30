#pragma once
#include <cstdint>
#include <Windows.h>
#include <CommCtrl.h>
#include <winrt/base.h>
#include <wil/resource.h>

namespace robmikh::common::desktop::controls
{
    namespace impl
    {
        inline HGDIOBJ SetBackgroundColorToWindowColor(HDC staticColorHdc)
        {
            auto color = static_cast<COLORREF>(GetSysColor(COLOR_WINDOW));
            SetBkColor(staticColorHdc, color);
            SetDCBrushColor(staticColorHdc, color);
            return GetStockObject(DC_BRUSH);
        }
    }

    enum class ControlType
    {
        Label,
        ComboBox,
        Button,
        CheckBox,
        Edit,
    };

    struct StackPanel
    {
        StackPanel(HWND parentWindow, HINSTANCE instance, uint32_t marginX, uint32_t marginY, uint32_t stepAmount, uint32_t width, uint32_t height)
        {
            m_parentWindow = parentWindow;
            m_instance = instance;
            m_unscaledMarginX = marginX;
            m_unscaledMarginY = marginY;
            m_unscaledStepAmount = stepAmount;
            m_unscaledWidth = width;
            m_unscaledHeight = height;

            CalculateForDpi(GetDpiForWindow(parentWindow));
        }

        StackPanel(HWND parentWindow, HINSTANCE instance, wil::shared_hfont font, uint32_t marginX, uint32_t marginY, uint32_t stepAmount, uint32_t width, uint32_t height)
        {
            m_parentWindow = parentWindow;
            m_instance = instance;
            m_font = font;
            m_unscaledMarginX = marginX;
            m_unscaledMarginY = marginY;
            m_unscaledStepAmount = stepAmount;
            m_unscaledWidth = width;
            m_unscaledHeight = height;

            CalculateForDpi(GetDpiForWindow(parentWindow));
        }

        HWND CreateControl(ControlType controlType, LPCTSTR windowName)
        {
            return CreateControl(controlType, windowName, 0);
        };

        HWND CreateControl(ControlType controlType, LPCTSTR windowName, DWORD style)
        {
            auto windowHandle = CreateControlWindow(GetControlClassName(controlType), windowName,
                style | GetControlStyle(controlType),
                GetOffsetY(controlType, m_currentOffsetY));
            if (m_font.get() != nullptr)
            {
                SendMessageW(windowHandle, WM_SETFONT, reinterpret_cast<WPARAM>(m_font.get()), true);
            }
            m_controls.push_back({windowHandle, controlType});
            return windowHandle;
        };

        void OnDpiChanged(wil::shared_hfont font)
        {
            CalculateForDpi(GetDpiForWindow(m_parentWindow));
            m_font = font;
            for (auto&& info : m_controls)
            {
                SendMessageW(info.WindowHandle, WM_SETFONT, reinterpret_cast<WPARAM>(font.get()), true);
                winrt::check_bool(SetWindowPos(
                    info.WindowHandle, 
                    nullptr, 
                    m_offsetX, 
                    GetOffsetY(info.Type, m_currentOffsetY),
                    m_width,
                    m_height,
                    SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER));
            }
        }

    private:
        struct ControlInfo
        {
            HWND WindowHandle;
            ControlType Type;
        };

        struct Stepper
        {
            uint32_t Value;
            uint32_t StepAmount;
            uint32_t Step()
            {
                return StepCustom(StepAmount);
            }
            uint32_t StepCustom(uint32_t stepAmount)
            {
                auto oldValue = Value;
                Value += stepAmount;
                return oldValue;
            }
        };

        void CalculateForDpi(uint32_t dpi)
        {
            m_dpi = dpi;
            m_offsetX = MulDiv(m_unscaledMarginX, m_dpi, 96);
            m_width = MulDiv(m_unscaledWidth, m_dpi, 96);
            m_height = MulDiv(m_unscaledHeight, m_dpi, 96);
            m_currentOffsetY = Stepper
            { 
                static_cast<uint32_t>(MulDiv(m_unscaledMarginY, m_dpi, 96)), 
                static_cast<uint32_t>(MulDiv(m_unscaledStepAmount, m_dpi, 96))
            };
        }

        HWND CreateControlWindow(LPCWSTR className, LPCTSTR windowName, DWORD style, uint32_t offsetY)
        {
            return winrt::check_pointer(CreateWindowExW(0,className, windowName,
                style | m_commonStyle,
                m_offsetX, offsetY, m_width, m_height, m_parentWindow, nullptr, m_instance, nullptr));
        };

        DWORD GetControlStyle(ControlType controlType)
        {
            switch (controlType)
            {
            case ControlType::Label:
                return 0;
            case ControlType::ComboBox:
                return WS_TABSTOP | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL;
            case ControlType::Button:
                return WS_TABSTOP | BS_DEFPUSHBUTTON;
            case ControlType::CheckBox:
                return WS_TABSTOP | BS_AUTOCHECKBOX;
            case ControlType::Edit:
                return WS_TABSTOP | WS_BORDER | ES_LEFT;
            default:
                return 0;
            }
        };

        LPCWSTR GetControlClassName(ControlType controlType)
        {
            switch (controlType)
            {
            case ControlType::Label:
                return WC_STATIC;
            case ControlType::ComboBox:
                return WC_COMBOBOX;
            case ControlType::Button:
                return WC_BUTTON;
            case ControlType::CheckBox:
                return WC_BUTTON;
            case ControlType::Edit:
                return L"EDIT";
            default:
                return 0;
            }
        };

        uint32_t GetOffsetY(ControlType controlType, Stepper& currentOffsetY)
        {
            switch (controlType)
            {
            case ControlType::Label:
                return currentOffsetY.StepCustom(MulDiv(20, m_dpi, 96));
            case ControlType::ComboBox:
                return currentOffsetY.Step();
            case ControlType::Button:
                return currentOffsetY.Step();
            case ControlType::CheckBox:
                return currentOffsetY.Step();
            case ControlType::Edit:
                return currentOffsetY.Step();
            default:
                return 0;
            }
        };

    private:
        HWND m_parentWindow = nullptr;
        HINSTANCE m_instance = nullptr;
        Stepper m_currentOffsetY;
        DWORD m_commonStyle = WS_CHILD | WS_OVERLAPPED | WS_VISIBLE;
        uint32_t m_offsetX = 10;
        uint32_t m_width = 200;
        uint32_t m_height = 30;

        uint32_t m_dpi;
        uint32_t m_unscaledMarginX;
        uint32_t m_unscaledMarginY;
        uint32_t m_unscaledStepAmount;
        uint32_t m_unscaledWidth;
        uint32_t m_unscaledHeight;

        std::vector<ControlInfo> m_controls;
        wil::shared_hfont m_font;
    };

    inline LRESULT StaticControlColorMessageHandler(WPARAM const wparam, LPARAM const)
    {
        return reinterpret_cast<LRESULT>(impl::SetBackgroundColorToWindowColor(reinterpret_cast<HDC>(wparam)));
    }

    inline wil::shared_hfont GetFontForDpi(uint32_t dpi)
    {
        NONCLIENTMETRICSW metrics = {};
        metrics.cbSize = sizeof(metrics);
        winrt::check_bool(SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0, dpi));
        return wil::shared_hfont(winrt::check_pointer(CreateFontIndirectW(&metrics.lfMessageFont)));
    }
}
