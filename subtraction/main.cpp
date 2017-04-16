//
//  main.cpp
//  backgroundsubtraction
//
//  Created by Ito Yuichi on 2017/04/13.
//  Copyright © 2017年 Ito Yuichi. All rights reserved.
//
#include <opencv/cv.hpp>
#include <opencv/cxcore.hpp>
#include <opencv/highgui.h>

#include <iostream>

//#include "maxflow-v3.01/graph.h"
#include "graph.h"

//typedef Graph<int, int, int> GraphType;


int Denoise(IplImage* img, IplImage* result)
{
    // 入力画像情報
    int width = img->width;
    int height = img->height;
    int wstep = img->widthStep;
    int bpp = ((img->depth & 255) / 8) * img->nChannels;
    unsigned char* image = (unsigned char*)img->imageData;
    unsigned char* image_result = (unsigned char*)result->imageData;
    // parameter
    int lambda = 10;//90;
    int kappa = 25;
    // グラフクラス (ノードサイズとエッジのサイズの大まかな数を指定)
    typedef Graph<int, int, int> GraphType;
    GraphType* g = new GraphType(width*height, width*height*4);
    // ノードの作成
    g->add_node(width*height);
    int dx[4] = {1, -1, 0, 1};
    int dy[4] = {0, 1, 1, 1};
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            // データ項の計算
            //            int data1 = ( bin(image[ i*wstep + j*bpp]) ) * lambda;
            //            int data2 = (1 - bin(image[ i*wstep + j*bpp]) ) * lambda;
            int data1 = ( 0.01*(image[ i*wstep + j*bpp]) ) * lambda;
            int data2 = (1 - 0.01*(image[ i*wstep + j*bpp]) ) * lambda;
            
            g->add_tweights( i*width + j, data1, data2);
            // 平滑化項の計算
            for(int d = 0; d < 4; d++){
                int x = j+dx[d];
                int y = i+dy[d];
                if(0 <= x && 0 <= 0 && x < width && y < height){
                    g->add_edge( i*width + j, y*width + x,kappa, kappa);
                }
            }
        }
    }
    //maxflow/mincut
    int flow = g->maxflow();
    // 出力結果
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(g->what_segment( i*width + j ) == GraphType::SOURCE){
                image_result[ i*wstep + j*bpp] = 255;
            }
            else{
                image_result[ i*wstep + j*bpp] = 0;
            }
        }
    }
    delete g;
    return 0;
}


