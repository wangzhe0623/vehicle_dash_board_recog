//
//  processing_ed2.cpp
//  ed2
//
//  Created by 王哲 on 2020/8/2.
//  Copyright © 2020 zhe. All rights reserved.
//

#include "processing_ed2.hpp"

using namespace std;
using namespace cv;


bool pointer_sel_preprocess(cv::Mat p_sel, cv::Mat& p_sel_out)
{
    if(p_sel.empty())
        return false;
    
    cv::Mat gray(p_sel.size(), CV_8UC1, cv::Scalar(0));
//    cv::Mat p_norm(p_sel.size(), CV_8UC1, cv::Scalar(0));
    
    if (p_sel.channels() == 3)
        cv::cvtColor(p_sel, gray, COLOR_BGR2GRAY);
    else
    {
        gray = p_sel;
    }
    cv::GaussianBlur(gray, gray, cv::Size(3, 3), 1);
//    cv::normalize(gray, p_sel_out, 1, 255, NORM_MINMAX);
    cv::convertScaleAbs(gray, p_sel_out, 5, 0);
    
    return true;
}


