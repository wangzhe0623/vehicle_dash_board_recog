//
//  scale_process.hpp
//  ed2
//
//  Created by 王哲 on 2020/8/1.
//  Copyright © 2020 zhe. All rights reserved.
//

#ifndef scale_process_hpp
#define scale_process_hpp

#include <stdio.h>
#include "utils_ed2.hpp"

struct match_info
{
    bool status = false;
    int match_target;
};

struct line_info
{
    cv::Vec4f line_one;
    float k = 0.0;
    float b = 0.0;
    cv::Point ct = cv::Point(0, 0);
    match_info match_one;
};

std::vector<cv::Point> get_ell_base_lsd(cv::Mat input_3c, cv::RotatedRect& ell);
cv::Mat get_circle_based_ell(cv::RotatedRect ell, cv::Mat input_3c, cv::Point& ct);


#endif /* scale_process_hpp */
