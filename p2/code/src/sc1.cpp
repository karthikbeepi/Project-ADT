
#include "sc.h"

using namespace cv;
using namespace std;

bool seam_carving(Mat& in_image, int new_width, int new_height, Mat& out_image){

    // some sanity checks
    // Check 1 -> new_width <= in_image.cols
    if(new_width>in_image.cols){
        cout<<"Invalid request!!! new_width has to be smaller than the current size!"<<endl;
        return false;
    }
    if(new_height>in_image.rows){
        cout<<"Invalid request!!! ne_height has to be smaller than the current size!"<<endl;
        return false;
    }
    
    if(new_width<=0){
        cout<<"Invalid request!!! new_width has to be positive!"<<endl;
        return false;

    }
    
    if(new_height<=0){
        cout<<"Invalid request!!! new_height has to be positive!"<<endl;
        return false;
        
    }

    
    return seam_carving_trivial(in_image, new_width, new_height, out_image);
}


// seam carves by removing trivial seams
bool seam_carving_trivial(Mat& in_image, int new_width, int new_height, Mat& out_image){

    Mat iimage = in_image.clone();
    Mat oimage = in_image.clone();
    int i=0;
    while(iimage.rows!=new_height || iimage.cols!=new_width){
        // horizontal seam if needed
        if(iimage.rows>new_height){
            reduce_horizontal_seam_trivial(iimage, oimage);
            iimage = oimage.clone();
        }
        if(i>10)
        break;
        i++;
        if(iimage.cols>new_width){
            reduce_vertical_seam_trivial(iimage, oimage);
            iimage = oimage.clone();
        }
    }
    
    out_image = oimage.clone();
    return true;
}

// horizontal trivial seam is a seam through the center of the image
bool reduce_horizontal_seam_trivial(Mat& in_image, Mat& out_image){

    Mat input_image = in_image.clone();
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;
    GaussianBlur( in_image, in_image, Size(5,5), 0, 0, BORDER_DEFAULT );
    Mat grad_x, grad_y, grad;
    Mat abs_grad_x, abs_grad_y;
    Scharr( in_image, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );
    Scharr( in_image, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
    int width = in_image.cols;
    int height = in_image.rows;
    int p[height+1][width+1];
    for(int i=0; i<width; i++)
        for(int j=0; j<height; j++)
        {
            Vec3b pix = grad.at<Vec3b>(j, i);
            p[j][i] = (int)(pix[0]);
            //p[j][i] = grad.at<int>(j,i);
            if(j==0)
                p[j][i] =99999;
        }
    int energyDiff[height][width];
    for(int i=width-1; i>=0; i--)
        for(int j=0; j<height; j++)
        {
            int left = abs(p[j][i-1]-p[j][i]);
            int upleft=99999, downleft=99999;
            if(j>0)
                upleft = abs(p[j-1][i-1]-p[j][i]);
            if(j<height-1)
                downleft = abs(p[j+1][i-1]-p[j][i]);
            energyDiff[j][i] = (left>upleft)?((left>downleft)?left:downleft):((upleft>downleft)?upleft:downleft);
        }
    int minEnergy = 99999, ind=99999;
    for(int j=0;j<height; j++)
    {
        if(energyDiff[j][0]>minEnergy)
        {
            minEnergy= energyDiff[j][0];
            ind = j;
        }
    }

    vector < pair < int, int > > vec ; //for retracing the seam
    int j=ind, i=0;
    while(i<width-1)
    {
        pair < int , int > p1 = make_pair(j,i);
        vec.push_back(p1);
        int right = abs(p[j][i+1]-p[j][i]);
        int upright=99999, downright=99999;
        if(j>0)
            upright = abs(p[j-1][i+1]-p[j][i]);
        if(j<height-1)
            downright = abs(p[j+1][i+1]-p[j][i]);
        int min = (right>upright)?((right>downright)?right:downright):((upright>downright)?upright:downright);
        i++;
        if(min==right)
            continue;
        else if(min==upright)
            j=j-1;
        else if(min==downright)
            j=j+1;
    }


    // retrieve the dimensions of the new image
    int rows = in_image.rows;
    int cols = in_image.cols;
    
    // create an image slighly smaller
    out_image = Mat(rows, cols, CV_8UC3);

    int out_image_x=0;
    int out_image_y=0;
    for(int i=0; i<input_image.cols; i++)
        for(int j=0; j<input_image.rows; j++)
        {
            // if(k>vec.size())
            //     break;
            // pair < int, int > temp = vec.at(k);
            // if(temp.first==j&&temp.second==i)
            // {
            //     Vec3b pix;
            //     pix[0] =0;
            //     pix[1] =0;
            //     pix[2] =0;
            //     out_image.at<Vec3b>(j, i)=pix;
            // }
            // else
            {
                Vec3b pix = input_image.at<Vec3b>(j,i);
                // if(pix[0]==0&&pix[1]==0&&pix[2]==0)
                //     continue;
                // if(out_image_x>=cols)
                //     out_image_x -= cols;
                // if(out_image_y>=rows)
                //     out_image_y -= rows;
                out_image.at<Vec3b>(j, i)=pix;
                // out_image_x++;
                // out_image_y++;
            }
            
        }


     for(int k=0; k<vec.size(); k++)
        {
            pair < int , int > temp = vec.at(k);
            Vec3b pix;
            pix[0] =0;
            pix[1] =0;
            pix[2] =0;
            out_image.at<Vec3b>(temp.first, temp.second) = pix;
        }

   

        

    
    //populate the image
   // int middle = findSeam_horizontal(in_image);
    
    

    return true;
}

//Find a seam here which must be cut
int findSeam_horizontal(Mat& in_image)
{
    
    return 0;
}

// vertical trivial seam is a seam through the center of the image
bool reduce_vertical_seam_trivial(Mat& in_image, Mat& out_image){
    // retrieve the dimensions of the new image
    int rows = in_image.rows;
    int cols = in_image.cols-1;
    
    // create an image slighly smaller
    out_image = Mat(rows, cols, CV_8UC3);
    
    //populate the image
    int middle = findSeam_vertical(in_image);
    
    for(int i=0;i<rows;++i)
        for(int j=0;j<=middle;++j){
            Vec3b pixel = in_image.at<Vec3b>(i, j);
            
            /* at operator is r/w
             pixel[0] --> red
             pixel[1] --> green
             pixel[2] --> blue
             */
            out_image.at<Vec3b>(i,j) = pixel;
        }
    
    for(int i=0;i<rows;++i)
        for(int j=middle+1;j<cols;++j){
            Vec3b pixel = in_image.at<Vec3b>(i, j+1);
            
            /* at operator is r/w
             pixel[0] --> red
             pixel[1] --> green
             pixel[2] --> blue
             */
            
            out_image.at<Vec3b>(i,j) = pixel;
        }
    
    return true;
}

//Find a seam here which must be cut
int findSeam_vertical(Mat& in_image)
{
    return 0;
}
