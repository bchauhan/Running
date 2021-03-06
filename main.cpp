// oneCameraFalseStart_24_Jan_2018.cpp : Defines the entry point for the console application.
//
#include<windows.h>
#include"stdafx.h"
#include<opencv2/opencv.hpp>
#include<opencv2/bgsegm.hpp>
#include<string>
#include<vector>
#include<tuple>
#include<iostream>
#include<algorithm>
#include<math.h>
//#include<opencv2/bgsegm.hpp>
#define point pair<int, int>


using namespace std;

using namespace cv;


float HEIGHT = 600;
float WIDTH = 0;
class False_start {
private:
	string videoname;
	Mat img;
public:
	void setVideoname(string videoname) {
		this->videoname = videoname;
	}
	string getVideoname() {
		return this->videoname;
	}
	point lineLineIntersection(point A, point B, point C, point D)
	{
		// Line AB represented as a1x + b1y = c1
		int a1 = B.second - A.second;
		int b1 = A.first - B.first;
		int c1 = a1 * (A.first) + b1 * (A.second);

		// Line CD represented as a2x + b2y = c2
		int a2 = D.second - C.second;
		int b2 = C.first - D.first;
		int c2 = a2 * (C.first) + b2 * (C.second);

		int determinant = a1 * b2 - a2 * b1;

		if (determinant == 0)
		{
			// The lines are parallel. This is simplified
			// by returning a pair of FLT_MAX
			return make_pair(FLT_MAX, FLT_MAX);
		}
		else
		{
			int x = (b2*c1 - b1 * c2) / determinant;
			int y = (a1*c2 - a2 * c1) / determinant;
			return make_pair(x, y);
		}
	}
	vector<tuple <vector<Point>,int>> sort(vector<vector<Point>> contours, int ContAreaThresh) {
		typedef vector<tuple <vector<Point>, int>> sorted_contours;
		sorted_contours st;
		tuple<vector<Point>, int> tp;
		vector<Point> cnt;
		int area=0;
		cnt.cbegin();
		st.cbegin();
		for (unsigned int i = 0; i < contours.size(); i++) {
			area = contourArea(contours[i]);
			cnt = contours[i];
			tp = make_tuple(cnt, area);
			std::cout << "Cnt area found in sort function : " << area;
			if (area > ContAreaThresh){
				st.push_back(tp);
			}
		}
		struct CustomLessThan
		{
			bool operator()(tuple <vector<Point>, int> const &lhs, tuple <vector<Point>, int> const &rhs) const
			{
				return std::get<1>(lhs) < std::get<1>(rhs);
			}
		};
		std::sort(st.begin(), st.end(), CustomLessThan());
		return st;
	}
	Mat readImage(VideoCapture cap) {
		
		Mat frame,img;
		Mat kernel = Mat::ones(5, 5, CV_32F) / (float)(5*5);
		cap >> frame;
		if (!frame.data)                              // Check for invalid input
		{
			std::cout << "Could not open or find the image" << std::endl;
			return kernel;
		}
		int w = frame.size().width;
		int h = frame.size().height;
		float a = float(h) / w;
		WIDTH = int(HEIGHT / a);
		Size size(HEIGHT, WIDTH);
		resize(img, img, size);
		return img;
	}

	float distance(vector <float> a, vector <float> b) {
		return sqrt(pow((a[0] - b[0]), 2) + pow((a[1] - b[1]), 2));
	}

	float is_between(vector <float> a, vector <float> b, vector <float> c) { // tells of point "c" lies on line "ab"
		return distance(a, c) + distance(c, b) == distance(a, b);
	}

	bool ccw(vector <float> A, vector <float> B, vector <float> C) {
		return (C[1] - A[1]) * (B[0] - A[0]) > (B[1] - A[1]) * (C[0] - A[0]);
	}

	bool intersect(vector <float> A, vector <float> B, vector <float> C, vector <float> D) {
		return ccw(A, C, D) != ccw(B, C, D) and ccw(A, B, C) != ccw(A, B, D);
	}
};

