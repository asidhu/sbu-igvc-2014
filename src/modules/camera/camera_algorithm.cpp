#include "modules/camera/camera_algorithm.h"
#include <time.h>
#include <string>


void camera_algorithm::beginCalib(algorithm_params* params){
	params->calibration_mode=true;
	//tmp
	params->m_calib.pattern_size = cv::Size(5,8);
	params->m_calib.num_frames=15;
	params->m_calib.image_delay=5;

	//not temp
	params->m_calib.timeStamp=clock();
	params->m_calib.left_points.clear();
	params->m_calib.right_points.clear();
	params->m_left.calibrated=false;
	params->m_right.calibrated=false;
}

void camera_algorithm::calib(algorithm_params* params){
	using namespace cv;
	std::vector<Point2f> pointsL,pointsR;
	bool foundL, foundR;
	Mat lview = params->m_left.normal;
	Mat rview = params->m_right.normal;
	calib_params calib = params->m_calib;
	Mat lGray = params->m_left.gray;
	Mat rGray = params->m_right.gray;
	//clone images
	// do we have enough calibration points, if not ge tmore.
	if(calib.left_points.size()<calib.num_frames){
		//this is for a pause interval between snapshots.
		if(clock()-calib.timeStamp > calib.image_delay*1e-3*CLOCKS_PER_SEC){
			//find chessboard corners...
			foundL = findChessboardCorners(lGray, calib.pattern_size,pointsL,
				CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE | CALIB_CB_FAST_CHECK);
			foundR = findChessboardCorners(rGray, calib.pattern_size,pointsR,
				CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE | CALIB_CB_FAST_CHECK);
			// refine left picture
			if(foundL){
				cornerSubPix(lGray,pointsL,Size(11,11),
					Size(-1,-1), TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,30,.1));
			}
			// refine right picture
			if(foundR){
				cornerSubPix(rGray,pointsR,Size(11,11),
					Size(-1,-1), TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,30,.1));
			}
			//only if we found chessboard in both frames, save the found points.
			if(foundL && foundR){
				calib.left_points.push_back(pointsL);
				calib.right_points.push_back(pointsR);
				calib.timeStamp = clock();
			}
		}
		// add text and draw chessboard corners.
		std::string msg = format("%d/%d",(int)calib.left_points.size(),calib.num_frames);
		int baseLine = 0;
		Size textSize = getTextSize(msg, 1, 1, 1, &baseLine);
		Point textOrigin(lview.cols - 2*textSize.width - 10, lview.rows - 2*baseLine - 10);
		putText(lview,msg,textOrigin,1,1,Scalar(0,255,0));
		drawChessboardCorners(lview,calib.pattern_size,Mat(pointsL),foundL);
		
		msg = format("%d/%d",(int)calib.right_points.size(),calib.num_frames);
		baseLine = 0;
		textSize = getTextSize(msg, 1, 1, 1, &baseLine);
		textOrigin=Point(rview.cols - 2*textSize.width - 10, rview.rows - 2*baseLine - 10);
		putText(rview,msg,textOrigin,1,1,Scalar(0,255,0));
		drawChessboardCorners(rview,calib.pattern_size,Mat(pointsR),foundR);
		//actually calibrate, based on points.
		if(calib.left_points.size()==calib.num_frames){
			performCalibration(params);
		}
	}
	//show cameras
	imshow("Right Camera", rview);
	imshow("Left Camera",lview);	
}	

void camera_algorithm::performCalibration(algorithm_params* params){
	using namespace cv;
	calib_params calib = params->m_calib;
	Size imgsize = params->m_left.normal.size();
	std::vector<std::vector<Point2f> > ptsL = calib.left_points, ptsR = calib.right_points;
	std::vector<Point3f> corners;
	for(int i=0;i<calib.pattern_size.height;i++){
		for(int j=0;j<calib.pattern_size.width;j++){
			corners.push_back(Point3f(float(j*calib.square_size),float(i*calib.square_size),0));
		}
	}
	std::vector<std::vector<Point3f> > objPts;
	objPts.resize(ptsL.size(),corners);
	Mat CM1 = Mat::eye(3,3,CV_64F);
	Mat CM2 = Mat::eye(3,3,CV_64F);
	Mat D1,D2;
	Mat R,T,E,F;
	
	stereoCalibrate(objPts,ptsL,ptsR,CM1,D1,CM2,D2,imgsize,
		R,T,E,F, TermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,100,1e-5),
		CV_CALIB_SAME_FOCAL_LENGTH | CV_CALIB_ZERO_TANGENT_DIST);
	
	Mat R1,R2,P1,P2,Q;
	stereoRectify(CM1,D1,CM2,D2,imgsize,
		R,T,R1,R2,P1,P2,Q);		
	params->m_left.CM=CM1;
	params->m_left.D=D1;
	params->m_left.R=R1;
	params->m_left.P=P1;
	params->m_right.CM=CM2;
	params->m_right.D=D2;
	params->m_right.R=R2;
	params->m_right.P=P2;
	
	Mat map1x,map1y,map2x,map2y;
	initUndistortRectifyMap(CM1,D1,R1,P1,imgsize,CV_32FC1,map1x,map1y);
	initUndistortRectifyMap(CM2,D2,R2,P2,imgsize,CV_32FC1,map2x,map2y);
	params->m_calib.SM.map1x=map1x;
	params->m_calib.SM.map1y=map1y;
	params->m_calib.SM.map2x=map2x;
	params->m_calib.SM.map2y=map2y;
	params->m_calib.SM.E=E;
	params->m_calib.SM.F=F;
	params->m_calib.SM.Q=Q;	
	params->m_left.calibrated=true;
	params->m_right.calibrated=true;
}


