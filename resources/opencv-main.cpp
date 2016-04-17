
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
using namespace std;
using namespace cv;

//int read_images_underfolder_test()
int main()
{
	vector<String> filenames; // notice here that we are using the Opencv's embedded "String" class
	String folder = "subs"; // again we are using the Opencv's embedded "String" class

	glob(folder, filenames); // new function that does the job ;-)
	for (size_t i = 0; i < filenames.size(); ++i)
	{
		Mat src = imread(filenames[i]);

		if (!src.data){
			cerr << "Problem loading image!!!" << endl;
		}

		/* do whatever you want with your images here */
		imshow("test", src);
		waitKey();
	}

}

Mat ImageResize(Mat origin, int layoutType)
{
	/*
	//如果图片较大，就不再放缩
	if (origin.cols*origin.rows > 100000)
	return origin;
	*/
	//按比例放缩，横向和纵向放缩的比例为3:4(对于页面3）

	Mat binary;
	if (layoutType == 1)
	{
		Size dsize = Size(origin.cols * 4, origin.rows * 4);
		Mat resized = Mat(dsize, CV_32S);
		resize(origin, resized, dsize);
		//进行二值化
		threshold(resized, binary, 180, 255, THRESH_BINARY);
	}
	else if (layoutType == 2)
	{
		Size dsize = Size(origin.cols * 4, origin.rows * 4);
		Mat resized = Mat(dsize, CV_32S);
		resize(origin, resized, dsize);
		//进行二值化
		threshold(resized, binary, 180, 255, THRESH_BINARY);
	}
	else
	{
		Size dsize = Size(origin.cols * 3, origin.rows * 4);
		Mat resized = Mat(dsize, CV_32S);
		resize(origin, resized, dsize);
		//进行二值化
		threshold(resized, binary, 180, 255, THRESH_BINARY);
	}

	return binary;
}

int generate_new_training_data()
{
	vector<String> filenames; // notice here that we are using the Opencv's embedded "String" class
	String folder = "training-data-organized"; // again we are using the Opencv's embedded "String" class

	glob(folder, filenames); // new function that does the job ;-)
	for (size_t i = 0; i < filenames.size(); ++i)
	{
		Mat src = imread(filenames[i], 0);
		if (!src.data){
			cerr << "Problem loading image!!!" << endl;
		}

		Mat dst;
		Mat eroded, binary_coded;

		string new_filename_temp = filenames[i];
		int slash_pos = new_filename_temp.find("\\"), dot_pos = new_filename_temp.find(".");
		string new_filename = new_filename_temp.substr(slash_pos + 1, dot_pos - slash_pos - 1) + ".tif";
		string new_filename_withDirectory = "new-training-data/" + new_filename;

		//提取主体部分
		Mat roi = src(Rect(0.2*src.cols, 0.167*src.rows, 0.73*src.cols, 0.8*src.rows));
		Mat output;
		if (new_filename.at(2) == '1')
			output = ImageResize(roi, 1);
		else if (new_filename.at(2) == '2')
			output = ImageResize(roi, 2);
		else
			output = ImageResize(roi, 3);
		//imshow(new_filename_withDirectory, output);
		imwrite(new_filename_withDirectory, output);
		cout << new_filename << endl;
	}
	waitKey();
	return 0;
}
/*
/// Global Variables
Mat img; Mat templ; Mat result;
char* image_window = "Source Image";
char* result_window = "Result window";

int match_method;
int max_Trackbar = 5;

/// Function Headers
void MatchingMethod(int, void*);

int main(int argc, char** argv)
{
	/// Load image and template
	img = imread("new-bingli1.png", 1);
	templ = imread("sidebar-template.jpg", 1);

	/// Create windows
	namedWindow(image_window, WINDOW_AUTOSIZE);
	namedWindow(result_window, WINDOW_AUTOSIZE);

	/// Create Trackbar
	char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
	createTrackbar(trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod);

	MatchingMethod(0, 0);

	waitKey(0);
	return 0;
}

void MatchingMethod(int, void*)
{
	/// Source image to display
	Mat img_display;
	img.copyTo(img_display);

	/// Create the result matrix
	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;

	result.create(result_cols, result_rows, CV_32FC1);

	/// Do the Matching and Normalize
	matchTemplate(img, templ, result, match_method);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	/// Localizing the best match with minMaxLoc
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
	if (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		matchLoc = maxLoc;
	}

	/// Show me what you got
	rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);

	imshow(image_window, img_display);
	imshow(result_window, result);

	return;
}
*/

