//
//  pointer_process.hpp
//  ed2
//
//  Created by 王哲 on 2020/8/1.
//  Copyright © 2020 zhe. All rights reserved.
//

#ifndef pointer_process_hpp
#define pointer_process_hpp

#include <stdio.h>
#include "utils_ed2.hpp"
#include "processing_ed2.hpp"

//分割指针区域
//输入：表盘RGB图像
//输出：表盘指针区域掩膜二值图
bool point_mask(cv::Mat input, cv::Mat& out_mask, bool postpre=true);
//直线检测
//mode = 0: Hough 检测；
//mode = 1: LSD
void lines_detect(cv::Mat input_3c, std::vector<cv::Vec4f>& line_data, int mode=0);
//根据指针特点，做线段筛选
void lines_select(std::vector<cv::Vec4f>& line_data, std::vector<int>& param, std::vector<cv::Vec4f>& line_out);
//指针定位
cv::Mat pointer_loc(cv::Mat input_3c, cv::Point ct, std::vector<cv::Point>& pointer);
//cal speed value
float cal_speed(std::vector<cv::Point>& pointer);

#endif /* pointer_process_hpp */
