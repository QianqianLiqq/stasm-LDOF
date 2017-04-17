// minimal.cpp: Display the landmarks of a face in an image.
//              This demonstrates stasm_search_single.
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "opencv/highgui.h"
#include <opencv2/opencv.hpp>
#include "stasm_lib.h"

using namespace std;
using namespace cv;

std::vector<cv::Point2d> initialPoints;
Rect faceCMT(0, 0, 0, 0);
Rect firstFace(0, 0, 0, 0);

int main()
{
	double t = 0, fps;
	char string[200];
	VideoCapture capture;
	capture.open("E:\\VS\\标记样本\\眨眼\\blink.avi");//filename//E:\\桌面2\\剪辑\\yawn
	VideoWriter save;
	save.open("../data/result.avi", CV_FOURCC('M', 'J', 'P', 'G'), 50, cv::Size(640, 480));
	if (!capture.isOpened())
		return -1;
	Mat frame, displayImage;
	long long unsigned int frameNum = 0;
	capture.read(frame);
	bool pause = false;

	int foundface;
	float landmarks[2 * stasm_NLANDMARKS]; // x,y coords (note the 2)
	static const char* const path = "../data/testface.png";

	while (capture.read(frame))
	{
		if (!frame.data)
			break;
		//resize(frame, frame, Size(frame.cols / 2, frame.rows / 2));		

		t = (double)cv::getTickCount();
		frame.copyTo(displayImage);		
		imwrite(path, frame);
		cv::Mat_<unsigned char> img(cv::imread(path, CV_LOAD_IMAGE_GRAYSCALE));
		
		if (!stasm_search_single(&foundface, landmarks,
			                            (const char*)img.data, img.cols, img.rows, path, "../data"))
		{
			  printf("Error in stasm_search_single: %s\n", stasm_lasterr());
			  exit(1);
		}
		
		if (!foundface){
			printf("No face found in %s\n", path);
			continue;
		}
		else
		{	
			// draw the landmarks on the image as white dots (image is monochrome)
			stasm_force_points_into_image(landmarks, img.cols, img.rows);
			for (int i = 0; i < stasm_NLANDMARKS; i++)
				circle(displayImage, cv::Point(cvRound(landmarks[i * 2]), cvRound(landmarks[i * 2 + 1])), 1, cv::Scalar(0, 0, 255), 2);
			
			initialPoints.push_back(cv::Point(200, 195));
			initialPoints.push_back(cv::Point(216, 177));
			initialPoints.push_back(cv::Point(236, 195));
			initialPoints.push_back(cv::Point(217, 220));

		}			
			

		//显示帧率	
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		fps = 1.0 / t;
		sprintf(string, "fps: %.2f , frameNum:%d", fps, frameNum);      // 帧率保留两位小数,每秒变化的画面数
		cv::putText(displayImage, string, cv::Point(30, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255));
		save << displayImage;
		cv::imshow("displayImage", displayImage);
		frameNum++;
					

		//暂停控制
		if (pause)
		{
			switch (cv::waitKey(0))
			{
			case     ' ':  pause = !pause; break; //space
			case 2555904:  break;             //arrow right 
			case      27:  return 0;				//esc
			default:
				break;
			}
		}
		else{
			switch (waitKey(1))
			{
			case    32:  pause = !pause; break; //space
			case    27:  return 0;              //esc
			default:
				break;
			}
		}
	}

	return 0;
}
