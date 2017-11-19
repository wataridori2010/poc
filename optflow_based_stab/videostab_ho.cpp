//
//  videostab_ho.cpp
//  NR
//
//  Created by Ito Yuichi on 2017/11/18.
//  Copyright © 2017年 Ito Yuichi. All rights reserved.
//

#include "videostab_ho.hpp"

#define TRAJECTORY_OPT 1

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cassert>
#include <cmath>
#include <fstream>

using namespace std;
using namespace cv;

// This video stablisation smooths the global trajectory using a sliding average window

const int SMOOTHING_RADIUS = 30; // In frames. The larger the more stable the video, but less reactive to sudden panning
const int HORIZONTAL_BORDER_CROP = 20; // In pixels. Crops the border to reduce the black borders from stabilisation being too noticeable.




// 1. Get previous to current frame transformation (dx, dy, da) for all frames
// 2. Accumulate the transformations to get the image trajectory
// 3. Smooth out the trajectory using an averaging window
// 4. Generate new set of previous to current transform, such that the trajectory ends up being the same as the smoothed trajectory
// 5. Apply the new transformation to the video

struct TransformParam
{
    TransformParam() {}
    TransformParam(double _dx, double _dy, double _da) {
        dx = _dx;
        dy = _dy;
        da = _da;
    }
    
    double dx;
    double dy;
    double da; // angle
};

struct Trajectory
{
    Trajectory() {}
    Trajectory(double _x, double _y, double _a) {
        x = _x;
        y = _y;
        a = _a;
    }
    
    double x;
    double y;
    double a; // angle
};



struct TransformParam5
{
    TransformParam5() {}
    TransformParam5(double _dx, double _dy, double _da,double _dzx,double _dzy) {
        dx = _dx;
        dy = _dy;
        da = _da;
        dzx = _dzx;
        dzy = _dzy;
    }
    
    double dx;
    double dy;
    double da; // angle
    double dzx;
    double dzy;
};

struct Trajectory5
{
    Trajectory5() {}
    Trajectory5(double _x, double _y, double _a, double _zx, double _zy) {
        x = _x;
        y = _y;
        a = _a;
        zx = _zx;
        zy = _zy;
    }
    
    double x;
    double y;
    double a; // angle
    double zx; //
    double zy; //
};


