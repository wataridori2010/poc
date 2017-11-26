//
//  estCameraPose2.cpp
//  NR
//
//  Created by Ito Yuichi on 2017/11/25.
//  Copyright © 2017年 Ito Yuichi. All rights reserved.
//

#include <stdio.h>


#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>

//#include <GL/glut.h>
#include <GLUT/glut.h>


using namespace std;

vector<cv::Mat> t_seq;
vector<cv::Mat> r_seq;
vector<cv::Mat> e_seq;
static int cnt;

void display(void)
{

    static float axis_pos[] = {-0.2f, -0.2f, -0.7f};
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(axis_pos[0], axis_pos[1], axis_pos[2]);
    glBegin(GL_LINES);
    
    
    
    
//    glColor3f ( 1.0f, 0.0f, 0.0f );
//    glVertex3f( 1.0f, 0.0f, 0.0f );
//    glVertex3f( 0.0f, 0.0f, 0.0f );
//    
//    glColor3f ( 0.0f, 1.0f, 0.0f );
//    glVertex3f( 0.0f, 1.0f, 0.0f );
//    glVertex3f( 0.0f, 0.0f, 0.0f );
//    
//    glColor3f ( 0.0f, 0.0f, 1.0f );
//    glVertex3f( 0.0f, 0.2f, 1.0f );
//    glVertex3f( 0.0f, 0.0f, 0.0f );
//    
//    glColor3f ( 0.0f, 0.0f, 1.0f );
//    glVertex3f( 0.2f, 0.4f, 0.4f );
//    glVertex3f( 0.2f, 0.2f, 0.2f );

    printf("cnt:%d\n",cnt);
    
    for(int i=0; i<cnt;i++){
    
    double gain = 0.1;
//    double bx = e_seq[cnt].at<double>(0,3)*gain;
//    double by = e_seq[cnt].at<double>(1,3)*gain;
//    double bz = e_seq[cnt].at<double>(2,3)*gain;
//        double bx = e_seq[i].at<double>(0,3)*gain;
//        double by = e_seq[i].at<double>(1,3)*gain;
//        double bz = e_seq[i].at<double>(2,3)*gain;
        double bx = t_seq[i].at<double>(0)*gain;
        double by = t_seq[i].at<double>(1)*gain;
        double bz = t_seq[i].at<double>(2)*gain;

        
    printf("bx:%lf, by:%lf, bz:%lf\n",bx,by,bz);
    double bar_length = 0.1f;
        
    glColor3f ( 1.0f, 0.0f, 0.0f );
        
    cv::Mat r1 = (cv::Mat_<double>(3, 1) << bar_length, 0,  0);
    cv::Mat r0 = (cv::Mat_<double>(3, 1) << 0, 0,  0);
    cv::Mat r1_= r_seq[i]*r1;
    cv::Mat r0_= r_seq[i]*r0;
    glVertex3f( r1_.at<double>(0)+bx, r1_.at<double>(1)+by, r1_.at<double>(2)+bz);
    glVertex3f( r0_.at<double>(0)+bx, r0_.at<double>(1)+by, r0_.at<double>(2)+bz );
        
    //glVertex3f( bar_length+bx, 0.0f+by, 0.0f+bz);
    //glVertex3f( 0.0f+bx, 0.0f+by, 0.0f+bz );
    
    glColor3f ( 0.0f, 1.0f, 0.0f );
    cv::Mat g1 = (cv::Mat_<double>(3, 1) << 0, bar_length,  0);
    cv::Mat g0 = (cv::Mat_<double>(3, 1) << 0, 0,  0);
    cv::Mat g1_= r_seq[i]*g1;
    cv::Mat g0_= r_seq[i]*g0;
    glVertex3f( g1_.at<double>(0)+bx, g1_.at<double>(1)+by, g1_.at<double>(2)+bz);
    glVertex3f( g0_.at<double>(0)+bx, g0_.at<double>(1)+by, g0_.at<double>(2)+bz );

        
    //glVertex3f( 0.0f+bx, bar_length+by, 0.0f+bz );
    //glVertex3f( 0.0f+bx, 0.0f+by, 0.0f+bz );
    
    glColor3f ( 0.0f, 0.0f, 1.0f );
    cv::Mat b1 = (cv::Mat_<double>(3, 1) << 0, 0,  bar_length);
    cv::Mat b0 = (cv::Mat_<double>(3, 1) << 0, 0,  0);
    cv::Mat b1_= r_seq[i]*b1;
    cv::Mat b0_= r_seq[i]*b0;
    glVertex3f( b1_.at<double>(0)+bx, b1_.at<double>(1)+by, b1_.at<double>(2)+bz);
    glVertex3f( b0_.at<double>(0)+bx, b0_.at<double>(1)+by, b0_.at<double>(2)+bz );
        
    //glVertex3f( 0.0f+bx, 0.0f+by, bar_length+bz );
    //glVertex3f( 0.0f+bx, 0.0f+by, 0.0f+bz );
    }

    
    glEnd();
    glPopMatrix();


    //glClear(GL_COLOR_BUFFER_BIT);
    //glColor3f(1.0, 0.0, 0.0);
    
    // 片面表示を有効にする
    //glEnable(GL_CULL_FACE);
    // 裏面を表示しない
    //glCullFace(GL_BACK);
    //glBegin(GL_TRIANGLES);
    //glVertex2d(0.0, 0.9);
    //glVertex2d(-0.9, -0.9);
    //glVertex2d(0.9, -0.9);
    //glEnd();

    glFlush();
}


