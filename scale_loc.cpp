//
//  scale_loc.cpp
//  job_no_1
//
//  Created by 王哲 on 2020/6/21.
//  Copyright © 2020 zhe. All rights reserved.
//

#include "scale_loc.hpp"

using namespace cv;
using namespace std;

bool scale_mask(cv::Mat input, cv::Mat& out_mask)
{
    //H S V
    cv::Scalar low(0, 0, 140);
    cv::Scalar up(180, 30, 255);

    cv::Scalar low_1(30, 40, 50);
    cv::Scalar up_1(61,66, 85);
    
    /* compare 3 channels thres(c21, c10, c2)*/
    
    if(input.empty())
    {
        cout << "Input Error!" << endl;
        return false;
    }
    
    Mat mask(input.rows, input.cols, CV_8UC1, Scalar(0));
    for(int i = 0; i < input.rows; i++)
        {
            for(int j = 0; j < input.cols; j++)
            {
//                float c21 = input.at<Vec3b>(i,j)[2] / (input.at<Vec3b>(i,j)[1] + 0.01);
//                float c10 = input.at<Vec3b>(i,j)[1] / (input.at<Vec3b>(i,j)[0] + 0.01);
                float h = input.at<Vec3b>(i,j)[0];
                float s = input.at<Vec3b>(i,j)[1];
                float v = input.at<Vec3b>(i,j)[2];
                if(h >= low[0] && h <= up[0] && s >= low[1] && s <= up[1] && v >= low[2] && v <= up[2])
                {
                    mask.at<uchar>(i,j) = 255;
                }
                if(h >= low_1[0] && h <= up_1[0] && s >= low_1[1] && s <= up_1[1] && v >= low_1[2] && v <= up_1[2])
                {
                    mask.at<uchar>(i,j) = 255;
                }
                
            }
        }
    
    // dilate mask
    int z = 2 * 2 + 1;
    int x = 1 * 2 + 1;

    Mat structureElement_d = getStructuringElement(MORPH_ELLIPSE, Size(z, z), cv::Point(-1, -1));
    Mat structureElement_e = getStructuringElement(MORPH_ELLIPSE, Size(x, x), cv::Point(-1, -1));

    dilate(mask, mask, structureElement_d, cv::Point(-1, -1), 2);

    //fill holes in mask
    fillHole(mask, mask);
    erode(mask, out_mask, structureElement_e, cv::Point(-1, -1), 2);
    return true;
}

void get_scale_ellipse(cv::Mat input_hsv, cv::RotatedRect& out_ell)
{
    cv::Mat mask_out(input_hsv.size(), CV_8UC1, cv::Scalar(0));
    std::vector<cv::Point> contour_out;
//    scale_mask(input_hsv, mask_out);
//    keypoints2mask(input_hsv, mask_out, contour_out);
    mask_out = input_hsv;
    mask2points(mask_out, contour_out);
//    cv::imshow("points", mask_out);
//    cv::waitKey(0);
//    vector<vector<cv::Point>> cons;
//    findContours(mask_out, cons, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    if(contour_out.size() > 0)
    {
        out_ell = fitEllipse(contour_out);
    }
    else
    {
        cout << "No Ellipse detected!" << endl;
    }
}

void ell2circle(cv::Mat input_3c, cv::Mat& ell_in, cv::Mat& circle_out)
{
    cv::RotatedRect ell;

    get_scale_ellipse(ell_in, ell);
    
//    ell.center.x -= 5;
//    ell.center.y -= 5;
//    ell.angle -= 2;
//    ell.size.width += 10;
    
    cv::Point2f* vertices = new cv::Point2f[4];
    cv::Point2f* dst_vertices = new cv::Point2f[4];
    
    cv::Point2f* src_hand = new cv::Point2f[4];
//    src_hand[0] = cv::Point2f(130, 106);
//    src_hand[1] = cv::Point2f(339, 70);
//    src_hand[2] = cv::Point2f(358, 176);
//    src_hand[3] = cv::Point2f(149, 211);
    
    src_hand[0] = cv::Point2f(49, 76);
    src_hand[1] = cv::Point2f(322, 24);
    src_hand[2] = cv::Point2f(332, 133);
    src_hand[3] = cv::Point2f(73, 178);
    
    ell.points(vertices);
    int shift = 100;
//    cout << ell.size << endl;
    dst_vertices[0] = cv::Point2f(shift, shift);
    dst_vertices[1] = cv::Point2f(shift + 300, shift);
    dst_vertices[2] = cv::Point2f(shift + 300, shift + 160);
    dst_vertices[3] = cv::Point2f(shift, shift + 160);


//    cv::cvtColor(ell_in, ell_in, cv::COLOR_HSV2BGR);

//    cv::ellipse(input_3c, ell, cv::Scalar(255, 0, 255), 1);
//    cv::circle(input_3c, cv::Point(int((49+332)/2), int((76+133)/2)), 3, cv::Scalar(0, 255, 0));

    circle_out = PerspectiveTrans(input_3c, src_hand, dst_vertices);
//    cv::circle(circle_out, cv::Point(shift + 115, shift + 100), 3, cv::Scalar(0, 255, 0));

//    circle_out = AffineTrans(ell_in, vertices, dst_vertices)

//    for (size_t i = 0; i < 4; i++)
//    {
//        cv::line(ell_in, vertices[i], vertices[(i + 1) % 4], cv::Scalar(255, 255, 255), 4, 8, 0);
//        cv::circle(ell_in, vertices[i], 3, cv::Scalar(0, 255, 255), -1);
//        cv::putText(ell_in, std::to_string(i), vertices[i], 1, 1, cv::Scalar(128, 128, 0));
//    }
//    cv::imshow("ell_int", ell_in);
//    cv::imshow("circle", circle_out);
//    cv::waitKey(0);
    return;
}

