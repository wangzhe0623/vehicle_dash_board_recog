//
//  scale_process.cpp
//  ed2
//
//  Created by 王哲 on 2020/8/1.
//  Copyright © 2020 zhe. All rights reserved.
//

#include "scale_process.hpp"
using namespace cv;
using namespace std;

void lines_pair(vector<Vec4f>& lines, vector<line_info>& lines_info)
{
    lines_info.clear();
    
    int max_length = 30;
    int min_length = 15;
    
    for(int li = 0; li < lines.size(); li ++)
    {
        float length = sqrt(pow((lines[li][0] - lines[li][2]), 2) + pow((lines[li][1] - lines[li][3]), 2));
        if (min_length <= length && length <= max_length)
        {
            float ct_x = (lines[li][0] + lines[li][2]) / 2.;
            float ct_y = (lines[li][1] + lines[li][3]) / 2.;
            if (ct_x >= 35 && ct_y <= 160)
            {
                Point ctp(ct_x, ct_y);
                float k, b;
                if (abs(lines[li][0] - lines[li][1]) > 0.000001)
                {
                    k = float(lines[li][3] - lines[li][1]) / float(lines[li][2] - lines[li][0]);
                    b = lines[li][1] - k * lines[li][0];
                }
                else
                {
                    k = 999999;
                    b = 0;
                }
                
                line_info info_one;
                info_one.ct = ctp;
                info_one.line_one = lines[li];
                info_one.k = k;
                info_one.b = b;
                lines_info.push_back(info_one);
            }
        }
    }
//    for idx, scale in enumerate(scale_seg):
//        if not match[idx]:
//            dist_min = 9999999
//            delta_k_min = 9999999
//            dist_all = []
//            k_all = []
//            rest_id = []
//            for idx2, scale2 in enumerate(scale_seg):
//                if not match[idx2]:
//                    if idx != idx2:
//                        dist = math.sqrt(pow(ctp[idx][0] - ctp[idx2][0], 2) + pow(ctp[idx][1] - ctp[idx2][1], 2))
//                        delta_k = abs(ks[idx] - ks[idx2])
//                        dist_all.append(dist)
//                        k_all.append(delta_k)
//                        rest_id.append(idx2)
//            dist_min_index = dist_all.index(min(dist_all))
//            if k_all[dist_min_index] < 0.1 and min(dist_all) < 10:
//                # print(min(dist_all), k_all[dist_min_index])
//                match[idx] = rest_id[dist_min_index]
//                match[rest_id[dist_min_index]] = idx
    for (int ii = 0; ii < lines_info.size(); ii ++)
    {
        if (!lines_info[ii].match_one.status)
        {
            vector<float> dist_all;
            vector<float> k_all;
            vector<int> rest_id;
            for (int jj = 0; jj < lines_info.size(); jj ++)
            {
                if (!lines_info[jj].match_one.status && ii != jj)
                {
                    float dist, delta_k;
                    dist = sqrt(pow(lines_info[ii].ct.x - lines_info[jj].ct.x, 2) + pow(lines_info[ii].ct.y - lines_info[jj].ct.y, 2));
                    delta_k = abs(lines_info[ii].k - lines_info[jj].k);
                    dist_all.push_back(dist);
                    k_all.push_back(delta_k);
                    rest_id.push_back(jj);
                }
            }
            if (dist_all.size() > 0)
            {
                auto smallest = std::min_element(std::begin(dist_all), std::end(dist_all));
                auto min_dist_index = std::distance(std::begin(dist_all), smallest);
//                std::cout << "min element is " << *smallest<< " at position " << min_dist_index << std::endl;
                
                if (k_all[min_dist_index] < 0.1 && *smallest < 10)
                {
                    lines_info[ii].match_one.match_target = rest_id[min_dist_index];
                    lines_info[ii].match_one.status = true;
                    lines_info[rest_id[min_dist_index]].match_one.match_target = ii;
                    lines_info[rest_id[min_dist_index]].match_one.status = true;
                }
            }
        }
    }
}


