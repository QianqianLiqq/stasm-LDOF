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

	cv::Rect faceboxKCF;//人脸跟踪的结果
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

//if (flagA == false)
//{
//	frame.copyTo(preFrame);
//	flagA = true;
//}
//else
//{
//	vector<Point2f> pIn;
//	vector<Point2f> pOut;
//	pIn.push_back(Point(cvRound(landmarks[2 * 38]), cvRound(landmarks[2 * 38 + 1])));
//	pIn.push_back(Point(cvRound(landmarks[2 * 39]), cvRound(landmarks[2 * 39 + 1])));
//	pIn.push_back(Point(cvRound(landmarks[2 * 59]), cvRound(landmarks[2 * 59 + 1])));
//	pIn.push_back(Point(cvRound(landmarks[2 * 65]), cvRound(landmarks[2 * 65 + 1])));
//	pIn.push_back(Point(cvRound(landmarks[2 * 62]), cvRound(landmarks[2 * 62 + 1])));
//	pIn.push_back(Point(cvRound(landmarks[2 * 74]), cvRound(landmarks[2 * 74 + 1])));
//	opticalFlow* opt = new opticalFlow();
//	opt->set(preFrame, frame);
//	opt->setR(10);
//	opt->computeOpticalFlow(pIn, pOut);
//	for (int t = 0; t < pOut.size(); t++)
//		circle(displayImage, pOut[t], 1, cv::Scalar(0, 0, 255), 2);
//	/*Mat color;
//	opt->disPlayFlow(color);
//	imshow("opt", color);
//	waitKey(0);*/
//	Mat preFrame(frame);


//faceBox = facial.getFacePosition(frame);	//检测加跟踪的结果
////faceboxKCF = facial.getFacePosKCF(frame);	//人脸跟踪的结果  这两句的位置不能变
//Rect face = faceBox;

//if (facial.getFaceDetectSuccess() == 1)  //正常驾驶状态下的人脸  
//{
//    //特征点检测
//	
//		stasm_search_single(&foundface, landmarks,
//			(const char*)img.data, img.cols, img.rows, path, "../data");

//		// draw the landmarks on the image as white dots (image is monochrome)
//		stasm_force_points_into_image(landmarks, img.cols, img.rows);
//	
//    for (int i = 0; i < stasm_NLANDMARKS; i++)
//	    circle(displayImage, cv::Point(cvRound(landmarks[i * 2]), cvRound(landmarks[i * 2 + 1])), 1, cv::Scalar(255, 255, 255), 1);
//	
//	//facial.getEyeMouthPosition(frame,landmarks);

//	rectangle(displayImage, face, cv::Scalar(0, 0, 255), 2, 8);    //正常驾驶状态下的人脸  可打开
//	//rectangle(displayImage, facial.leftEyeRect, cv::Scalar(0, 0, 255), 2, 8);
//	//rectangle(displayImage, facial.rightEyeRect, cv::Scalar(0, 0, 255), 2, 8);
//	//rectangle(displayImage, facial.mouthRect, cv::Scalar(0, 0, 255), 2, 8);

//	ObjectDetect object;
//	object.eyemouthdetect(frame,landmarks);

//	Rect leftEyeBox = runCMT(leftEyeCMT, frame(face), Rect(object.leftEyeRect.x - face.x, object.leftEyeRect.y - face.y, object.leftEyeRect.width, object.leftEyeRect.height), frameNum);
//	Rect rightEyeBox = runCMT(rightEyeCMT, frame(face), Rect(object.rightEyeRect.x - face.x, object.rightEyeRect.y - face.y, object.rightEyeRect.width, object.rightEyeRect.height), frameNum);//Rect(faceCMT.x, faceCMT.y, faceCMT.width / 2, faceCMT.height / 2)
//	Rect mouthBox = runCMT(mouthCMT, frame(face), Rect(object.mouthRect.x - face.x, object.mouthRect.y - face.y, object.mouthRect.width, object.mouthRect.height), frameNum);
//	mapRect(face, leftEyeBox);
//	mapRect(face, rightEyeBox);
//	mapRect(face, mouthBox);
//	rectangle(displayImage, leftEyeBox, cv::Scalar(0, 0, 255), 2, 8);
//	rectangle(displayImage, rightEyeBox, cv::Scalar(0, 0, 255), 2, 8);
//	rectangle(displayImage, mouthBox, cv::Scalar(0, 0, 255), 2, 8);

//	if (initFaceNum <= 100)
//	{
//		x1 += faceBox.x;
//		y1 += faceBox.y;
//		width1 += faceBox.width;
//		height1 += faceBox.height;
//	}
//	if (initFaceNum == 101)
//	{
//		srcFace.x = x1 / 100;
//		srcFace.y = y1 / 100;
//		srcFace.width = width1 / 100;
//		srcFace.height = height1 / 100;
//	}

//}else  //检测不到人脸
//{
//	continue;								
//}
