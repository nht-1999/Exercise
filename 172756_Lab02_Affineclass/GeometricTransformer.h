#pragma once

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include<iostream>
#include<vector>

const float PI = 3.14159;
using namespace cv;
using namespace std;

Mat Mutiple_Mat(const Mat& a, const Mat& b) 
{
	Mat result = Mat(a.rows, b.cols, a.type());
	float* pR = (float*)result.data;
	float* pA = (float*)a.data;
	float* pB = (float*)b.data;
	for (int i = 0; i < a.rows; i++) 
	{
		for (int j = 0; j < b.cols; j++) 
		{
			pR[i * result.cols + j]=0;
			for (int k = 0; k < a.cols; k++) 
			{
				//result[i][j] += mat_a[i][k] * mat_b[k][j];
				pR[i * result.cols + j] += pA[i * a.cols + k] * pB[k * b.cols + j];
            	
			}
		}
	}
	return result;
}

/*
 Lớp base dùng để nội suy màu của 1 pixel
*/
class PixelInterpolate
{
public:
	/*
	Hàm tính giá trị màu của ảnh kết quả từ nội suy màu trong ảnh gốc
	Tham số
		- (tx,ty): tọa độ thực của ảnh gốc sau khi thực hiện phép biến đổi affine
		- pSrc: con trỏ ảnh gốc
		- srcWidthStep: widthstep của ảnh gốc
		- nChannels: số kênh màu của ảnh gốc
	Trả về
		- Giá trị màu được nội suy
	*/
	virtual uchar Interpolate(
		float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels) = 0;
	PixelInterpolate();
	~PixelInterpolate();
};

/*
Lớp nội suy màu theo phương pháp song tuyến tính
*/
class BilinearInterpolate : public PixelInterpolate
{
public:
	uchar Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels);
	BilinearInterpolate();
	~BilinearInterpolate();
};
/*
Lớp biểu diễn pháp biến đổi affine
*/
class AffineTransform
{
	Mat _matrixTransform;//ma trận 3x3 biểu diễn phép biến đổi affine
public:
	void Translate(float dx, float dy)// xây dựng phép tịnh tiến theo vector (dx,dy)
	{
		swap(dx, dy);//do tính toán bằng tọa độ ma trận nên phải đảo lại
		Mat temp = Mat(3, 3, CV_32FC1);

		// Kiểm tra
		if (temp.empty() )
			return;

		for (int i = 0; i < 3; i++) {
			float* pRow = temp.ptr<float>(i);
			for (int j = 0; j < 3; j++,pRow += temp.channels()) 
			{
				pRow[0]=0;
				if(i==j) pRow[0]=1;
			}
		}
		//  vị trí Mat[0][2] = dx
		float* pRow = temp.ptr<float>(0);
		for (int j = 0; j < 3; j++,pRow += temp.channels()) 
			{
				if(j==2) pRow[0]=dx;
			}
		//  vị trí Mat[1][2] = dy
		float* pRow = temp.ptr<float>(1);
		for (int j = 0; j < 3; j++,pRow += temp.channels()) 
			{
				if(j==2) pRow[0]=dy;
			}
		_matrixTransform = Mutiple_Mat(temp, _matrixTransform);
	}
	void Rotate(float angle)//xây dựng phép xoay 1 góc angle đơn vị là độ
	{
		angle = angle * PI / 180;
		Mat temp = Mat(3, 3, CV_32FC1);
		
		if (temp.empty())
			return;
		for (int i = 0; i < 3; i++) {
			float* pRow = temp.ptr<float>(i);
			for (int j = 0; j < 3; j++,pRow += temp.channels()) 
			{
				 pRow[0]=0;
			}
		}
		for (int i = 0; i < 3; i++) {
			float* pRow = temp.ptr<float>(i);
			for (int j = 0; j < 3; j++,pRow += temp.channels()) 
			{
				if(i==0 && j==0) pRow[0]=cos(angle);// vị trí Mat[0][0] = cos(angle)
				if(i==0 && j==1) pRow[0]=-sin(angle);// vị trí Mat[0][1] = -sin(angle)
				if(i==1 && j==0) pRow[0]=sin(angle);// vị trí Mat[1][0] = sin(angle)
				if(i==1 && j==1) pRow[0]=cos(angle);// vị trí Mat[1][1] = cos(angle)
				if(i==2 && j==2) pRow[0]= 1;// vị trí Mat[2][2] = 1
			}
		}
	
		_matrixTransform = Mutiple_Mat(temp, _matrixTransform);// tích ma trận hiện tại với affine thuận

	}
	void Scale(float sx, float sy) //xây dựng phép co giãn theo hệ số
	{
		swap(sx, sy); // do ở bên dưới tính toán bằng tọa độ ma trận nên sx với sy bị ngược so với thực tế
		Mat temp = Mat(3, 3, CV_32FC1);
		if (temp.empty())return;
		for (int i = 0; i < 3; i++) {
			float* pRow = temp.ptr<float>(i);
			for (int j = 0; j < 3; j++,pRow += temp.channels()) 
			{
				 pRow[0]=0;
			}
		}
		for (int i = 0; i < 3; i++) {
			float* pRow = temp.ptr<float>(i);
			for (int j = 0; j < 3; j++,pRow += temp.channels()) 
			{
				if(i==0 && j==0) pRow[0]=sx;// vị trí Mat[0][0] = sx
				if(i==1 && j==1) pRow[0]=sy;// vị trí Mat[1][1] = sy
				if(i==2 && j==2) pRow[0]= 1;// vị trí Mat[2][2] = 1
			}
		}
		_matrixTransform = Mutiple_Mat(temp, _matrixTransform);

	}
	void TransformPoint(float& x, float& y)//transform 1 điểm (x,y) theo matrix transform đã có
	{
		Mat temp = Mat(3, 1, CV_32FC1);
		if (temp.empty()) return;
		
		// khởi tạo 
		for (int i = 0; i < 3; i++) 
		{
			float* pRow = temp.ptr<float>(i);
			if(i==0 ) pRow[0]=x;// vị trí Mat[0][0] = x
			if(i==1 ) pRow[0]=y;// vị trí Mat[1][0] = y
			if(i==2 ) pRow[0]=1;// vị trí Mat[2][0] = 1
			pRow += temp.channels();
		}
		
		temp = Mutiple_Mat(_matrixTransform, temp);// nhân affine thuận 3x3 với ma trận 3x1 để ra tọa độ x , y mới
		for (int i = 0; i < 3; i++) 
		{
			float* pRow = temp.ptr<float>(i);
			if(i==0 ) x = pRow[0];// tọa độ mới x = Mat[0][0] 
			if(i==1 ) y= pRow[0];// tọa độ mới y= Mat[1][0] 
			pRow += temp.channels();
		}
	}

