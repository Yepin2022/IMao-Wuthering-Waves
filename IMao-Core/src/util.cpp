#include "util.h"
#include "Coordinate/CoordinateStruct.h"
#include <tlhelp32.h>
using namespace std;


//转换GBK编码
std::string UTF8ToGBK(const std::string& utf8) {
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    std::wstring wstr(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], len);

    len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string gbk(len, 0);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &gbk[0], len, nullptr, nullptr);

    return gbk;
}

HWND GetWindowHandleByTitle(const wchar_t* lpClassName,const wchar_t* windowTitle) {
    return FindWindowW(lpClassName, windowTitle);//UnrealWindow
}

bool CalculateNonClientAreaSize(const HWND& hwnd, NonClientRegion& nonClientRegion) {
    RECT windowRect, clientRect;
    if (GetClientRect(hwnd, &clientRect)) {
        HRESULT hr = DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &windowRect, sizeof(windowRect));
        int win_height = windowRect.bottom - windowRect.top;//整个窗口的区域 包含窗口标题栏
        int win_width = windowRect.right - windowRect.left;

        int cli_height = clientRect.bottom;//窗口客户区域
        int cli_width = clientRect.right;

        nonClientRegion.non_client_height_total = win_height - cli_height;
        nonClientRegion.non_client_width_total = win_width - cli_width;

        return true;
    }
     return false;
}

bool CalculateWindowScalingFactors(const HWND& hwnd, double& HorizontalFactor,double& VerticaFactor) {
    RECT w_Rect;
    if (!GetClientRect(hwnd, &w_Rect))
        return false;

    double w_width = w_Rect.right;
    double w_height = w_Rect.bottom;

    HorizontalFactor = w_width / GameWindowsScreenData::w_width;//水平缩放因子
    VerticaFactor = w_height / GameWindowsScreenData::w_height;//垂直缩放因子


    return true;
}

bool CalculateWindowScalingFactors(const RECT& w_Rect, double& HorizontalFactor, double& VerticaFactor) {

    double w_width = w_Rect.right;
    double w_height = w_Rect.bottom;

    HorizontalFactor = w_width / GameWindowsScreenData::w_width;//水平缩放因子
    VerticaFactor = w_height / GameWindowsScreenData::w_height;//垂直缩放因子

    return true;
}

// 判断指定窗口是否聚焦
bool IsWindowFocused(HWND windowHwnd) {
    HWND foregroundWindow = GetForegroundWindow();

    if (foregroundWindow != NULL) {
        int foregroundTitleLength = GetWindowTextLengthW(foregroundWindow);
        if (foregroundTitleLength > 0) {
            wchar_t* titleBuffer = new wchar_t[foregroundTitleLength + 1];
            GetWindowTextW(foregroundWindow, titleBuffer, foregroundTitleLength + 1);

            wchar_t windowTitle[256];
            GetWindowText(windowHwnd, windowTitle, 256);
            bool isMatch = wcscmp(titleBuffer, windowTitle) == 0;
            delete[] titleBuffer;

            return isMatch;
        }
    }
    return false;
}

//获取指定窗口4个边角的屏幕坐标
WindowCorners GetWindowClientCorners(HWND hwnd) {
    RECT clientRect;
    if (!GetClientRect(hwnd, &clientRect)) {
        std::cout << "无法获取客户区矩形" << std::endl;
        return {};
    }

    POINT topLeft = { clientRect.left, clientRect.top };
    POINT topRight = { clientRect.right, clientRect.top };
    POINT bottomLeft = { clientRect.left, clientRect.bottom };
    POINT bottomRight = { clientRect.right, clientRect.bottom };

    if (!ClientToScreen(hwnd, &topLeft) ||
        !ClientToScreen(hwnd, &topRight) ||
        !ClientToScreen(hwnd, &bottomLeft) ||
        !ClientToScreen(hwnd, &bottomRight)) {
        std::cout << "无法转换为屏幕坐标" << std::endl;
        return {};
    }

    return {
        { topLeft.x, topLeft.y },
        { topRight.x, topRight.y },
        { bottomLeft.x, bottomLeft.y },
        { bottomRight.x, bottomRight.y }
    };
}

float CalculatePointDistance(Coordinate a, Coordinate b) {
    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}


string GetCurrentPath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}


string ReplaceSlashes(const string& input) {
     string result = input;
    // 遍历字符串中的每个字符
    for (size_t i = 0; i < result.length(); ++i) {
        if (result[i] == '/') {
            result[i] = '\\';
        }
    }
    return result;
}

HWND GetWindowHandleByProcessName(const wchar_t* processName) {
    PROCESSENTRY32 processEntry = { 0 };
    processEntry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    DWORD processId = 0;
    if (snapshot != INVALID_HANDLE_VALUE && Process32First(snapshot, &processEntry)) {
        do {
            if (_wcsicmp(processEntry.szExeFile, processName) == 0) {
                processId = processEntry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &processEntry));
    }
    CloseHandle(snapshot);

    if (processId == 0) return NULL;

    struct WindowData {
        DWORD processId;
        HWND hwnd;
    } windowData = { processId, NULL };

    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        WindowData* data = reinterpret_cast<WindowData*>(lParam);
        DWORD windowProcessId = 0;

        GetWindowThreadProcessId(hwnd, &windowProcessId);
        if (windowProcessId == data->processId && IsWindowVisible(hwnd)) {
            data->hwnd = hwnd;
            return FALSE;
        }
        return TRUE; 
        }, reinterpret_cast<LPARAM>(&windowData));

    return windowData.hwnd;
}


bool isKeyPressed(int keyCode) {
    return (GetAsyncKeyState(keyCode) & 0x8000) != 0;
}
