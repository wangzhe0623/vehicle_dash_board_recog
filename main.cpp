//
//  main.cpp
//  job_no_1
//
//  Created by 王哲 on 2020/6/21.
//  Copyright © 2020 zhe. All rights reserved.
//

#include "preprocessing.hpp"
#include "ellipse_process.hpp"
#include "pointer_loc.hpp"
#include "scale_loc.hpp"
#include "utils.h"

using namespace cv;
using namespace std;


int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    //find pointers cross point
    std::string save_path ="/Users/zhewang/Downloads/private/job/data_part3/re";
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
    
    
    //find scale; fit ellipse; get spped value
//    char* path ="/Users/zhewang/Downloads/private/job/data_part3/frames_cut_cut";  //normal
    char* path ="/Users/zhewang/Downloads/private/job/data_part3/frames_lb_cut";  //light blur

    vector<string> image_names;
    get_fileNames(path, image_names);
    string path_str = path;

    for(int i = 0; i < image_names.size(); i ++)
    {
        cout << "processing image: " << image_names[i] << endl;
        Mat test_img = imread(path_str + "/" + image_names[i]);
        if(!test_img.empty())
        {
            cv::resize(test_img, test_img, cv::Size(350, 200));
            Mat circle_out;
            Mat img_gray, img_out;
            std::vector<cv::Point> pointer;
            float value = 0;

            img_gray = rm_pointer_mask(test_img);
            dashboard_preprocess(img_gray, img_out);
            ell2circle(test_img, img_out, circle_out);
            pointer_loc(circle_out, pointer);
            float sp = cal_speed(pointer);
            std::cout << "speed: " << sp << std::endl;
            cv::putText(circle_out, std::to_string(sp), cv::Point(200, 200), 1, 2, cv::Scalar(0, 255,0));
        
//            cvtColor(test_img, test_img, COLOR_BGR2HSV);
//            ell2circle(test_img, circle_out);
//            pointer_loc(circle_out, pointer);
            
//            std::vector<cv::Scalar> colors;
//            colors.push_back(cv::Scalar(255, 0, 0));
//            colors.push_back(cv::Scalar(0, 255, 0));
//            colors.push_back(cv::Scalar(0, 0, 255));
//            colors.push_back(cv::Scalar(255, 255, 0));
//            colors.push_back(cv::Scalar(255, 0, 255));
//            colors.push_back(cv::Scalar(0, 255, 255));
//
//            std::vector<Ring> out_ring;
//            std::vector<Polygon> out_ply;
//            elsd_wz(test_img, out_ring, out_ply);
//            for(int pi = 0; pi < out_ply.size(); pi ++)
//            {
//                if(out_ply[pi].dim == 2)
//                {
////                    cout << "dim: " << std::to_string(out_ply[pi].dim) << endl;
//                    float length = sqrt(pow((out_ply[pi].pts[0].x - out_ply[pi].pts[1].x), 2) + pow((out_ply[pi].pts[0].y - out_ply[pi].pts[1].y), 2));
//                    cout << "lenght: " << std::to_string(length);
//                    if(length >= 10 && length <= 20)
//                    {
//                        for(int i = 0; i < out_ply[pi].dim; i += 1)
//                         {
//                             if(i % 2 == 0)
//                             {
//                                 cv::circle(test_img, cv::Point(out_ply[pi].pts[i].x, out_ply[pi].pts[i].y), 3, colors[pi % 6]);
//                             }
//                             else
//                             {
//                                 cv::rectangle(test_img, cv::Point(out_ply[pi].pts[i].x - 3, out_ply[pi].pts[i].y - 3), cv::Point(out_ply[pi].pts[i].x + 3,out_ply[pi].pts[i].y + 3), colors[pi % 6], 1);
//                             }
//                         }
////                        cv::putText(test_img, std::to_string(length), cv::Point(out_ply[pi].pts[0].x, out_ply[pi].pts[0].y), 1, 1, colors[pi % 6]);
//                        cv::line(test_img, cv::Point(out_ply[pi].pts[0].x, out_ply[pi].pts[0].y), cv::Point(out_ply[pi].pts[1].x, out_ply[pi].pts[1].y), colors[pi % 6]);
//                    }
//                }
//            }
//
////            for(int ri = 0; ri < out_ring.size(); ri ++)
////            {
////                cv::Point ct(out_ring[ri].cx, out_ring[ri].cy);
////                cv::circle(test_img, ct, 4, cv::Scalar(0, 255, 0), 1);
////                cv::ellipse(test_img, ct, cv::Size(out_ring[ri].ax, out_ring[ri].bx), out_ring[ri].theta, 0, 360, cv::Scalar(255, 255, 255));
////        //        cv::ellipse(ell_in, ct, cv::Size(out_ring[ri].ax, out_ring[ri].bx), out_ring[ri].theta, out_ring[ri].ang_start, out_ring[ri].ang_end, cv::Scalar(0, 0, 255));
////            }
            
            imshow("test", test_img);
//            imshow("test_gray", img_gray);
            
            imshow("circle_out", circle_out);
            waitKey(0);
//            cv::imwrite(save_path + "/" + "src_" + names[i], test_img);
//            cv::imwrite(save_path + "/" + "out_" + names[i], circle_out);
        }
        else
        {
            std::cout << "invalid image!" << std::endl;
        }

    }
    
    return 0;

}
