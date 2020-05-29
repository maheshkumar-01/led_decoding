/* 
    This file contains test functions for testing the quad detection and led
    status detection code. This file mocks the functionality of the web 
    assembly program
*/
#include <ctime>
#include <signal.h>
#include <sys/time.h>

#include <chrono>
#include "opencv_demo.hpp"

// 1/30fps sampling interval
std::clock_t led_blink_interval_usec = (333);
std::clock_t end_time = clock() + 
                        (led_blink_interval_usec * CLOCKS_PER_SEC)/1000000;
uint32_t pattern = 0;
int led_count = 0;
int main(int argc, char *argv[])
{
    // Capture from gif video. 
    VideoCapture cap("bitpattern.mp4");
    
    while(true)
    {
        Mat frame;
        cap>>frame;
        if(frame.empty())
        {
            // Loop back to beginning of the frame
            cap.set(CV_CAP_PROP_POS_FRAMES, 0);
            cap >> frame;
        }
        if(clock()>end_time)
        {
            end_time = clock() + 
                       (led_blink_interval_usec * CLOCKS_PER_SEC)/1000000;
            int ret_val = detect(&frame);
            // If no quad detected, continue
            if(ret_val ==-1)
                continue;
            pattern = (pattern<<1 )|ret_val;
            led_count++;
            // If 12 bits are obtained, check the pattern
            if(led_count ==12)
            {
                led_count = 0;
                if(check_if_pattern_exists(pattern))
                {
                    printf("\n Pattern %x exists", pattern);
                }
                else
                {
                    printf("\n Pattern %x does not exist", pattern);
                }
                pattern = 0;
                
            }

        }
        else
        {

            continue;
        }
        // Visualization purpose. Displays the frame ROI that contains LED
        // try
        // {
        //     imshow("LED ROI",frame);
        // }
        // catch(exception& e)
        // {
        //     cout<<"Exception :" << e.what() << endl;
        // }
        
        // if (waitKey(10) >= 0)
        //     break;

    }


}