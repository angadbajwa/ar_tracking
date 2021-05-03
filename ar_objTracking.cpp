// ar_objTracking.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "ar_tracker.h"
#include "histArithmetic.h"
#include <iostream>
#include <stdio.h>

using namespace cv;

int main()
{
    cv::Mat inputImg;
    inputImg = cv::imread
        ("C:/Users/anbaj/OneDrive/Documents/Waterloo/2A/MTE 201/Discovery Exercise #2/coinLegoImages/coinLegoImages/lego width.jpg");
    ARTracker mainTracker;

    mainTracker.retrieveROI(inputImg);

    mainTracker.generateAuxiliaryImage(mainTracker._f_frame);

    imshow("testWindow1", mainTracker.aux_img);

    imshow("testWindow", mainTracker._f_frame);

    waitKey(0);
}
