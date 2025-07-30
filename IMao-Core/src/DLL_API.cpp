#include "DLL_API.h"
#include <iostream>
#include"App/App.h"
#include "ImguiDraw/Items/DrawItemBase.h"
#include "Coordinate/IdentifyWorldCoordinates/IdentifyWorldCoordinates.h"
#include "ImguiDraw/ImGuiOverWindows.h"
#include "WindowsCapture/BitBltCapture/BitBltCapture.h"
#include "ImguiDraw/InteractiveInterface/Notification.h"
#include "ImguiDraw/Items/DrawItemOnGameMap.h"
#pragma comment(lib, "dwmapi.lib")
using namespace std;

HINSTANCE g_hDllInstance = NULL;
std::unique_ptr<App> app;

bool clickedStartButton = false;
bool clickedStopButton = true;
int CaptureWay = 0;
int minMapDataUpdateCycle = 100;
int mapDataUpdateCycle = 60;
bool enabledMapShowItem = false;
bool enabledMinMapShowItem = true;
HWND hwnd;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        g_hDllInstance = hModule;
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

void GetMyMessage(char* buffer, int bufferSize)
{
    const char* message = "Hello from C++ DLL!";
    if (buffer != nullptr && bufferSize > 0) {
        // 安全复制字符串，防止缓冲区溢出
        strncpy_s(buffer, bufferSize, message, _TRUNCATE);
    }
}

void SetMinMapDataUpdateCycle(int cycleTime){
    App::SetUpdateMinMapDataCycleTime(cycleTime);
    minMapDataUpdateCycle = cycleTime;
}

void SetMapDataUpdateCycle(int cycleTime){
    App::SetUpdateMapDataCycleTime(cycleTime);
    mapDataUpdateCycle = cycleTime;
}

void MainThread() {
	while (clickedStopButton) {
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}

	optional<BitBltCapture> bitBltCapture;
	optional<CaptureSnapshot> graphicsCapture;
	if (CaptureWay == 0) {
		bitBltCapture = BitBltCapture(hwnd);
	}
	else if (CaptureWay == 1) {
		graphicsCapture = CaptureSnapshot(hwnd);
	}

	Notification::Start();
    
	app = make_unique<App>(graphicsCapture, bitBltCapture, hwnd);
	ImGuiOverWindows imguioverwindows(hwnd, *app);

	App::SetUpdateMapDataCycleTime(mapDataUpdateCycle);
	App::SetUpdateMinMapDataCycleTime(minMapDataUpdateCycle);
	App::SetEnabledMapShowItem(enabledMapShowItem);
	App::SetEnabledMinMapShowItem(enabledMinMapShowItem);

	app->StartTasks();

	while (clickedStartButton) {
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}

    //TODO:内存泄漏未完全解决
    Notification::Stop();
	imguioverwindows.Stop();
	app->StopTasks();
    app.release();
    app.reset();
	MainThread();
}

void Initi()
{
    SetConsoleOutputCP(CP_UTF8);
    DrawItemBase::Init();
    std::thread mainThread(MainThread);
    mainThread.detach();
}

int Start() {
    hwnd = GetWindowHandleByProcessName(L"Client-Win64-Shipping.exe");
    if (hwnd && !app) {
        clickedStartButton = true;
        clickedStopButton = false;
        return 1;
    }
    return 0;
}

void Stop(){
    clickedStartButton = false;
    clickedStopButton = true;
}

void EnabledMinMapShowItem(bool setValue)
{
    App::SetEnabledMinMapShowItem(setValue);
    enabledMinMapShowItem = setValue;
}

void EnabledMapShowItem(bool setValue)
{
    App::SetEnabledMapShowItem(setValue);
    enabledMapShowItem = setValue;
}

void AddItem(const char* itemId)
{
    DrawItemBase::AddItemDataFromJson(String(itemId));
}

void ClearItem(const char* itemId)
{
    DrawItemBase::ClearItemData(String(itemId));
}

void SetCaptureWay(int setValue) {
    if (setValue != 0 and setValue != 1) {
        CaptureWay = 0;
    }
    else {
        CaptureWay = setValue;
    }
}

void SetVisibleSavedPoints(bool setValue) {
    DrawItemOnGameMap::SetVisibleSavedPoints(setValue);
}