void lineDetector(cv::Mat& img, algorithm_params* param, std::vector<cv::Mat>& debug_images){
	using namespace cv;
	line_detector_params L = param->m_line;
	if(param->calibration_mode){
		debug_images.push_back(img);
	}
	Mat channels[3];
	split(img,channels);
	Mat blue = channels[0],green=channels[1],red=channels[2];
	Mat edges = blue.clone();

/*
	Mat HSV;
	cvtColor( img,HSV,CV_BGR2HSV);
	split(HSV,channels);
	blue = channels[2];
	threshold(blue,blue,L.initial_thresh,255,THRESH_BINARY);
	blue = blue > 200;
	bitwise_and(HSV,Scalar(0,0,0),HSV,blue);
	blue=HSV;
	imshow("white",blue);
*/	
	threshold(blue,blue,L.initial_thresh,255,0);
	threshold(green,green,L.initial_thresh,255,0);
	threshold(red,red,L.initial_thresh,255,0);
	
	bitwise_and(green,red,green);
	bitwise_and(blue,green,blue);	
	Mat skel(blue.size(), CV_8UC1, Scalar(0));
	Mat temp;
	Mat eroded; 
	Mat element = getStructuringElement(MORPH_CROSS, Size(3, 3));
	assert(L.gauss_blur_kernel_size%2!=0);
	GaussianBlur(blue,blue,Size(L.gauss_blur_kernel_size,L.gauss_blur_kernel_size),
		L.gauss_blur,L.gauss_blur);
	if(param->calibration_mode){
		Mat tst;
		cvtColor(blue,tst,CV_GRAY2BGR);
		debug_images.push_back(tst);
	}
	int iterations=0;
	bool done;              
	do
	{
		erode(blue, eroded, element);
		dilate(eroded, temp, element); // temp = open(img)
		subtract(blue, temp, temp);
		bitwise_or(skel, temp, skel);
		eroded.copyTo(blue);
		done = (countNonZero(blue) == 0);
		iterations++;
	} while (!done && iterations < 50);
	//GaussianBlur(skel,skel,Size(3,3),1.5,1.5);
	if(param->calibration_mode){
		Mat tst;
		cvtColor(skel,tst,CV_GRAY2BGR);
		debug_images.push_back(tst);
	}
	Canny(skel,edges,L.canny_thresh1,L.canny_thresh2,3);
	//GaussianBlur(edges,edges,Size(7,7),1.5,1.5);
	std::vector<Vec4i> lines;
	HoughLinesP(edges,lines,L.hough_rho,L.hough_theta,L.hough_thresh,
		L.hough_min_line,L.hough_max_gap);
	if(lines.size()<20){
		for(size_t i=0;i<lines.size();i++){
			Vec4i v = lines[i];
			line(edges, Point(v[0],v[1]),Point(v[2],v[3]),Scalar(255),3,CV_AA);
		}
	}
	if(param->calibration_mode){
		Mat tst;
		cvtColor(edges,tst,CV_GRAY2BGR);
		debug_images.push_back(tst);
	}
}

void paint_debug_images(std::vector<cv::Mat>& imgs, int ncols,cv::Mat& output){
	using namespace cv;
	if(imgs.size()==0)
		return;
	int nrows = imgs.size()/ncols;
	if(imgs.size()%ncols)
		nrows++;
	Size sz = imgs[0].size();
	output = Mat(sz.height*nrows,sz.width*ncols,CV_8UC3);
	Size osz= output.size();
	for(int i=0;i<imgs.size();i++){
		int x = i%ncols,
			y=i/ncols;
		Mat ROI_output = output(Rect(x*sz.width,y*sz.height,sz.width,sz.height));
		imgs[i].copyTo(ROI_output);
	}
	
}

void camera_algorithm::lineDetection(algorithm_params* params){
	using namespace cv;
	Mat L = params->m_left.normal,
		R=params->m_right.normal;
	std::vector<Mat> m_debug_imgs;
	lineDetector(L,params,m_debug_imgs);
	Mat m_display;
	paint_debug_images(m_debug_imgs,2,m_display);
	imshow("left",m_display);
		
	lineDetector(R,params,m_debug_imgs);
}