int main(int argc, char **argv)
{
//    if(argc < 2) {
//        cout << "./VideoStab [video.avi]" << endl;
//        return 0;
//    }
    
    // For further analysis
    ofstream out_transform("/Users/itoyuichi/github/playground/OpenCV/stab-opencv/prev_to_cur_transformation.txt");
    ofstream out_trajectory("/Users/itoyuichi/github/playground/OpenCV/stab-opencv/trajectory.txt");
    ofstream out_smoothed_trajectory("/Users/itoyuichi/github/playground/OpenCV/stab-opencv/smoothed_trajectory.txt");
    ofstream out_new_transform("/Users/itoyuichi/github/playground/OpenCV/stab-opencv/new_prev_to_cur_transformation.txt");
    
    VideoCapture cap("/Users/itoyuichi/github/playground/OpenCV/stab-opencv/STAGE_VIDEO_3.MOV");//argv[1]);
    assert(cap.isOpened());
    
    Mat cur, cur_grey;
    Mat prev, prev_grey;
    
    cap >> prev;
    cvtColor(prev, prev_grey, COLOR_BGR2GRAY);
    
    // Step 1 - Get previous to current frame transformation (dx, dy, da) for all frames
//    vector <TransformParam> prev_to_cur_transform; // previous to current
    vector <TransformParam5> prev_to_cur_transform; // previous to current
  
    
    int k=1;
    int max_frames = cap.get(CV_CAP_PROP_FRAME_COUNT);
    Mat last_T;
    
    while(true) {
        cap >> cur;
        
        if(cur.data == NULL) {
            break;
        }
        
        cvtColor(cur, cur_grey, COLOR_BGR2GRAY);
        
        // vector from prev to cur
        vector <Point2f> prev_corner, cur_corner;
        vector <Point2f> prev_corner2, cur_corner2;
        vector <uchar> status;
        vector <float> err;
        
        goodFeaturesToTrack(prev_grey, prev_corner, 200, 0.01, 30);
        calcOpticalFlowPyrLK(prev_grey, cur_grey, prev_corner, cur_corner, status, err);
        
        // weed out bad matches
        for(size_t i=0; i < status.size(); i++) {
            if(status[i]) {
                prev_corner2.push_back(prev_corner[i]);
                cur_corner2.push_back(cur_corner[i]);
            }
        }
        
        // translation + rotation only
        Mat T = estimateRigidTransform(prev_corner2, cur_corner2, false); // false = rigid transform, no scaling/shearing
        
        // in rare cases no transform is found. We'll just use the last known good transform.
        if(T.data == NULL) {
            last_T.copyTo(T);
        }
        
        T.copyTo(last_T);
        
        // decompose T
        double dx = T.at<double>(0,2);
        double dy = T.at<double>(1,2);
        double da = atan2(T.at<double>(1,0), T.at<double>(0,0));
//        prev_to_cur_transform.push_back(TransformParam(dx, dy, da));
 
//        double dzx = sqrt(T.at<double>(1,0)*T.at<double>(1,0)+T.at<double>(0,0)*T.at<double>(0,0));
//        double dzy = sqrt(T.at<double>(1,1)*T.at<double>(1,1)+T.at<double>(0,1)*T.at<double>(0,1));
        double dzx = sqrt(T.at<double>(1,0)*T.at<double>(1,0)+T.at<double>(0,0)*T.at<double>(0,0))-1;
        double dzy = sqrt(T.at<double>(1,1)*T.at<double>(1,1)+T.at<double>(0,1)*T.at<double>(0,1))-1;
        prev_to_cur_transform.push_back(TransformParam5(dx, dy, da, dzx, dzy));
        
        out_transform << k << " " << dx << " " << dy << " " << da << endl;
        
        cur.copyTo(prev);
        cur_grey.copyTo(prev_grey);
        
        cout << "Frame: " << k << "/" << max_frames << " - good optical flow: " << prev_corner2.size() << endl;
        k++;
    }
    
    // Step 2 - Accumulate the transformations to get the image trajectory
    
    // Accumulated frame to frame transform
    double a = 0;
    double x = 0;
    double y = 0;
    double zx = 0;
    double zy = 0;
    
//    vector <Trajectory> trajectory; // trajectory at all frames
    vector <Trajectory5> trajectory; // trajectory at all frames
    
    for(size_t i=0; i < prev_to_cur_transform.size(); i++) {
        x += prev_to_cur_transform[i].dx;
        y += prev_to_cur_transform[i].dy;
        a += prev_to_cur_transform[i].da;
        zx += prev_to_cur_transform[i].dzx;
        zy += prev_to_cur_transform[i].dzy;
        
//        trajectory.push_back(Trajectory(x,y,a));
        trajectory.push_back(Trajectory5(x,y,a,zx,zy));
        
        out_trajectory << (i+1) << " " << x << " " << y << " " << a << endl;
    }
    
    
    // Step 3 - Smooth out the trajectory using an averaging window
//    vector <Trajectory> smoothed_trajectory; // trajectory at all frames
    vector <Trajectory5> smoothed_trajectory; // trajectory at all frames
/*
    for(size_t i=0; i < trajectory.size(); i++) {
        double sum_x = 0;
        double sum_y = 0;
        double sum_a = 0;
        int count = 0;
        
        for(int j=-SMOOTHING_RADIUS; j <= SMOOTHING_RADIUS; j++) {
            if(i+j >= 0 && i+j < trajectory.size()) {
                sum_x += trajectory[i+j].x;
                sum_y += trajectory[i+j].y;
                sum_a += trajectory[i+j].a;
                
                count++;
            }
        }
        
        double avg_a = sum_a / count;
        double avg_x = sum_x / count;
        double avg_y = sum_y / count;
        
        smoothed_trajectory.push_back(Trajectory(avg_x, avg_y, avg_a));
        
        out_smoothed_trajectory << (i+1) << " " << avg_x << " " << avg_y << " " << avg_a << endl;
    }
 */

    {
    
    double avg_a = trajectory[0].a;
    double avg_x = trajectory[0].x;
    double avg_y = trajectory[0].y;
    double avg_zx = trajectory[0].zx;
    double avg_zy = trajectory[0].zy;
        
    double rec_weight = 0.9;
    for(size_t i=0; i < trajectory.size(); i++) {
        
        avg_a = rec_weight * avg_a + (1-rec_weight)*trajectory[i].a;
        avg_x = rec_weight * avg_x + (1-rec_weight)*trajectory[i].x;
        avg_y = rec_weight * avg_y + (1-rec_weight)*trajectory[i].y;
        avg_zx = rec_weight * avg_zx + (1-rec_weight)*trajectory[i].zx;
        avg_zy = rec_weight * avg_zy + (1-rec_weight)*trajectory[i].zy;
//        avg_a = rec_weight * avg_a + (1-rec_weight)*huber(trajectory[i].a,avg_a, 0.1);
//        avg_x = rec_weight * avg_x + (1-rec_weight)*huber(trajectory[i].x,avg_x, 50);
//        avg_y = rec_weight * avg_y + (1-rec_weight)*huber(trajectory[i].y,avg_y, 50);
//        avg_zx = rec_weight * avg_zx + (1-rec_weight)*huber(trajectory[i].zx,avg_zx, 0.2);
//        avg_zy = rec_weight * avg_zy + (1-rec_weight)*huber(trajectory[i].zy,avg_zy, 0.2);
        
//        smoothed_trajectory.push_back(Trajectory(avg_x, avg_y, avg_a));
        smoothed_trajectory.push_back(Trajectory5(avg_x, avg_y, avg_a, avg_zx, avg_zy));
        
        out_smoothed_trajectory << (i+1) << " " << avg_x << " " << avg_y << " " << avg_a << endl;
    }
        
#ifdef TRAJECTORY_OPT
        avg_zx = 0;
        avg_zy = 0;
        
        smoothed_trajectory.clear();
        ifstream opt_trajectory("/Users/itoyuichi/github/playground/OpenCV/stab-opencv/traj_krr.txt");
        int id=0;
        for(size_t i=0; i < prev_to_cur_transform.size(); i++) {
            opt_trajectory >> id >> avg_x >> avg_y >> avg_a ;
            printf("id: %d, %f, %f, %f    ",id, avg_x, avg_y, avg_a);
            smoothed_trajectory.push_back(Trajectory5(avg_x,avg_y,avg_a,avg_zx,avg_zy));
        }
#endif
        
    }
    

    
    
    
    // Step 4 - Generate new set of previous to current transform, such that the trajectory ends up being the same as the smoothed trajectory
//    vector <TransformParam> new_prev_to_cur_transform;
    vector <TransformParam5> new_prev_to_cur_transform;

    
    // Accumulated frame to frame transform
    a = 0;
    x = 0;
    y = 0;
    zx = 0;
    zy = 0;

    
    for(size_t i=0; i < prev_to_cur_transform.size(); i++) {
        x += prev_to_cur_transform[i].dx;
        y += prev_to_cur_transform[i].dy;
        a += prev_to_cur_transform[i].da;
        zx += prev_to_cur_transform[i].dzx;
        zy += prev_to_cur_transform[i].dzy;
        
        // target - current
        double diff_x = smoothed_trajectory[i].x - x;
        double diff_y = smoothed_trajectory[i].y - y;
        double diff_a = smoothed_trajectory[i].a - a;
        double diff_zx = smoothed_trajectory[i].zx - zx;
        double diff_zy = smoothed_trajectory[i].zy - zy;
        
        
        double dx = prev_to_cur_transform[i].dx + diff_x;
        double dy = prev_to_cur_transform[i].dy + diff_y;
        double da = prev_to_cur_transform[i].da + diff_a;
        double dzx = prev_to_cur_transform[i].dzx + diff_zx;
        double dzy = prev_to_cur_transform[i].dzy + diff_zy;

        
//        new_prev_to_cur_transform.push_back(TransformParam(dx, dy, da));
//        new_prev_to_cur_transform.push_back(TransformParam5(dx, dy, da, dzx, dzy));
        new_prev_to_cur_transform.push_back(TransformParam5(dx, dy, da, 0, 0)); // tentative
        
        out_new_transform << (i+1) << " " << dx << " " << dy << " " << da << endl;
    }
    
    // Step 5 - Apply the new transformation to the video
    cap.set(CV_CAP_PROP_POS_FRAMES, 0);
    Mat T(2,3,CV_64F);
    
    int vert_border = HORIZONTAL_BORDER_CROP * prev.rows / prev.cols; // get the aspect ratio correct
    
    VideoWriter writer;//("test1.avi", CV_FOURCC_DEFAULT, 10, Size(640, 480), true);

    
    
    k=0;
    while(k < max_frames-1) { // don't process the very last frame, no valid transform
        cap >> cur;
        
        if(cur.data == NULL) {
            break;
        }
        
        T.at<double>(0,0) = cos(new_prev_to_cur_transform[k].da)*(1+new_prev_to_cur_transform[k].dzx);
        T.at<double>(0,1) = -sin(new_prev_to_cur_transform[k].da)*(1+new_prev_to_cur_transform[k].dzy);
        T.at<double>(1,0) = sin(new_prev_to_cur_transform[k].da)*(1+new_prev_to_cur_transform[k].dzx);
        T.at<double>(1,1) = cos(new_prev_to_cur_transform[k].da)*(1+new_prev_to_cur_transform[k].dzy);
/*
        T.at<double>(0,0) = cos(new_prev_to_cur_transform[k].da);
        T.at<double>(0,1) = -sin(new_prev_to_cur_transform[k].da);
        T.at<double>(1,0) = sin(new_prev_to_cur_transform[k].da);
        T.at<double>(1,1) = cos(new_prev_to_cur_transform[k].da);
*/
        
        T.at<double>(0,2) = new_prev_to_cur_transform[k].dx;
        T.at<double>(1,2) = new_prev_to_cur_transform[k].dy;
        
        Mat cur2;
        
        warpAffine(cur, cur2, T, cur.size());
        
        cur2 = cur2(Range(vert_border, cur2.rows-vert_border), Range(HORIZONTAL_BORDER_CROP, cur2.cols-HORIZONTAL_BORDER_CROP));
        
        // Resize cur2 back to cur size, for better side by side comparison
        resize(cur2, cur2, cur.size());
        
        // Now draw the original and stablised side by side for coolness
        Mat canvas = Mat::zeros(cur.rows, cur.cols*2+10, cur.type());
        
        cur.copyTo(canvas(Range::all(), Range(0, cur2.cols)));
        cur2.copyTo(canvas(Range::all(), Range(cur2.cols+10, cur2.cols*2+10)));
        

        //if(canvas.cols > 1920) {
        resize(canvas, canvas, Size(canvas.cols/2, canvas.rows/2));
        //}
        
        
        
        imshow("before and after", canvas);
        
        //video writer
        if(k==0){
            writer.open("/Users/itoyuichi/github/playground/OpenCV/stab-opencv/stabilized_result_ho6.mov", VideoWriter::fourcc('H', '2', '6', '4'), 30, Size(canvas.cols, canvas.rows));//CV_FOURCC_DEFAULT, 30, Size(canvas.cols, canvas.rows));
            //VideoWriter::fourcc('H', '2', '6', '4')
        }

        writer << canvas;
        
        //char str[256];
        //sprintf(str, "images/%08d.jpg", k);
        //imwrite(str, canvas);
        
        waitKey(10);
        
        k++;
    }
    
    return 0;
}
