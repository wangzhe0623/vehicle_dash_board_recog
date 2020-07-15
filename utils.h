//
//  utils.h
//  job_no_1
//
//  Created by 王哲 on 2020/6/21.
//  Copyright © 2020 zhe. All rights reserved.
//

#ifndef utils_h
#define utils_h

#include <EResult.h>
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
//elsd算法封装
void elsd_wz(cv::Mat input, std::vector<Ring>& out_ring, std::vector<Polygon>& out_ply);
//颜色及通道 分割
bool seg_mask(cv::Mat input, cv::Mat& out_mask, cv::Scalar thres, bool postpre=true);
//掩膜转点集
void mask2points(cv::Mat src_binary, std::vector<cv::Point>& dst);
//仿射变换
cv::Mat PerspectiveTrans(cv::Mat src, cv::Point2f* scrPoints, cv::Point2f* dstPoints);
cv::Mat AffineTrans(cv::Mat src, cv::Point2f* scrPoints, cv::Point2f* dstPoints);
//关键点转掩膜
void keypoints2mask(cv::Mat input_rgb, cv::Mat& mask_out, std::vector<cv::Point>& contour);
//对比度拉伸方法1
cv::Mat contrastStretch1(cv::Mat srcImage);
//对比度拉伸方法2
cv::Mat contrastStretch2(cv::Mat &srcImage);
//求表盘中心
cv::Mat get_center(std::string path, std::vector<std::string>& names);
//求两个直线交点
cv::Point2f getCrossPoint(std::vector<cv::Point>& pointerA, std::vector<cv::Point>& pointerB);

#endif /* utils_h */