/*
Mat MatchingMethod(Mat, Mat, int);

int main(int, char** argv)
{
	//img = imread(argv[1], 1);
	//templ = imread(argv[2], 1);
	Mat img = imread("new-bingli1.png", 1);
	Mat title_template = imread("title-template.jpg", 1);
	Mat sidebar_template = imread("sidebar-template.jpg", 1);
	Mat cut_title = MatchingMethod(img, title_template, 0);
	Mat cut_sidebar = MatchingMethod(cut_title, sidebar_template, 1);
	imshow("cut-title", cut_title);
	imshow("cut-sidebar", cut_sidebar);
	//imwrite("cut_title.png", cut_title);
	//imwrite("new-bingli1-cut.png", cut_sidebar);
	waitKey(0);
	return 0;
}

Mat MatchingMethod(Mat src, Mat _template, int template_type)
{
	// template_type option:  0 -> title, 1 -> sidebar
	Mat result;
	int match_method = CV_TM_CCORR_NORMED;
	Mat img_display;
	src.copyTo(img_display);
	int result_cols = src.cols - _template.cols + 1;
	int result_rows = src.rows - _template.rows + 1;
	result.create(result_rows, result_cols, CV_32FC1);
	matchTemplate(src, _template, result, match_method);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	matchLoc = maxLoc;
	Mat roi;
	if (template_type)
		roi = src(Rect(matchLoc.x + _template.cols, matchLoc.y, src.cols - matchLoc.x - _template.cols, _template.rows));
	else
		roi = src(Rect(0, matchLoc.y + _template.rows, matchLoc.x + _template.cols, src.rows - matchLoc.y - _template.rows));
	return roi;
}
*/

int eroding_and_dilating()
//int main()
{
	Mat image = imread("dangan.png", 0);
	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	Mat eroded, dilated;
	erode(image, eroded, element);
	dilate(image, dilated, element);
	imwrite("dangan_eroded.png", eroded);
	imwrite("dangan_dilated.png", dilated);
	imshow("origin", image);
	imshow("eroded", eroded);
	imshow("dilated", dilated);
	waitKey();
	return 0;
}

