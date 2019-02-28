#include <opencv2/opencv.hpp>
#include<iostream>
#include<fstream>

using namespace cv;
using namespace std;

int edgeVal[700][500][4];
int parent[700][500];
bool vis[700][500];

bool hasPath(int src[2], int snk[2], int width, int height )
{
    for(int i=0; i<width; i++)
        for(int j=0; j<height; j++)
        {
            vis[j][i]=false;
        }
    queue <pair<int, int> > q;
    pair<int, int> p1 = make_pair(src[0], src[1]);
    q.push(p1);
    int x, y;
   // vis[src[0]][src[1]] = true;
    parent[src[0]][src[1]] = -1;
    
    while(!q.empty()){
        
        p1 = q.front();
        q.pop();
        y = p1.first;
        x = p1.second;
        if(vis[y][x]==true||x<0 || x>=width || y<0 || y>=height)
            continue;
        vis[y][x] = true;
        if(y+1<height&&vis[y+1][x]==false)
        {
            p1.first = y+1;
            q.push(p1);
            parent[y+1][x] = 0;
        }
        
        if(y-1>=0&&vis[y-1][x]==false)
        {
            p1.first = y-1;
            q.push(p1);
            parent[y-1][x] = 1;
        }
        
        if(x+1<width&&vis[y][x+1]==false)
        {
            p1.first = y;
            p1.second = x+1;
            q.push(p1);
            parent[y][x+1] = 2;
        }
        
        if(x-1>=0&&vis[y][x-1]==false)
        {
            p1.second = x-1;
            q.push(p1);
            parent[y][x-1] = 3;
        }
    }

    if( vis[snk[0]][snk[1]]==true)
        return true;
    else
    {
        return false;
    }
        
}

void findCut(int x[], int y[], int t[], int n, int width, int height)
{
    // int src[10][2];
    // int snk[10][2];
    // int srcCount =0;
    // int snkCount =0;
    // for(int i=0; i<n; i++)
    // {
    //     if(t[i]==1)
    //     {
    //         src[srcCount++][0] = x[i];
    //         src[srcCount++][1] = y[i];
    //     } 
    //     else
    //     {
    //         snk[snkCount++][0] = x[i];
    //         snk[snkCount++][1] = y[i];
    //     }
    // }
    int src[2], snk[2];
    src[0] = y[0];
    src[1] = x[0];
    snk[0] = y[1];
    snk[1] = x[1];
    while(hasPath(src, snk, width, height))
    {
        int flow = 99999;
        int y1, x1, temp;
        y1 = snk[0];
        x1 = snk[1];
        while(y1!=src[0]&&x1!=src[1]){
            temp = parent[y1][x1];
            flow = std::min(flow, edgeVal[y1][x1][temp]);
            
            switch (temp)
            {

                case 0: y1--;
                    break;
                case 1: y1++;
                    break;
                case 2: x1--;
                    break;
                case 3: x1++;
                    break;
            }
        }
        while(y1!=src[0]&&x1!=src[1]){
            int temp = parent[y1][x1];
            int y2, x2;
            switch (temp)
            {
                case 0: y2= y1+1; x2= x1;
                    break;
                case 1: y2=y1-1; x2=x1;
                    break;
                case 2: x2=x1+1; y2=y1;
                    break;
                case 3: x2=x1-1; y2=y1;
                    break;
            }
            edgeVal[y1][x1][temp] -= flow;
            switch (temp)
            {
                case 0: edgeVal[y2][x2][1] -=flow;
                    break;
                case 1: edgeVal[y2][x2][0] -=flow;
                    break;
                case 2: edgeVal[y2][x2][3] -=flow;
                    break;
                case 3: edgeVal[y2][x2][2] -=flow;
                    break;
            }
        }
      //  cout<<flow;
      break;
    }

}

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
    src = imread( argv[1] );
    
    out_image = src.clone();
    int maxVal = -1;
    int p[height][width];
    for(int i=0; i<width; i++)
        for(int j=0; j<height; j++)
        {
            Vec3b pixel1 = out_image.at<Vec3b>(j, i);
            p[j][i] = (int) (pixel1[0] +  pixel1[1] + pixel1[2])/3;
            if(maxVal<p[j][i])
                maxVal = p[j][i];
        }
    
    for(int i=0; i<width; i++)
        for(int j=0; j<height; j++)
        {
            //i, j, i, j-1 U 0
            if(j-1>=0)
            {
                edgeVal[j][i][0] = maxVal - (p[j-1][i]-p[j][i]);
                // edgeVal[j][i][0] *= edgeVal[j][i][0];
                // edgeVal[j][i][0] *= edgeVal[j][i][0];
            }
            else
            {
                edgeVal[j][i][0] = 0;
            }
            

            //i, j, i, j+1 D 1
            if(j+1<=height)
            {
                edgeVal[j][i][1] = maxVal - ( p[j+1][i]-p[j][i]);
                // edgeVal[j][i][1] *= edgeVal[j][i][0];
                // edgeVal[j][i][1] *= edgeVal[j][i][0];
            }

            else
            {
                edgeVal[j][i][1] = 0;
            }

            //i, j, i-1, j L 2
            if(i-1>=0)
            {
                edgeVal[j][i][2] = maxVal - (p[j][i-1]-p[j][i]);
                // edgeVal[j][i][2] *= edgeVal[j][i][0];
                // edgeVal[j][i][2] *= edgeVal[j][i][0];
            }

            else
            {
                edgeVal[j][i][2] = 0;
            }

            //i, j, i+1, j R 3
            if(i+1<=width)
            {
                edgeVal[j][i][3] = maxVal - (p[j][i+1]-p[j][i]);
                // edgeVal[j][i][3] *= edgeVal[j][i][0];
                // edgeVal[j][i][3] *= edgeVal[j][i][0];
            }

            else
            {
                edgeVal[j][i][3] = 0;
            }
        }
    ofstream fout;
    fout.open("test.txt"); 
    for(int i=0; i<width; i++)
    {
        for(int j=0; j<height; j++)
        {
                fout<<edgeVal[j][i][0];
        }
        fout<<"\n";
    }

    int n;
    f>>n;
    int x1[n], y1[n], t1[n];
    for(int i=0;i<n;++i)
    {
        f>>x1[i]>>y1[i]>>t1[i];
        if(x1[i]<=0 || x1[i]>=width || y1[i]<=0 || y1[i]>=height){
            cout<<"Invalid pixel mask!"<<endl;
            return -1;
        }
    }

    findCut(x1, y1, t1, n, width, height);

    queue <pair<int, int> > q;
    pair<int, int> p1 = make_pair(y1[1], x1[1]);
    q.push(p1);
    int x, y;
    Mat out_image2 = Mat::zeros(out_image.rows, out_image.cols, CV_8UC3);
    while(!q.empty())
    {
            p1 = q.front();
            q.pop();
            y = p1.first;
            x = p1.second;
            if(p[y][x]==0||x<0 || x>=width || y<0 || y>=height)
                continue;
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
