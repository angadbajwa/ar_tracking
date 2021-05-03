#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

class ARTracker {
public:
	cv::Mat t_hist; //target histogram
	cv::Mat roi; //region of target interest
	cv::Mat mask; //masking region for histogram generation
	cv::Mat _f_frame; //first target frame
	bool finishedROI = false; //class ROI bool
	std::vector<cv::Point> vertices; // ROI verticies
	cv::Size imageResize;

	//AA image variables
	cv::Mat aux_img;

	//histogram intrinsics - 16 bins
	int channels[3] = { 0, 1, 2 };
	int dims = 3;
	int _histSize[3] = { 16, 16, 16 };
	float range[2] = { 0, 256 };
	const float* ranges[3] = { range, range, range };

	//default constructor
	ARTracker() {

	}

	//generate target histogram
	void generateTargetHistogram();
	void generateAuxiliaryImage(cv::Mat input_fr);

	//static and member mouse callback
	static void ROICallBack(int event, int x, int y, int flags, void* userdata);
	void ROICallBack(int event, int x, int y);

	//ROI retrieval function
	void retrieveROI(cv::Mat img);
};

cv::Mat resizeWithAspectRatio(const cv::Mat img, float& resizeScale);
