//
//  main_ed2.cpp
//  ed2
//
//  Created by 王哲 on 2020/8/1.
//  Copyright © 2020 zhe. All rights reserved.
//

#include <stdio.h>

#include "utils_ed2.hpp"
#include "scale_process.hpp"
#include "pointer_process.hpp"
#include "processing_ed2.hpp"

using namespace cv;
using namespace std;


int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    //find pointers cross point
    /*
    char* p ="/Users/zhewang/Downloads/private/job/data_part3/joint";
    vector<string> names;
    string name_str = p;
    Mat circle_0(cv::Size(600, 500), CV_8UC3, cv::Scalar(0, 0, 0));
    get_fileNames(p, names);
    circle_0 = get_center(name_str, names);

    //show poiter lines and cross point
    imshow("circle_0", circle_0);
    waitKey(0);
    cv::destroyAllWindows();
    */
    
    
    //find scale; fit ellipse; get spped value
    std::string save_path ="out";
    std::string path ="imgs";  //normal
//    char* path ="/Users/zhewang/Downloads/private/job/data_part3/frames_lb_cut";  //light blur

    vector<string> image_names;
	vector<string> files;

    //get_fileNames(path, image_names);
	get_fileNames(path, files, image_names);

    string path_str = path;

    for(int i = 0; i < image_names.size(); i ++)
    {
        cout << "processing image: " << image_names[i] << endl;
        Mat test_img = imread(path_str + "/" + image_names[i]);
        if(!test_img.empty())
        {
//            cv::resize(test_img, test_img, cv::Size(350, 200));
            Mat circle_out;
            Mat img_gray, img_out;
            RotatedRect ell;
            Point ct(0, 0);
            vector<cv::Point> pointer;
            
            vector<Point> ell_points;
            ell_points = get_ell_base_lsd(test_img, ell);
            circle_out = get_circle_based_ell(ell, test_img, ct);
            for (int ei = 0; ei < ell_points.size(); ei ++)
            {
                circle(test_img, ell_points[ei], 2, Scalar(0, 255, 0), -1);
            }
            pointer_loc(circle_out, ct, pointer);
            float sp = cal_speed(pointer);
            std::cout << "speed: " << sp << std::endl;
            if (sp == 0.00)
            {
                cv::putText(circle_out, "<= 10", cv::Point(100, 100), 1, 1, cv::Scalar(0, 255,0));
            }
            else
            {
                cv::putText(circle_out, std::to_string(sp), cv::Point(100, 100), 1, 1, cv::Scalar(0, 255,0));
            }
            
//            imshow("test", test_img);
//            imshow("circle_out", circle_out);
//            waitKey(0);
            cv::imwrite(save_path + "/" + "2th_" + image_names[i], test_img);
            cv::imwrite(save_path + "/" + "out_" + image_names[i], circle_out);
        }
        else
        {
            std::cout << "invalid image!" << std::endl;
        }

    }
    
    return 0;

}