	AffineTransform() 
	{
		_matrixTransform = Mat(3, 3, CV_32FC1);//khởi tạo ma trận đơn vị 

		for (int i = 0; i < 3; i++) 
		{
			float* pRow = _matrixTransform.ptr<float>(i);
			for (int j = 0; j < 3; j++,pRow += _matrixTransform.channels()) 
			{
				if(i == j) {
					pRow[0]=1;// vị trí Mat[0][0] = s
				}
				else {
					pRow[0]=0;
					}
			}
		}
	}
	//_matrixTransform = Mat(3, 3, CV_32FC1);
	~AffineTransform() {}
};


/*
Lớp thực hiện phép biến đổi hình học trên ảnh
*/

class GeometricTransformer
{
public:
	/*
	Hàm biến đổi ảnh theo 1 phép biến đổi affine đã có
	Tham số
	 - beforeImage: ảnh gốc trước khi transform
	 - afterImage: ảnh sau khi thực hiện phép biến đổi affine
	 - transformer: phép biến đổi affine
	 - interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int Transform(
		const Mat &beforeImage, 
		Mat &afterImage, 
		AffineTransform* transformer, 
		PixelInterpolate* interpolator);

	/*
	Hàm xoay bảo toàn nội dung ảnh theo góc xoay cho trước
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi thực hiện phép xoay
	- angle: góc xoay (đơn vị: độ)
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int RotateKeepImage(
		const Mat &srcImage, Mat &dstImage, float angle, PixelInterpolate* interpolator);

	/*
	Hàm xoay không bảo toàn nội dung ảnh theo góc xoay cho trước
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi thực hiện phép xoay
	- angle: góc xoay (đơn vị: độ)
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int RotateUnkeepImage(
		const Mat &srcImage, Mat &dstImage, float angle, PixelInterpolate* interpolator);

	/*
	Hàm phóng to, thu nhỏ ảnh theo tỉ lệ cho trước
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi thực hiện phép xoay	
	- sx, sy: tỉ lệ phóng to, thu nhỏ ảnh	
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int Scale(
		const Mat &srcImage, 
		Mat &dstImage, 
		float sx, float sy, 
		PixelInterpolate* interpolator);
		
	
	/*
	Hàm thay đổi kích thước ảnh
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi thay đổi kích thước
	- newWidth, newHeight: kích thước mới
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int Resize(
		const Mat &srcImage, 
		Mat &dstImage, 
		int newWidth, int newHeight, 
		PixelInterpolate* interpolator);

	/*
	Hàm lấy đối xứng ảnh
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi lấy đối xứng
	- direction = 1 nếu lấy đối xứng theo trục ngang và direction = 0 nếu lấy đối xứng theo trục đứng
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int Flip(const Mat &srcImage, Mat &dstImage, bool direction, PixelInterpolate* interpolator);

	GeometricTransformer();
	~GeometricTransformer();
};

