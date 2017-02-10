#pragma once
#include "Tara.h"

using namespace Tara;
using namespace cv;

class depthFeatures
{
public:

	//Initialise
	int Init();

private:

	int ManualExposure;

	//Camera Streaming
	int CameraStreaming();

	//Object to access Disparity
	Disparity _Disparity;	
};

//Call back function
void DepthPointSelection(int MouseEvent, int x, int y, int flags, void* param) ;
