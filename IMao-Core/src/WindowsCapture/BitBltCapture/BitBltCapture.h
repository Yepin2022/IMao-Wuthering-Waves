#pragma once
#include <Windows.h>
#include <opencv2/opencv.hpp>
class BitBltCapture
{
	public:
		BitBltCapture(HWND hwnd) : hwnd(hwnd) {}
		 bool GetSnapshot(cv::Mat& snapshot);
	private:
		 HWND hwnd;
};

