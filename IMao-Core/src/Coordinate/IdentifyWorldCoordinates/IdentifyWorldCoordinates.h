#pragma once
#include <include/paddleocr.h>
#include <include/args.h>
#include <memory> 
#include "../CoordinateStruct.h"
#include "../../ImageProcessing/ImageProcessing.h"
using namespace cv;
using namespace PaddleOCR;

class IdentifyWorldCoordinates
{
public:
	//IdentifyWorldCoordinates() = default;
	//IdentifyWorldCoordinates(std::string det_model_dir, std::string rec_model_dir,std::string cls_model_dir);
	static void Init(std::string det_model_dir, std::string rec_model_dir, std::string rec_char_dict_path,std::string cls_model_dir);
	static bool IdentifyCoordinate(const Mat& ImgCoordinates, Coordinate& outPlayerWorldCoordinate);
	static bool IdentifyCoordinateFromSnapshot(const Mat& snapshot, Coordinate& outPlayerWorldCoordinate, RECT rect);
	static bool IdentifyCoordinateFromSnapshot(const Mat& snapshot, Coordinate& outPlayerWorldCoordinate, HWND w_hwnd);
	static bool isLoaded;
private:
	static std::unique_ptr<PPOCR> p_ppocr;
};

