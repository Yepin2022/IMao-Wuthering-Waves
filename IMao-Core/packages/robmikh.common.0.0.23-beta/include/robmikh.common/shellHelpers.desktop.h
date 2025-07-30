#pragma once
#include <windows.h>
#include <winrt/base.h>
#include <wil/resource.h>
#include <shellapi.h>
#include <optional>
#include <functional>

namespace robmikh::common::desktop
{
    struct PopupMenu
    {
        PopupMenu()
        {
            m_menu.reset(winrt::check_pointer(CreatePopupMenu()));
            MENUINFO menuInfo = {};
            menuInfo.cbSize = sizeof(menuInfo);
            menuInfo.fMask = MIM_STYLE;
            menuInfo.dwStyle = MNS_NOTIFYBYPOS;
            winrt::check_bool(SetMenuInfo(m_menu.get(), &menuInfo));
        }

        void ShowMenu(HWND window, int x, int y)
        {
            winrt::check_bool(TrackPopupMenuEx(
                m_menu.get(),
                TPM_LEFTALIGN | TPM_BOTTOMALIGN,
                x,
                y,
                window,
                nullptr));
        }

        void AppendMenuItem(std::wstring const& name, std::function<void()> callback)
        {
            auto id = m_menuItems.size();
            m_menuItems.push_back({ name, callback });
            winrt::check_bool(AppendMenuW(m_menu.get(), MF_STRING, id, name.c_str()));
        }

        std::optional<LRESULT> MessageHandler(WPARAM const wparam, LPARAM const lparam)
        {
            auto menu = reinterpret_cast<HMENU>(lparam);
            auto index = static_cast<int>(wparam);
            if (menu == m_menu.get())
            {
                const auto& menuItem = m_menuItems[index];
                menuItem.Callback();
                return std::optional(0);
            }
            else
            {
                return std::nullopt;
            }
        }

    private:
        struct MenuItem
        {
            std::wstring Name;
            std::function<void()> Callback;
        };

        wil::unique_hmenu m_menu;
        std::vector<MenuItem> m_menuItems;
    };

    template<typename T>
    struct SyncPopupMenu
    {
        struct MenuItem
        {
            std::wstring Name;
            T Value;
        };

        SyncPopupMenu(std::vector<MenuItem> menuItems);
        std::optional<T> ShowMenu(HWND window, int x, int y);

    private:
        wil::unique_hmenu m_menu;
        std::vector<MenuItem> m_menuItems;
    };

    template<typename T>
    inline SyncPopupMenu<T>::SyncPopupMenu(std::vector<SyncPopupMenu<T>::MenuItem> menuItems)
    {
        m_menu.reset(winrt::check_pointer(CreatePopupMenu()));
        m_menuItems = menuItems;
        for (auto i = 0; i < m_menuItems.size(); i++)
        {
            const auto& item = m_menuItems[i];
            auto id = i + 1;
            winrt::check_bool(AppendMenuW(m_menu.get(), MF_STRING, id, item.Name.c_str()));
        }
    }

    template<typename T>
    inline std::optional<T> SyncPopupMenu<T>::ShowMenu(HWND window, int x, int y)
    {
        int result = TrackPopupMenuEx(
            m_menu.get(),
            TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD,
            x,
            y,
            window,
            nullptr);

        if (result > 0)
        {
            auto index = result - 1;
            const auto& item = m_menuItems[index];
            return std::optional(item.Value);
        }
        else
        {
            return std::nullopt;
        }
    }

    struct TrayIcon
    {
    public:
        TrayIcon(HWND window, HICON icon, uint32_t message, uint32_t id, std::wstring const& tip)
        {
            m_window = window;
            m_id = id;
            RegisterTrayIcon(window, icon, message, id, tip);
        }
        ~TrayIcon()
        {
            UnregisterRayIcon(m_id);
        }

    private:
        void RegisterTrayIcon(HWND window, HICON icon, uint32_t message, uint32_t id, std::wstring const& tip)
        {
            NOTIFYICONDATAW trayIconDesc = {};
            trayIconDesc.cbSize = sizeof(trayIconDesc);
            trayIconDesc.hWnd = window;
            trayIconDesc.uID = id;
            trayIconDesc.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP;
            trayIconDesc.uCallbackMessage = message;
            trayIconDesc.hIcon = icon;
            memcpy(trayIconDesc.szTip, tip.data(), tip.size() * sizeof(wchar_t));
            winrt::check_bool(Shell_NotifyIconW(NIM_ADD, &trayIconDesc));
            trayIconDesc.uVersion = NOTIFYICON_VERSION_4;
            winrt::check_bool(Shell_NotifyIconW(NIM_SETVERSION, &trayIconDesc));
        }

        void UnregisterRayIcon(uint32_t id)
        {
            NOTIFYICONDATAW trayIconDesc = {};
            trayIconDesc.cbSize = sizeof(trayIconDesc);
            trayIconDesc.hWnd = m_window;
            trayIconDesc.uID = id;
            winrt::check_bool(Shell_NotifyIconW(NIM_DELETE, &trayIconDesc));
        }

    private:
        HWND m_window = nullptr;
        uint32_t m_id = 0;
    };

    struct HotKey
    {
        HotKey(uint32_t modifiers, uint32_t key)
        {
            m_id = GetNextId();
            winrt::check_bool(RegisterHotKey(nullptr, m_id, modifiers, key));
        }

        ~HotKey()
        {
            UnregisterHotKey(nullptr, m_id);
        }

    private:
        static int32_t GetNextId()
        {
            static std::atomic<int32_t> hotKeyId = 0;
            return hotKeyId.fetch_add(1) + 1;
        }

    private:
        int32_t m_id = 0;
    };
}