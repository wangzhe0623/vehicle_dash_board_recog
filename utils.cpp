//
//  utils.cpp
//  job_no_1
//
//  Created by 王哲 on 2020/6/21.
//  Copyright © 2020 zhe. All rights reserved.
//

#include "utils.h"
//#include <stdio.h>

using namespace std;
using namespace cv;

bool get_fileNames(char* path, vector<string> &names)
{
    names.clear();
    struct dirent *dirp;
    DIR* dir = opendir(path);
    
    while ((dirp = readdir(dir)) != nullptr)
    {
        if (dirp->d_type == DT_REG)
        {
            // 文件
//            printf("%s\n", dirp->d_name);
            names.push_back(dirp->d_name);
        }
        else if (dirp->d_type == DT_DIR)
        {
            // 文件夹
        }
    }
    
    closedir(dir);
    return true;
}

void fillHole(const cv::Mat srcimage, cv::Mat &dstimage)
{
    Size m_Size = srcimage.size();
    Mat temimage = Mat::zeros(m_Size.height + 2, m_Size.width + 2, srcimage.type());//延展图像
    srcimage.copyTo(temimage(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));
    floodFill(temimage, cv::Point(0, 0), Scalar(255));
    Mat cutImg;//裁剪延展的图像
    temimage(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);
    dstimage = srcimage | (~cutImg);
}

