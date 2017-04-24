//
//  main.cpp
//  RIPOC Test Code
//  Created by Ito Yuichi on 2017/04/13.
//  Copyright © 2017年 Ito Yuichi. All rights reserved.
//
#include <opencv/cv.hpp>
#include <opencv/cxcore.hpp>
#include <opencv/highgui.h>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

// based on OpenCV Sample
// http://docs.opencv.org/2.4/doc/tutorials/core/discrete_fourier_transform/discrete_fourier_transform.html
Mat dft_image(Mat I){

    //if( I.empty()) return -1;
    Mat padded;                            //expand input image to optimal size
    int m = getOptimalDFTSize( I.rows );
    int n = getOptimalDFTSize( I.cols ); // on the border add zero values
    copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0));

    Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
    Mat complexI;
    merge(planes, 2, complexI);         // Add to the expanded another plane with zeros

    dft(complexI, complexI);            // this way the result may fit in the source matrix

    // compute the magnitude and switch to logarithmic scale
    // => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
    split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
    magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
    Mat magI = planes[0];

    magI += Scalar::all(1);                    // switch to logarithmic scale
    log(magI, magI);

    // crop the spectrum, if it has an odd number of rows or columns
    magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));

    // rearrange the quadrants of Fourier image  so that the origin is at the image center
    int cx = magI.cols/2;
    int cy = magI.rows/2;

    Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
    Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right

    Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);

    normalize(magI, magI, 0, 1, CV_MINMAX); // Transform the matrix with float values into a viewable image form (float between values 0 and 1).
    
    //imshow("Input Image"       , I   );    // Show the result
    //imshow("spectrum magnitude", magI);
    //waitKey(0);
    
    return magI;
}


// RIPOC Test
int main(int argc, const char * argv[]) {
    
    const int numImage=2;
    
    char* filename[numImage] ={
//        "/Users/itoyuichi/github/playground/BackgroundSubtraction/IMG_3586.PNG",
//        "/Users/itoyuichi/github/playground/BackgroundSubtraction/IMG_3587.PNG"
//       "/Users/itoyuichi/github/playground/BackgroundSubtraction/IMG_3588.PNG",
//       "/Users/itoyuichi/github/playground/BackgroundSubtraction/IMG_3590.PNG"
//        "/Users/itoyuichi/github/playground/BackgroundSubtraction/lenna.jpg",
//        "/Users/itoyuichi/github/playground/BackgroundSubtraction/lenna-rot.jpg"
        "/Users/itoyuichi/github/playground/BackgroundSubtraction/lena.jpg",
        "/Users/itoyuichi/github/playground/BackgroundSubtraction/lena_80p.png"
        //"/Users/itoyuichi/github/playground/BackgroundSubtraction/lena_-90deg.jpg"
    };
    
    cv::Mat curr = imread(filename[0], CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat prev = imread(filename[1], CV_LOAD_IMAGE_GRAYSCALE);
    
    //-------------------------------------------------------------------
    // Estimate Rotation, Scaling facetor
    // DFT
    Mat curr_log_polar, prev_log_polar;
    prev_log_polar=dft_image(prev);
    curr_log_polar=dft_image(curr);
    
    // Log polar (Angle, Scale)
    Point2f center( (float)curr.cols / 2, (float)curr.rows / 2 );
    double radius = (double)curr.cols / 4;
    double M = (double)curr.cols / log(radius);
    
    logPolar(curr_log_polar,curr_log_polar, center, M, INTER_LINEAR + WARP_FILL_OUTLIERS);
    logPolar(prev_log_polar,prev_log_polar, center, M, INTER_LINEAR + WARP_FILL_OUTLIERS);
    
    namedWindow( "curr_Log-Polar", WINDOW_AUTOSIZE );
    imshow("curr_Log-Polar", curr_log_polar );
    namedWindow( "prev_Log-Polar", WINDOW_AUTOSIZE );
    imshow("prev_Log-Polar", prev_log_polar );
    waitKey(0);
    
    cv::Mat hann, prev64f, curr64f;
    createHanningWindow(hann, curr_log_polar.size(), CV_64F);
    
    prev_log_polar.convertTo(prev64f, CV_64F);
    curr_log_polar.convertTo(curr64f, CV_64F);
    Point2d shift_log = phaseCorrelate(prev64f, curr64f, hann);
    
    double scale = 1.0 - shift_log.x / 100;
    double angle = shift_log.y / curr_log_polar.cols * 360;
    printf("scale: %lf, rot: %lf\n",scale,angle);

    //-------------------------------------------------------------------
    // Image Interpolation (rotation , scaling)
    const cv::Mat affine_matrix = cv::getRotationMatrix2D(center, angle, scale );
    std::cout << "affine_matrix=\n" << affine_matrix << std::endl;
    
    Mat prev_interp;
    warpAffine(prev, prev_interp, affine_matrix, prev.size());
    
    namedWindow( "prev_interp", WINDOW_AUTOSIZE );
    imshow("prev_interp", prev_interp );
    waitKey(0);

    //-------------------------------------------------------------------
    // Translation (X,Y)
    //prev.convertTo(prev64f, CV_64F);
    prev_interp.convertTo(prev64f, CV_64F);
    curr.convertTo(curr64f, CV_64F);
    Point2d shift = phaseCorrelate(prev64f, curr64f, hann);
    
    //double radius = cv::sqrt(shift.x*shift.x + shift.y*shift.y);
    printf("dx: %lf, dy: %lf\n",shift.x,shift.y);
    
    return 0;
}




