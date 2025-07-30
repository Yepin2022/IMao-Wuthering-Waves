#include "Notification.h"
#include "../ImGuiOverWindows.h"

 std::vector<NotificationDatas> Notification::notifications;
 std::thread Notification::timerThread;
 bool Notification::timerStopFlag;

static int location = 3;
bool p_open;
ImGuiIO& io = ImGui::GetIO();
ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

void Notification::DrawInfo() {
    if (notifications.size()==0) {
        return;
    }
    
    if (location >= 0)
    {
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size = viewport->WorkSize;

        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    else if (location == -2)
    {
        // Center window
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        window_flags |= ImGuiWindowFlags_NoMove;
    }

    //ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    if (ImGui::Begin("Simple Notifications", &p_open, window_flags))
    {
        for (int i = 0; i < notifications.size(); i++) {
            if (notifications[i].timeDuration > 0) {
                ImGui::TextUnformatted(notifications[i].content.c_str());
                ImGui::SameLine();
                ImGui::Text("(%ds)", notifications[i].timeDuration);
                ImGui::Separator();
            }
            else {
                notifications.erase(notifications.begin() + i);
            }
        }
    }
    ImGui::End();
}

void Notification::AddInfo(NotificationDatas addNotificationDatas)
{
    for (auto& notification : notifications) {
        if (notification.content == addNotificationDatas.content) {
            notification.timeDuration = addNotificationDatas.timeDuration;
            return;
        }
    }
    notifications.push_back(addNotificationDatas);
}


void Notification::Timer() {
    while (!timerStopFlag) {
        try {
            for (auto& notification : notifications) {
                notification.timeDuration -= 1;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        catch (Exception ex) {
            std::cout << "Notification Timer error:" << ex.what()<<std::endl;
        }
    }
}

