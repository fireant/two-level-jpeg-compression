//g++ lodepng.cpp toojpeg.cpp jpgd.cpp main.cpp ./libtf.a -std=c++11 -pedantic -Wall -Wextra -O3 

#include "lodepng.h"
#include "toojpeg.h"
#include "jpgd.h"
#include "tf_sdk.h"
#include "tf_data_types.h"

#include <iostream>
#include <fstream>

std::ofstream myFile("compressed.jpg", std::ios_base::out | std::ios_base::binary);

void myOutput(unsigned char byte) {
    myFile << byte;
}

void decodeOneStep(const char* filename) {
    Trueface::SDK tfSdk;
    tfSdk.setLicense("************* REPLACE THIS WITH A VALID LICENSE KEY ****************");
    tfSdk.setImage(filename);
    Trueface::FaceBoxAndLandmarks faceBoxAndLandmarks;
    bool found;
    tfSdk.detectLargestFace(faceBoxAndLandmarks, found);
    std::cout<<"bx "<<faceBoxAndLandmarks.bottomRight.x<<std::endl;

    std::vector<unsigned char> image; //the raw pixels
    unsigned width, height;

    //decode
    unsigned error = lodepng::decode(image, width, height, filename, LCT_RGB);

    //if there's an error, display it
    if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

    const bool isRGB      = true;  // true = RGB image, else false = grayscale
    const auto quality    = 10;    // compression quality: 0 = worst, 100 = best, 80 to 90 are most often used
    const bool downsample = false; // false = save as YCbCr444 JPEG (better quality), true = YCbCr420 (smaller file)
    const char* comment = "Two-level compression test"; // arbitrary JPEG comment
    int left = faceBoxAndLandmarks.topLeft.x;
    int top = faceBoxAndLandmarks.topLeft.y;
    int right = faceBoxAndLandmarks.bottomRight.x;
    int bottom = faceBoxAndLandmarks.bottomRight.y;
    TooJpeg::writeJpeg(myOutput, image.data(), width, height, isRGB, quality, downsample, comment, 
                       left, top, right, bottom);

    int actual_comps;
    int req_comps = 3;
    
    myFile.flush();
    myFile.close();

    unsigned char * decompressed = jpgd::decompress_jpeg_image_from_file("compressed.jpg", (int*)&width, (int*)&height, 
                                        &actual_comps, req_comps, 0, left, top, right, bottom);
    
    lodepng_encode24_file("decoded.png",
                          decompressed, width, height);
}

int main(int argc, char *argv[]) {
  const char* filename = argc > 1 ? argv[1] : "example_small.png";

  decodeOneStep(filename);
}
