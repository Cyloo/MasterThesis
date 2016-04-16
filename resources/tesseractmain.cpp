/**********************************************************************
* File:        tessedit.cpp  (Formerly tessedit.c)
* Description: Main program for merge of tess and editor.
* Author:                  Ray Smith
* Created:                 Tue Jan 07 15:21:46 GMT 1992
*
* (C) Copyright 1992, Hewlett-Packard Ltd.
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** http://www.apache.org/licenses/LICENSE-2.0
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*
**********************************************************************/

// Include automatically generated configuration file if running autoconf
#ifdef HAVE_CONFIG_H
#include "config_auto.h"
#endif

#include <iostream>

#include "allheaders.h"
#include "baseapi.h"
#include "basedir.h"
#include "renderer.h"
#include "strngs.h"
#include "tprintf.h"
#include "openclwrapper.h"
#include "osdetect.h"

//opencv lib
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

/**********************************************************************
 *  main()
 *
 **********************************************************************/
/*
int main(int argc, char **argv) {
  if ((argc == 2 && strcmp(argv[1], "-v") == 0) ||
      (argc == 2 && strcmp(argv[1], "--version") == 0)) {
    char *versionStrP;

    fprintf(stderr, "tesseract %s\n", tesseract::TessBaseAPI::Version());

    versionStrP = getLeptonicaVersion();
    fprintf(stderr, " %s\n", versionStrP);
    lept_free(versionStrP);

    versionStrP = getImagelibVersions();
    fprintf(stderr, "  %s\n", versionStrP);
    lept_free(versionStrP);


#ifdef USE_OPENCL
    cl_platform_id platform;
    cl_uint num_platforms;
    cl_device_id devices[2];
    cl_uint num_devices;
    char info[256];
    int i;

    fprintf(stderr, " OpenCL info:\n");
    clGetPlatformIDs(1, &platform, &num_platforms);
    fprintf(stderr, "  Found %d platforms.\n", num_platforms);
    clGetPlatformInfo(platform, CL_PLATFORM_NAME, 256, info, 0);
    fprintf(stderr, "  Platform name: %s.\n", info);
    clGetPlatformInfo(platform, CL_PLATFORM_VERSION, 256, info, 0);
    fprintf(stderr, "  Version: %s.\n", info);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 2, devices, &num_devices);
    fprintf(stderr, "  Found %d devices.\n", num_devices);
    for (i = 0; i < num_devices; ++i) {
      clGetDeviceInfo(devices[i], CL_DEVICE_NAME, 256, info, 0);
      fprintf(stderr, "    Device %d name: %s.\n", i+1, info);
    }
#endif
    exit(0);
  }


  // Make the order of args a bit more forgiving than it used to be.
  const char* lang = "eng";
  const char* image = NULL;
  const char* outputbase = NULL;
  const char* datapath = NULL;
  bool noocr = false;
  bool list_langs = false;
  bool print_parameters = false;
  GenericVector<STRING> vars_vec, vars_values;

  tesseract::PageSegMode pagesegmode = tesseract::PSM_AUTO;
  int arg = 1;
  while (arg < argc && (outputbase == NULL || argv[arg][0] == '-')) {
    if (strcmp(argv[arg], "-l") == 0 && arg + 1 < argc) {
      lang = argv[arg + 1];
      ++arg;
    } else if (strcmp(argv[arg], "--tessdata-dir") == 0 && arg + 1 < argc) {
      datapath = argv[arg + 1];
      ++arg;
    } else if (strcmp(argv[arg], "--user-words") == 0 && arg + 1 < argc) {
      vars_vec.push_back("user_words_file");
      vars_values.push_back(argv[arg + 1]);
      ++arg;
    } else if (strcmp(argv[arg], "--user-patterns") == 0 && arg + 1 < argc) {
      vars_vec.push_back("user_patterns_file");
      vars_values.push_back(argv[arg + 1]);
      ++arg;
    } else if (strcmp(argv[arg], "--list-langs") == 0) {
      noocr = true;
      list_langs = true;
    } else if (strcmp(argv[arg], "-psm") == 0 && arg + 1 < argc) {
      pagesegmode = static_cast<tesseract::PageSegMode>(atoi(argv[arg + 1]));
      ++arg;
    } else if (strcmp(argv[arg], "--print-parameters") == 0) {
      noocr = true;
      print_parameters = true;
    } else if (strcmp(argv[arg], "-c") == 0 && arg + 1 < argc) {
      // handled properly after api init
      ++arg;
    } else if (image == NULL) {
      image = argv[arg];
    } else if (outputbase == NULL) {
      outputbase = argv[arg];
    }
    ++arg;
  }

  if (argc == 2 && strcmp(argv[1], "--list-langs") == 0) {
    list_langs = true;
    noocr = true;
  }

  if (outputbase == NULL && noocr == false) {
    fprintf(stderr, "Usage:\n  %s imagename|stdin outputbase|stdout "
            "[options...] [configfile...]\n\n", argv[0]);

    fprintf(stderr, "OCR options:\n");
    fprintf(stderr, "  --tessdata-dir /path\tspecify the location of tessdata"
                      " path\n");
    fprintf(stderr, "  --user-words /path/to/file\tspecify the location of user"
            " words file\n");
    fprintf(stderr, "  --user-patterns /path/to/file\tspecify the location of"
            " user patterns file\n");
    fprintf(stderr, "  -l lang[+lang]\tspecify language(s) used for OCR\n");
    fprintf(stderr, "  -c configvar=value\tset value for control parameter.\n"
                      "\t\t\tMultiple -c arguments are allowed.\n");
    fprintf(stderr, "  -psm pagesegmode\tspecify page segmentation mode.\n");
    fprintf(stderr, "These options must occur before any configfile.\n\n");
    fprintf(stderr,
            "pagesegmode values are:\n"
            "  0 = Orientation and script detection (OSD) only.\n"
            "  1 = Automatic page segmentation with OSD.\n"
            "  2 = Automatic page segmentation, but no OSD, or OCR\n"
            "  3 = Fully automatic page segmentation, but no OSD. (Default)\n"
            "  4 = Assume a single column of text of variable sizes.\n"
            "  5 = Assume a single uniform block of vertically aligned text.\n"
            "  6 = Assume a single uniform block of text.\n"
            "  7 = Treat the image as a single text line.\n"
            "  8 = Treat the image as a single word.\n"
            "  9 = Treat the image as a single word in a circle.\n"
            "  10 = Treat the image as a single character.\n\n");
    fprintf(stderr, "Single options:\n");
    fprintf(stderr, "  -v --version: version info\n");
    fprintf(stderr, "  --list-langs: list available languages for tesseract "
                      "engine. Can be used with --tessdata-dir.\n");
    fprintf(stderr, "  --print-parameters: print tesseract parameters to the "
                      "stdout.\n");
    exit(1);
  }

  if (outputbase != NULL && strcmp(outputbase, "-") &&
      strcmp(outputbase, "stdout")) {
    tprintf("Tesseract Open Source OCR Engine v%s with Leptonica\n",
           tesseract::TessBaseAPI::Version());
  }
  PERF_COUNT_START("Tesseract:main")
  tesseract::TessBaseAPI api;

  api.SetOutputName(outputbase);
  int rc = api.Init(datapath, lang, tesseract::OEM_DEFAULT,
                &(argv[arg]), argc - arg, &vars_vec, &vars_values, false);

  if (rc) {
    fprintf(stderr, "Could not initialize tesseract.\n");
    exit(1);
  }

  char opt1[255], opt2[255];
  for (arg = 0; arg < argc; arg++) {
    if (strcmp(argv[arg], "-c") == 0 && arg + 1 < argc) {
      strncpy(opt1, argv[arg + 1], 255);
      char *p = strchr(opt1, '=');
      if (!p) {
        fprintf(stderr, "Missing = in configvar assignment\n");
        exit(1);
      }
      *p = 0;
      strncpy(opt2, strchr(argv[arg + 1], '=') + 1, 255);
      opt2[254] = 0;
      ++arg;

      if (!api.SetVariable(opt1, opt2)) {
        fprintf(stderr, "Could not set option: %s=%s\n", opt1, opt2);
      }
    }
  }

  if (list_langs) {
     GenericVector<STRING> languages;
     api.GetAvailableLanguagesAsVector(&languages);
     fprintf(stderr, "List of available languages (%d):\n",
             languages.size());
     for (int index = 0; index < languages.size(); ++index) {
       STRING& string = languages[index];
       fprintf(stderr, "%s\n", string.string());
     }
     api.End();
     exit(0);
  }

  if (print_parameters) {
     FILE* fout = stdout;
     fprintf(stdout, "Tesseract parameters:\n");
     api.PrintVariables(fout);
     api.End();
     exit(0);
  }

  // We have 2 possible sources of pagesegmode: a config file and
  // the command line. For backwards compatability reasons, the
  // default in tesseract is tesseract::PSM_SINGLE_BLOCK, but the
  // default for this program is tesseract::PSM_AUTO. We will let
  // the config file take priority, so the command-line default
  // can take priority over the tesseract default, so we use the
  // value from the command line only if the retrieved mode
  // is still tesseract::PSM_SINGLE_BLOCK, indicating no change
  // in any config file. Therefore the only way to force
  // tesseract::PSM_SINGLE_BLOCK is from the command line.
  // It would be simpler if we could set the value before Init,
  // but that doesn't work.
  if (api.GetPageSegMode() == tesseract::PSM_SINGLE_BLOCK)
     api.SetPageSegMode(pagesegmode);

  if (pagesegmode == tesseract::PSM_AUTO_ONLY ||
      pagesegmode == tesseract::PSM_OSD_ONLY) {
    int ret_val = 0;

    Pix* pixs = pixRead(image);
    if (!pixs) {
      fprintf(stderr, "Cannot open input file: %s\n", image);
      exit(2);
    }
    api.SetImage(pixs);

    if (pagesegmode == tesseract::PSM_OSD_ONLY) {
       OSResults osr;
       if (api.DetectOS(&osr)) {
         int orient = osr.best_result.orientation_id;
         int script_id = osr.get_best_script(orient);
         float orient_oco = osr.best_result.oconfidence;
         float orient_sco = osr.best_result.sconfidence;
         tprintf("Orientation: %d\nOrientation in degrees: %d\n" \
                 "Orientation confidence: %.2f\n" \
                 "Script: %d\nScript confidence: %.2f\n",
                 orient, OrientationIdToValue(orient), orient_oco,
                 script_id, orient_sco);
       } else {
         ret_val = 1;
       }
    } else {
       tesseract::Orientation orientation;
       tesseract::WritingDirection direction;
       tesseract::TextlineOrder order;
       float deskew_angle;
       tesseract::PageIterator* it =  api.AnalyseLayout();
       if (it) {
         it->Orientation(&orientation, &direction, &order, &deskew_angle);
         tprintf("Orientation: %d\nWritingDirection: %d\nTextlineOrder: %d\n" \
                 "Deskew angle: %.4f\n",
                 orientation, direction, order, deskew_angle);
       } else {
         ret_val = 1;
       }
       delete it;
    }
    pixDestroy(&pixs);
    exit(ret_val);
  }

  bool b;
  tesseract::PointerVector<tesseract::TessResultRenderer> renderers;
  api.GetBoolVariable("tessedit_create_hocr", &b);
  if (b) {
    bool font_info;
    api.GetBoolVariable("hocr_font_info", &font_info);
    renderers.push_back(new tesseract::TessHOcrRenderer(outputbase, font_info));
  }
  api.GetBoolVariable("tessedit_create_pdf", &b);
  if (b) {
    renderers.push_back(new tesseract::TessPDFRenderer(outputbase,
                                                       api.GetDatapath()));
  }
  api.GetBoolVariable("tessedit_write_unlv", &b);
  if (b) renderers.push_back(new tesseract::TessUnlvRenderer(outputbase));
  api.GetBoolVariable("tessedit_create_boxfile", &b);
  if (b) renderers.push_back(new tesseract::TessBoxTextRenderer(outputbase));
  api.GetBoolVariable("tessedit_create_txt", &b);
  if (b) renderers.push_back(new tesseract::TessTextRenderer(outputbase));
  if (!renderers.empty()) {
    // Since the PointerVector auto-deletes, null-out the renderers that are
    // added to the root, and leave the root in the vector.
    for (int r = 1; r < renderers.size(); ++r) {
      renderers[0]->insert(renderers[r]);
      renderers[r] = NULL;
    }
    if (!api.ProcessPages(image, NULL, 0, renderers[0])) {
      fprintf(stderr, "Error during processing.\n");
      exit(1);
    }
  }

  PERF_COUNT_END
  return 0;                      // Normal exit
}
*/

