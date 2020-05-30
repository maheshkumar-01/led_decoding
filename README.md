# led_decoding
The detect() function defined in the ledpattern_detect.cpp currently detects a 
quad in the frame and checks for the LED status within the quad if quad exists
within the frame. The status of the LED is then returned back to the caller.

The test_mock.cpp file mocks the functionality of webassembly module. This function
currently reads the frames from an input gif video and feeds it to the detect function.
The frames are currently read every 33 ms. The video is encoded with the pattern 0xC93.

The test_mock aggregates 12 bits of detection then calls check_if_pattern_exists to 
check if the pattern is indeed the encoded value. 

This repo can be used to test the native code. The native code is integrated along with the
WASM in this repo https://github.com/maheshkumar-01/ledPatternWasm