vector <float> main(int argc,  char** argv)
{
	False_start fs = False_start();
	vector <float> result;// = 0, 0
	result.push_back(0);
	result.push_back(1);

	/*
	std::cout<< "arguments : ", len(sys.argv)
	std::cout<< "arguments0 : ", sys.argv[0]
	std::cout<< "Video name : ", sys.argv[1]
	std::cout<< "Video start time in (1970ms) : ", sys.argv[2]
	std::cout<< "Runner start time in (1970ms) : ", sys.argv[3]
	std::cout<< "Distance : ", sys.argv[4]
	
	string videoname = argv[1];
	string videoStartTimeSTR = (argv[2]);
	double videoStartTime = stod(videoStartTimeSTR);

	string runnerStartTimeSTR = (argv[3]);
	double runnerStartTime = stod(argv[3]);

	string distanceSTR = argv[4];
	int distance = stoi(distanceSTR);

	*/
	/*'''***************inputs********************'''*/
	string videoname = "Videos/gg2.mp4";
	double videoStartTime = 1516269369.878;
	double runnerStartTime = 1516269375.159;
	int distance = 5;
	/*'''**************************************'''*/
	double elapsedTime = runnerStartTime - videoStartTime;
	double delay = elapsedTime - 1;// the moment we will start processing, decreasing by one second to reduce the processing time
	double ContAreaThresh = 15000 / distance;
	int font = CV_FONT_HERSHEY_SIMPLEX;
	VideoCapture capture(videoname);

	/*////////#creating big frame to find lines ////*/
	Mat img, firstFrame,bigImage,frame, fgmaskGMG, fgmaskGMG2, fgmaskGMG3;
	Mat kernel3 = Mat::ones(3, 3, CV_32F) / (float)(3 * 3);
	Mat kernel5 = Mat::ones(5, 5, CV_32F) / (float)(5 * 5);
	Mat kernel7 = Mat::ones(7, 7, CV_32F) / (float)(7 * 7);
	Mat kernel11 = Mat::ones(11, 11, CV_32F) / (float)(11 * 11);
	Mat element3 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3), Point(2, 2));
	Mat element5 = getStructuringElement(MORPH_ELLIPSE, Size(5, 5), Point(4, 4));
	Mat element7 = getStructuringElement(MORPH_ELLIPSE, Size(7, 7), Point(6, 6));
	Mat element11 = getStructuringElement(MORPH_ELLIPSE, Size(11, 11), Point(10, 10));

	vector <tuple<int, int, int, int>> dimensions;
	vector <int> trackPoints;
	vector <Point> lineOp;
	Ptr<BackgroundSubtractor> pGMG;
	int framenumber = 0;

	//capture.read(img);
	if (!capture.read(img)) {
		std::cout << "Video not found";
		return result;
	}
	int h= img.rows; //old image height
	int w = img.cols; // old image width
	float ar = float(h) / w;
	int H = 800; // new frame height
	int W = int(H / ar); // new frame height
	std::cout << "Width And Height of big image : " << W << "    " << H << endl;
	Size size1(H, W);
	resize(img, bigImage, size1); //resizing
	/*////////////# finish //////////////*/
	/*////#Reading normal frame by manual function readImage for general tasks //////*/
	frame = fs.readImage(capture);
	if (!frame.data) {
		std::cout << "Video not found";
		return result;
	}
	int HEIGHT = frame.rows;
	int fps = capture.get(CV_CAP_PROP_FPS);
	int frames = capture.get(CV_CAP_PROP_FRAME_COUNT);
	std::cout << "Number of frames : " << frames;
	std::cout << "fps : " << fps;
	//dimensions
	Point pt1(10, HEIGHT - 10);
	putText(frame, "Video : " + videoname, pt1, font, 0.5, (0, 0, 255), 2);
	frame.copyTo(firstFrame);//cv2.resize(frame, (1400, 800))
	//lineOp = houghlinesp_18_jan.main(bigImage)
	Point lpt1(10, 20), lpt2(160, 210), lpt3(110, 220), lpt4(100, 200);
	std::cout << "result got from houghline script : " << lpt1, lpt2, lpt3, lpt4;
	if (lpt1.x == 0 && pt1.y == 0) {
		std::cout<< "All lines not detected";
			return result;
	}
	pGMG = cv::bgsegm::createBackgroundSubtractorGMG(30);
	pGMG->apply(firstFrame, fgmaskGMG);
	int frameThresh = delay * fps + 10;

	/********************* While loop starts **************************/
	while (0) {
		vector <int> runner;
		// Capture a frame
		frame = fs.readImage(capture);
		//key = cv2.waitKey(1) #20
		if (!frame.data) { //exit on ESC
			destroyAllWindows();
			break;
		}
		framenumber += 1;
		if (framenumber > frameThresh){
			if (framenumber < frameThresh + 31) {
				pGMG->apply(firstFrame, fgmaskGMG);
				//fgmaskGMG = fgbg.apply(frame);
			}
			else if (framenumber > frameThresh + 30 ) // and framenumber % 2 == 0{
				pGMG->apply(firstFrame, fgmaskGMG);
			}

			//////# Morphing ////////////
			//print "1 : ",datetime.now()
			/*
			Opening: MORPH_OPEN : 2
			Closing: MORPH_CLOSE: 3
			Gradient: MORPH_GRADIENT: 4
			Top Hat: MORPH_TOPHAT: 5
			Black Hat: MORPH_BLACKHAT: 6
			*/
			morphologyEx(fgmaskGMG, fgmaskGMG2, MORPH_OPEN, element3);
			//fgmaskGMG2 = cv2.morphologyEx(fgmaskGMG, cv2.MORPH_OPEN, kernel3)
			medianBlur(fgmaskGMG2, fgmaskGMG2, 5);
			//fgmaskGMG2 = cv2.medianBlur(fgmaskGMG2, 5)
			morphologyEx(fgmaskGMG2, fgmaskGMG2, MORPH_OPEN, element5);
			fgmaskGMG3 = cv2.morphologyEx(fgmaskGMG2, cv2.MORPH_OPEN, kernel5)
			fgmaskGMG3 = cv2.medianBlur(fgmaskGMG3, 3)
			fgmaskGMG3 = cv2.morphologyEx(fgmaskGMG3, cv2.MORPH_OPEN, kernel7)
			//print "2 : ",datetime.now()
			_, contours, _ = cv2.findContours(fgmaskGMG3, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
			//////# shadow removal part//////
			sortedContours = sort(contours, ContAreaThresh)
			lenContours = len(sortedContours)
			if lenContours >= 2:
					bigContour = sortedContours[lenContours - 1][0]
						smallContour = sortedContours[lenContours - 2][0]
						xb, yb, wb, hb = cv2.boundingRect(bigContour)
						xs, ys, ws, hs = cv2.boundingRect(smallContour)
						if ys < yb :
							runner = smallContour
						else:
					runner = bigContour
						//////# shadow removal done ////#

						////// if no shadow //////////#
						elif len(contours) != 0 :
						runner = max(contours, key = cv2.contourArea)
						////// shadow part done ////////    

						if len(runner) != 0 :
							area = cv2.contourArea(runner)
							std::cout << "Area : ", area
							x, y, w, h = cv2.boundingRect(runner)
							if (area > ContAreaThresh) and (w < h) :
								////////// 
								cx = x + w / 2
								cy = y + h / 2
								trackPoints.append(((cx, cy), framenumber))
								cv2.circle(firstFrame, (cx, cy), 3, (0, 0, 255), -1)
								dimensions.append((x, y, w, h))
								cv2.rectangle(frame, (x, y), (x + w, y + h), (255, 255, 0), 1)
								//////////
		}
		cv2.line(frame, point1, point2, (0, 255, 0), 2, cv2.LINE_AA)
		cv2.line(frame, point3, point4, (0, 0, 255), 2, cv2.LINE_AA)
		cv2.imshow('frame', frame)
	}
	//# out of while loop - CALCULATION FOR THE INTERSECTION ////#
	std::cout<< "dimensions : ", dimensions
	cv2.line(firstFrame, point1, point2, (0, 255, 0), 2, cv2.LINE_AA) #stop line
	cv2.line(firstFrame, point3, point4, (0, 0, 255), 2, cv2.LINE_AA)  #start line
	/*
	cv::Mat image;
	image = imread("C:\\Users\\Public\\Pictures\\Sample Pictures\\Penguins.jpg");
	imshow("image",image);
	*/
	waitKey(0);
    return 0;
}
