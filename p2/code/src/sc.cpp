//https://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/sobel_derivatives/sobel_derivatives.html was used for learning Scharr
#include "sc.h"

using namespace cv;
using namespace std;

int min3 (int a, int b, int c)
{
	return (a<b)?((a<c)?a:c):((b<c)?b:c);
}

bool seam_carving(Mat& in_image, int new_width, int new_height,
		Mat& out_image, Mat & unch) {

	// some sanity checks
	// Check 1 -> new_width <= in_image.width
	if (new_width > in_image.cols) {
		cout
				<< "Invalid request!!! new_width has to be smaller than the current size!"
				<< endl;
		return false;
	}
	if (new_height > in_image.rows) {
		cout
				<< "Invalid request!!! ne_height has to be smaller than the current size!"
				<< endl;
		return false;
	}

	if (new_width <= 0) {
		cout << "Invalid request!!! new_width has to be positive!" << endl;
		return false;

	}

	if (new_height <= 0) {
		cout << "Invalid request!!! new_height has to be positive!" << endl;
		return false;

	}

	return seam_carving_perfected(in_image, new_width, new_height, out_image, unch);
}

// seam carves by removing trivial seams
bool seam_carving_perfected(Mat& in_image, int new_width, int new_height,
		Mat& out_image, Mat& unchangedImage) {

	Mat iimage = in_image.clone();
	Mat oimage = in_image.clone();
	while (iimage.rows != new_height || iimage.cols != new_width) {
		// horizontal seam if needed
		if (iimage.rows > new_height) {
			reduce_horizontal_seam_perfected(iimage, oimage, unchangedImage);
			iimage = oimage.clone();
		}

		if (iimage.cols > new_width) {
			reduce_vertical_seam_perfected(iimage, oimage, unchangedImage);
			iimage = oimage.clone();
		}
	}

	out_image = oimage.clone();
	// namedWindow( "Original image with seams", WINDOW_AUTOSIZE );
    // imshow( "Original image", iimage );
	return true;
}

