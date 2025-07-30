#include "IdentifyWorldCoordinates.h"
#include "..\..\util.h"
#include "..\locationCalculator\ScreenCoordinate.h"
using namespace std;
std::unique_ptr<PPOCR> IdentifyWorldCoordinates::p_ppocr;
bool IdentifyWorldCoordinates::isLoaded = false;

void IdentifyWorldCoordinates::Init(string det_model_dir, string rec_model_dir,string rec_char_dict_path,string cls_model_dir) {
	FLAGS_det_model_dir = det_model_dir;
	FLAGS_rec_model_dir = rec_model_dir;
	if (!rec_char_dict_path.empty()) {
		FLAGS_rec_char_dict_path = rec_char_dict_path;
	}
	FLAGS_cls_model_dir = cls_model_dir;
	p_ppocr = make_unique<PPOCR>();
	isLoaded = true;
}

bool IsValidSingleOCRForCoords(const vector<OCRPredictResult>& ocr_result, string& outWorldCoordinateText) {

	if (ocr_result.size() != 1) {
		return false;
	}

	if (ocr_result[0].score <= 0.85) {
		return false;
	}

	string ocr_textResult = UTF8ToGBK(ocr_result[0].text);

	int i = 0;
	for (char& c : ocr_textResult) {
		if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
			return false;
		}

		if (c == '.' || c == ',' || c == ':') {
			c = '|';
			i++;
		}
	}

	if (i != 2) {
		return false;
	}

	outWorldCoordinateText = ocr_textResult;
	return true;
}


vector<int> SplitStringToIntArray(const string& input) {
	std::vector<int> result;
	std::stringstream ss(input);
	std::string token;

	while (std::getline(ss, token, '|')) {
		try {
			result.push_back(std::stoi(token));
		}
		catch (const std::invalid_argument& e) {
			std::cerr << "输入的数字格式无效: " << token << std::endl;
		}
		catch (const std::out_of_range& e) {
			std::cerr << "数字超出范围: " << token << std::endl;
		}
	}
	return result;
}


bool IdentifyWorldCoordinates::IdentifyCoordinate(const Mat& imgCoordinates,Coordinate& outPlayerWorldCoordinate) {;
	cout << "!!ocr识别!!" ;
	vector<OCRPredictResult> ocr_result = p_ppocr->ocr(imgCoordinates, true, true, true);
	string playerWorldCoordinate;
	if (IsValidSingleOCRForCoords(ocr_result, playerWorldCoordinate)) {
		vector<int> pWorldCoordinateArr= SplitStringToIntArray(playerWorldCoordinate);
		if (pWorldCoordinateArr.size() == 3) {
			Coordinate playerWorldCoordinate(pWorldCoordinateArr[0], pWorldCoordinateArr[1]);
			outPlayerWorldCoordinate = playerWorldCoordinate;
			return true;
		}
	}
	return false;
}

bool IdentifyWorldCoordinates::IdentifyCoordinateFromSnapshot(const Mat& snapshot, Coordinate& outPlayerWorldCoordinate, RECT rect) {
	Mat imgCoordinates = ImageProcessing::CropToShowWorldCoordinateAreaImg(snapshot, rect);
	if (imgCoordinates.empty())
		return false;

	imgCoordinates = ImageProcessing::centerAndScaleImage(imgCoordinates, 2);
	return IdentifyCoordinate(imgCoordinates, outPlayerWorldCoordinate);
}

bool IdentifyWorldCoordinates::IdentifyCoordinateFromSnapshot(const Mat& snapshot, Coordinate& outPlayerWorldCoordinate, HWND w_hwnd) {
	Mat imgCoordinates = ImageProcessing::CropToShowWorldCoordinateAreaImg(snapshot, w_hwnd);
	if (imgCoordinates.empty())
		return false;

	imgCoordinates = ImageProcessing::centerAndScaleImage(imgCoordinates, 2);

	return IdentifyCoordinate(imgCoordinates, outPlayerWorldCoordinate);
}
