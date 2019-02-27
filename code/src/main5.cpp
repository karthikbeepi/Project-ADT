#include <opencv2/opencv.hpp>
#include<iostream>
#include<fstream>

using namespace cv;
using namespace std;


int main( int argc, char** argv )
{
    if(argc!=4){
        cout<<"Usage: ../seg input_image initialization_file output_mask"<<endl;
        return -1;
    }
    
    // Load the input image
    Mat in_image;
    in_image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
   
    if(!in_image.data)
    {
        cout<<"Could not load input image!!!"<<endl;
        return -1;
    }

    if(in_image.channels()!=3){
        cout<<"Image does not have 3 channels!!! "<<in_image.depth()<<endl;
        return -1;
    }
    
    // the output image
    Mat out_image = in_image.clone();
   // Mat out_image;
    ifstream f(argv[2]);
    if(!f){
        cout<<"Could not load initial mask file!!!"<<endl;
        return -1;
    }
    
    int width = in_image.cols;
    int height = in_image.rows;    
    Mat src;
    Mat grad;
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;
    src = imread( argv[1] );
    GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
    //cvtColor( src, src, CV_BGR2GRAY );
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;
    Scharr( src, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
    //Sobel( src, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );
    Scharr( src, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
    //Sobel( src, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
    
    out_image = grad.clone();
    int p[height][width];
    for(int i=0; i<width; i++)
        for(int j=0; j<height; j++)
        {
            Vec3b pixel= out_image.at<Vec3b>(j, i);
            if(pixel[0]<100&&pixel[1]<100&&pixel[2]<100)
            {
                p[j][i] = 9999; 
            }      
            else
            {
                p[j][i]=0;
            }
        }
     int edgeVal[height][width][4];
    for(int i=0; i<width; i++)
        for(int j=0; j<height; j++)
        {
            //i, j, i, j-1 U 0
            if(j-1>=0)
            {
                edgeVal[j][i][0] = p[j-1][i];
            }

            //i, j, i, j+1 D 1
            if(j+1<=height)
            {
                edgeVal[j][i][1] = p[j+1][i];
            }

            //i, j, i-1, j L 2
            if(i-1>=0)
            {
                edgeVal[j][i][2] = p[j][i-1];
            }

            //i, j, i+1, j R 3
            if(i+1<=width)
            {
                edgeVal[j][i][3] = p[j][i+1];
            }
        }
    // ofstream fout;
    // fout.open("test.txt"); 
    // for(int i=0; i<width; i++)
    // {
    //     for(int j=0; j<height; j++)
    //     {
    //             fout<<edgeVal[j][i][0];
    //     }
    //     fout<<"\n";
    // }

    // write it on disk
    imwrite( argv[3], out_image);
    
    // also display them both
    namedWindow( "Original image", WINDOW_AUTOSIZE );
    namedWindow( "Show Marked Pixels", WINDOW_AUTOSIZE );
    imshow( "Original image", in_image );
    imshow( "Show Marked Pixels", out_image );
    waitKey(0);

    return 0;
}