// horizontl trivial seam is a seam through the center of the image
bool reduce_horizontal_seam_perfected(Mat& in_image, Mat& out_image, Mat& unch) {

	// retrieve the dimensions of the new image
	int height = in_image.rows - 1;
	int width = in_image.cols;

	// create an image slighly smaller
	out_image = Mat(height, width, CV_8UC3);

	Mat grad;
	int seamEnergy[height+1][width];
	Mat src_gray;
	Mat blur_src;
	GaussianBlur(in_image, blur_src, Size(3, 3), 0, 0, BORDER_DEFAULT);
	cvtColor(blur_src, src_gray, COLOR_BGR2GRAY);
	Mat xGradient, yGradient;
	Mat xGradientAbs, yGradientAbs;
	Scharr(src_gray, xGradient, CV_16S, 1, 0, 1, 0, BORDER_DEFAULT);
	Scharr(src_gray, yGradient, CV_16S, 0, 1, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(xGradient, xGradientAbs);
	convertScaleAbs(yGradient, yGradientAbs);
	addWeighted(xGradientAbs, 0.5, yGradientAbs, 0.5, 0, grad);

	//store the gradient values in a 2D array
	uchar diffEnergy[grad.rows][grad.cols];
	for(int x=0; x<grad.cols; x++)
		for(int y=0; y<grad.rows; y++)
			diffEnergy[y][x] = grad.at<uchar>(y,x);

	//find the seam energy for first column
	for(int y=0;y<grad.rows;y++)
			seamEnergy[y][0] = diffEnergy[y][0];
		
	
	//find the total seam energy
	for(int x=1;x<grad.cols;x++)
	{
		for(int y=0;y<grad.rows;y++)
		{
			if(y==0)
				seamEnergy[y][x] = diffEnergy[y][x] + min(seamEnergy[y][x-1], seamEnergy[y+1][x-1]);
			else if(y==grad.rows-1)
				seamEnergy[y][x] = diffEnergy[y][x]+ min(seamEnergy[y][x-1], seamEnergy[y-1][x-1]);
			else
				seamEnergy[y][x] = diffEnergy[y][x] +  min3(seamEnergy[y][x-1], seamEnergy[y-1][x-1], seamEnergy[y+1][x-1]);
		}
	}

	//find the lowest seam energy from the last column
    int lowestSeamEnergy = seamEnergy[0][width - 1];
	int minRow = 0;
	for (int y=1; y<height; y++) 
	{
		if (lowestSeamEnergy>seamEnergy[y][width-1]) 
		{
			lowestSeamEnergy=seamEnergy[y][width-1];
			minRow = y;
		}
	}
	int index = minRow;
	for (int x=width-1; x>=0; x--) 
	{
		int y_index = 0;
		for (int y=0; y<height; y++) 
		{
			out_image.at<Vec3b>(y_index, x) = in_image.at<Vec3b>(y,x);
			if (y!=index) 
					y_index++;
			else
			{
				Vec3b pix ;
				pix[0] = pix[1] = pix[2] =0;
				unch.at<Vec3b>(y,x) = pix;
			}
		}
			
		if (x > 0) 
		{
			if (index == 0) 
				if (seamEnergy[index+1][x-1]< seamEnergy[index][x-1]) 
					index++;
			
			else if (index == height - 1) 
				if (seamEnergy[index-1][x-1] < seamEnergy[index][x-1])
					index--;
			
			else 
			{
				if (seamEnergy[index-1][x-1] < min(seamEnergy[index][x-1],seamEnergy[index+1][x-1])) 
				{
					index--;
					// cout<<"IND";
				}
				else if (seamEnergy[index+1][x-1] < min(seamEnergy[index-1][x-1], seamEnergy[index][x-1])) {
					index++;
			}
		}
	}
	}
	return true;
}

// vertical trivial seam is a seam through the center of the image
bool reduce_vertical_seam_perfected(Mat& in_image, Mat& out_image, Mat & unch) {
	// retrieve the dimensions of the new image
	int height = in_image.rows;
	int width = in_image.cols - 1;

	// create an image slighly smaller
	out_image = Mat(height, width, CV_8UC3);

	Mat grad;
    int seamEnergy[height][width+1];
    Mat src_gray;
	Mat blur_src;
	GaussianBlur(in_image, blur_src, Size(3, 3), 0, 0, BORDER_DEFAULT);
	cvtColor(blur_src, src_gray, COLOR_BGR2GRAY);
	Mat xGradient, yGradient;
	Mat xGradientAbs, yGradientAbs;
	Scharr(src_gray, xGradient, CV_16S, 1, 0, 1, 0, BORDER_DEFAULT);
	Scharr(src_gray, yGradient, CV_16S, 0, 1, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(xGradient, xGradientAbs);
	convertScaleAbs(yGradient, yGradientAbs);
	addWeighted(xGradientAbs, 0.5, yGradientAbs, 0.5, 0, grad);

	uchar diffEnergy[grad.rows][grad.cols];
	for(int x=0; x<grad.cols; x++)
		for(int y=0; y<grad.rows; y++)
		{
			diffEnergy[y][x] = grad.at<uchar>(y,x);
		}

	for(int x=0;x<grad.cols;x++)
		seamEnergy[0][x] = diffEnergy[0][x];

	for(int y = 1;y<grad.rows;y++)
		for(int x=0;x<grad.cols;x++)
		{
			if(x==0)
				seamEnergy[y][x] = diffEnergy[y][x] + min(seamEnergy[y-1][x], seamEnergy[y-1][x+1]);
			else if(x == grad.cols-1)
				seamEnergy[y][x] = diffEnergy[y][x] + min(seamEnergy[y-1][x-1], seamEnergy[y-1][x]);
			else 
				seamEnergy[y][x] = diffEnergy[y][x] + min3( seamEnergy[y-1][x-1], seamEnergy[y-1][x], seamEnergy[y-1][x+1]);
		}
    int lowestSeamEnergy = seamEnergy[height-1][0];
	int minCol = 0;
	for (int x = 1; x < width; x++)
		if (lowestSeamEnergy > seamEnergy[height-1][x]) 
		{
			lowestSeamEnergy = seamEnergy[height-1][x];
			minCol = x;
		}
	int index = minCol;
	for (int y=height-1; y>=0; y--) 
	{
		int x_index = 0;
		for (int x=0; x<width; x++) 
		{
			out_image.at<Vec3b>(y, x_index) = in_image.at<Vec3b>(y,x);
			if (x != index) 
				x_index++;
			// else
			// {
			// 	Vec3b pix ;
			// 	pix[0] = pix[1] = pix[2] =0;
			// 	unch.at<Vec3b>(y,x) = pix;
			// }
			
		}
		
		if (y>0) 
			if (index==0) 
			{
				if (seamEnergy[y-1][index+1]<seamEnergy[y-1][index]) 
					index++;
			} 
			else if (index==width-1) 
			{
				if (seamEnergy[y-1][index-1]<seamEnergy[y-1][index]) 
					index--;
			} 
			else 
			{
				if (seamEnergy[y-1][index-1]<min(seamEnergy[y-1][index], seamEnergy[y-1][index+1])) 
					index--;
				else if (seamEnergy[y-1][index+1]<min(seamEnergy[y-1][index-1], seamEnergy[y-1][index])) 
					index++;
			}
	}
	return true;
}
