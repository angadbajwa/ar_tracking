#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

//testing iterator through histogram
void iterateHist(cv::Mat t_hist, const int histSize[]);