//
//  ellipse_process.cpp
//  job_no_1
//
//  Created by 王哲 on 2020/6/21.
//  Copyright © 2020 zhe. All rights reserved.
//

#include "ellipse_process.hpp"
#include "pointer_loc.hpp"
#include "scale_loc.hpp"

using namespace cv;
using namespace std;

cv::Mat rm_pointer_mask(cv::Mat input_3c)
{
    cv::Size circle_size(input_3c.cols, input_3c.rows);
    Mat pointer_mask_out(circle_size, CV_8UC1, Scalar(0));
    Mat pointer_sel(circle_size, CV_8UC3, Scalar(0, 0, 0));
    Mat pointer_sel_processed(circle_size, CV_8UC1, Scalar(0));
    Mat input_gray;
    
    cv::cvtColor(input_3c, input_gray, COLOR_BGR2GRAY);
    point_mask(input_3c, pointer_mask_out, false);
    cv::bitwise_and(input_gray, pointer_mask_out, pointer_sel);
    pointer_sel_preprocess(pointer_sel, pointer_sel_processed);
    
    //inverse mask ang cover pointer
    pointer_mask_out = 255 - pointer_mask_out;
    cv::bitwise_and(input_gray, pointer_mask_out, input_gray);
    
    return input_gray;
}

cv::Mat get_center(std::string path, std::vector<std::string>& names)
{
    Mat circle_0(cv::Size(600, 500), CV_8UC3, cv::Scalar(0, 0, 0));
    std::vector<std::vector<cv::Point>> lines;
    cv::Point2f ct_point;
    bool draw_ = false;
    for(int i = 0; i < names.size(); i ++)
    {
        Mat test_img = imread(path + "/" + names[i]);
        if(!test_img.empty())
        {
            cv::resize(test_img, test_img, cv::Size(350, 200));
            Mat circle_out;
            Mat img_gray, img_out;
            std::vector<cv::Point> pointer;
            float value = 0;
            img_gray = rm_pointer_mask(test_img);
            dashboard_preprocess(img_gray, img_out);
            ell2circle(test_img, img_out, circle_out);
            pointer_loc(circle_out, pointer);
            if(!draw_)
            {
                circle_0 = circle_out;
                draw_ = true;
            }
            else
            {
                cv::addWeighted(circle_0, 0.5, circle_out, 0.5, 0, circle_0);
            }
            cv::line(circle_0, pointer[0], pointer[1], cv::Scalar(0, 255, 0));
            lines.push_back(pointer);
        }
    }
    ct_point = getCrossPoint(lines[0], lines[1]);
    cv::circle(circle_0, ct_point, 3, cv::Scalar(0, 0, 255));
    return circle_0;
}
