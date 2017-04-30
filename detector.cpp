
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <tchar.h>
#include <ctype.h>
#include <vector>
#include <iostream>
#include <shlwapi.h>
#include <direct.h>
#include <math.h>
#include "common.h"

#include "harris.h"
#include "susan.h"
#include "fast.h"
#include "klt/klt.h"



void DetHarris(IplImage *src_gray){

		int *imin,*imout;
		int		fsize;
		double	sigma;
		double	rk;
		double	s;
		double	min_d;
		int		l, num, max_c;
		int		*u0, *v0;
		int		opt;

		// defualts
		fsize = Mask_Size;	// 半分を指定．実際は 5x5 のサイズとなる．
		sigma = Sigma;
		rk = K_value;
		s = Rel_r;
		max_c = Max_Corner;
		min_d = Min_Distance;

		//imout
		imin = new int [src->width * src->height];
		imout = new int [src->width * src->height];

		//iminに値代入
		for(int i=0;i<src->height;i++){
			for(int j=0;j<src->width;j++){
				int ad_ipl=i*src->widthStep+j;
				int ad=i*src->width+j;
				imin[ad]=src_gray->imageData[ad];
		}}

	    l = ob_HA.harris_(imin, imout, src->width, src->height, &u0, &v0, &num,
                    fsize, sigma, rk, s, max_c, min_d);

		//コーナー点を出力,描画
		ob_HA.output_corners("corners.txt", u0, v0, num,dst);

		delete imin,imout;

		//Keypoint に値代入
		int n=0;
		while(n<num && n <MAX_NUM){
			keypoints1[n].x=u0[n];
			keypoints1[n].y=v0[n];
			n++;
		}
		size1=n;

}


void DetSusan(IplImage *src_gray){

		uchar  *in, *bp, *mid;
//		float  dt=4.0;
//		int    *r,argindex=3, bt=20, max_no_corners=1850,x_size, y_size;
		float  dt=4.0;
		int    *r,argindex=3, bt=20, max_no_corners=1000,x_size, y_size;

		CORNER_LIST corner_list;


		in = new uchar [src->width * src->height];
		//inに値代入
		for(int i=0;i<src->height;i++){
			for(int j=0;j<src->width;j++){
				int ad_ipl=i*src->widthStep+j;
				int ad=i*src->width+j;
				in[ad]=src_gray->imageData[ad];
		}}
		x_size=src->width;	y_size=src->height;

		r   = new int [src->width * src->height];
		ob_SU.setup_brightness_lut(&bp,bt,6);
		ob_SU.susan_corners(in,r,bp,max_no_corners,corner_list,x_size,y_size);
		//コーナー点を出力,描画
		ob_SU.output_corners(corner_list, dst);

		//Keypoint に値代入
		int n=0;
		while(corner_list[n].info != 7 && n<MAX_NUM){
			keypoints1[n].x=corner_list[n].x;
			keypoints1[n].y=corner_list[n].y;
			n++;
		}
		size1=n;

		delete in,r;

}


void DetFast(IplImage *src_gray){

			uchar  *data=new uchar [src->width * src->height];
		//値代入
		for(int i=0;i<src->height;i++){
			for(int j=0;j<src->width;j++){
				int ad_ipl=i*src->widthStep+j;
				int ad=i*src->width+j;
				data[ad]=(uchar)src_gray->imageData[ad];
		}}
		int xsize=src->width, ysize=src->height;
		int stride=src->width;			//画像の幅
		int b=80;						//検出閾値
		int* ret_num_corners=new int[1];//検出点数を格納する領域
		xy* corners_;					//検出点データ
		corners_= ob_FA.fast9_detect_nonmax(data, xsize, ysize, stride, b, ret_num_corners);
//		corners_= ob_FA.fast10_detect_nonmax(data, xsize, ysize, stride, b, ret_num_corners);
//		corners_= ob_FA.fast11_detect_nonmax(data, xsize, ysize, stride, b, ret_num_corners);		
//		corners_= ob_FA.fast12_detect_nonmax(data, xsize, ysize, stride, b, ret_num_corners);

		//コーナー点を出力,描画
		ob_FA.output_corners(corners_, dst, ret_num_corners);

		//Keypoint に値代入
		int n=0;
		while(n<ret_num_corners[0] && n <MAX_NUM){
			keypoints1[n].x=corners_[n].x;
			keypoints1[n].y=corners_[n].y;
			n++;
		}
		size1=n;



		delete data,ret_num_corners;
		delete corners_;

}


