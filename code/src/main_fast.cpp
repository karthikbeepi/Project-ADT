//This is an implementation without the use of mincut-maxflow but finds the fastest edges faster and more efficient. 
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
    // the image should be a 3 channel image by default but we will double check that in teh seam_carving
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
    
    Mat out_image2;
    out_image2 = Mat::zeros(out_image.rows, out_image.cols, CV_8UC3);

    int n;
    f>>n;
    int x, y, t;
    // get the initial pixels
    for(int i=0;i<n;++i)
    {
        int x1, y1;
        f>>x1>>y1>>t;
        
        if(x1<0 || x1>=width || y1<0 || y1>=height){
            cout<<"Invalid pixel mask!"<<endl;
            return -1;
        }
        if(t==1)
        {
            x = x1;
            y = y1;
            queue <pair<int, int> > q;
            pair<int, int> p1 = make_pair(y, x);
            q.push(p1);

            
            while(!q.empty())
            {
                p1 = q.front();
                q.pop();
                y = p1.first;
                x = p1.second;
                if(p[y][x]==0||x<0 || x>=width || y<0 || y>=height)
                {
                    continue;
                }
                Vec3b pixel;
                pixel[1] = 255;
                pixel[2] = 255;
                pixel[0] = 255; 
                out_image2.at<Vec3b>(y, x) = pixel;
                p[y][x] = 0;
                p1.first = y+1;
                q.push(p1);
                p1.first = y-1;
                q.push(p1);
                p1.first = y;
                p1.second = x+1;
                q.push(p1);
                p1.second = x-1;
                q.push(p1);
                
            }
    
        }
    }

    
    // write it on disk
    imwrite( argv[3], out_image);
    
    // also display them both
    namedWindow( "Original image", WINDOW_AUTOSIZE );
    namedWindow( "Show Marked Pixels", WINDOW_AUTOSIZE );
    namedWindow( "test", WINDOW_AUTOSIZE );
    imshow( "Original image", in_image );
    imshow( "Show Marked Pixels", out_image );
    imshow( "test", out_image2 );
    waitKey(0);

    return 0;
}
