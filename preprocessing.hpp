//
//  preprocessing.hpp
//  job_no_1
//
//  Created by 王哲 on 2020/6/21.
//  Copyright © 2020 zhe. All rights reserved.
//

#ifndef preprocessing_hpp
#define preprocessing_hpp

#include "utils.h"
#include <stdio.h>

/*表盘画预处理
 输入：表盘灰度图；
 输出：处理后表盘灰度图
 */
bool dashboard_preprocess(cv::Mat input, cv::Mat& output);
/*指针分割预处理
 输入：分割后指针图像
 输出：处理后指针图像
 */
bool pointer_sel_preprocess(cv::Mat p_sel, cv::Mat& p_sel_out);

#endif /* preprocessing_hpp */