void contour2ellipse(cv::Mat input_gray, std::vector<cv::RotatedRect> ellipses)
{
    ellipses.clear();
    vector<vector<cv::Point>> contours;
    vector<cv::Vec4i> hierarchy;
    cv::Mat imgcontours;
    cv::Point2f center;

    findContours(input_gray, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    for(int i = 0; i < contours.size(); i ++)
    {
        //fitEllipse/fitEllipseAMS/fitEllipseDirect(InputArray points) try on them
        cv::RotatedRect ellipse = fitEllipse(contours[i]);
        ellipses.push_back(ellipse);
    }
}

void elsd_wz(cv::Mat input, std::vector<Ring>& out_ring, std::vector<Polygon>& out_ply)
{
    out_ring.clear();
    out_ply.clear();
    
    EResult ret;
    ret.execute(input);
    for(int ri = 0; ri < ret.ringNum; ri ++)
    {
        out_ring.push_back(ret.rings[ri]);
        
    }
    
    for(int pi = 0; pi < ret.polygonNum; pi ++)
    {
        out_ply.push_back(ret.polygons[pi]);
    }
}

bool seg_mask(cv::Mat input, cv::Mat& out_mask, cv::Scalar thres, bool postpre)
{
    /* compare 3 channels thres(c21, c10, c2)*/
    
    if(input.empty())
    {
        cout << "Input Error!" << endl;
        return false;
    }
    
    Mat mask(input.rows, input.cols, CV_8UC1, Scalar(0));
    for(int i = 0; i < input.rows; i++)
        {
            for(int j = 0; j < input.cols; j++)
            {
                float c21 = input.at<Vec3b>(i,j)[2] / (input.at<Vec3b>(i,j)[1] + 0.01);
                float c10 = input.at<Vec3b>(i,j)[1] / (input.at<Vec3b>(i,j)[0] + 0.01);
                float c2 = input.at<Vec3b>(i,j)[2];
                if(c21 >= thres[0] && c10 >= thres[1] && c2 >= thres[2])
                    mask.at<uchar>(i,j) = 255;
            }
        }
    
    
    if(postpre)
    {
        // dilate mask
        int z = 4 * 2 + 1;
        int x = 1 * 2 + 1;

        Mat structureElement_d = getStructuringElement(MORPH_ELLIPSE, Size(z, z), cv::Point(-1, -1));
        Mat structureElement_e = getStructuringElement(MORPH_ELLIPSE, Size(x, x), cv::Point(-1, -1));

        dilate(mask, mask, structureElement_d, cv::Point(-1, -1), 2);

        //fill holes in mask
        fillHole(mask, mask);
        erode(mask, mask, structureElement_e, cv::Point(-1, -1), 2);

        vector<vector<cv::Point>> contours;
        vector<cv::Vec4i> hierarchy;
        cv::Mat imgcontours;
        cv::Point2f center;
        findContours(mask, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
        
        vector<Moments> mu(contours.size());    //计算轮廓矩
         for (int i = 0; i < contours.size(); i++)
         {
          mu[i] = moments(contours[i], false);
         }
         vector<Point2f>  mc(contours.size());    //计算轮廓中心
         for (int i = 0; i < contours.size(); i++)
         {
          mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
         }

        for(int i = 0; i < contours.size(); i ++)
        {
            cv::Moments mu = moments(contours[i]);
            cv::Point mc(mu.m10 / mu.m00, mu.m01 / mu.m00);
            double area = contourArea(contours[i]);
            double area_thre = 500;
            if(mc.x >= input.cols * 0.1 && mc.x <= input.cols * 0.9 && mc.y >= input.rows * 0.1 && mc.y <= input.rows * 0.9 && area >= area_thre)
            {
                cv::drawContours(out_mask, contours, i, cv::Scalar(255), -1);
            }
        }
        
    }
    
    return true;
}

void mask2points(cv::Mat src_binary, std::vector<cv::Point>& dst)
{
    dst.clear();
    for(int i=0; i < src_binary.cols; i ++)
    {
        for(int j=0; j < src_binary.rows; j ++)
        {
            
            int value = src_binary.at<uchar>(j,i);
            if (value > 0)
            {
                cv::Point p;
                p.x = i;
                p.y = j;
                dst.push_back(p);
            }
        }
    }
    return;
}

cv::Mat PerspectiveTrans(cv::Mat src, cv::Point2f* scrPoints, cv::Point2f* dstPoints)
{
//    Mat dst(src.size(), CV_8UC3, cv::Scalar(255, 255, 255));
    Mat dst;
    Mat Trans = getPerspectiveTransform(scrPoints, dstPoints);
    warpPerspective(src, dst, Trans, Size(600, 500), cv::INTER_CUBIC);
    
    return dst;
}

Mat AffineTrans(Mat src, Point2f* scrPoints, Point2f* dstPoints)
{
    Mat dst;
    Mat Trans = getAffineTransform(scrPoints, dstPoints);
    warpAffine(src, dst, Trans, Size(src.cols, src.rows), cv::INTER_CUBIC);
    return dst;
}

void keypoints2mask(Mat input_rgb, Mat& mask_out, std::vector<cv::Point>& contour)
{
    contour.clear();
    //通过ORB算法检测两幅图像中的特征点，并计算各自的二值描述子
    auto orb = ORB::create(200, 1.6, 8, 31, 0, 2, ORB::HARRIS_SCORE, 31, 20);
    
    //harris
//    cv::Mat gray, harris, normImage, scaledImage;
//    cv::cvtColor(input_rgb, gray, cv::COLOR_BGR2GRAY);
//    cornerHarris(gray, harris, 2, 3, 0.04);
//    normalize(harris, normImage, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
//        //将归一化后的图线性变换成8位无符号整形
//        convertScaleAbs(normImage, scaledImage);
//        //将检测到的，且符合阀值条件的角点绘制出来
//        for (int i = 0; i < normImage.rows; i++)
//        {
//            for (int j = 0; j < normImage.cols; j++)
//            {
//                if ((int)normImage.at<float>(i, j) > 30 + 80)
//                {
//                    cv::circle(mask_out, cv::Point(j, i), 3, Scalar(255), -1);
//                }
//            }
//        }

    vector<KeyPoint> keyPoints;
    Mat descriptors;
    orb->detectAndCompute(input_rgb, Mat(), keyPoints, descriptors, false);
    for(int i = 0; i < keyPoints.size(); i ++)
    {
        contour.push_back(keyPoints[i].pt);
        cv::circle(mask_out, keyPoints[i].pt, 3, cv::Scalar(255), -1);
    }
//    cv::imshow("des", descriptors);
//    cv::waitKey(0);
    return;
}

cv::Mat contrastStretch1(cv::Mat srcImage)
{
    cv::Mat resultImage = srcImage.clone();
    int nRows = resultImage.rows;
    int nCols = resultImage.cols;
    // 图像连续性判断
    if(resultImage.isContinuous()){
        nCols = nCols * nRows;
        nRows = 1;
    }

    // 计算图像的最大最小值
    double pixMin,pixMax;
    cv::minMaxLoc(resultImage,&pixMin,&pixMax);
//    std::cout << "min_a=" << pixMin << " max_b=" << pixMax << std::endl;

    // 对比度拉伸映射
    for(int j = 0; j < nRows; j ++){
        uchar *pDataMat = resultImage.ptr<uchar>(j);
        for(int i = 0; i < nCols; i ++){
            pDataMat[i] = (pDataMat[i] - pixMin) *
                255 / (pixMax - pixMin);    //255/(pixMax - pixMin)是斜率 y=k(x-a)
        }
    }
    return resultImage;
}

cv::Mat contrastStretch2(cv::Mat &srcImage)
{
    cv::Mat dstImage;
    if( srcImage.empty() )
    {
        std::cerr << "image empty" << std::endl;
        return dstImage;
    }
    // 计算图像的最大最小值
    double pixMin, pixMax, pixMax2;
    cv::minMaxLoc(srcImage,&pixMin,&pixMax);
//    std::cout << "min_a=" << pixMin << " max_b=" << pixMax << std::endl;
    pixMin = 40;
    pixMax = 80;
    pixMax2 = 252;
    //create lut table
    cv::Mat lut( 1, 256, CV_8U);
    for( int i = 0; i < 256; i++ ){
//        if (i < pixMin && i >= pixMax2)
        if (i < pixMin)
        {
            lut.at<uchar>(i)= 0;
        }
        else if (i > pixMax)
        {
            lut.at<uchar>(i)= 255;
        }
        else
            lut.at<uchar>(i)= static_cast<uchar>(255.0*(i-pixMin)/(pixMax-pixMin)+0.5);
    }
    //apply lut
    LUT(srcImage, lut, dstImage);
    return dstImage;
}

cv::Point2f getCrossPoint(std::vector<cv::Point>& pointerA, std::vector<cv::Point>& pointerB)
{
    double ka, kb;
    ka = (double)(pointerA[1].y - pointerA[0].y) / (double)(pointerA[1].x - pointerA[0].x); //求出LineA斜率
    kb = (double)(pointerB[1].y - pointerB[0].y) / (double)(pointerB[1].x - pointerB[0].x); //求出LineB斜率

    Point2f crossPoint;
    crossPoint.x = (ka*pointerA[0].x - pointerA[0].y - kb*pointerB[0].x + pointerB[0].y) / (ka - kb);
    crossPoint.y = (ka*kb*(pointerA[0].x - pointerB[0].x) + ka*pointerB[0].y - kb*pointerA[0].y) / (ka - kb);
    return crossPoint;
}