using namespace cv;
using namespace std;

void trim(char str[]);
char* resultTransfer(char* str);
//utf-8转unicode  
wchar_t * Utf_8ToUnicode(char* szU8)
{
	//UTF8 to Unicode  
	//由于中文直接复制过来会成乱码，编译器有时会报错，故采用16进制形式  

	//预转换，得到所需空间的大小  
	int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), NULL, 0);
	//分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间  
	wchar_t* wszString = new wchar_t[wcsLen + 1];
	//转换  
	::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), wszString, wcsLen);
	//最后加上'\0'  
	wszString[wcsLen] = '\0';
	return wszString;
}

//将宽字节wchar_t*转化为单字节char*    
char* UnicodeToAnsi(const wchar_t* szStr)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL);
	if (nLen == 0)
	{
		return NULL;
	}
	char* pResult = new char[nLen];

	WideCharToMultiByte(CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL);

	return pResult;

}

int test_main()
{
	Mat src = imread("bingli.jpg", 0);
	Mat dst;
	Mat eroded, binary_coded;
	//提取主体部分
	Mat roi = src(Rect(0.2*src.cols, 0.15*src.rows, 0.73*src.cols, 0.8*src.rows));

	imshow("src", src);

	threshold(roi, dst, 0, 255, THRESH_BINARY);
	Mat element = getStructuringElement(MORPH_RECT, Size(35, 7));
	imshow("binary", dst);
	erode(dst, eroded, element);
	imshow("erode", eroded);


	//the pram. for findContours,  
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(eroded, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/*
	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
	}
	*/
	return 0;
}

Mat ImageResize(Mat origin, double scale, int binary_threshold)
{
	/*
	//如果图片较大，就不再放缩
	if (origin.cols*origin.rows > 100000)
		return origin;
		*/
	//按比例放缩，横向和纵向放缩的比例为3:4(对于页面3）
	
	Size dsize = Size(origin.cols*3, origin.rows*4);
	Mat resized = Mat(dsize, CV_32S);
	resize(origin, resized, dsize);

	//进行二值化
	Mat binary;
	threshold(resized, binary, 180, 255, THRESH_BINARY);

	return binary;
}

int main() {

	/*
	//文件操作相关
	FILE* file = fopen("test.csv", "r+");
	if (file == NULL)
		file = fopen("test.csv", "w+");
	fseek(file, 0, SEEK_END);
	fputs("hello,world\n", file);
	fputs("hah,test", file);
	fseek(file, -1L, SEEK_CUR);
	fputs("a\n", file);
	fclose(file);
	*/

	Mat src = imread("bingli7.png", 0);
	Mat dst;
	Mat eroded, binary_coded;

	//提取主体部分
	Mat roi = src(Rect(0.2*src.cols, 0.15*src.rows, 0.73*src.cols, 0.8*src.rows));

	imshow("src", src);

	threshold(roi, dst, 0, 255, THRESH_BINARY);
	Mat element = getStructuringElement(MORPH_RECT, Size(35, 7));
	imshow("binary", dst);
	erode(dst, eroded, element);
	imshow("erode", eroded);

	/*
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(eroded, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point>> contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());

	// 寻找最大连通域
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
	// 将轮廓转为矩形框
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
	
	// transfer to binary_image
	threshold(eroded, binary_coded, 0, 255, THRESH_BINARY);
	//imshow("binary_coded", binary_coded);
	*/

	//the pram. for findContours,  
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(eroded, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
	}


	/// Draw polygonal contour + bonding rects + circles
	Mat drawing = Mat::zeros(roi.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		//Scalar color = Scalar(0, 0, 0);
		//将轮廓线设为白色，不影响进一步的识别
		Scalar color = Scalar(255, 255, 255);
		//drawContours(drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		if (boundRect[i].height < 13 || boundRect[i].width < 50)
			continue;
		rectangle(roi, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
	}

	/// Show in a window
	namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	imshow("Contours", roi);
	Mat example;
	Mat binary;
	threshold(roi, binary, 200, 255, THRESH_BINARY);
	imshow("binary_roi", binary);

	tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
	FILE* fp = fopen("result.txt", "wb");

	// print the tesseract version
	printf("Tesseract-ocr version: %s\n", api->Version());

	// print the leptonica version
	printf("Leptonica version: %s\n", getLeptonicaVersion());


	GenericVector<STRING> pars_vec;
	pars_vec.push_back("user_words_suffix");
	pars_vec.push_back("load_system_dawg");
	pars_vec.push_back("load_freq_dawg");
	pars_vec.push_back("language_model_ngram_on");
	pars_vec.push_back("language_model_penalty_non_dict_word");
	pars_vec.push_back("language_model_penalty_non_freq_dict_word");
	GenericVector<STRING> pars_value;
	pars_value.push_back("user-words");
	pars_value.push_back("T");
	pars_value.push_back("T");
	pars_value.push_back("1");
	pars_value.push_back("0.9");
	pars_value.push_back("0.9");

	// init the tesseract
	if (api->Init("D:\\Program Files\\tesseract-build\\tesseract\\vs2013", "chi_sim", tesseract::OEM_DEFAULT, 0, 0, &pars_vec, &pars_value, false))
	//if (api->Init("D:\\Program Files\\tesseract-build\\tesseract\\vs2013", "chi_sim"))
	{
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	/*
	api->SetVariable("chop_enable", "T");
	api->SetVariable("use_new_state_cost", "F");
	api->SetVariable("segment_segcost_rating", "F");
	api->SetVariable("enable_new_segsearch", "0");
	api->SetVariable("language_model_ngram_on", "0");
	api->SetVariable("textord_force_make_prop_words", "F");
	api->SetVariable("language_model_penalty_non_dict_word", "1");
	api->SetVariable("stopper_nondict_certainty_base", "-100");
	*/

	// load and set image
	//Pix* pix = pixRead("example.jpg");
	//api->SetImage(pix);
	//api->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);

	//for (int i = 0; i < boundRect.size(); i++)
	for (int i = 0; i < 1; i++)
	{
		example = roi(boundRect[i]);
		Mat resized = ImageResize(example, 4 , 180);
		char s[16];
		sprintf(s, "%d", i);
		//if (i == 3)
			//imwrite("main.jpg", resized);
		imshow(s, resized);

		api->SetImage((uchar*)resized.data, resized.cols, resized.rows, 1, resized.step1());
		// recognize image
		char* outText = api->GetUTF8Text();
		char* resulttemp = resultTransfer(outText);
		//strcpy(temp, resulttemp);
		printf("region num%d is: %s\n", i, resulttemp);
		fprintf(fp,"region num%d is: %s\n", i, outText);

	}


	//fwrite(outText, sizeof(char), strlen(outText), fp);

	// free resources
	fclose(fp);
	api->Clear();
	api->End();
	//delete[] outText;
	//pixDestroy(&pix);
	waitKey();
	return 0;
}

//比较函数，用来判断某个字符串s1是不是以字符串s2开头
int startWith(char s1[], char s2[])
{
#define UpperChar(c) (((c)>='a'&&(c)<='z')?(c)+'A'-'a':(c))
	int i;
	for (i = 0; s1[i] != '\0'&&s2[i] != '\0'; i++)
	{
		/*any different?*/
		if (UpperChar(s1[i]) != UpperChar(s2[i])) return 0;
	}
	/*s2 is longer than s1*/
	if (s2[i] != '\0') return 0;
	return 1;
}

//去除回车，空格，tab等符号
void trim(char str[])
{
#define InvalidChar(c) ( ( (c) == '\n' || (c) == '\t' || (c) == ' ' ) ? 1:0 )
	int len = strlen(str);
	for (int i = 0; i < len; i++)
	{
		if (InvalidChar(str[i]))
			str[i] = '\0';
	}
}

int test1(void)
{
	tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
	FILE* fp = fopen("result.txt", "wb");

	// print the tesseract version
	printf("Tesseract-ocr version: %s\n", api->Version());

	// print the leptonica version
	printf("Leptonica version: %s\n", getLeptonicaVersion());

	/*
	GenericVector<STRING> pars_vec;
	pars_vec.push_back("user_words_suffix");
	pars_vec.push_back("load_system_dawg");
	pars_vec.push_back("load_freq_dawg");
	pars_vec.push_back("language_model_ngram_on");
	pars_vec.push_back("language_model_penalty_non_dict_word");
	GenericVector<STRING> pars_value;
	pars_value.push_back("user-words");
	pars_value.push_back("F");
	pars_value.push_back("F");
	pars_value.push_back("1");
	pars_value.push_back("1");
	*/



	// init the tesseract
	//if (api->Init("D:\\Program Files\\tesseract-build\\tesseract\\vs2013", "chi_sim", tesseract::OEM_DEFAULT, 0, 0, &pars_vec, &pars_value, false))
	if (api->Init("D:\\Program Files\\tesseract-build\\tesseract\\vs2013", "chi_sim", tesseract::OEM_DEFAULT))
	{
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	/*
	api->SetVariable("chop_enable", "T");
	api->SetVariable("use_new_state_cost", "F");
	api->SetVariable("segment_segcost_rating", "F");
	api->SetVariable("enable_new_segsearch", "0");
	api->SetVariable("language_model_ngram_on", "0");
	api->SetVariable("textord_force_make_prop_words", "F");
	api->SetVariable("language_model_penalty_non_dict_word", "1");
	api->SetVariable("stopper_nondict_certainty_base", "-100");
	*/

	// load and set image
	Pix* pix = pixRead("huli.png");
	api->SetImage(pix);
	api->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
	char* outText = api->GetUTF8Text();
	wchar_t *tempchar;
	char * resulttemp;
	tempchar = Utf_8ToUnicode(outText);
	resulttemp = UnicodeToAnsi(tempchar);
	//strcpy(temp, resulttemp);
	printf("%s", resulttemp);
	trim(resulttemp);
	printf("%s", resulttemp);
	if (startWith(resulttemp, "今夭不用上班5"))
		cout << "match" << endl;
	else
		cout << "not match" << endl;
	if (strcmp(resulttemp, "今夭不用上班5") == 0)
		cout << "match" << endl;
	else
		cout << "not match" << endl;

	cout << "length1:" << strlen(resulttemp) << endl;
	cout << "length2:" << strlen("今夭不用上班5") << endl;
	system("pause");

}

char* resultTransfer(char* str)
{
	char* temp = UnicodeToAnsi(Utf_8ToUnicode(str));
	//trim(temp);
	return temp;
}

int confidence_check(void)
//int main(void)
{
	FILE* file = fopen("result_resize.txt", "w+");
	Pix *image = pixRead("bingqing.jpg");
	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

	GenericVector<STRING> pars_vec;
	pars_vec.push_back("user_words_suffix");
	pars_vec.push_back("load_system_dawg");
	pars_vec.push_back("load_freq_dawg");
	pars_vec.push_back("language_model_ngram_on");
	pars_vec.push_back("language_model_penalty_non_dict_word");
	pars_vec.push_back("language_model_penalty_non_freq_dict_word");
	GenericVector<STRING> pars_value;
	pars_value.push_back("user-words");
	pars_value.push_back("T");
	pars_value.push_back("T");
	pars_value.push_back("1");
	pars_value.push_back("0.0");
	pars_value.push_back("0.0");

	// init the tesseract
	if (api->Init("D:\\Program Files\\tesseract-build\\tesseract\\vs2013", "chi_sim", tesseract::OEM_DEFAULT, 0, 0, &pars_vec, &pars_value, false))
		//if (api->Init("D:\\Program Files\\tesseract-build\\tesseract\\vs2013", "chi_sim"))
	{
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	//api->SetVariable("language_model_penalty_non_dict_word", "0.0");
	//api->SetVariable("language_model_penalty_non_freq_dict_word", "0.0");
	//api->Init("D:\\Program Files\\tesseract-build\\tesseract\\vs2013", "chi_sim", tesseract::OEM_DEFAULT);
	api->SetImage(image);
	api->SetVariable("save_blob_choices", "T");
	//api->SetRectangle(37, 228, 548, 31);
	api->Recognize(NULL);

	tesseract::ResultIterator* ri = api->GetIterator();
	tesseract::PageIteratorLevel level = tesseract::RIL_SYMBOL;
	if (ri != 0) {
		do {
			char* symbol = ri->GetUTF8Text(level);
			const char* sym = UnicodeToAnsi(Utf_8ToUnicode(symbol));
			float conf = ri->Confidence(level);
			if (symbol != 0) {
				printf("symbol %s, conf: %f", sym, conf);
				fprintf(file,"symbol %s, conf: %f", sym, conf);
				bool indent = false;
				tesseract::ChoiceIterator ci(*ri);
				do {
					if (indent) printf("\t\t ");
					printf("\t- ");
					const char* choice = ci.GetUTF8Text();
					char* choo = new char[strlen(choice) + 1];
					strcpy(choo, choice);
					const char* cho = UnicodeToAnsi(Utf_8ToUnicode(choo));
					printf("%s conf: %f\n", cho, ci.Confidence());
					fprintf(file, "%s conf: %f\n", cho, ci.Confidence());
					indent = true;
				} while (ci.Next());
			}
			printf("---------------------------------------------\n");
			fprintf(file,"---------------------------------------------\n");
			delete[] symbol;
		} while ((ri->Next(level)));
	}
	fclose(file);
	//waitKey();
	system("pause");
	return 0;
}
