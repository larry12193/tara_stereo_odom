/* stereoStream.cpp - source for generating stereo camera video stream       *\
|                                                                             |
|	Carnegie Mellon University                                                |
|	Lawrence Papincak                                                         |
|																			  |
|	02/09/17																  |
\*                                                                           */


#include "stereoStream.h"
#include "utils.h" // Drawing and printing functions
#include <iostream>
#include <stdio.h>
#include "opencv2/core.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace cv::xfeatures2d;

const double akaze_thresh = 3e-4; // AKAZE detection threshold set to locate about 1000 keypoints

int main()
{
	if(DEBUG_ENABLED) 
	{
		cout << "Stereo Odometry\n";
		cout << "------------\n\n";
	}

	//Object creation
	stereoStream sstream;
	sstream.Init();
	sstream.enableStream();

	if(DEBUG_ENABLED) 
		cout << "Exit : Stereo Odometry\n";

	return TRUE;
}

stereoStream::stereoStream() {
	this->isEnabled = 0;
	this->isInitialized = 0;
}

//Initialises all the necessary files
int stereoStream::Init()
{
	//Initialise the Camera
	if(!_Disparity.InitCamera(true, true))
	{
		if(DEBUG_ENABLED) 
			cout << "Camera Initialisation Failed\n";
		return FALSE;
	}
	cout << "Camera Initialized\n";
	// Set initialized flag
	isInitialized = 1;
	return TRUE;
}

//Streams the input from the camera
int stereoStream::CameraStreaming()
{	
	
	char WaitKeyStatus;			// Keypress status
	int firstFrame    = 1;		// Set to initialize tracker with first recieved frame
	int frameCount    = 0;		// Keeps track of frame count, used to reseed features

	// Camera image matricies
	Mat LeftImage, RightImage;
	// Resulting matched pairs
	Mat resLeft, resRight;
	// Disparity map matricies
	Mat gDisparityMap, gDisparityMap_viz;
	
	VideoWriter outVid("out.avi", CV_FOURCC('M', 'J', 'P', 'G'),10, Size(1280,480),true);

	//Window Creation
	namedWindow("Matched Pairs - Left Tracker", WINDOW_AUTOSIZE);

	cout << endl << "Press q/Q/Esc on the Image Window to quit the application!" << endl;
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	string Inputline;

	// Define SURF feature detector 
	int minhessin = 400;
	Ptr<AKAZE> Ldetector = AKAZE::create();
	Ldetector->setThreshold(akaze_thresh);
	// Define feature matcher
	Ptr<DescriptorMatcher> Lmatcher = DescriptorMatcher::create("BruteForce-Hamming");

	// Define feature tracker
	tracker Ltracker(Ldetector, Lmatcher);

	std::vector<KeyPoint> Lkeypoints;	// Keypoints from tracking
	Mat Ldescriptors;					// Point descriptors

	// Run stereo odometry while enabled
	while(this->isEnabled)
	{
		//Reads the frame and returns the rectified image
		if(!_Disparity.GrabFrame(&LeftImage, &RightImage)) 
		{
			destroyAllWindows();
			break;
		}

		// Initialize tracker with first frame
		if( firstFrame < 10) {
			firstFrame += 1;
			//outVid.write(LeftImage);
			if( firstFrame == 10 ) {
				Ltracker.setFirstFrame(LeftImage);
				cout << "Camera started..." << endl;
			}
		}
		// Run stereo odometry
		else {
			//Get disparity
			_Disparity.GetDisparity(LeftImage, RightImage, &gDisparityMap, &gDisparityMap_viz);

			//Estimate the Depth of the point selected
			//_Disparity.EstimateDepth(g_SelectedPoint, &DepthValue);

			// Detect points
			resLeft = Ltracker.process(LeftImage);
			// Draw keypoints
			//Mat left_keypoints;
			//drawKeypoints(LeftImage, keypoints, left_keypoints, Scalar::all(-1), DrawMatchesFlags::DEFAULT );

			//Display the Images
			imshow("Matched Pairs - Left Tracker", resLeft);
			//outVid.write(resLeft);		
		}
		
		//waits for the Key input
		WaitKeyStatus = waitKey(1);
		if(WaitKeyStatus == 'q' || WaitKeyStatus == 'Q' || WaitKeyStatus == 27) //Quit
		{	
			disableStream();
			outVid.release();
			break;
		}		
	}

	return TRUE;
}

// Run to enable video stream
void stereoStream::enableStream() {
	if( this->isInitialized ) {
		destroyAllWindows();
		this->isEnabled = 1;
		cout << "Stereo video stream enabled..." << endl;
		CameraStreaming();	
	}
	else {
		cout << "Please initialize camera!" << endl;
	}
}

// Run to disable video stream
void stereoStream::disableStream() {
	if( this->isInitialized && this->isEnabled ) {
		this->isEnabled = 0;
		cout << "Stereo video stream disabled..." << endl;
	}
	else if( this->isInitialized && !this->isEnabled ) {
		cout << "Stream already disabled!" << endl;
	}
	else {
		cout << "Please initialize camera!" << endl;
	}
}


// Sets frame to compare points to
void tracker::setFirstFrame(const Mat frame)
{
    first_frame = frame.clone();
    detector->detectAndCompute(first_frame, noArray(), first_kp, first_desc);
}

// Processes new frame for tracked points
Mat tracker::process(const Mat frame)
{
    vector<KeyPoint> kp;
    Mat desc;
    // Detect and compute new features in frame
    detector->detectAndCompute(frame, noArray(), kp, desc);

    vector< vector<DMatch> > matches;
    vector<KeyPoint> matched1, matched2;
    // Match points from first frame to new frame
    matcher->knnMatch(first_desc, desc, matches, 2);
    for(unsigned i = 0; i < matches.size(); i++) {
        if(matches[i][0].distance < nn_match_ratio * matches[i][1].distance) {
            matched1.push_back(first_kp[matches[i][0].queryIdx]);
            matched2.push_back(      kp[matches[i][0].trainIdx]);
        }
    }

    // Filter out outliers with RANSAC
    Mat inlier_mask, homography;
    vector<KeyPoint> inliers1, inliers2;
    vector<DMatch> inlier_matches;
    if(matched1.size() >= 4) {
        homography = findHomography(Points(matched1), Points(matched2),
                                    RANSAC, ransac_thresh, inlier_mask);
    }

    // Return blank frame if there are low numbers of inliers
    if(matched1.size() < 4 || homography.empty()) {
        Mat res;
        hconcat(first_frame, frame, res);
        return res;
    }

    // Save inliers if there are enough
    for(unsigned i = 0; i < matched1.size(); i++) {
        if(inlier_mask.at<uchar>(i)) {
            int new_i = static_cast<int>(inliers1.size());
            inliers1.push_back(matched1[i]);
            inliers2.push_back(matched2[i]);
            inlier_matches.push_back(DMatch(new_i, new_i, 0));
        }
    }

    // Draw inliers with lines connecting them back to first_frame
    Mat res;
    vector<char> matchedMask;
    drawMatches(first_frame, inliers1, frame, inliers2,
                inlier_matches, res,
                Scalar::all(-1), Scalar::all(-1), 
                matchedMask, DrawMatchesFlags::DEFAULT);
    return res;
}

//Call back function
/*void DepthPointSelection(int MouseEvent, int x, int y, int flags, void* param) 
{
    if(MouseEvent == CV_EVENT_LBUTTONDOWN)  //Clicked
	{
		g_SelectedPoint = Point(x, y);
    }
}*/