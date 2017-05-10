////////////////////////////////////////////////////////////////
//	2010.06.18
////////////////////////////////////////////////////////////////


//#include "cv.h"
//#include "cxcore.h"
//#include "highgui.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include "opencv\\highgui.h"
//#include "opencv\\cv.h"
//#include "opencv\\cxcore.h"

#include <opencv/cv.hpp>
#include <opencv/cxcore.hpp>
#include <opencv/highgui.h>
#include <iostream>


#include <time.h>
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define bilatelal	//
//#define epsilon	//
#include "fileload.h"



#define PFRAME 1//

//int pyr3DNR(unsigned int **src, int width, int height){
IplImage* pyr3DNR(IplImage **img1, int width, int height){
    
    int width1=width/1.0;
    int width2=width/2.0;
    int width4=width/4.0;
    int width8=width/8.0;
    
    int height1=height/1.0;
    int height2=height/2.0;
    int height4=height/4.0;
    int height8=height/8.0;
    
    float sigmax=10;
    int Xth=3;
    
    float sigmad1=50; //
    int Dth1=100;	////45
    
    
    //IplImage **img1, **img4, **img16;
    IplImage **img4, **img16, **img64;
    
    //img1 = (IplImage **) cvAlloc (sizeof (IplImage *) * PFRAME);
    img4 = (IplImage **) cvAlloc (sizeof (IplImage *) * PFRAME);
    img16 = (IplImage **) cvAlloc (sizeof (IplImage *) * PFRAME);
    img64 = (IplImage **) cvAlloc (sizeof (IplImage *) * PFRAME);
    IplImage *re1, *re4, *re16, *re64;
    IplImage *noise1,*noise4,*noise16, *noise64;
    IplImage *dst;
    
    //1/1
    re1 = cvCreateImage(cvSize(width1,height1), IPL_DEPTH_8U, 1);
    //for(int num=0; num<2; num++)		img1[num]		 = cvCloneImage(pColorPhotoI);
    noise1 = cvCreateImage(cvSize(width1,height1), IPL_DEPTH_8U, 1);
    dst	= cvCreateImage(cvSize(width1,height1), IPL_DEPTH_8U, 1);
    
    
    //1/4
    re4=cvCreateImage(cvSize(width2,height2), IPL_DEPTH_8U, 1);
    cvResize(img1[0],re4,CV_INTER_LINEAR);
    for(int num=0; num<PFRAME; num++){
        img4[num]		 = cvCloneImage(re4);//
        cvResize(img1[num],img4[num]);
    }
    noise4 = cvCreateImage(cvSize(width2,height2), IPL_DEPTH_8U, 1);
    
    //1/16
    re16=cvCreateImage(cvSize(width4,height4), IPL_DEPTH_8U, 1);
    cvResize(img1[0],re16,CV_INTER_LINEAR);
    for(int num=0; num<PFRAME; num++){
        img16[num]		 = cvCloneImage(re16);//
        cvResize(img1[num],img16[num]);
    }
    noise16 = cvCreateImage(cvSize(width4,height4), IPL_DEPTH_8U, 1);

    //1/64
    re64=cvCreateImage(cvSize(width8,height8), IPL_DEPTH_8U, 1);
    cvResize(img1[0],re64,CV_INTER_LINEAR);
    for(int num=0; num<PFRAME; num++){
        img64[num]		 = cvCloneImage(re64);//
        cvResize(img1[num],img64[num]);
    }
    noise64 = cvCreateImage(cvSize(width8,height8), IPL_DEPTH_8U, 1);

    unsigned char *x_coef;
    unsigned char *d_coef;
    
    d_coef=(unsigned char*)calloc(512,sizeof(unsigned char));
    
    for(int num=0;num<512;num++){
        if(	(num-255)>-(Dth1)	&&	(num-255)<(Dth1)){
            d_coef[num]=255*exp(( (-1)*(255-num)*(255-num) )/(2*sigmad1*sigmad1));//bilatelal
        }
        else{
            d_coef[num]=0;
        }
    }
    
    x_coef=(unsigned char*)calloc(121,sizeof(unsigned char));
    
    for(int s=0;s<11;s++){
        for(int t=0;t<11;t++){
            if(	(s-5)>-(Xth)	&&	(s-5)<(Xth)	&&	(t-5)>-(Xth)	&&	(t-5)<(Xth)		){
                x_coef[s*11+t]=255*exp(( (-1)*((5-s)*(5-s)+(5-t)*(5-t)) )/(2*sigmax*sigmax));
            }else{
                x_coef[s*11+t]=0;
            }
        }}
    
    
    unsigned char t1,v1;
    int s,t;
    
    double rgb;
    double dv3;
    double sum_p;
    double sum=0;
    
    unsigned char xv=0;
    unsigned char tb;
    
    
    int address0=0;
    int address1=0;
    int address2=0;
    int address3=0;
    int ttt1;
    
    
    cvResize(img1[0],img64[0],CV_INTER_LINEAR);
    
    //1/64
    address0=0;
    address1=0;
    
    for(int i=0;i<height8;i++){
        
        address0=i*img64[0]->widthStep;
        
        for(int j=0;j<width8;j++){
            
            address1=address0 + j;
            
            t1=(unsigned char)img64[0]->imageData[address1];
            noise64->imageData[address1]=0;
            
            
            tb=t1;
            
            sum=0;
            rgb=0;sum_p=0;
            
            for(int num=0;num<PFRAME;num++){
                
                address2=address1-2*img64[0]->widthStep;//(2,2)
                for(s=-2;s<3;s++){
                    
                    
                    address3=address2-2;
                    for(t=-2;t<3;t++){
                        
                        if( (address3>=0)&&(address3<=img64[num]->widthStep*img64[num]->height) ){
                            
                            xv=x_coef[(s+5)*11+(t+5)];
                            v1=(unsigned char)img64[num]->imageData[address3 +0];
                            dv3=xv*d_coef[255+(v1-t1)];
                            rgb+=dv3*v1;
                            sum_p+=dv3;
                            
                        }
                        address3++;
                        
                    }//t
                    address2+=img64[0]->widthStep;
                    
                }//s
            }
            
            //Y
            if(sum_p>0){ dv3=sum_p/4; tb=(rgb +dv3)/sum_p;}
            re64->imageData[address1    ]=tb;
            noise64->imageData[address1 ]=(128+tb-t1);
            
        }}
    cvResize(noise64,noise16,CV_INTER_LINEAR);
    
    cvNamedWindow("src");
    cvShowImage("src",img64[0]);
    cvNamedWindow("re");
    cvShowImage("re",re64);
    
    cvWaitKey(0);

    
    //------------------------------------------------------------------------------
    
    cvResize(img1[0],img16[0],CV_INTER_LINEAR);
    //cvZero(re_img2);cvZero(noise_img2);
    
//    cvCopy(img16[0],re16);
    
    cvNamedWindow("src1");
    cvShowImage("src1",img16[0]);
    cvWaitKey(0);
    
    
    
    address1=0;
    address0=0;
    for(int i=0;i<height4;i++){
        
        address1=address0;
        for(int j=0;j<width4;j++){
            
            ttt1=(unsigned char)img16[0]->imageData[address1]+(unsigned char)noise16->imageData[address1]-128;
            if(ttt1>255){t1=255;}else if(ttt1<0){t1=0;}else{t1=ttt1;}
            img16[0]->imageData[address1 +0]=(unsigned char)t1;
            
            address1+=1;
        }
        address0+=img16[0]->widthStep;
    }
    
    cvNamedWindow("src2");
    cvShowImage("src2",img16[0]);
    cvWaitKey(0);
    
    //1/16
    address0=0;
    address1=0;
    
    for(int i=0;i<height4;i++){
        
        address0=i*img16[0]->widthStep;
        
        for(int j=0;j<width4;j++){
            
            address1=address0 + j;
            
            t1=(unsigned char)img16[0]->imageData[address1];
            noise16->imageData[address1]=0;
            
            
            tb=t1;
            
            sum=0;
            rgb=0;sum_p=0;
            
            for(int num=0;num<PFRAME;num++){
                
                address2=address1-2*img16[0]->widthStep;//(2,2)
                for(s=-2;s<3;s++){
                    
                    
                    address3=address2-2;
                    for(t=-2;t<3;t++){
                        
                        if( (address3>=0)&&(address3<=img16[num]->widthStep*img16[num]->height) ){
                            
                            xv=x_coef[(s+5)*11+(t+5)];
                            v1=(unsigned char)img16[num]->imageData[address3 +0];
                            dv3=xv*d_coef[255+(v1-t1)];
                            rgb+=dv3*v1;
                            sum_p+=dv3;
                            
                        }
                        address3++;
                        
                    }//t
                    address2+=img16[0]->widthStep;
                    
                }//s
            }
            
            //Y
            if(sum_p>0){ dv3=sum_p/4; tb=(rgb +dv3)/sum_p;}
            re16->imageData[address1    ]=tb;
            noise16->imageData[address1 ]=(128+tb-t1);
            
    }}
    cvResize(noise16,noise4,CV_INTER_LINEAR);
    cvNamedWindow("noise4");
    cvShowImage("noise4",noise4);

    
    cvNamedWindow("src");
    cvShowImage("src",img16[0]);
    cvNamedWindow("re");
    cvShowImage("re",re16);
    
    cvWaitKey(0);
    
    //------------------------------------------------------------------------------
    
    cvResize(img1[0],img4[0],CV_INTER_LINEAR);
//    cvCopy(img4[0],re4);
    
    cvNamedWindow("src1");
    cvShowImage("src1",img4[0]);
    cvWaitKey(0);

    
    address1=0;
    address0=0;
    for(int i=0;i<height2;i++){
        
        address1=address0;
        for(int j=0;j<width2;j++){
            
            ttt1=(unsigned char)img4[0]->imageData[address1 +0]+(unsigned char)noise4->imageData[address1 +0]-128;
            if(ttt1>255){t1=255;}else if(ttt1<0){t1=0;}else{t1=ttt1;}
            img4[0]->imageData[address1 +0]=(unsigned char)t1;
            
            address1+=1;
        }
        address0+=img4[0]->widthStep;
    }
    
    cvNamedWindow("src2");
    cvShowImage("src2",img4[0]);
    cvWaitKey(0);
    
    //------------------------------------------------------------------------------
    address0=0;
    address1=0;
    
    for(int i=0;i<height2;i++){
        
        address0=i*img4[0]->widthStep;
        
        for(int j=0;j<width2;j++){
            
            address1=address0 + 3*j;
            
            t1=(unsigned char)img4[0]->imageData[address1 +0];
            
            tb=t1;
            sum=0;
            rgb=0;sum_p=0;
            
            for(int num=0;num<PFRAME;num++){
                
                address2=address1-2*img16[0]->widthStep;
                for(s=-2;s<3;s++){
                    
                    address3=address2-2;
                    for(t=-2;t<3;t++){
                        
                        
                        if( (address3>=0)&&(address3<=img4[num]->widthStep*img4[num]->height) ){
                            
                            xv=x_coef[(s+5)*11+(t+5)];
                            v1=(unsigned char)img4[num]->imageData[address3];
                            dv3=xv*d_coef[255+(v1-t1)];
                            rgb+=dv3*v1;
                            sum_p+=dv3;
                            
                        }
                        address3+=1;
                        
                    }//t
                    address2+=img4[0]->widthStep;
                    
                }//s
            }
            
            //Y
            if(sum_p>0){ dv3=sum_p/4; tb=(rgb +dv3)/sum_p;}
            
            re4->imageData[address1     ]=tb;
            noise4->imageData[address1  ]+=(tb-t1);
            
        }}
    
//    cvCopy(re4, img4[1]);
    
    cvResize(noise4,noise1,CV_INTER_LINEAR);
    
    cvNamedWindow("src");
    cvShowImage("src",img4[0]);
    cvNamedWindow("re");
    cvShowImage("re",re4);
    
    cvWaitKey(0);
    

    //------------------------------------------------------------------------------
    
    address1=0;
    address0=0;
    for(int i=0;i<height1;i++){
        
        address1=address0;
        
        for(int j=0;j<width1;j++){
            
            ttt1=(unsigned char)img1[0]->imageData[address1]+(unsigned char)noise1->imageData[address1]-128;
            if(ttt1>255){t1=255;}else if(ttt1<0){t1=0;}else{t1=ttt1;}
            img1[0]->imageData[address1 +0]=(unsigned char)t1;
            
            address1+=1;
        }
        address0+=img1[0]->widthStep;
    }

    address1=0;
    address0=0;
        for(int i=0;i<height1;i++){
            
            address0=i*img1[0]->widthStep;
            
            for(int j=0;j<width1;j++){
                
                
                address1=address0 + j;
                
                t1=(unsigned char)img1[0]->imageData[address1 +0];
                tb=t1;
                
                sum=0;
                rgb=0;sum_p=0;
                for(int num=0;num<PFRAME;num++){
                    
                    address2=address1-img1[0]->widthStep;
                    for(s=-1;s<2;s++){
                        
                        address3=address2-1;
                        for(t=-1;t<2;t++){
                            
                            
                            if( (address3>=0)&&(address3<=img1[num]->widthStep*img1[num]->height) ){
                                
                                xv=x_coef[(s+5)*11+(t+5)];
                                v1=(unsigned char)img1[num]->imageData[address3];
                                dv3=xv*d_coef[255+(v1-t1)];
                                rgb+=dv3*v1;
                                sum_p+=dv3;
                                
                            }
                            address3+=1;
                            
                        }//t
                        address2+=img1[0]->widthStep;
                        
                    }//s
                }
                
                //Y
                if(sum_p>0){ dv3=sum_p/4; tb=(rgb +dv3)/sum_p;}
                
                dst->imageData[address1 +0]=tb;
            }
        }
    cvNamedWindow("src");
    cvShowImage("src",img1[0]);
    cvNamedWindow("re");
    cvShowImage("re",dst);
    
    cvWaitKey(0);


    //cvReleaseImage(img1);
    cvReleaseImage(img4);
    cvReleaseImage(img16);
    cvReleaseImage(&re1);
    cvReleaseImage(&re4);
    cvReleaseImage(&re16);
    cvReleaseImage(&noise1);
    cvReleaseImage(&noise4);
    cvReleaseImage(&noise16);
    //cvReleaseImage(&dst);


    free(d_coef);
    free(x_coef);

    return dst;
}

int main(int argc, char** argv){

  char* filename[PFRAME] ={
        "/Users/itoyuichi/github/playground/BackgroundSubtraction/lena.jpg",
//        "/Users/itoyuichi/github/playground/BackgroundSubtraction/lena_80p.png"
  };
    
    IplImage**src;
    src = (IplImage **) cvAlloc (sizeof (IplImage *) * PFRAME);
    src[0] = cvLoadImage(filename[0],0);
    src[1] = cvLoadImage(filename[0],0);
//    IplImage*src = cvLoadImage(filename[1]);
    
//    pyr3DNR(unsigned int **src, int width, int height){
    IplImage*dst=pyr3DNR(src, src[0]->width, src[0]->height);
    
    cvvNamedWindow("dst");
    cvvShowImage("dst",dst);
    cvWaitKey(0);
    
    return 1;

}