int main(int argc, const char * argv[]) {
    
    const int numImage=4;
    
    char* filename[numImage] ={
      "/Users/itoyuichi/github/playground/BackgroundSubtraction/IMG_3586.PNG",
      "/Users/itoyuichi/github/playground/BackgroundSubtraction/IMG_3587.PNG",
      "/Users/itoyuichi/github/playground/BackgroundSubtraction/IMG_3588.PNG",
      "/Users/itoyuichi/github/playground/BackgroundSubtraction/IMG_3590.PNG"
    };
    
    IplImage **src =(IplImage **)calloc(numImage,sizeof(IplImage*));
    
    for(int i=0; i<numImage; i++){
        
        // image loading
        src[i]= cvLoadImage( filename[i], CV_LOAD_IMAGE_COLOR );
        //smoothing
        cvSmooth(src[i],src[i],CV_BLUR,3,3);
        cvCvtColor(src[i], src[i], CV_BGR2YCrCb);
        
        //cvShowImage("window", src[i] );
        //cvWaitKey(200);
    }

    // distance map
    float *distance_map =(float *)calloc(src[0]->width*src[0]->height,sizeof(float));

    float *distance_map_min =(float *)calloc(src[0]->width*src[0]->height,sizeof(float));
    int im_size     = src[0]->width*src[0]->height;
    int im_width    = src[0]->width;
    int im_height   = src[0]->height;

    IplImage *distance_map_ipl = (IplImage*)cvCreateImage(cvSize(im_width,im_height), IPL_DEPTH_8U, 1);
    IplImage *distance_map_min_ipl = (IplImage*)cvCreateImage(cvSize(im_width,im_height), IPL_DEPTH_8U, 1);
    IplImage *average_map_ipl = (IplImage*)cvCreateImage(cvSize(im_width,im_height), IPL_DEPTH_8U, 3);
    IplImage *distance_map_max_id = (IplImage*)cvCreateImage(cvSize(im_width,im_height), IPL_DEPTH_8U, 1);
    
    // variance
    for(int s=0; s<im_height; s++){
        for(int t=0; t<im_width; t++){
            
            float avg = 0;
            float var = 0;
            float ycc_sum[3] = {0,0,0};
            
            int address_gray  = s*distance_map_ipl->widthStep + t;
            int address_color = s*src[0]->widthStep + src[0]->nChannels*t;
            
            for(int i=0; i<numImage; i++){

                int val = (unsigned char)src[i]->imageData[address_color + 0];
                    
                avg += val;
                var += val*val;
                
                for(int j=0; j<3; j++){
                        ycc_sum[j] += (unsigned char)src[i]->imageData[address_color + j];
                }
            }
            avg = avg/numImage;
            var = var/numImage;
            var = var - avg*avg;
            int v=(0.1*var);
            if(v>255) v= 255;
            else if(v<0) v =0;
            distance_map_ipl->imageData[address_gray]=(unsigned char)v;
            
            for(int j=0; j<3; j++){
                ycc_sum[j] = ycc_sum[j]/numImage;
                average_map_ipl->imageData[address_color+j] =(unsigned char)ycc_sum[j];
            }
        }
    }

    IplImage *distance_map_max_ipl=cvCloneImage(distance_map_min_ipl);
    IplImage *distance_map_max_ipl2=cvCloneImage(distance_map_min_ipl);
    cvSetZero(distance_map_max_ipl);
    
    // min distance
    for(int s=0; s<im_height; s++){
        for(int t=0; t<im_width; t++){
            
            float min_val = 3*65540;
            
            int address_gray  = s*distance_map_ipl->widthStep + t;
            int address_color = s*src[0]->widthStep + src[0]->nChannels*t;
            
            // min distance of each pairs
            for(int i=0; i<numImage; i++){
                for(int j=i+1; j<numImage; j++){

                    int diff = 0;
                    for(int k=0; k<3; k++){
                        int v_temp =(unsigned char)src[i]->imageData[address_color + k];
                        int v_targ =(unsigned char)src[j]->imageData[address_color + k];
                        diff+=(v_temp - v_targ)*(v_temp - v_targ);
                    }
                    
                    if(diff < min_val) min_val = diff;
                }
            }
            int v=(0.1*min_val);
            if(v>255) v= 255;
            else if(v<0) v =0;
            distance_map_min_ipl->imageData[address_gray] = (unsigned char)v;
            
            if((unsigned char)distance_map_min_ipl->imageData[address_gray]==0)
            {
                if((unsigned char)distance_map_ipl->imageData[address_gray]> 20){
                    distance_map_max_ipl->imageData[address_gray] = (unsigned char)255;
                }
                
            }else if((unsigned char)distance_map_ipl->imageData[address_gray]
                > 3*(unsigned char)distance_map_min_ipl->imageData[address_gray]){
                
                distance_map_max_ipl->imageData[address_gray] = (unsigned char)255;
            }
            
            // distance from average
            float max_val = 0;
            float max_id  = 0;
            
            for(int i=0; i<numImage; i++){
                
                int diff = 0;
                for(int k=0; k<3; k++){
                    int v_temp =(unsigned char)average_map_ipl->imageData[address_color + k];
                    int v_targ =(unsigned char)src[i]->imageData[address_color + k];
                    diff+=(v_temp - v_targ)*(v_temp - v_targ);
                }
                
                if(diff > max_val){
                    max_val = diff;
                    max_id  = i;
                }
            }
            distance_map_max_id->imageData [address_gray] = (unsigned char)max_id;

        }
    }

    // region growing
    int rad = 7;
    for(int s=rad; s<im_height-rad; s++){
        for(int t=rad; t<im_width-rad; t++){
            
            int address_gray_  = s*distance_map_ipl->widthStep + t;
            int address_color_ = s*src[0]->widthStep + src[0]->nChannels*t;
            

            if((unsigned char)distance_map_max_ipl->imageData[address_gray_]){

                int max_id = distance_map_max_id->imageData [address_gray_];
                int ref_color =(unsigned char)src[max_id]->imageData[address_color_ + 0];
            
                for(int i=-rad; i<rad+1; i++){
                    for(int j=-rad; j<rad+1; j++){
                    
                        int address_gray  = (s+i)*distance_map_ipl->widthStep + t+j;
                        int address_color = (s+i)*src[max_id]->widthStep + src[max_id]->nChannels*(t+j);
                    
                        int tgt_color =(unsigned char)src[max_id]->imageData[address_color + 0];
                        int diff= abs(tgt_color - ref_color);
                    
                        if(diff < 10){
                            distance_map_max_ipl2->imageData[address_gray] = (unsigned char)255;
                        }
                    }
                }
            }
            
        }
    }

    cvCvtColor(average_map_ipl, average_map_ipl, CV_YCrCb2BGR);
    cvSaveImage("/Users/itoyuichi/github/playground/BackgroundSubtraction/average.PNG", average_map_ipl);
    
    cvSaveImage("/Users/itoyuichi/github/playground/BackgroundSubtraction/dst.PNG", distance_map_ipl);
    cvSaveImage("/Users/itoyuichi/github/playground/BackgroundSubtraction/dst_min.PNG", distance_map_min_ipl);
    cvSaveImage("/Users/itoyuichi/github/playground/BackgroundSubtraction/dst_max.PNG", distance_map_max_ipl);
    cvSaveImage("/Users/itoyuichi/github/playground/BackgroundSubtraction/dst_max2.PNG", distance_map_max_ipl2);
    
    // graphcut
    Denoise(distance_map_max_ipl, distance_map_max_ipl2);
    cvSaveImage("/Users/itoyuichi/github/playground/BackgroundSubtraction/dst_max3.PNG", distance_map_max_ipl2);
    
    // insert code here...
    std::cout << "Done!\n";
    //cvReleaseImage( &src );
    return 0;
}



