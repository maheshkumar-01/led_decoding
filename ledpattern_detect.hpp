/* 
    This file contains declarations of functions for detecting a quad within an 
    image frame and detect the led status within the quad. 
*/
#include <iostream>
#include <stdlib.h>
#include <float.h>
#include <string.h>

#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"


using namespace std;
using namespace cv;
using namespace std::chrono;

bool update_led_status(Mat);
bool check_if_pattern_exists(uint32_t);
int detect(Mat* cam_frame);