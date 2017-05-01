/*
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/superres/optical_flow.hpp>
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;
using namespace cv::superres;

int main(int argc, const char *argv[])
{

    if (argc < 3)
    {
        cerr << "Usage : " << argv[0] << "<frame0> <frame1> [<output_flow>]" << endl;
        return -1;
    }

    Mat frame0 = imread(argv[1], IMREAD_GRAYSCALE);
    Mat frame1 = imread(argv[2], IMREAD_GRAYSCALE);

    if (frame0.empty())
    {
        cerr << "Can't open image ["  << argv[1] << "]" << endl;
        return -1;
    }
    if (frame1.empty())
    {
        cerr << "Can't open image ["  << argv[2] << "]" << endl;
        return -1;
    }

    if (frame1.size() != frame0.size())
    {
        cerr << "Images should be of equal sizes" << endl;
        return -1;
    }

//    Mat_<Point2f> flow;
//    Ptr<DenseOpticalFlow> tvl1 = createOptFlow_DualTVL1();

		Ptr<DenseOpticalFlowExt> opticalFlow = superres::createOptFlow_DualTVL1();	//TV-L1
//		Ptr<DenseOpticalFlowExt> opticalFlow = superres::createOptFlow_Simple();	//SF
//		Ptr<DenseOpticalFlowExt> opticalFlow = superres::createOptFlow_Farneback();	//FarneBach
		Mat flowX, flowY;



	  //TV-L1
	  //default parameter
//    tau            = 0.25;
//    lambda         = 0.15;
//    theta          = 0.3;
//    nscales        = 5;
//    warps          = 5;
//    epsilon        = 0.01;
//    iterations     = 300;
	  opticalFlow->set("tau",    0.5);
	  opticalFlow->set("lambda", 0.1);
	  opticalFlow->set("theta",  0.1);
	  opticalFlow->set("nscales", 5);
	  opticalFlow->set("warps", 5);
	  opticalFlow->set("epsilon", 0.001);
	  opticalFlow->set("iterations", 10);





    const double start = (double)getTickCount();

	opticalFlow->calc(frame0, frame1, flowX, flowY);

	 //FarneBach
//	Mat dst;		calcOpticalFlowFarneback(frame0,frame1,dst,0.1, 10, 10, 10, 1, 1.1, 1 );
//	vector<cv::Mat> planes;		cv::split(dst, planes);
//	flowX.push_back( planes[0] );
//	flowY.push_back( planes[1] );


    const double timeSec = (getTickCount() - start) / getTickFrequency();

    cout << "calcOpticalFlowDual_TVL1 : " << timeSec << " sec" << endl;


	// オプティカルフローの可視化（色符号化）
	//  オプティカルフローを極座標に変換（角度は[deg]）
	Mat magnitude, angle;
	cartToPolar(flowX, flowY, magnitude, angle, true);
	//  色相（H）はオプティカルフローの角度
	//  彩度（S）は0～1に正規化したオプティカルフローの大きさ
	//  明度（V）は1
	Mat hsvPlanes[3];		
	hsvPlanes[0] = angle;
	normalize(magnitude, magnitude, 0, 1, NORM_MINMAX); // 正規化
	hsvPlanes[1] = magnitude;
	hsvPlanes[2] = Mat::ones(magnitude.size(), CV_32F);
	//  HSVを合成して一枚の画像にする
	Mat hsv;
	merge(hsvPlanes, 3, hsv);
	//  HSVからBGRに変換
	Mat flowBgr;
	cvtColor(hsv, flowBgr, cv::COLOR_HSV2BGR);

	Mat oimg; oimg.create(magnitude.size(),CV_8UC3);
	normalize(flowBgr, flowBgr, 0, 255, NORM_MINMAX); // 正規化
	flowBgr.convertTo(oimg, CV_8UC3); // or CV_32F works (too)


	char ofn[512];sprintf(ofn,"%s_res.png",argv[1]);
	cv::imwrite(ofn,oimg);
	cv::imshow("optical flow", oimg);


	// 表示
//	cv::imshow("input", curr);
//	cv::imshow("optical flow", flowBgr);


    waitKey(0);

    return 0;

}
*/


//using namespace std;
#include <opencv2/opencv.hpp>
#include <opencv2/superres/superres.hpp>

int main(int argc, const char *argv[])
{
	using namespace cv;
	using namespace cv::superres;

        // 超解像度処理用クラスの生成
	Ptr<SuperResolution> superResolution = createSuperResolution_BTVL1();
        // 入力ソースを動画ファイルに設定する
	superResolution->setInput(createFrameSource_Video(argv[1]));
        // 超解像処理のパラメータ設定（デフォルトのままだとものすごく時間がかかる）
	superResolution->set("scale", 2);
	superResolution->set("temporalAreaRadius", 1);
	superResolution->set("iterations", 2);

	int counts=0;
	while (waitKey(1) == -1)
	{
                // 高解像度化された画像を取得して表示する
		Mat frame;
		superResolution->nextFrame(frame);
		imshow("Super Resolution", frame);
		char ofn[512];sprintf(ofn,"%s_res_%04d.png",argv[1],counts);
		imwrite(ofn,frame);
		printf("%d frame\n",counts);
		counts++;
	}
}