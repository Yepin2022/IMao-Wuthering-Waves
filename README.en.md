# IMao-Wuthering-Waves

#### Introduction
An interactive overlay map that sits right on top of your game window. Using real-time image recognition, it keeps the player’s position and the map perfectly in sync, eliminating the need to alt-tab between the game and external map tools.

#### Function Demonstration
<details>
  <summary>Minimap Navigation</summary>
  <img src="https://github.com/user-attachments/assets/058fec38-70c2-4fb9-9be5-4594970c7dce"/>
</details>

<details>
  <summary>Interactive Large Map</summary>
  <img src="https://github.com/user-attachments/assets/22ba7107-3640-4fc3-9a25-f030ab5106ef"/>
</details>

#### Usage Instructions
1. Prepare the operating environment: [NET 8.0](https://dotnet.microsoft.com/en-us/download/dotnet/8.0) and [Microsoft Visual C++ 2015-2022 Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170).
2. Go to [Releases](https://github.com/Yepin2022/IMao-Wuthering-Waves/releases) to download the latest version. Extract the files and run IMao-WinUI.exe.
3. Open the game and adjust its resolution to 16:9.
4. After ensuring that the coordinates in the lower-left corner of the game are clearly displayed, click the start button. Wait for the information in the lower-right corner to disappear, which means the correct coordinates have been successfully recognized. Then, enable the required functions on the function page.

#### Project Dependencies (including but not limited to)
* [OpenCV](https://github.com/opencv/opencv)
* [PaddleOCR](https://github.com/PaddlePaddle/PaddleOCR)
* [Win32CaptureSample](https://github.com/robmikh/Win32CaptureSample)
* [ImGui](https://github.com/ocornut/imgui)
* [nlohmann/json](https://github.com/nlohmann/json)
* [CommunityToolkit/Windows](https://github.com/CommunityToolkit/Windows)
* [microsoft-ui-xaml](https://github.com/microsoft/microsoft-ui-xaml)

#### Development
[How to Compile the Project](Docs/Compile_en.md)