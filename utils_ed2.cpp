//
//  utils_ed2.cpp
//  ed2
//
//  Created by 王哲 on 2020/8/1.
//  Copyright © 2020 zhe. All rights reserved.
//

#include "utils_ed2.hpp"

using namespace std;
using namespace cv;

/*
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
*/

void get_fileNames(std::string path, std::vector<std::string>& files, std::vector<std::string>& names)
{
	//文件句柄，win10用long long，win7用long就可以了
	long long hFile = 0;
	//文件信息 
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之 //如果不是,加入列表 
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					get_fileNames(p.assign(path).append("\\").append(fileinfo.name), files, names);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				names.push_back(fileinfo.name);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
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
                //pixel
                float c21 = input.at<Vec3b>(i,j)[2] / (input.at<Vec3b>(i,j)[1] + 0.01);
                float c20 = input.at<Vec3b>(i,j)[2] / (input.at<Vec3b>(i,j)[0] + 0.01);
                float c10 = input.at<Vec3b>(i,j)[1] / (input.at<Vec3b>(i,j)[0] + 0.01);
                float c2 = input.at<Vec3b>(i,j)[2];
                if(c21 >= thres[0] && c10 >= thres[1] && c20 > thres[1] && c2 >= thres[2])
                    mask.at<uchar>(i,j) = 255;
            }
        }
    
//    imshow("pure mask", mask);
//    waitKey(0);
    
    if(postpre)
    {
        // dilate mask
        int z = 1 * 2 + 1;
        int x = 0 * 2 + 1;

        Mat structureElement_d = getStructuringElement(MORPH_ELLIPSE, Size(z, z), cv::Point(-1, -1));
        Mat structureElement_e = getStructuringElement(MORPH_ELLIPSE, Size(x, x), cv::Point(-1, -1));
        dilate(mask, mask, structureElement_d, cv::Point(-1, -1), 2);
//        imshow("dilate mask", mask);

        //fill holes in mask
        fillHole(mask, mask);
        erode(mask, mask, structureElement_e, cv::Point(-1, -1), 2);
        erode(mask, mask, structureElement_e, cv::Point(-1, -1), 2);
        erode(mask, mask, structureElement_e, cv::Point(-1, -1), 2);

        vector<vector<cv::Point>> contours;
        vector<cv::Vec4i> hierarchy;
        cv::Mat imgcontours;
        cv::Point2f center;
        findContours(mask, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
        
        vector<Moments> mu(contours.size());   
         for (int i = 0; i < contours.size(); i++)
         {
          mu[i] = moments(contours[i], false);
         }
         vector<Point2f>  mc(contours.size());    
         for (int i = 0; i < contours.size(); i++)
         {
          mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
         }

        for(int i = 0; i < contours.size(); i ++)
        {
            cv::Moments mu = moments(contours[i]);
            cv::Point mc(mu.m10 / mu.m00, mu.m01 / mu.m00);
            double area = contourArea(contours[i]);
            double area_thre = 300;
            if(mc.x >= input.cols * 0.1 && mc.x <= input.cols * 0.9 && mc.y >= input.rows * 0.1 && mc.y <= input.rows * 0.9 && area >= area_thre)
            {
                cv::drawContours(out_mask, contours, i, cv::Scalar(255), -1);
//                cv::putText(out_mask, std::to_string(area), mc, 1, 1, Scalar(255));
            }
        }
//        erode(out_mask, out_mask, structureElement_e, cv::Point(-1, -1), 2);
    }
    
    return true;
}

cv::Mat PerspectiveTrans(cv::Mat src, cv::Point2f* scrPoints, cv::Point2f* dstPoints)
{
    Mat dst;
    Mat Trans = getPerspectiveTransform(scrPoints, dstPoints);
    warpPerspective(src, dst, Trans, Size(240, 240), cv::INTER_CUBIC);
    
    return dst;
}

Mat AffineTrans(Mat src, Point2f* scrPoints, Point2f* dstPoints)
{
    Mat dst;
    Mat Trans = getAffineTransform(scrPoints, dstPoints);
    warpAffine(src, dst, Trans, Size(src.cols, src.rows), cv::INTER_CUBIC);
    return dst;
}

cv::Mat contrastStretch2(cv::Mat &srcImage)
{
    cv::Mat dstImage;
    if( srcImage.empty() )
    {
        std::cerr << "image empty" << std::endl;
        return dstImage;
    }
	// cal max and min value
    double pixMin, pixMax, pixMax2;
    cv::minMaxLoc(srcImage,&pixMin,&pixMax);
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
