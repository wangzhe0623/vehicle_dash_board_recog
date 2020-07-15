//
//  scale_loc.hpp
//  job_no_1
//
//  Created by 王哲 on 2020/6/21.
//  Copyright © 2020 zhe. All rights reserved.
//

#ifndef scale_loc_hpp
#define scale_loc_hpp

#include "utils.h"
#include <stdio.h>

/*刻度分割*/
bool scale_mask(cv::Mat input, cv::Mat& out_mask);
/*椭圆矫正*/
void ell2circle(cv::Mat input_3c, cv::Mat& ell_in, cv::Mat& circle_out);

#endif /* scale_loc_hpp */
