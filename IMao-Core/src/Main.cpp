#include <iostream>
#include "DLL_API.h"
#include "ImguiDraw/Items/DrawItemBase.h"
#include"App/App.h"
#include "Coordinate/IdentifyWorldCoordinates/IdentifyWorldCoordinates.h"
#include "ImguiDraw/ImGuiOverWindows.h"
#include "WindowsCapture/BitBltCapture/BitBltCapture.h"
#include "ImguiDraw/InteractiveInterface/Notification.h"
using namespace std;

int main() {
    SetConsoleOutputCP(CP_UTF8);
    DrawItemBase::Init();
    DrawItemBase::AddItemDataFromJson("sx");
    DrawItemBase::AddItemDataFromJson("qzx_01");
    DrawItemBase::AddItemDataFromJson("qzx_02");
    DrawItemBase::AddItemDataFromJson("qzx_03");
    DrawItemBase::AddItemDataFromJson("T_IconC_046_UI");
    DrawItemBase::AddItemDataFromJson("SP_IconMonsterHead_326_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_SM_Gat_19A_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_049_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_SM_Gat_22A_UI");
    DrawItemBase::AddItemDataFromJson("SP_IconMonsterHead_331_UI");
    DrawItemBase::AddItemDataFromJson("sx_lgn");
    DrawItemBase::AddItemDataFromJson("cx_02");
    DrawItemBase::AddItemDataFromJson("cx_01");
    DrawItemBase::AddItemDataFromJson("cx_03");
    DrawItemBase::AddItemDataFromJson("SP_IconMonsterHead_315_UI");
    DrawItemBase::AddItemDataFromJson("SSP_IconMonsterHead_977_UI");
    DrawItemBase::AddItemDataFromJson("SP_IconMonsterHead_32030_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_029_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_030_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_031_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_032_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_035_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_036_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_037_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_038_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_039_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_040_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_041_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_042_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_043_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_044_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_045_UI");
    DrawItemBase::AddItemDataFromJson("T_IconC_046_UI");

    DrawItemBase::AddItemDataFromJson("T_IconC_054_UI");
    auto hwnd = GetWindowHandleByProcessName(L"Client-Win64-Shipping.exe");

    optional<BitBltCapture> bitBltCapture;
    optional<CaptureSnapshot> graphicsCapture;
    //graphicsCapture = CaptureSnapshot(hwnd);
    bitBltCapture = BitBltCapture(hwnd);

    App app(graphicsCapture, bitBltCapture, hwnd);

    ImGuiOverWindows imguioverwindows(hwnd, app);
    Notification::Start();
    Notification::AddInfo(NotificationDatas("The resource is loading, please be patient.", 15));
    App::SetUpdateMapDataCycleTime(60);
    App::SetUpdateMinMapDataCycleTime(100);
    App::SetEnabledMapShowItem(true);
    App::SetEnabledMinMapShowItem(true);

    app.StartTasks();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}