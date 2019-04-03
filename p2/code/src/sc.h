#ifndef SEAMCARVINGCOMP665156
#define SEAMCARVINGCOMP665156

#include <opencv2/opencv.hpp>

// the function you need to implement - by defaiult it calls seam_carving_perfected
bool seam_carving(cv::Mat& in_image, int new_width, int new_height, cv::Mat& out_image, cv::Mat& unch);


bool seam_carving_perfected(cv::Mat& in_image, int new_width, int new_height, cv::Mat& out_image, cv::Mat& unc);

bool reduce_horizontal_seam_perfected(cv::Mat& in_image, cv::Mat& out_image, cv::Mat& unc);


bool reduce_vertical_seam_perfected(cv::Mat& in_image, cv::Mat& out_image, cv::Mat& unc);



#endif
