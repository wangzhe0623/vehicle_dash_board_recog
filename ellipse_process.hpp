//
//  ellipse_process.hpp
//  job_no_1
//
//  Created by 王哲 on 2020/6/21.
//  Copyright © 2020 zhe. All rights reserved.
//

#ifndef ellipse_process_hpp
#define ellipse_process_hpp

#include <stdio.h>
#include "utils.h"

/*抹去指针区域
 输入：表盘RGB图像
 输出：去除指针区域的表盘灰度图像
 */
cv::Mat rm_pointer_mask(cv::Mat input_3c);
/*通过两张对齐后的表盘图，获得表盘中心
 输入 path：图片所在文件夹路径
 输入 names：待处理的图片名
 输出：画有指针和交点的表盘图像
 */
cv::Mat get_center(std::string path, std::vector<std::string>& names);

#endif /* ellipse_process_hpp */
