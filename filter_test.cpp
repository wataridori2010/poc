
/*Visual Studioとの互換性確保 */
#define _CRT_SECURE_NO_WARNINGS

/*ヘッダファイルのインクルード*/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
//#include <opencv2/videoio.hpp>
#include <opencv/cv.h>
#include <opencv/cxcore.h>

#include <opencv/highgui.h>


//using namespace std;
using namespace cv;


/****************/
/*  main()関数  */
/****************/
int main()
{
    
    // 初期化時に塗りつぶす
    //    cv::Mat red_img(cv::Size(640, 480), CV_8UC3, cv::Scalar(0,0,255));
    //    cv::Mat white_img(cv::Size(640, 480), CV_8UC3, cv::Scalar::all(255));
    //    cv::Mat black_img = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);
    //    // 初期化後に塗りつぶす
    //    cv::Mat green_img = red_img.clone();
    //    green_img = cv::Scalar(0,255,0);
    //    cv::imshow("red", red_img);
    //    cv::waitKey(10);
    
    
    //1系読み込み
    IplImage *ref =cvLoadImage("/Users/itoyuichi/github/playground/DeepLeaningFromScratch/Beanbags/frame10.png");
    IplImage *tgt =cvLoadImage("/Users/itoyuichi/github/playground/DeepLeaningFromScratch/Beanbags/frame11.png");
    IplImage *det =cvCreateImage(cvSize(ref->width,ref->height),8,1);

    int ref_bgr[3];
    int tgt_bgr[3];
    
    for(int j=0;j<ref->height;j++){
        for(int i=0;i<ref->width;i++){

            int pt_g=j* det->widthStep + i;
            int pt_c=j*ref->widthStep + 3*i;
            
            ref_bgr[0] = (unsigned char)ref->imageData[pt_c+0];
            ref_bgr[1] = (unsigned char)ref->imageData[pt_c+1];
            ref_bgr[2] = (unsigned char)ref->imageData[pt_c+2];
            tgt_bgr[0] = (unsigned char)tgt->imageData[pt_c+0];
            tgt_bgr[1] = (unsigned char)tgt->imageData[pt_c+1];
            tgt_bgr[2] = (unsigned char)tgt->imageData[pt_c+2];

            int diff =
            abs(ref_bgr[0] - tgt_bgr[0])+
            abs(ref_bgr[1] - tgt_bgr[1])+
            abs(ref_bgr[2] - tgt_bgr[2]);
            
            det->imageData[pt_g]=diff;
        }
    }
    cvShowImage("det", det);
    cvWaitKey(0);

    //cvSaveImage("det.bmp",det);
    //ガンマ
    float *gamma = (float*)calloc(sizeof(float), 1024);
    for(int i=0;i<1024;i++){
        gamma[i] = pow((i/1024),(1/2.4));
    }
    
    //2系
    // Users/itoyuichi/github/playground/DeepLeaningFromScratch/ch1/
    //    VideoCapture cap("/Users/itoyuichi/github/playground/DeepLeaningFromScratch/ch1/big_buck_bunny.mp4"); // open the video file for reading
    //    printf("1...");
    //    if ( !cap.isOpened() )  // if not success, exit program
    //    {
    //        return -1;
    //    }
    //    printf("2...");
    //    cap.set(CV_CAP_PROP_POS_MSEC, 300); //start the video at 300ms
    //
    //    double fps = cap.get(CV_CAP_PROP_FPS); //get the frames per seconds of the video
    //    namedWindow("MyVideo",CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
    //    printf("3...");
    //    while(1)
    //    {
    //        printf("4...");
    //        Mat frame;
    //
    //        bool bSuccess = cap.read(frame); // read a new frame from video
    //
    //        if (!bSuccess) //if not success, break loop
    //        {
    //            break;
    //        }
    //        imshow("MyVideo", frame); //show the frame in "MyVideo" window
    //        waitKey(10);
    //    }
    //
    //
    //    while(fgets(linebuf,BUFSIZE,stdin)!=NULL){
    //        /*行の読み取りが可能な間繰り返す*/
    //        if(sscanf(linebuf,"%lf",&data)!=0){/*変換できたら*/
    //            sum+=data ;
    //            sum2+=data*data ;
    //            printf("%lf\t%lf\n",sum,sum2) ;
    //        }
    //    }
    
    
    return 0 ;
}
