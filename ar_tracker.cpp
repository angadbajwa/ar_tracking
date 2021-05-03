#include "ar_tracker.h"
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <iostream>

using namespace cv;

void ARTracker::generateTargetHistogram() {
	//normalization constants
	float alpha = 1; float beta = 0;
	//generate and normalize 3D histogram from retrieved ROI
	calcHist(&_f_frame, 1, channels, mask, t_hist, dims, _histSize, ranges);
	normalize(t_hist, t_hist, alpha, beta, NORM_MINMAX);
}

void ARTracker::generateAuxiliaryImage(Mat input_fr) {
	aux_img = Mat::zeros(_f_frame.rows, _f_frame.cols, CV_8U);

	//resize input_image to specified size
	cv::resize(input_fr, input_fr, imageResize);

	//use a pixel buffer to iterate through pixel values and set to valid if present in histogram
	uint8_t* pixelPtr = (uint8_t*)input_fr.data;
	int cn = _f_frame.channels();
	Scalar_<uint8_t> bgrPixel;

	for (int i = 0; i < aux_img.rows; i++)
	{
		for (int j = 0; j < aux_img.cols; j++)
		{
			bgrPixel.val[0] = pixelPtr[i * _f_frame.cols * cn + j * cn + 0]; // B
			bgrPixel.val[1] = pixelPtr[i * _f_frame.cols * cn + j * cn + 1]; // G
			bgrPixel.val[2] = pixelPtr[i * _f_frame.cols * cn + j * cn + 2]; // R

			//std::cout << "B: " << (int)(bgrPixel.val[0] / 16) << " G: " << (int)(bgrPixel.val[1] / 16) << " R: " << (int)(bgrPixel.val[2] / 16) << std::endl;
			//check against histogram bin to see if p(x) > 0
			if (sqrtf(t_hist.at<float>((bgrPixel.val[0] / 16), (bgrPixel.val[1] / 16), (bgrPixel.val[2] / 16))) > 0) {
				// if present, set pixel to valid
				uint8_t& intensity = aux_img.at<uint8_t>(i, j);
				intensity = 255;
			}
		}
	}
}

void ARTracker::ROICallBack(int event, int x, int y, int flags, void* userdata) {
	//if returned class instance is a nullptr, exit function without returning anything
	if (userdata == nullptr) {
		return;
	}

	//cast class instance to pointer and then call mouseCallback function (can use static or reinterpret cast)
	ARTracker* arTracker = static_cast<ARTracker*>(userdata);
	arTracker->ROICallBack(event, x, y);
}

void ARTracker::ROICallBack(int event, int x, int y) {
	//if left button was clicked, add corner to ROI polygon 
	if (event == EVENT_RBUTTONDOWN) {
		std::cout << "Right mouse button clicked at (" << x << ", " << y << ")" << std::endl;
		if (vertices.size() < 2) {
			std::cout << "You need a minimum of three points!" << std::endl;
			return;
		}
		// Close polygon
		line(_f_frame, vertices[vertices.size() - 1], vertices[0], Scalar(0, 0, 0));

		// Mask is black with white where our ROI is - copyTo automatically copies non-zero members to output array
		mask = Mat::zeros(_f_frame.rows, _f_frame.cols, CV_8UC1);
		std::vector<std::vector<Point>> pts{ vertices };
		fillPoly(mask, pts, Scalar(255, 255, 255));
		_f_frame.copyTo(roi, mask);
		finishedROI = true;

		return;
	}
	if (event == EVENT_LBUTTONDOWN) {
		std::cout << "Left mouse button clicked at (" << x << ", " << y << ")" << std::endl;
		if (vertices.size() == 0) {
			// First click - just draw point
			_f_frame.at<Vec3b>(x, y) = Vec3b(255, 0, 0);
		}
		else {
			// Second, or later click, draw line to previous vertex
			line(_f_frame, Point(x, y), vertices[vertices.size() - 1], Scalar(0, 0, 0));
		}
		vertices.push_back(Point(x, y));
		return;
	}
}

Mat resizeWithAspectRatio(const Mat img, float &resizeScale) { //1050x1050
	//compute minimum resize scale
	float maxSize = 800;
	resizeScale = std::min(maxSize / img.rows, maxSize / img.cols);

	Mat resized;
	resize(img, resized, Size(resizeScale * img.cols, resizeScale * img.rows));
	return resized;
}

void ARTracker::retrieveROI(Mat img) {

	//use 1500x1050 as max screen dimensions, find scale ratio to scale vector outputs
	float resizeScale = 0;
	Mat resized = resizeWithAspectRatio(img, resizeScale);
	imageResize = Size(resizeScale * img.cols, resizeScale * img.rows); //set image resize
	resized.copyTo(_f_frame);

	//set namedWindow and mouseCallback
	namedWindow("ROIDisplay");

	cv::setMouseCallback("ROIDisplay", ROICallBack, this);

	//generate irregular ROI
	while (!finishedROI) {
		imshow("ROIDisplay", _f_frame);
		//highgui processing time
		waitKey(30);
	}
	_f_frame = resized;

	//after finishing ROI, generate target histogram
	generateTargetHistogram();
}