# 编译项目 CMake编译
## 环境准备
Visual Studio 2022 、Windows SDK 10.0.26000.0+、[OpenCV](https://github.com/opencv/opencv)（受专利保护问题，自行编译带SURF的版本）、[Paddle C++ Inference CPU Library](https://www.paddlepaddle.org.cn/inference/master/guides/install/download_lib.html#windows)

## 获取源码
git clone https://github.com/Yepin2022/IMao-Wuthering-Waves.git

## 开始运行
### Step1:构建Visual Studio项目
打开cmake-gui，在第一个输入框处填写源代码路径，第二个输入框处填写编译输出路径

### Step2:执行cmake配置
点击界面下方的`Configure`按钮，第一次点击会弹出提示框进行Visual Studio配置，如下图，选择你的Visual Studio版本即可，目标平台选择x64。然后点击`finish`按钮即开始自动执行配置。

第一次执行会报错，这是正常现象，接下来进行Opencv和预测库的配置

  - OPENCV_DIR：填写opencv lib文件夹所在位置
  - OpenCV_DIR：同填写opencv lib文件夹所在位置
  - PADDLE_LIB：paddle_inference文件夹所在位置
<div>  
  <img src="https://github.com/user-attachments/assets/4fea0c65-ed1c-4c80-87b1-8d02092b6c01"/>
</div>

配置完成后，再次点击`Configure`按钮

### Step3:生成Visual Studio 项目
点击`Generate`按钮即可生成Visual Studio 项目的sln文件。
点击`Open Project`按钮即可在Visual Studio 中打开项目。
点击`生成->生成解决方案`，即可在源码目录下的`x64/Release/`文件夹下看见可执行文件。

## 注意 
1. 不出意外的话，`x64/Release/`目录下仅缺OpenCV dll扩展，需要将其拷贝到此目录下。
2. `x64/Release/Asset/` 目录下的部分文件过大，需要手动从 Release 包中获取并拷贝至对应的编译目录下。

## 其他
此方案为C# 和 C++ 混合，若要调试C++ 'IMao-Core'，将其设为启动项目并改为exe程序。