int main(int argc, char** argv)
{

    //    cv::initModule_nonfree(); //モジュールの初期化
    
    //カメラパラメータの読み込みとレンズ歪の除去
    cv::Mat img1; //入力画像1
    cv::Mat img2; //入力画像2
    
    cv::Mat K = (cv::Mat_<double>(3, 3) << 1625, 0, 952, 0, 1625, 547, 0, 0, 1);
    cv::Mat distCoeffs = (cv::Mat_<double>(5, 1) << 0.07892476, -0.14349866,  0.00076881, -0.00032049,  0.16328739);
    
    cv::Mat Ext = cv::Mat::eye(4,4,CV_64FC1);
    cv::Mat Ext_tmp = cv::Mat::eye(4,4,CV_64FC1);
    cnt = 0;

    

    
    
    
    for(int s=0; s<5;s++){
        
        

    char fn1[512]={0};
    char fn2[512]={0};
    
    sprintf(fn1,"/Users/itoyuichi/github/playground/OpenCV/calib/scene1/s%03d.jpg",s);
    sprintf(fn2,"/Users/itoyuichi/github/playground/OpenCV/calib/scene1/s%03d.jpg",s+1);
    
    cv::undistort(cv::imread(fn1), img1, K, distCoeffs);
    cv::undistort(cv::imread(fn2), img2, K, distCoeffs);
    
    //特徴抽出
    //http://dronebiz.net/tech/opencv3/akaze
    auto algorithm = cv::AKAZE::create();
    
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce"); //対応点探索方法の設定
    
    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat descriptor1, descriptor2;
    algorithm->detect(img1, keypoints1);
    algorithm->compute(img1, keypoints1, descriptor1);
    
    algorithm->detect(img2, keypoints2);
    algorithm->compute(img2, keypoints2, descriptor2);
    
    //対応点の探索
    std::vector<cv::DMatch> dmatch;
    std::vector<cv::DMatch> dmatch12, dmatch21;
    
    matcher->match(descriptor1, descriptor2, dmatch12); //img1 -> img2
    matcher->match(descriptor2, descriptor1, dmatch21); //img2 -> img1
    
    for (size_t i = 0; i < dmatch12.size(); ++i)
    {
        //img1 -> img2 と img2 -> img1の結果が一致しているか検証
        cv::DMatch m12 = dmatch12[i];
        cv::DMatch m21 = dmatch21[m12.trainIdx];
        
        if (m21.trainIdx == m12.queryIdx)
            dmatch.push_back( m12 );
    }
    
    //十分な数の対応点があれば基礎行列を推定
    if (dmatch.size() > 5)
    {
        std::vector<cv::Point2d> p1;
        std::vector<cv::Point2d> p2;
        //対応付いた特徴点の取り出しと焦点距離1.0のときの座標に変換
        for (size_t i = 0; i < dmatch.size(); ++i)
        {
            cv::Mat ip(3, 1, CV_64FC1);
            cv::Point2d p;
            
            ip.at<double>(0) = keypoints1[dmatch[i].queryIdx].pt.x;
            ip.at<double>(1) = keypoints1[dmatch[i].queryIdx].pt.y;
            ip.at<double>(2) = 1.0;
            ip = K.inv()*ip;
            p.x = ip.at<double>(0);
            p.y = ip.at<double>(1);
            p1.push_back( p );
            
            ip.at<double>(0) = keypoints2[dmatch[i].trainIdx].pt.x;
            ip.at<double>(1) = keypoints2[dmatch[i].trainIdx].pt.y;
            ip.at<double>(2) = 1.0;
            ip = K.inv()*ip;
            p.x = ip.at<double>(0);
            p.y = ip.at<double>(1);
            p2.push_back( p );
        }
        
        cv::Mat essentialMat = cv::findEssentialMat(p1, p2);
        std::cout << "Essential Matrix\n" << essentialMat << std::endl;
        
        cv::Mat r, t;
        cv::recoverPose(essentialMat, p1, p2, r, t);
        std::cout << "R:\n" << r << std::endl;
        std::cout << "t:\n" << t << std::endl;
        
        
        Ext_tmp.at<double>(0,0) =r.at<double>(0,0);
        Ext_tmp.at<double>(0,1) =r.at<double>(0,1);
        Ext_tmp.at<double>(0,2) =r.at<double>(0,2);
        Ext_tmp.at<double>(1,0) =r.at<double>(1,0);
        Ext_tmp.at<double>(1,1) =r.at<double>(1,1);
        Ext_tmp.at<double>(1,2) =r.at<double>(1,2);
        Ext_tmp.at<double>(2,0) =r.at<double>(2,0);
        Ext_tmp.at<double>(2,1) =r.at<double>(2,1);
        Ext_tmp.at<double>(2,2) =r.at<double>(2,2);

        Ext_tmp.at<double>(0,3) =t.at<double>(0);
        Ext_tmp.at<double>(1,3) =t.at<double>(1);
        Ext_tmp.at<double>(2,3) =t.at<double>(2);
        
//        Ext = Ext*Ext_tmp;
        Ext = Ext + Ext_tmp;
        std::cout << "Ext:\n" << r << std::endl;
        
//
        if(cnt==0){
            t_seq.push_back(t);
            r_seq.push_back(r);
            e_seq.push_back(Ext);
            
        }else{
            t_seq.push_back( t+t_seq[cnt-1]);
            r_seq.push_back( r*r_seq[cnt-1]);
            e_seq.push_back(Ext);
            
        }
        


        
        cnt++;
    }
        
    }
/*
    glutInitWindowSize(800, 600); // Windowサイズを指定
//    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA);
    glutCreateWindow("test");
    
    static float axis_pos[] = {0.0f, 0.0f, 0.0f};
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(axis_pos[0], axis_pos[1], axis_pos[2]);
    glBegin(GL_LINES);
    
    glColor3f ( 1.0f, 0.0f, 0.0f );
    glVertex3f( 1.0f, 0.0f, 0.0f );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    
    glColor3f ( 0.0f, 1.0f, 0.0f );
    glVertex3f( 0.0f, 1.0f, 0.0f );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    
    glColor3f ( 0.0f, 0.0f, 1.0f );
    glVertex3f( 0.0f, 0.0f, 1.0f );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    
    glEnd();
    glPopMatrix();
*/
    
    glutInitWindowSize(800, 600); // Windowサイズを指定
    glutInit(&argc, argv);
    glutCreateWindow("test");
    
    glutDisplayFunc(display);
    glutMainLoop();
    
    return 0;
}
