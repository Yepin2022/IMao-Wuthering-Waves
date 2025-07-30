# Compile the Project with CMake
## Environment Preparation
Visual Studio 2022, Windows SDK 10.0.26000.0+, [OpenCV](https://github.com/opencv/opencv) (Due to patent protection, compile a version with SURF yourself), [Paddle C++ Inference CPU Library](https://www.paddlepaddle.org.cn/inference/master/guides/install/download_lib.html#windows)

## Get the Source Code
git clone https://github.com/Yepin2022/IMao-Wuthering-Waves.git

## Start Running
### Step 1: Build the Visual Studio Project
Open cmake-gui. Enter the source code path in the first input box and the compiled output path in the second input box.

### Step 2: Run CMake Configuration
Click the `Configure` button at the bottom of the interface. The first click will pop up a dialog box for Visual Studio configuration, as shown below. Select your Visual Studio project. Studio version, select x64 as the target platform. Then click the `Finish` button to automatically begin configuration.

You may receive an error the first time you run it, which is normal. Next, configure OpenCV and the prediction library.

- OPENCV_DIR: Enter the location of the OpenCV lib folder.
- OPENCV_DIR: Also enter the location of the OpenCV lib folder.
- PADDLE_LIB: Enter the location of the paddle_inference folder.
<div>
<img src="https://github.com/user-attachments/assets/4fea0c65-ed1c-4c80-87b1-8d02092b6c01"/>
</div>

After configuration is complete, click the `Configure` button again.

### Step 3: Generate the Visual Studio Project
Click the `Generate` button to generate the Visual Studio project's .sln file.
Click the `Open Project` button to open the project in Visual Studio.
Click Build -> Build Solution. You should see the executable file in the x64/Release/ folder under the source code directory.

## Notes
1. If nothing goes wrong, only the OpenCV DLL extension will be missing from the x64/Release/ directory. You will need to copy it there.
2. Some files in the x64/Release/Asset/ directory are too large. You will need to manually retrieve them from the Release package and copy them to the corresponding build directory.

## Others
This solution uses a mix of C# and C++. To debug the C++ 'IMao-Core' project, set it as the startup project and convert it to an exe file.