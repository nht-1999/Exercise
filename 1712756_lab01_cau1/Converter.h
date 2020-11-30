#pragma once

#include <iostream>
#include <vector>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

namespace Converter
{
	enum class Type {
  	RGB2GrayScale,
  	GrayScale2RGB,
	};
	/*
	Hàm chuyển đổi không gian màu của ảnh từ RGB sang GrayScale
	sourceImage: ảnh input
	destinationImage: ảnh output	
	Hàm trả về
	0: nếu chuyển thành công
	1: nếu chuyển thất bại (không đọc được ảnh input,...)
	*/
	int RGB2GrayScale(const Mat& sourceImage, Mat& destinationImage){
		// kiem tra input
		if(!sourceImage.data) {
			std::cout << "Error: the image wasn't correctly loaded." << std::endl;
			return 1;
		}
		float red = 0.299;
		float green = 0.587;
		float blue = 0.114;
		// Source image
		int nrow = sourceImage.rows; 
		int ncol = sourceImage.cols;
		uchar* pSource = (uchar*)sourceImage.data;
		int nChannel_S = sourceImage.step[1];
		int widthStep_S = sourceImage.step[0];
		// destination Image
		uchar* pDes = (uchar*)destinationImage.data;
		int nChannel_D = destinationImage.step[1];
		int widthStep_D = destinationImage.step[0];

		for (int j = 0; j<ncol; j++, pSource += widthStep_S, pDes += widthStep_D) {
			uchar* pRow = pSource;
			uchar* pRowDes = pDes;
			for (int i = 0; i<nrow; i++, pRow+=nChannel_S, pRowDes+=nChannel_D) {
				// xử lý trên mỗi pixel
				pRowDes[0] = pRow[0]*blue + pRow[1]*green +pRow[2]*red ;
			}   
		}
		return 0;	
	}

		/*
		Hàm chuyển đổi không gian màu của ảnh từ GrayScale sang RGB
		sourceImage: ảnh input
		destinationImage: ảnh output
		Hàm trả về
		0: nếu chuyển thành công
		1: nếu chuyển thất bại (không đọc được ảnh input,...)
		*/
	int GrayScale2RGB(const Mat& sourceImage, Mat& destinationImage){
		// kiem tra input
		if(!sourceImage.data) {
			std::cout << "Error: the image wasn't correctly loaded." << std::endl;
			return 1;
		}
		float red = 0.299;
		float green = 0.587;
		float blue = 0.114;
		// Source image
		int nrow = sourceImage.rows; 
		int ncol = sourceImage.cols;
		uchar* pSource = (uchar*)sourceImage.data;
		int nChannel_S = sourceImage.step[1];
		int widthStep_S = sourceImage.step[0];
		// destination Image
		uchar* pDes = (uchar*)destinationImage.data;
		int nChannel_D = destinationImage.step[1];
		int widthStep_D = destinationImage.step[0];

		for (int j = 0; j<ncol; j++, pSource += widthStep_S, pDes += widthStep_D) {
			uchar* pRow = pSource;
			uchar* pRowDes = pDes;
			for (int i = 0; i<nrow; i++, pRow+=nChannel_S, pRowDes+=nChannel_D) {
				// xử lý trên mỗi pixel
				pRowDes[0]= pRow[0]*blue;
				pRowDes[1]= pRow[0]*green;
				pRowDes[2]= pRow[0]*red ;
			}   
		}
		return 0;
	}
	
	//public:
		/*
		Hàm chuyển đổi không gian màu của ảnh
		sourceImage: ảnh input
		destinationImage: ảnh output cùng kích thước, cùng loại với ảnh input
		type: loại chuyển đổi
			0,1: chuyển từ RGB sang GrayScale và ngược lại
			2,3: chuyển từ RGB sang HSV và ngược lại
		Hàm trả về
			true: nếu chuyển thành công
			false: nếu chuyển thất bại (không đọc được ảnh input hay type không chính xác,...)
		*/
	bool Convert(const Mat& sourceImage, Mat& destinationImage, Type type)
	{
		if (sourceImage.empty()) return false;// nếu ảnh gốc lỗi
		destinationImage = Mat(sourceImage.rows, sourceImage.cols, sourceImage.type());// tạo ảnh kết quả
		if (destinationImage.empty())return false;// nếu ảnh kết quả tạo lỗi
		int nChanel = sourceImage.step[1];
		int nCols = sourceImage.cols;
		int nRows = sourceImage.rows;
		uchar* pDataDes = (uchar*)destinationImage.data;
		int widthStep = sourceImage.step[0];
		int widthStepDes = destinationImage.step[0];
		int nChanelDes = destinationImage.step[1];
		for (int i = 0; i < nRows; i++) 
		{
			for (int j = 0; j < nCols; j++) 
			{
				for (int z = 0; z < nChanel; z++)
					switch (type)
					{
					case Type::RGB2GrayScale:
						return RGB2GrayScale(sourceImage, destinationImage);
						break;
					case Type::GrayScale2RGB:
						return GrayScale2RGB(sourceImage, destinationImage);
						break;
					default:
  					return false;
					}
				return true;
			}
		}
	}
}


