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

#include "allheaders.h"
#include "baseapi.h"
#include "basedir.h"
#include "renderer.h"
#include "strngs.h"
#include "tprintf.h"
#include "openclwrapper.h"
#include "osdetect.h"

#include <iostream>
#include <hash_map>

//opencv lib-------
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//-----------------

using namespace cv;
using namespace std;

int startWith(string s1, string s2);

//建立字段映射---------------------------------
struct token_unit{
	string name;
	int index;
};
static string token_names[] = {
	"姓名",
	"性别",
	"出生日期",
	"年龄",
	"血型",
	"身高",
	"体重",
	"国籍",
	"籍贯",
	"出生地",
	"民族",
	"文化程度",
	"宗教信仰",
	"婚姻状况",
	"证件类型",
	"证件号码",
	"家庭地址",
	"户口地址",
	"入科时间",
	"入院时间",
	"病案号",
	"主治医生",
	"床号",
	"护理等级",
	"诊疗组",
	"当前科室",
	"当前病区",
	"病情",
	"入院科室",
	"入院病区",
	"门诊号",
	"住院次数",
	"住址邮编",
	"手机",
	"固定电话",
	"Email",
	"联系人姓名",
	"联系人手机",
	"关系",
	"职业",
	"单位名称",
	"单位电话",
	"单位住址",
	"单位邮编",
	"计费类型",
	"医保号",
	"随访号",
	"军人",
	"军兵种",
	"临床路径",
	"病种名",
	"加入单病种",
	"特殊病人",
	"特殊患者",
	"入院日期",
	"采史日期",
	"病史陈述者",
	"可靠程度",
	"供史人关系",
	"供史人电话",
	"供史人地址",
	"签名人姓名",
	"病历信息"
};
const static token_unit tokens[] = {
	{ "姓名", 0 },
	{ "性别", 1 },
	{ "出生日期", 2 },
	{ "出生日其丑", 2 },
	{ "出生日其且", 2 },
	{ "年龄", 3 },
	{ "血型", 4 },
	{ "身高", 5 },
	{ "体重", 6 },
	{ "国籍", 7 },
	{ "国藉", 7 },
	{ "籍贯", 8 },
	{ "藉贯", 8 },
	{ "出生地", 9 },
	{ "民族", 10 },
	{ "文化程度", 11 },
	{ "宗教信仰", 12 },
	{ "婚姻状况", 13 },
	{ "证件类型", 14 },
	{ "证件号码", 15 },
	{ "证f牛号码", 15 },
	{ "家庭地址", 16 },
	{ "户口地址", 17 },
	{ "入科时间", 18 },
	{ "入科日寸间", 18 },
	{ "入院时间", 19 },
	{ "入F完日寸间", 19 },
	{ "病案号", 20 },
	{ "主治医生", 21 },
	{ "床号", 22 },
	{ "护理等级", 23 },
	{ "诊疗组", 24 },
	{ "当前科室", 25 },
	{ "当前病区", 26 },
	{ "病情", 27 },
	{ "病惰", 27 },
	{ "入院科室", 28 },
	{ "入院病区", 29 },
	{ "门诊号", 30 },
	{ "住院次数", 31 },
	{ "住址邮编", 32 },
	{ "手机", 33 },
	{ "固定电话", 34 },
	{ "E=ai1", 35 },
	{ "Emai1", 35 },
	{ "联系人姓名", 36 },
	{ "联系人手机", 37 },
	{ "关系", 38 },
	{ "职业", 39 },
	{ "单位名称", 40 },
	{ "单位电话", 41 },
	{ "单位住址", 42 },
	{ "单位邮编", 43 },
	{ "计费类型", 44 },
	{ "医保号", 45 },
	{ "随访号", 46 },
	{ "军人", 47 },
	{ "军兵种", 48 }
};
const int tokens_length = sizeof(token_names) / sizeof(token_names[0]);

string Join_Tokens(string str[], int length)
{
	string result = str[0];
	for (int i = 1; i < length; i++)
		result += "," + str[i];
	result += "\n";
	return result;
}

bool insert_token(string str, string output[], int length)
{
	for (int i = 0; i < length; i++)
	{
		if (startWith(str, tokens[i].name) && output[tokens[i].index] == "")
		{
			int index = str.find('=', tokens[i].name.length() - 1);
			if (index == str.length() - 1)
				return 1;
			string result = str.substr(index + 1);
			output[tokens[i].index] = result;
			return 1;
		}
	}
	return 0;
}
//--------------------------------------------

//用来判断版面类型的静态常量-------------------
static Mat template1 = imread("layout_template_1.png", 0);
static Mat template1_another = imread("layout_template_1_another.png", 0);
static Mat template2 = imread("layout_template_2.png", 0);
static double layout_threshold1 = 7.5e+010;
static double layout_threshold1_another = 6.5e+010;
static double layout_threshold2 = 8.0e+010;
//------------------------------------------