int binary_test()
//int main(int argc, char** argv)
{
	cv::Mat image = cv::imread("lena.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	if (image.empty())
	{
		std::cout << "read image failure" << std::endl;
		return -1;
	}

	// ȫ�ֶ�ֵ��
	Mat global1, global2;
	cv::threshold(image, global1, 70, 255, CV_THRESH_BINARY);
	cv::threshold(image, global2, 130, 255, CV_THRESH_BINARY);
	//cv::threshold(image, global, th, 255, THRESH_BINARY);
	//cv::threshold(image, global, th, 255, THRESH_BINARY + THRESH_OTSU);


	// �ֲ���ֵ��

	int blockSize = 25;
	int constValue = 10;
	cv::Mat local;
	//cv::adaptiveThreshold(image, local, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, blockSize, constValue);
	cv::adaptiveThreshold(image, local, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, blockSize, constValue);


	cv::imwrite("lena_binary_70.png", global1);
	cv::imwrite("lena_binary_130.png", global2);
	cv::imwrite("lena_binary_adaptive.png", local);

	cv::imshow("global1Threshold", global1);
	cv::imshow("global2Threshold", global2);
	cv::imshow("localThreshold", local);
	cv::waitKey(0);


	return 0;
}

int grey_test(void)
// int main(void)
{
	Mat lena = imread("lena.bmp", 1);
	imshow("lena_origin", lena);
	Mat lena_grey = imread("lena.bmp", 0);
	imwrite("lena_grey.png", lena_grey);
	imshow("lena_grey", lena_grey);
	Mat _binary;
	threshold(lena_grey, _binary, 80, 255, THRESH_BINARY);
	imshow("lena_binary", _binary);
	waitKey();
	return 0;
}

int test_main()
//int main()
{
	Mat src = imread("bingli.jpg", 0);
	Mat dst;
	Mat eroded, binary_coded;

	//��ȡ���岿��
	Mat roi = src(Rect(0.2*src.cols, 0.15*src.rows, 0.73*src.cols, 0.8*src.rows));

	//��ֵ��
	threshold(roi, dst, 0, 255, THRESH_BINARY);

	imshow("src", src);

	/*
	//���е�һ����ʴ����ʴǿ�Ƚϸߣ��Ա��ҵ�������ͨ����
	Mat element = getStructuringElement(MORPH_RECT, Size(40, 70));
	erode(dst, eroded, element);
	imshow("erode", eroded);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(eroded, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	drawContours(src, contours, -1, Scalar(123));

	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point>> contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());

	// Ѱ��������ͨ��
	double maxArea = 0, secondArea = 0;
	vector<cv::Point> maxContour;
	for (size_t i = 0; i < contours.size(); i++)
	{
		double area = cv::contourArea(contours[i]);
		if (area > maxArea)
		{
			maxArea = area;
			maxContour = contours[i];
		}
	}

	// ������תΪ���ο�
	Rect maxRect = cv::boundingRect(maxContour);

	Rect maxRegion;
	int maxArea = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
		if ( boundRect[i].area() > maxArea){
			maxRegion = boundRect[i];
			maxArea = boundRect[i].area();
		}
	}

	Mat drawing = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		//Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		Scalar color = Scalar(0, 0, 0);
		//drawContours(drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		rectangle(eroded, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
	}
	imshow("src_withContours", eroded);

	Mat mainRegion = src(maxRect);
	imshow("mainRegion", mainRegion);
	*/

	Mat element = getStructuringElement(MORPH_RECT, Size(35, 7));
	erode(dst, eroded, element);
	imshow("erode", eroded);
	// transfer to binary_image
	threshold(eroded, binary_coded, 0, 255, THRESH_BINARY);
	imshow("binary_coded", binary_coded);

	//the pram. for findContours,
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours( binary_coded, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
		Mat window = roi(boundRect[i]);
		char str[10];
		_itoa(i, str, 10);
		string prefix = str;
		string filename = "subs/example" + prefix + ".png";
		imwrite(filename, window);
	}



	/// Draw polygonal contour + bonding rects + circles
	Mat drawing = Mat::zeros(roi.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		//Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		//Scalar color = Scalar(0,0,0);
		Scalar color = Scalar(255,255,255);
		//drawContours(drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		if (boundRect[i].height < 13 || boundRect[i].width < 50)
			continue;
		rectangle(roi, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
	}

	/// Show in a window
	namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	imshow("Contours", roi);

	waitKey();
	return 0;
}

int test1_main(void)
{
	Mat src = imread("ruyuan.JPG", 0);
	Mat dst[5];
	Size dsize = Size(src.cols * 4, src.rows * 4);
	cv::String str[5];
	for (int i = 0; i < 5; i++)
	{
		resize(src, dst[i], dsize, 0.0, 0.0, i);
		char s[3];
		sprintf(s, "%d", i);
		str[i] = s;
		imshow("resized" + str[i], dst[i]);
	}
	waitKey();
	return 0;
}
