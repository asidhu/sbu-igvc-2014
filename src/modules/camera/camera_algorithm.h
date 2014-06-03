#ifndef _CAMERA_ALGORITHMS_H
#define _CAMERA_ALGORITHMS_H
#include <opencv2/opencv.hpp>
#include <vector>
#include <time.h>
//Defines interface for opencv camera algorithms...

struct cam_params{
	bool calibrated;
	cv::Mat CM,D,R,P;
	std::vector<cv::Mat> rVecs,tVecs;
	cv::Mat normal;
	cv::Mat gray;
	cv::Mat channels[3];
};
struct stereomap{
	cv::Mat map1x,map1y,map2x,map2y;
	cv::Mat E,F,Q;
};
struct calib_params{
	cv::Size pattern_size;
	float square_size;
	clock_t timeStamp;
	int num_frames;
	int image_delay;
	std::vector<std::vector<cv::Point2f> > left_points, right_points;
	stereomap SM;
};

struct line_detector_params{
	int initial_thresh;
	float gauss_blur;
	int gauss_blur_kernel_size;
	int canny_thresh1, canny_thresh2;
	double hough_rho;
	double hough_theta;
	int hough_thresh;
	double hough_min_line;
	double hough_max_gap;
	line_detector_params():initial_thresh(150),gauss_blur(2.5f),gauss_blur_kernel_size(7),
		canny_thresh1(100),canny_thresh2(150),hough_rho(1),hough_theta(CV_PI/180),
		hough_thresh(50),hough_min_line(50),hough_max_gap(10){}
};

struct algorithm_params{
	cam_params  m_left, m_right;
	calib_params m_calib;
	line_detector_params m_line;
};


class camera_algorithm{

public:

	virtual void beginCalib(algorithm_params*);	
	virtual void calib(algorithm_params*);	
	virtual void endCalib(algorithm_params*){}	
	
	void performCalibration(algorithm_params*);
		

	virtual void lineDetection(algorithm_params*);
};
#endif
