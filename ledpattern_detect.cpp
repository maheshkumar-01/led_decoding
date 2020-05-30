/* 
    This file contains definitions for detecting a quad within an image 
    frame and detect the led status within the quad. 
*/

#include "ledpattern_detect.hpp"
// Hard Coded blink pattern
#define BLINK_PATTERN (uint32_t) 0xC93
#define THRESH_VAL (90)
#define THRESH_MAX (255)
#define LED_ON (1)
#define LED_OFF (0)
#define BLUR_KERNEL_SIZE (5)

// Random border colour generator for contour detection
RNG rng(12345);

// The ROI coordinates within our quad (LED's region)
double x1_,y1_,x2_,y2_ = 0;

// Two set of lookup tables to compare the input number
uint32_t compare_0_codes[6] = {0x24F,0x49E,0x279,0x4F2,0x3C9,0x792};
uint32_t compare_1_codes[6] = {0xC93,0x93C,0x927,0x9E4,0xF24,0xE49};

/**
 * @brief Compares the given number to the combination of pattern that blink 
 * pattern can be encoded. If one such pattern is found, returns true else false
 * 
 * @return bool true if valid combination of pattern, false otherwise
 */
bool check_if_pattern_exists(uint32_t num)
{
    for(int i=0;i<6;i++)
    {
        if(compare_0_codes[i]==num)
        {
            return 1;
        }
        else if(compare_1_codes[i]==num)
        {
            return 1;
        }
    }
    return 0;
    
}
/**
 * @brief Detects the quad and checks the status of LED within the quad 
 * using thresholding of images.
 * 
 * @return int status of the led (ON/OFF) or -1 if no quad detected
 */

int detect(Mat* cam_frame)
{
        Mat gray;

        // Convert the image buffer to a CV matrix to support CV functions
        cvtColor(*cam_frame, gray, COLOR_BGR2GRAY);
        // Perform Gaussian blur with kernel size 5 to reduce image noise
        GaussianBlur(gray, gray, Size(BLUR_KERNEL_SIZE, BLUR_KERNEL_SIZE), 
                    BORDER_DEFAULT);
        // Thresholding to detect bright spots
        threshold(gray,gray, THRESH_VAL, THRESH_MAX, THRESH_BINARY);

        // Quad Detection code 
        vector<vector<Point> > contours;
        // Detect contours . RETR_LIST returns all polygons
        findContours(gray, contours, RETR_LIST, CHAIN_APPROX_NONE );
        vector<vector<Point> > contours_poly( contours.size() );
        // For visualization of quad detection within the frame
        vector<Rect> boundRect( contours.size());
        Rect Rect_cp;
        
        // Mat drawing = Mat::zeros(gray.size(), CV_8UC3 );
        int smallestContourIdx = -1;
        float smallestContourArea = FLT_MAX;
        /* Contour detection can pick up frame boundary as a contour, so pick
            the smallest bounding box */
        for( size_t i = 0; i < contours.size(); i++ )
        {
             // Group all the polynomials detected
            double peri = arcLength(contours[i],true);
            float coefficient = 0.05;
            approxPolyDP( contours[i], contours_poly[i],coefficient*peri, true );
            // If it is a 4 sided figure
            if(contours_poly[i].size()==4)
            {
                // Sets aspect ratio to detect only rectangles
                Rect_cp = boundingRect( contours_poly[i] );
                double asp_ratio = (double)Rect_cp.width/(double)Rect_cp.height;
                if(asp_ratio<0.95 || asp_ratio >1.05)
                {
                    float ctArea= cv::contourArea(contours_poly[i]);
                    if(ctArea<smallestContourArea)
                    {
                        smallestContourArea = ctArea;
                        smallestContourIdx = i;
                    }
                    boundRect[i] = Rect_cp;
                }
            }    
        }
        /* Now that the bounding box is updated, draw rectangle for the smallest
            contour , only for visualisation !! */
        // for( size_t i = 0; i< contours.size(); i++ )
        // {
        //     Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), 
        //                            rng.uniform(0,256) );
        //     //drawContours( drawing, contours_poly, (int)i, color );
        //     if(i==smallestContourIdx)
        //         rectangle( gray, boundRect[i].tl(), 
        //                        boundRect[i].br(), color, 2 );
        // }
        // try
        // {
        //     imshow("Bright Spots",gray);
        // }
        // catch(exception& e)
        // {
        //     cout<<"Exception :" << e.what() << endl;
        // }
        
        // waitKey(10) ;

        // Check if quad exist and if so, check led status and return it
        int idx = smallestContourIdx;
        if(idx>=0)
        {
            // Scale the frame to set ROI within the bounding box 

            x1_ = boundRect[idx].x + boundRect[idx].width*0.3;
            y1_ = boundRect[idx].y + boundRect[idx].height*0.3;
            x2_ = boundRect[idx].x + boundRect[idx].width*0.8;
            y2_ = boundRect[idx].y + boundRect[idx].height*0.8;
                    
            if(x1_>0 && y1_>0 && x2_ > 0 && y2_ > 0)
            {
                
                Rect myRoi(x1_,y1_,abs(x2_-x1_),abs(y2_-y1_));
                // Check the dimension constraints
                if(myRoi.x >= 0 && myRoi.y >= 0 && myRoi.width + myRoi.x < 
                cam_frame->cols && myRoi.height + myRoi.y < cam_frame->rows
                && myRoi.height>0 && myRoi.width>0)
                {
                    Mat cam_frame_cp = *cam_frame;
                    Mat cropped_img = cam_frame_cp(myRoi);
                    Mat frame_grey;
                    // Color to greyscale
                    cvtColor(cropped_img, frame_grey, COLOR_BGR2GRAY);
                    // Remove white noises in the image
                    GaussianBlur(frame_grey, frame_grey, Size(BLUR_KERNEL_SIZE, 
                                BLUR_KERNEL_SIZE), BORDER_DEFAULT);
                    // Binary thresholing to detect bright spots in the image
                    threshold(frame_grey,frame_grey, THRESH_VAL, THRESH_MAX, 
                                THRESH_BINARY);
                    // Update the led status of current frame
                    *cam_frame = frame_grey;
                    // Get led status
                    bool res = update_led_status(frame_grey);
                    return res;

                }
                return -1;
            }
            return -1;
        }
        else
        {
            return -1;
        }
}
/**
 * @brief Checks the cropped ROI image for any contours within the ROI.
 * 
 * @return bool status of the led (ON/OFF)
 */

bool update_led_status(Mat thresh_img )
{    
    vector<vector<Point> > contours;
    // Find contours within the cropped image. 
    findContours(thresh_img, contours, RETR_TREE, CHAIN_APPROX_SIMPLE );
    // Bounding box also detected as a contour
    if(contours.size()==2)
        return LED_ON;
    else
        return LED_OFF;
} 