void DetSURF(IplImage *src_gray){


		cv::Mat gray_img1=src_gray;
		cv::Mat gray_img2=src_gray_old;

		cv::normalize(gray_img1, gray_img1, 0, 255, cv::NORM_MINMAX);
		cv::normalize(gray_img2, gray_img2, 0, 255, cv::NORM_MINMAX);
		
		cout << "keypoints detecting..." <<endl;
		
		std::vector<cv::KeyPoint> keypoints1_, keypoints2_;
//		std::vector<cv::KeyPoint>::iterator itk;

		//ディテクター検出器
//		cv::OrbFeatureDetector detector(4);
//		cv::FastFeatureDetector detector(20);
		cv::SurfFeatureDetector detector(700);
//		cv::SiftFeatureDetector detector(2000);
		detector.detect(gray_img1, keypoints1_);
		detector.detect(gray_img2, keypoints2_);

	cout << "keypoint1.size(): " << keypoints1_.size() << " ,keypoints2.size():"<< keypoints2_.size()<<endl;
}


void DetSIFT(IplImage *src_gray){


		cv::Mat gray_img1=src_gray;
		cv::Mat gray_img2=src_gray_old;

		cv::normalize(gray_img1, gray_img1, 0, 255, cv::NORM_MINMAX);
		cv::normalize(gray_img2, gray_img2, 0, 255, cv::NORM_MINMAX);
		
		cout << "keypoints detecting..." <<endl;
		
		std::vector<cv::KeyPoint> keypoints1_, keypoints2_;
//		std::vector<cv::KeyPoint>::iterator itk;

		//ディテクター検出器
//		cv::OrbFeatureDetector detector(4);
//		cv::FastFeatureDetector detector(20);
//		cv::SurfFeatureDetector detector(700);
		cv::SiftFeatureDetector detector(2000);
		detector.detect(gray_img1, keypoints1_);
		detector.detect(gray_img2, keypoints2_);

	cout << "keypoint1.size(): " << keypoints1_.size() << " ,keypoints2.size():"<< keypoints2_.size()<<endl;

}


void DetKLT(IplImage *src_gray){

		if(counter==0){
			gbuf=(unsigned char *)malloc((int)(src->width*src->height));
			ogbuf=(unsigned char *)malloc((int)(src->width*src->height));
		}


		//Convert to grayscale
		for(int i=0;i<src->height;i++){
			for(int j=0;j<src->width;j++){
				int ad_ipl=i*src->widthStep+j;
				int ad=i*src->width+j;
				gbuf[ad]=(uchar)src_gray->imageData[ad];
		}}

		if(counter==0){
			 KLTSelectGoodFeatures(tc,gbuf,src->width,src->height,fl);
			 memcpy(ogbuf, gbuf, src->width*src->height);
		}


		//Extract KLT feature
		KLTTrackFeatures(tc,ogbuf,gbuf,src->width,src->height,fl); 
		KLTReplaceLostFeatures(tc,gbuf,src->width,src->height,fl); 


		//trajectory
		int t_num=0;
		int count_num=0;int xx,yy;int i,j;
		for(i=0;i<nFeatures;i++){

			xx=(int)(fl->feature[i]->x+0.5);    
			yy=(int)(fl->feature[i]->y+0.5);
			cvRectangle(dst,cvPoint(xx-2,yy-2),cvPoint(xx+2,yy+2),CV_RGB(0,255,0),2,8,0);


			if(fl->feature[i]->val==0.0){//トラッキングできた場合

				consective_num[i]+=1;
				//Renew trajectory
				for(j=trajectory_num-1;j>0;j--){
						point_trace[i][j].x=point_trace[i][j-1].x;
						point_trace[i][j].y=point_trace[i][j-1].y;
//						point_traceBG[i][j].x=point_traceBG[i][j-1].x;
//						point_traceBG[i][j].y=point_traceBG[i][j-1].y;
				}

				point_trace[i][0].x=xx;
				point_trace[i][0].y=yy;
//				keypoints1[t_num].x=xx;
//				keypoints1[t_num].y=yy;
				keypoints1[i].x=xx;
				keypoints1[i].y=yy;

				t_num+=1;//count up


				if(consective_num[i]>=trajectory_num){
						count_num+=1;
						cvRectangle(dst,cvPoint(xx-2,yy-2),cvPoint(xx+2,yy+2),CV_RGB(250,250,250),2,8,0);
				}
			
			}else{

//				cout << consective_num[i] << endl;
				//initialize
				for(j=trajectory_num-1;j>=0;j--){
						point_trace[i][j].x=xx;
						point_trace[i][j].y=yy;
				}
				consective_num[i]=0;

			}   
		}
		cout << "count_num: " << count_num << endl;
		cout << "t_num: " << t_num << endl;

		size1=t_num;

		//old feature
		//(double)point_trace[i][1].x;	(double)point_trace[i][1].y;
		//correspoding current feature
		//(double)point_trace[i][0].x;	(double)point_trace[i][0].y;

		for(i=0;i<MAX_NUM;i++)	ret_match_index1[i]=i;

}
