#pragma once
#include <Windows.h>
#include <opencv2/opencv.hpp>
class BitBltCapture
{
	public:
		BitBltCapture(HWND hwnd) : hwnd(hwnd) {}
		 bool GetSnapshot_PrintWindow(cv::Mat& snapshot);
	private:
		 HWND hwnd;
};

