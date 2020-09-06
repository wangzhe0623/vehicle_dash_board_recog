//
//  pointer_process.cpp
//  ed2
//
//  Created by 王哲 on 2020/8/1.
//  Copyright © 2020 zhe. All rights reserved.
//

#include "pointer_process.hpp"

using namespace cv;
using namespace std;

bool point_mask(cv::Mat input, cv::Mat& out_mask, bool postpre)
{
    cv::Scalar thre_point(1.3, 1, 50);
    seg_mask(input, out_mask, thre_point, postpre);
    return true;
}

void lines_detect(cv::Mat input_3c, std::vector<cv::Vec4f>& line_data, int mode)
{
    line_data.clear();

    //use Hough transform method to find lines
    if(mode == 0)
    {
        HoughLinesP(input_3c, line_data, 1, CV_PI/180.0, 80, 10 ,20);
        //draw lines
        //    Scalar color = Scalar(255, 0, 0);
        //    for (size_t i = 0; i < line_data.size(); i++){
        //        Vec4f temp = line_data[i];
        //        line(src, Point(temp[0], temp[1]), Point(temp[2], temp[3]), color, 2);
        //    }
    }
    //use LSD method to find lines
    else if(mode == 1)
    {
        Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_ADV);
         
        // Detect the lines
        ls->detect(input_3c, line_data);
          
        // Show found lines
        ls->drawSegments(input_3c, line_data);
    }
    else
    {
        cout << "Unknown circle detection method! " << endl;
        return;
    }
}

void lines_select(std::vector<cv::Vec4f>& line_data, vector<int>& param, std::vector<cv::Vec4f>& line_out)
{
    line_out.clear();
    for(int li = 0; li < line_data.size(); li ++)
    {
       float length = 0;
       length = sqrt(pow((line_data[li][0] - line_data[li][2]), 2) + pow((line_data[li][1] - line_data[li][3]), 2));
       if(length > param[0] && length < param[1])
       {
           line_out.push_back(line_data[li]);
       }
    }
    return;
}

cv::Mat pointer_loc(cv::Mat input_3c, cv::Point ct, std::vector<cv::Point>& pointer)
{
    pointer.clear();
    cv::Size circle_size(input_3c.cols, input_3c.rows);
    Mat pointer_mask_out(circle_size, CV_8UC1, Scalar(0));
    Mat pointer_sel(circle_size, CV_8UC3, Scalar(0, 0, 0));
    Mat pointer_sel_processed(circle_size, CV_8UC1, Scalar(0));
    Mat input_gray, w_oPointer;
    std::vector<cv::Vec4f> lines, line_data;
    vector<cv::Point> mask_points;
    vector<int> temp;
    
    temp.push_back(0);
    temp.push_back(1000);
    cv::cvtColor(input_3c, input_gray, COLOR_BGR2GRAY);
    point_mask(input_3c, pointer_mask_out, true);
    
    mask2points(pointer_mask_out, mask_points);
    float max_dist = -1;
    int max_index = 0;
    for (int pi = 0; pi < mask_points.size(); pi ++)
    {
        float dist = pow(mask_points[pi].x - ct.x, 2) + pow(mask_points[pi].y - ct.y, 2);
        if (dist >= max_dist)
        {
            max_dist = dist;
            max_index = pi;
        }
    }
    
    // show pointer mask
//    cv::imshow("pointer_mask_out", pointer_mask_out);
//    cv::waitKey(0);
    
    lines_detect(pointer_mask_out, lines, 0);
    lines_select(lines, temp, line_data);
    
    //inverse mask ang cover pointer
    pointer_mask_out = 255 - pointer_mask_out;
    cv::bitwise_and(input_gray, pointer_mask_out, input_gray);
    
    
    for(int li = 0; li < line_data.size(); li ++)
    {
//        cv::line(input_3c, cv::Point(line_data[li][0], line_data[li][1]), cv::Point(line_data[li][2], line_data[li][3]), cv::Scalar(0, 255, 0));
        cv::circle(input_3c, cv::Point(line_data[li][0], line_data[li][1]), 1, cv::Scalar(0, 255, 255));
//        cv::circle(input_3c, cv::Point(line_data[li][2], line_data[li][3]), 1, cv::Scalar(0, 0, 255));

    }
    
//    std::vector<cv::Point> st_ps, ed_ps;
    cv::Point st_m, ed_m;
    float st_x = 0;
    float st_y = 0;
    float ed_x = 0;
    float ed_y = 0;
    for(int li = 0; li < line_data.size(); li ++)
    {
        st_x += line_data[li][0];
        st_y += line_data[li][1];
        ed_x += line_data[li][2];
        ed_y += line_data[li][3];
//        st_ps.push_back(cv::Point(line_data[li][0], line_data[li][1]));
//        ed_ps.push_back(cv::Point(line_data[li][2], line_data[li][3]));
    }
    st_m.x = st_x / line_data.size();
    st_m.y = st_y / line_data.size();
    ed_m.x = ed_x / line_data.size();
    ed_m.y = ed_y / line_data.size();
    

//    cv::line(input_3c, st_m, ed_m, cv::Scalar(0, 255, 255), 3);
//    pointer.push_back(st_m);
//    pointer.push_back(ed_m);
//    pointer.push_back(ct);
    if (mask_points.size())
    {
        cv::line(input_3c, mask_points[max_index], ct, cv::Scalar(0, 255, 0));
        pointer.push_back(mask_points[max_index]);
        pointer.push_back(ct);
    }
    else
    {
        pointer.push_back(ct);
        pointer.push_back(ct);
    }

    
    
//    cv::Moments st_m, ed_m;
//    st_m = cv::moments(st_ps);
//    ed_m = cv::moments(ed_ps);
//
//    circle(input_3c, cv::Point(st_m.m10 / st_m.m00, st_m.m01 / st_m.m00), 5, cv::Scalar(255, 255, 255), -1);
//    circle(input_3c, cv::Point(ed_m.m10 / ed_m.m00, ed_m.m01 / ed_m.m00), 5, cv::Scalar(0, 255, 255), -1);
//    cv::line(input_3c, cv::Point(st_m.m10 / st_m.m00, st_m.m01 / st_m.m00), cv::Point(ed_m.m10 / ed_m.m00, ed_m.m01 / ed_m.m00), cv::Scalar(255, 0, 0));

//    imshow("poiters", input_3c);
//    waitKey(0);
    return input_gray;
}

float cal_speed(std::vector<cv::Point>& pointer)
{
    float value;
    float speed = 0;
    if((pointer[1].x - pointer[0].x) > 0.000001)
    {
        value = (double)(pointer[1].y - pointer[0].y) / (double)(pointer[1].x - pointer[0].x); //求出LineA斜率
        value = atan(value) * 180. / 3.1415926;

//        float delta_y = pointer[1].y - pointer[0].y;
//        float delta_x = pointer[1].x - pointer[0].x;
//        value1 = atan2(delta_y, delta_x) * 180. / 3.1415926;

        std::cout << "value: " << value << std::endl;
        float a = 2. / 3.;
        float b = 30;
        float delta = -1. / 24 * value + 15. / 4.;
//        value += delta;
        if(value < -30)
        {
            speed = 0;
        }
        else
        {
//            speed = (28. / 39) * value + (90 - 840. / 13) ;
            speed = a * value + b ;

        }
    }
    else
    {
        speed = 90;
    }
    
    return speed;
}
