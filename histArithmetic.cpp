#include "histArithmetic.h"
#include <iostream>

using namespace cv;

void iterateHist(Mat t_hist, const int histSize[]) {
    for (int i = 0; i < histSize[0]; i++) {
        for (int j = 0; j < histSize[1]; j++) {
            for (int k = 0; k < histSize[2]; k++) {
                std::cout << "Value(" << i << ", " << j << ", " << k << "): " << t_hist.at<float>(i, j, k) << "\n"; //matrix depth of 5, 1 channel (32F)
            }
        }
    }
    std::cout << std::flush;
}