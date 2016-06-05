#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv){
	char filename[40] = "images/1.jpg";
	Mat frame;
	Mat frame32f;
	Mat avg;
	const int count = 16;
	const int width = 1280;
	const int height = 720;
	const int lowThreshold = 40;
	const int upThreshold = 120;
	const int kernel_size = 3;
	avg = Mat::zeros(height,width,CV_32FC3);
	for (int i = 1; i <= count; i++){
		filename[7] = i + 48;
		frame = imread(filename, IMREAD_COLOR);
		medianBlur(frame, frame, kernel_size);
		frame.convertTo(frame32f, CV_32FC3);
		avg += frame32f;
	}
	avg = avg * (1.0 / count);
	avg.convertTo(avg, CV_8UC3);
	imshow("Average",avg);
	imwrite("images/average.jpg",avg);
	waitKey(0);
}
