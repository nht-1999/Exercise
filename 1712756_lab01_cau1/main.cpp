
#include <vector>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "Converter.h"

using namespace std;
using namespace cv;

 
void ShowImages(Mat src, Mat des) {
	namedWindow("Source Image");
	imshow("Source Image", src);
	namedWindow("Destination Image");
	imshow("Destination Image", des);
	waitKey(0);
}

int main(int argc, char** argv)
{
	Converter converter = Converter();
	//<ProgramName.exe> <Command> <InputPath> <CommandArguments>
	string command = string(argv[1]);
	string path = string(argv[2]);
	Mat source = imread(path, cv::IMREAD_COLOR);
	Mat des;
	if (command.find("RGB2GrayScale") != -1)
	{
		if (Converter::RGB2GrayScale(source, des))
  		ShowImages(source, des);
		else cout << "Conversion failed";
		return 0;
	}
	else if (command.find("GrayScale2RGB") != -1)
	{
		if (Converter::GrayScale2RGB(source, des))
  		ShowImages(source, des);
		else cout << "Conversion failed";
		return 0;
	}
}