void get_ell_points(vector<line_info>& lines_info, cv::Size img_size, vector<Point>& ell_points)
{
    ell_points.clear();
//    for iii, item in enumerate(match):
//        if item:
//            temp = []
//            if ctp[iii][0] < img_np.shape[1] / 2:
//                if scale_seg[iii][0] < scale_seg[iii][2]:
//                    temp = [scale_seg[iii][2], scale_seg[iii][3]]
//                else:
//                    temp = [scale_seg[iii][0], scale_seg[iii][1]]
//            else:
//                if scale_seg[iii][0] < scale_seg[iii][2]:
//                    temp = [scale_seg[iii][0], scale_seg[iii][1]]
//                else:
//                    temp = [scale_seg[iii][2], scale_seg[iii][3]]
//            ell_points.append(temp)
//            cv2.circle(copy3, (temp[0], temp[1]), 3, (255, 255, 255))
    for (int idx = 0; idx < lines_info.size(); idx ++)
    {
        if (lines_info[idx].match_one.status)
        {
            Point temp;
            if (lines_info[idx].ct.x < img_size.width / 2)
            {
                if (lines_info[idx].line_one[0] < lines_info[idx].line_one[2])
                {
                    temp = Point(lines_info[idx].line_one[2], lines_info[idx].line_one[3]);
                }
                else
                {
                    temp = Point(lines_info[idx].line_one[0], lines_info[idx].line_one[1]);
                }
            }
            else
            {
               if (lines_info[idx].line_one[0] < lines_info[idx].line_one[2])
                {
                    temp = Point(lines_info[idx].line_one[0], lines_info[idx].line_one[1]);
                }
                else
                {
                    temp = Point(lines_info[idx].line_one[2], lines_info[idx].line_one[3]);
                }
            }
            
            ell_points.push_back(temp);
        }
    }
}


vector<Point> get_ell_base_lsd(Mat input_3c, RotatedRect& ell)
{
    Mat input_gray, gray_3c, all_lines, sel_lines;
    cvtColor(input_3c, input_gray, CV_BGR2GRAY);
    cvtColor(input_gray, gray_3c, CV_GRAY2BGR);
    
    gray_3c.copyTo(all_lines);
    gray_3c.copyTo(sel_lines);
    
//    #if 1
//        Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_STD);
//    #else
//        Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_ADV);
//    #endif

    Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_ADV, 0.8, 0.6, 2.0, 20);

    vector<Vec4f> lines;
    vector<line_info> lines_info;
    vector<Point> ell_points;

    // Detect the lines
    ls->detect(input_gray, lines);
      
    // Show found lines
    Mat drawnLines(input_3c);
    ls->drawSegments(all_lines, lines);
    
    lines_pair(lines, lines_info);
    cv::Size img_size = input_gray.size();
    get_ell_points(lines_info, img_size, ell_points);
    
    for (int li = 0; li < lines_info.size(); li ++)
    {
        cv::line(sel_lines, Point(lines_info[li].line_one[0], lines_info[li].line_one[1]), Point(lines_info[li].line_one[2], lines_info[li].line_one[3]), Scalar(0, 255, 0));
    }
    
    for (int pi = 0; pi < ell_points.size(); pi ++)
    {
        circle(sel_lines, ell_points[pi], 3, Scalar(0, 255, 0), -1);
    }

    ell = fitEllipse(ell_points);
    
//    imshow("all_lines", all_lines);
    imshow("sel_lines", sel_lines);
    return ell_points;
}

Mat get_circle_based_ell(RotatedRect ell, Mat input_3c, Point& ct)
{
    cv::Point2f* vertices = new cv::Point2f[4];
    cv::Point2f* dst_vertices = new cv::Point2f[4];
    
    ell.points(vertices);
//    float ell_width = ell.size.width;
//    float ell_height = ell.size.height;
    float circle_edge = 200;
    int shift = 180;

//    cv::Point2f* src_hand = new cv::Point2f[4];
//    src_hand[0] = cv::Point2f(49, 76);
//    src_hand[1] = cv::Point2f(322, 24);
//    src_hand[2] = cv::Point2f(332, 133);
//    src_hand[3] = cv::Point2f(73, 178);
//    int ell_width = sqrt(pow(src_hand[1].x - src_hand[0].x, 2) + pow(src_hand[1].y - src_hand[0].y, 2));
//    int ell_height = sqrt(pow(src_hand[3].x - src_hand[0].x, 2) + pow(src_hand[3].y - src_hand[0].y, 2));
    
    dst_vertices[0] = cv::Point2f(shift, shift);
    dst_vertices[1] = cv::Point2f(shift + circle_edge, shift);
    dst_vertices[2] = cv::Point2f(shift + circle_edge, shift + circle_edge);
    dst_vertices[3] = cv::Point2f(shift, shift + circle_edge);
    
    cv::ellipse(input_3c, ell, Scalar(255, 255, 255));

    Mat circle_out;
    circle_out = PerspectiveTrans(input_3c, vertices, dst_vertices);
    
    ct.x = shift + circle_edge / 2;
    ct.y = shift + circle_edge / 2;
    circle(circle_out, ct, 1, Scalar(0, 255, 0), -1);
    
    return circle_out;
}
