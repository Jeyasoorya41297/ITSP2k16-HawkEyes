/*Camera calibration with some images
	Gets image folder(images to be named numberically), format of image and output file as input

	Output file contains
		1. Camera matrix
		2. Distortion coefficient
		3. rvec and tvec of the last image wrt camera
		4. optimal camera matrix
		5. rms error

*/

#include <calibration.h>

#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768
void correctFormat();
using namespace cv;
using namespace std;
const int n_cols = 7;
const int n_rows = 8;
const int n_points = n_cols * n_rows;
const float sqSize = 7.5;
int n_boards;
const Size board_size = Size(n_cols, n_rows);
Mat img;
Mat img_gray;
Mat camera_matrix = Mat::zeros(3, 3, CV_64F);
Mat dist_coeff = Mat::zeros(5, 1, CV_64F);
vector<Mat> rvecs;
vector<Mat> tvecs;
Mat temp;
double rms;
bool found;
Size image_size;

void initObj_pts(vector<vector<Point3f> > &obj_pts){
	for (int i = 0; i < n_rows; i++)				
		for (int j = 0; j < n_cols; j++)
			obj_pts[0].push_back(Point3f(j * sqSize, i * sqSize, 0));
	obj_pts.resize(n_boards, obj_pts[0]);
}
int main(int argc, char** argv){
	if (argc != 4){
		cout << "Wrong number of arguments" << endl;
		correctFormat();
		return 1;
	}
	else{
		correctFormat();
		cout << "Check whether input is in above format.\nContinue?(y/n)?";
		char correct = 'n';
		cin >> correct;
		if (correct != 'y')
			return 1;
		cout << "Enter number of images";
		cin >> n_boards;
		int flag = 0;
		flag |= CALIB_FIX_K3 ;// CALIB_ZERO_TANGENT_DIST |CALIB_FIX_K2 | CALIB_FIX_K1;

		char filename[50];
		vector<vector<Point3f> > obj_pts(1);
		vector<vector<Point2f> > img_pts;
		vector<Point2f> pointBuf;
		initObj_pts(obj_pts);
		namedWindow("Chessboard Corners", WINDOW_NORMAL);
		resizeWindow("Chessboard Corners", SCREEN_WIDTH, SCREEN_WIDTH);
		
		for (int i = 1; i <= n_boards; i++){
			sprintf(filename, "%s/%d.%s", argv[1], i, argv[2]);
			img = imread(filename, IMREAD_COLOR);
			found = findChessboardCorners(img, board_size, pointBuf);
			if (!found)
				continue;
			cvtColor(img, img_gray, COLOR_BGR2GRAY);
			cornerSubPix(img_gray, pointBuf,Size(11,11),Size(-1,-1),TermCriteria(TermCriteria::EPS+TermCriteria::COUNT,30, 0.1));
			img_pts.push_back(pointBuf);
			drawChessboardCorners(img, board_size, pointBuf, found);
			imshow("Chessboard Corners", img);
			waitKey(0);
		}
		destroyWindow("Chessboard Corners");
		image_size = Size(img.cols, img.rows);
		rms = calibrateCamera(obj_pts, img_pts, image_size, camera_matrix, dist_coeff, rvecs, tvecs,flag);
		Mat optCamMat = getOptimalNewCameraMatrix(camera_matrix,dist_coeff,image_size,1,image_size);
		Mat map1, map2;
		initUndistortRectifyMap(camera_matrix, dist_coeff, Mat(),optCamMat,image_size,CV_16SC2,map1,map2);
		Mat rot_vec;
		Mat proj_mat;
		Rodrigues(rvecs[n_boards - 1], rot_vec);
		Mat rotaugtrans = Mat::zeros(3, 4, CV_64F);
		for (int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
				rotaugtrans.at<double>(i, j) = rot_vec.at<double>(i, j);
			rotaugtrans.at<double>(i, 3) = tvecs[n_boards - 1].at<double>(i, 0);
		}
		proj_mat = optCamMat * rotaugtrans;
		FileStorage f(argv[3], FileStorage::WRITE);
		f << "Camera_matrix" << camera_matrix;
		f << "OptimalCamMatrix" << optCamMat;
		f << "dist_coeff" <<dist_coeff;
		f << "rms_error" << rms;
		f << "rvec" << rvecs[n_boards - 1];
		f << "tvec" << tvecs[n_boards - 1];
		f << "undist_mapx" << map1;
		f << "undist_mapy" << map2;
		f << "proj_mat" << proj_mat;
		f.release();
		
		namedWindow("Undistorted Images", WINDOW_NORMAL);
		resizeWindow("Undistorted Images", SCREEN_WIDTH, SCREEN_WIDTH);
		char dest_file[50];
		for (int i = 1; i <= n_boards; i++){
			sprintf(filename, "%s/%d.%s",argv[1], i, argv[2]);
			sprintf(dest_file, "%s/undistorted/%d.%s", argv[1], i, argv[2]);
			img = imread(filename, IMREAD_COLOR);
			remap(img, temp, map1, map2, INTER_LINEAR);
			imwrite(dest_file, temp);
			imshow("Undistorted Images", temp);
			waitKey(0);
		}
		return 0;
	}
}

void correctFormat(){
	cout << "First argument: folder containing calibration images" << endl;
	cout << "Second argument: format of images" << endl;
	cout << "Third argument: destination of calibration data" << endl;
	cout << "Example: ./calibration bouncetest/camera1 jpg bouncetest/camera1/calib1.xml" << endl;
	cout << "Make sure that images are named numberically starting from 1 (Ex. 1.jpg, 2.jpg ...)" << endl; 
}