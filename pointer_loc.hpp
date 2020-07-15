//
//  pointer_loc.hpp
//  job_no_1
//
//  Created by 王哲 on 2020/6/21.
//  Copyright © 2020 zhe. All rights reserved.
//

#ifndef pointer_loc_hpp
#define pointer_loc_hpp

#include "utils.h"
#include "preprocessing.hpp"
#include <stdio.h>

/*分割指针区域
 输入：表盘RGB图像
 输出：表盘指针区域掩膜二值图
 */
bool point_mask(cv::Mat input, cv::Mat& out_mask, bool postpre=true);
/*直线检测
 mode = 0: Hough 检测；
 mode = 1: LSD
 mode = 2: ELSD
 */
void lines_detect(cv::Mat input_3c, std::vector<cv::Vec4f>& line_data, int mode=0);
/*根据指针特点，做线段筛选*/
void lines_select(std::vector<cv::Vec4f>& line_data, std::vector<int>& param, std::vector<cv::Vec4f>& line_out);
/*logo检测*/
void logo_detect(cv::Mat input_3c, std::vector<cv::Vec4f>& circle_data, cv::Rect logo_roi, int mode=0);
/*指针定位*/
cv::Mat pointer_loc(cv::Mat input_3c, std::vector<cv::Point>& pointer);
/*速度值计算*/
float cal_speed(std::vector<cv::Point>& pointer);

#endif /* pointer_loc_hpp */
