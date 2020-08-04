//
//  processing_ed2.hpp
//  ed2
//
//  Created by 王哲 on 2020/8/2.
//  Copyright © 2020 zhe. All rights reserved.
//

#ifndef processing_ed2_hpp
#define processing_ed2_hpp

#include <stdio.h>
#include "utils_ed2.hpp"

/*指针分割预处理
 输入：分割后指针图像
 输出：处理后指针图像
 */
bool pointer_sel_preprocess(cv::Mat p_sel, cv::Mat& p_sel_out);

#endif /* processing_ed2_hpp */
