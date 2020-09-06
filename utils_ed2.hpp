//
//  utils_ed2.hpp
//  ed2
//
//  Created by 王哲 on 2020/8/1.
//  Copyright © 2020 zhe. All rights reserved.
//

#ifndef utils_ed2_hpp
#define utils_ed2_hpp

#include <stdio.h>

#include <opencv2/opencv.hpp>

#include <iostream>
#include <math.h>
#include <dirent.h>
#include <vector>


//路径处理
bool get_fileNames(char* path, std::vector<std::string> &names);
//填二值图孔洞
void fillHole(const cv::Mat srcimage, cv::Mat &dstimage);
//轮廓拟合椭圆
void contour2ellipse(cv::Mat input_gray, std::vector<cv::RotatedRect> ellipses);
//颜色及通道 分割
bool seg_mask(cv::Mat input, cv::Mat& out_mask, cv::Scalar thres, bool postpre=true);
//仿射变换
cv::Mat PerspectiveTrans(cv::Mat src, cv::Point2f* scrPoints, cv::Point2f* dstPoints);
cv::Mat AffineTrans(cv::Mat src, cv::Point2f* scrPoints, cv::Point2f* dstPoints);
//对比度拉伸方法2
cv::Mat contrastStretch2(cv::Mat &srcImage);
//求两个直线交点
cv::Point2f getCrossPoint(std::vector<cv::Point>& pointerA, std::vector<cv::Point>& pointerB);
//掩膜转点集
void mask2points(cv::Mat src_binary, std::vector<cv::Point>& dst);

#endif /* utils_ed2_hpp */
