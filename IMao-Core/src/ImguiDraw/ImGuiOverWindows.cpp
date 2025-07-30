#include "ImGuiOverWindows.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../Base/stb_image.h"
#include <iostream>
#include "../ImguiDraw/Items/DrawItemOnMinMap.h"
#include "../ImguiDraw/Items/DrawItemOnGameMap.h"
#include "../util.h"
#include "InteractiveInterface\Debug.h"
#include "InteractiveInterface/Notification.h"
#include "../DLL_API.h"

HWND ImGuiOverWindows::overWindowsHwnd;
// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool  ImGuiOverWindows::LoadTextureFromPath(const char* filePath, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height) {
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filePath, &image_width, &image_height, NULL, 4);
    if (!image_data)
        return false;

    // 创建DirectX纹理
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA subResource = {};
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;

    ID3D11Texture2D* pTexture = nullptr;
    if (FAILED(g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture)))
    {
        stbi_image_free(image_data);
        return false;
    }

    // 创建着色器资源视图
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    if (FAILED(g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv)))
    {
        pTexture->Release();
        stbi_image_free(image_data);
        return false;
    }

    // 清理资源
    pTexture->Release();
    stbi_image_free(image_data);

    *out_width = image_width;
    *out_height = image_height;
    return true;
}


bool ImGuiOverWindows::LoadTextureFromResource(const wchar_t* resourceName, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
    HRSRC hResource = FindResource(g_hDllInstance, resourceName, L"PNG");
    if (!hResource) return false;

    HGLOBAL hData = LoadResource(g_hDllInstance, hResource);
    if (!hData) return false;

    DWORD imageSize = SizeofResource(g_hDllInstance, hResource);
    const unsigned char* pData = static_cast<const unsigned char*>(LockResource(hData));

    // 使用内存缓冲区加载图像 
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load_from_memory(pData, imageSize, &image_width, &image_height, NULL, 4);
    if (!image_data)
    {
        UnlockResource(hData);
        return false;
    }

    // 创建纹理
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA subResource = {};
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;

    ID3D11Texture2D* pTexture = nullptr;
    if (FAILED(g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture)))
    {
        stbi_image_free(image_data);
        UnlockResource(hData);
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    if (FAILED(g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv)))
    {
        pTexture->Release();
        stbi_image_free(image_data);
        UnlockResource(hData);
        return false;
    }

    pTexture->Release();
    stbi_image_free(image_data);
    UnlockResource(hData);

    *out_width = image_width;
    *out_height = image_height;
    return true;
}

// 释放纹理资源的函数
void  ImGuiOverWindows::ReleaseTexture(ID3D11ShaderResourceView* texture)
 {
    if (texture) {
        texture->Release();
        texture = nullptr;
    }
}

// Main code
int ImGuiOverWindows::start()
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
     overWindowsHwnd = ::CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED, wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_POPUP, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(overWindowsHwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(overWindowsHwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(overWindowsHwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(overWindowsHwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    std::string FontsPath = GetCurrentPath() + "\\Assets\\Fonts\\msyh.ttc";
    ImFont* font = io.Fonts->AddFontFromFileTTF(FontsPath.c_str(), 15.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
    if (!font) {
       font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyh.ttc", 15.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
    }
    //IM_ASSERT(font != nullptr);

    //设置透明窗口
    ImVec4 clear_color = ImVec4(0, 0, 0, 0);
    SetLayeredWindowAttributes(overWindowsHwnd, ImColor(0, 0, 0, 0), 0, LWA_COLORKEY);

    //std::vector<ID3D11ShaderResourceView*> texturesToRelease; // 用于存储需要释放的纹理
    // Main loop
    while (!stopFlag)
    {
        RECT GameRect;
        GetClientRect(h_window, &GameRect);
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                stopFlag = true;
        }
        if (stopFlag)
            break;

        // Handle window being minimized or screen locked
        //if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        //{
        //    ::Sleep(10);
        //    continue;
        //}
        //g_SwapChainOccluded = false;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            DrawItemOnMinMap::DrawItemsOnMinMap(GameRect);
            DrawItemOnGameMap::DrawItemsOnGameMap(GameRect, h_window);
            Notification::DrawInfo();
           // Debug::DebugWindow(io,app);
            LONG exStyle = GetWindowLong(overWindowsHwnd, GWL_EXSTYLE);
            SetWindowLong(overWindowsHwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
        }

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present
        HRESULT hr = g_pSwapChain->Present(1, 0);   // Present with vsync
        //HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);

        //刷新窗口
        POINT ClientD2D = { GameRect.left,GameRect.top };
        ClientToScreen(h_window, &ClientD2D);
        MoveWindow(overWindowsHwnd, ClientD2D.x, ClientD2D.y, GameRect.right, GameRect.bottom, true);

        // 在渲染周期结束后释放纹理
       //for (auto texture : texturesToRelease) {
       //    ImGuiOverWindows::ReleaseTexture(texture);
       //}
       //texturesToRelease.clear();
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(overWindowsHwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{    
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}


