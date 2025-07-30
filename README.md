[简中](README.md) | [EN](README.en.md) 

### 介绍
一款叠加在游戏窗口上的交互地图，借助图像识别技术，实时同步玩家/地图位置，减少玩家在探索过程中来回切换游戏与地图工具的操作次数。

### 功能演示
<details>
  <summary>小地图导航</summary>
  <img src="https://github.com/user-attachments/assets/058fec38-70c2-4fb9-9be5-4594970c7dce"/>
</details>

<details>
  <summary>交互式大地图</summary>
  <img src="https://github.com/user-attachments/assets/22ba7107-3640-4fc3-9a25-f030ab5106ef"/>
</details>

### 使用方法
1.  运行环境准备 [NET 8.0](https://dotnet.microsoft.com/en-us/download/dotnet/8.0) 和 [Microsoft Visual C++ 2015-2022 Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170)
2.  前往[Releases](https://github.com/Yepin2022/IMao-Wuthering-Waves/releases)下载最新版 解压后运行IMao-WinUI.exe
3.  打开游戏，并将其分辨率调成16:9
4.  在确保游戏左下角坐标清晰显示后单击启动按钮，等待右下角信息消失后，即成功识别到正确的坐标后，在功能页开启需要的功能即可

### 项目依赖（包括但不限于）
* [OpenCV](https://github.com/opencv/opencv)
* [PaddleOCR](https://github.com/PaddlePaddle/PaddleOCR)
* [Win32CaptureSample](https://github.com/robmikh/Win32CaptureSample)
* [ImGui](https://github.com/ocornut/imgui)
* [nlohmann/json](https://github.com/nlohmann/json)
* [CommunityToolkit/Windows](https://github.com/CommunityToolkit/Windows)
* [microsoft-ui-xaml](https://github.com/microsoft/microsoft-ui-xaml)

### 开发
[如何编译项目](Docs/Compile_zh-Hans.md)