//去除回车，空格，tab等符号，并将英文的逗号换成中文逗号
string Trim(string str)
{
#define InvalidChar(c) ( ( (c) == '\n' || (c) == '\t' || (c) == ' ' ) ? 1:0 )
	string result = "";
	int len = str.length();
	for (int i = 0; i < len; i++)
	{
		if (!InvalidChar(str[i]))
		{
			if (str[i] == ',')
				result += "，";
			else
				result += str[i];
		}
	}
	return result;
}
//------------------------------------------

//比较函数，用来判断某个字符串s1是不是以字符串s2开头
int startWith(string s1, string s2)
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
//-------------------------------------------------

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

char* resultTransfer(char* str)
{
	return UnicodeToAnsi(Utf_8ToUnicode(str));
}

Mat ImageResize(Mat origin, int layoutType)
{
	/*
	//如果图片较大，就不再放缩
	if (origin.cols*origin.rows > 100000)
	return origin;
	*/

	Mat binary, resized;
	if (layoutType)
	{
		//按比例放缩，横向和纵向放缩的比例为3:4(对于页面3）
		Size dsize = Size(origin.cols * 4, origin.rows * 4);
		resized = Mat(dsize, CV_32S);
		resize(origin, resized, dsize);
	}
	else
	{
		Size dsize = Size(origin.cols * 3, origin.rows * 4);
		resized = Mat(dsize, CV_32S);
		resize(origin, resized, dsize);
	}

	threshold(resized, binary, 180, 255, THRESH_BINARY);
	return binary;
}

int getLayoutType(Mat src)
{
	Mat eroded, image_reverse;
	Mat binary = ImageResize(src, 1);

	//进行第一步腐蚀，腐蚀强度较高，以便找到最大连通区域
	Mat element = getStructuringElement(MORPH_RECT, Size(40, 7));
	erode(binary, eroded, element);

	bitwise_not(eroded, image_reverse);  //取反

	//逐一比较各个模板的相似度
	if (image_reverse.dot(template1)> layout_threshold1)
		return 1;
	if (image_reverse.dot(template1_another) > layout_threshold1_another)
		return 1;
	if (image_reverse.dot(template2) > layout_threshold2)
		return 2;
	return 0;
}
/**********************************************************************
*  main()
*
**********************************************************************/


int main(int argc, char **argv)
{
	//文件操作相关
	FILE* file = fopen("test.csv", "r+");
	if (file == NULL)
	{
		file = fopen("test.csv", "w+");
		fputs(Join_Tokens(token_names, tokens_length).c_str(), file);
	}
	fseek(file, 0, SEEK_END);

	string output[tokens_length];

	Mat src = imread("bingli.jpg", 0);
	Mat dst;
	Mat eroded, binary_coded;

	//提取主体部分
	Mat roi = src(Rect(0.2*src.cols, 0.17*src.rows, 0.73*src.cols, 0.78*src.rows));
	int layoutType = getLayoutType(roi);
	cout << "layoutType: " << layoutType << endl;

	//imshow("src", src);

	threshold(roi, dst, 0, 255, THRESH_BINARY);
	Mat element = getStructuringElement(MORPH_RECT, Size(35, 7));
	//imshow("binary", dst);
	erode(dst, eroded, element);
	//imshow("erode", eroded);

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

	Mat binary;
	threshold(roi, binary, 200, 255, THRESH_BINARY);
	//imshow("binary_roi", binary);

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

	for (int i = 1; i < boundRect.size(); i++)
	{
		Mat example = roi(boundRect[i]);
		Mat resized = ImageResize(example, layoutType);
		//char s[6];
		//sprintf(s, "%d", i);
		//imshow(s, resized);
		//Mat resized = example;

		if (resized.rows < 50 || resized.cols < 20)
			continue;
		api->SetImage((uchar*)resized.data, resized.cols, resized.rows, 1, resized.step1());
		char* outText = api->GetUTF8Text();
		char* resulttemp = resultTransfer(outText);
		string result = Trim(resulttemp);
		//printf("region num%d is: %s\n", i, &result);
		cout << "region" << i << " is: " << result << endl;
		insert_token(result, output, sizeof(tokens) / sizeof(tokens[0]));
		fprintf(fp, "region num%d is: %s\n", i, outText);
		delete[] outText;
	}
	//cout << "the final output is:" << endl << Join_Tokens(output, tokens_length) << endl;
	fputs(Join_Tokens(output, tokens_length).c_str(), file);

	//fwrite(outText, sizeof(char), strlen(outText), fp);

	// free resources
	fclose(fp);
	fclose(file);
	api->Clear();
	api->End();
	//waitKey();
	system("pause");
	return 0;
}
