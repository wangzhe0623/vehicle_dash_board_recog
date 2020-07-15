//
//  preprocessing.cpp
//  job_no_1
//
//  Created by 王哲 on 2020/6/21.
//  Copyright © 2020 zhe. All rights reserved.
//

#include "preprocessing.hpp"
using namespace std;
using namespace cv;

/*
 dashboard ROI preprocess module
*/
bool dashboard_preprocess(cv::Mat input, cv::Mat& output)
{
    cv::Mat gray, gray_eh, gray_hole;
    cv::Mat test_img_binary(input.size(), CV_8UC1, Scalar(0));
    if(input.channels() == 3)
        cv::cvtColor(input, gray, COLOR_BGR2GRAY);
    else
    {
        gray = input;
    }
    double minVal;
    double maxVal;
    int minIdx[2] = {}, maxIdx[2] = {};    // minnimum Index, maximum Index
    cv::minMaxIdx(gray, &minVal, &maxVal, minIdx, maxIdx);
//    cout << "max: " << maxVal << endl;
//    cv::convertScaleAbs(gray, gray_eh, 3, 0);
//    equalizeHist(gray, gray_eh);

    gray_eh = contrastStretch2(gray);
//    cv::GaussianBlur(gray_eh, gray_eh, cv::Size(3, 3), 0);
    cv::medianBlur(gray_eh, gray_eh, 3);
    threshold(gray_eh, test_img_binary, 0, 255, cv::THRESH_OTSU);
    
    //cover bigger contour
    fillHole(test_img_binary, gray_hole);
    
    cv::Mat gray_con;
    gray_hole.copyTo(gray_con);
    vector<vector<cv::Point>> contours;
    vector<cv::Vec4i> hierarchy;
    cv::Mat imgcontours;
    cv::Point2f center;
    findContours(gray_con, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    
    vector<Moments> mu(contours.size());    //计算轮廓矩
     for (int i = 0; i < contours.size(); i++)
     {
      mu[i] = moments(contours[i], false);
     }
     vector<Point2f>  mc(contours.size());    //计算轮廓中心
     for (int i = 0; i < contours.size(); i++)
     {
      mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
     }

    cv::drawContours(gray, contours, -1, cv::Scalar(128));
    for(int i = 0; i < contours.size(); i ++)
    {
        cv::Moments mu = moments(contours[i]);
        cv::Point mc(mu.m10 / mu.m00, mu.m01 / mu.m00);
        double area = contourArea(contours[i]);
        double area_thre = 150;
//        if(mc.x < input.cols * 0.1 || mc.x >= input.cols * 0.9 || mc.y < input.rows * 0.1 || mc.y >= input.rows * 0.9 || area > area_thre)
        if((mc.x >= input.cols * 0.9 && mc.y >= input.rows * 0.9) || area > area_thre)
        {
            cv::drawContours(gray_hole, contours, i, cv::Scalar(0), -1);
            cv::putText(gray_eh, std::to_string(int(area)), mc, 1, 2, cv::Scalar(128));
        }
    }
    output = gray_hole;
//    cv::imshow("gray", gray);
//    cv::imshow("gray_eh", gray_eh);
//    cv::imshow("gray_hole", gray_hole);
//    cv::imshow("binay", test_img_binary);
//    cv::waitKey(0);
    
    return true;
}

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



