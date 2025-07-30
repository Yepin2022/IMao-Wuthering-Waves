#pragma once
#include <opencv2/opencv.hpp>
#include "pch.h"
#include <vector>
#include <thread>
#include <include/paddleocr.h>
#include <include/paddlestructure.h>
#include <opencv2/features2d.hpp> 
#include <Windows.h>
#include "Coordinate\CoordinateStruct.h"

struct NonClientRegion {
	int non_client_height_total;
	int non_client_width_total;
};

struct ScreenPoint {
    int x;
    int y;
};


struct WindowCorners {
    ScreenPoint topLeft;
    ScreenPoint topRight;
    ScreenPoint bottomLeft;
    ScreenPoint bottomRight;
};


struct WindowSize {
    int height;
    int width;
};

std::string UTF8ToGBK(const std::string& utf8);

HWND GetWindowHandleByTitle(const wchar_t* lpClassName, const wchar_t* windowTitle);

bool CalculateNonClientAreaSize(const HWND& hwnd, NonClientRegion& nonClientRegion);

bool CalculateWindowScalingFactors(const HWND& hwnd,double& HorizontalFactor,double& VerticaFactor);
bool CalculateWindowScalingFactors(const RECT& w_Rect, double& HorizontalFactor, double& VerticaFactor);
bool IsWindowFocused(HWND windowHwnd);

WindowCorners GetWindowClientCorners(HWND hwnd);

float CalculatePointDistance(Coordinate a, Coordinate b);

std::string GetCurrentPath();
std::string ReplaceSlashes(const std::string& input);
HWND GetWindowHandleByProcessName(const wchar_t* processName);
bool isKeyPressed(int keyCode);