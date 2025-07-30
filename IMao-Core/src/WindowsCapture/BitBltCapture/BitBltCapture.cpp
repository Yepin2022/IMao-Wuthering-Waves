#include "BitBltCapture.h"
#include <iostream>
using namespace std;
using namespace cv;

bool BitBltCapture::GetSnapshot(Mat& snapshot) {
    if (!IsWindow(hwnd)) {
        cout << "无效的窗口句柄" << endl;
        return false;
    }

    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;

    HDC hdcScreen = GetDC(hwnd);
    if (!hdcScreen) {
        cout << "获取窗口DC失败" << std::endl;
        return false;
    }

    // 创建兼容DC和位图
    HDC hdcMemory = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);

    if (!hdcMemory || !hBitmap) {
        cout << "创建兼容DC或位图失败" << std::endl;
        if (hBitmap) DeleteObject(hBitmap);
        if (hdcMemory) DeleteDC(hdcMemory);
        ReleaseDC(hwnd, hdcScreen);
        return false;
    }

    // 选择位图到内存DC
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmap);
    bool success;
    BOOL result = PrintWindow(hwnd, hdcMemory, 3);
    if (result == 0) {
        //cout << "PrintWindow 操作失败，错误码:" << GetLastError() << endl;
        return false;
    }
    else {
        BITMAPINFOHEADER bi;
        bi.biSize = sizeof(BITMAPINFOHEADER);
        bi.biWidth = width;
        bi.biHeight = -height;  // 负值表示自上而下的DIB
        bi.biPlanes = 1;
        bi.biBitCount = 32;
        bi.biCompression = BI_RGB;
        bi.biSizeImage = 0;
        bi.biXPelsPerMeter = 0;
        bi.biYPelsPerMeter = 0;
        bi.biClrUsed = 0;
        bi.biClrImportant = 0;

        cv::Mat image(height, width, CV_8UC4);

        GetDIBits(hdcMemory, hBitmap, 0, height, image.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

        Mat bgrImage;
        cvtColor(image, bgrImage, cv::IMREAD_COLOR);
        bgrImage.copyTo(snapshot);
    }
    // 清理资源
    SelectObject(hdcMemory, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMemory);
    ReleaseDC(hwnd, hdcScreen);
    return true;
}

