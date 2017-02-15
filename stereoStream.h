#include "Tara.h"
#include <opencv2/features2d.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#include <iomanip>
#include "utils.h" // Drawing and printing functions

#ifndef _STEREOSTREAM_H
#define _STEREOSTREAM_H

using namespace Tara;
using namespace cv;

class stereoStream
{
public:
	stereoStream();
	//Initialise
	int Init();

	// Enable and disable video stream
	void enableStream();
	void disableStream();

private:

	int ManualExposure;
	int isEnabled;
	int isInitialized;
	//Camera Streaming
	int CameraStreaming();

	//Object to access Disparity
	Disparity _Disparity;	
};

const double nn_match_ratio = 0.8f; // Nearest-neighbour matching ratio
const double ransac_thresh = 2.5f;  // RANSAC inlier threshold

class tracker
{
public:
	tracker( Ptr<Feature2D> _detector, Ptr<DescriptorMatcher> _matcher) : 
		detector(_detector),
		matcher(_matcher) {}

	void setFirstFrame(const Mat frame);
	Mat process(const Mat frame);
    Ptr<Feature2D> getDetector() {
        return detector;
    }
protected:
    Ptr<Feature2D> detector;
    Ptr<DescriptorMatcher> matcher;
    Mat first_frame, first_desc;
    vector<KeyPoint> first_kp;
};

#endif
//Call back function
//void DepthPointSelection(int MouseEvent, int x, int y, int flags, void* param) ;
