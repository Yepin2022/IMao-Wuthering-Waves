#include "Debug.h"
bool show_demo_window = false;
float X, Y = 0;
float scaleFactor = 1.0f;
float multiple = 1.0f;

void Debug::DebugWindow(const ImGuiIO& io, App& app) {
   if(show_demo_window)
       ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::Begin("Debug");                          
    ImGui::Checkbox("Demo Window", &show_demo_window);

    Coordinate centerPointCoordinate = app.GetCenterPointCoordinate();
    if (ImGui::TreeNode("Coordinate")) {
          Coordinate playerCoordinate = app.GetlastPlayerCoordinate();
          ImGui::Text("PlayerMapCoordiante:(%.2f,%.2f)", playerCoordinate.x, playerCoordinate.y);
          ImGui::SameLine();
          ImGui::Text("MatchesSize:%d", app.GetGoodMatchSize_IconTask());

          ImGui::Text("GameMapCenterCoordinate:(%.2f,%.2f)", centerPointCoordinate.x, centerPointCoordinate.y);
          ImGui::SameLine();
          ImGui::Text("MatchesSize:%d", app.GetGoodMatchSize_IconWavePlateCrystal());

          ImGui::TreePop();
    }

    if (ImGui::TreeNode("MapSwipe")) {

         ImGui::DragFloat("ScaleFactor", &scaleFactor, 0.01f, 0.0f, 5.0f, "%.3f");
         app.SetScaleFactor(scaleFactor);
         
         ImGui::DragFloat("multiple", &multiple, 0.1f, 0.0f, 50.0f, "%.2f");
         app.SetInertiaStep(multiple);

         Coordinate mapCoordinatesOfMousePos = app.GetMapCoordinatesOfMousePos();
         ImGui::Text("Map coordinates of the mouse position:(%.1f,%.1f)", mapCoordinatesOfMousePos.x, mapCoordinatesOfMousePos.y);

         std::vector<cv::Point2f> sceneCorners = app.GetCaptrueCorners();
         if (sceneCorners.empty()) {
             ImGui::Text("Capture map content scope[W:%.1f H:%.1f]", 0, 0);
         }
         else {
            ImGui::Text("Capture map content scope[W:%.1f H:%.1f]", sceneCorners[2].x - sceneCorners[0].x, sceneCorners[2].y - sceneCorners[0].y);
         }


         Coordinate dragingCoordinates = app.GetMapCenterCoordinateByMouseMonitoring();
         ImGui::Text("Center map coordinates:(%.1f,%.1f)", dragingCoordinates.x, dragingCoordinates.y);

         ImGui::TreePop();
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    ImGui::End();
}