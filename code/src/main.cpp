#include <opencv2/opencv.hpp>
#include<iostream>
#include<fstream>

using namespace cv;
using namespace std;

int edgeVal[2500][2000][4];
int parent[2500][2000];
bool vis[2500][2000];
int src2[100][2], snk2[100][2];
int srcCount=0, snkCount=0;

bool hasPath( int width, int height )
{
    for(int i=0; i<width; i++)
        for(int j=0; j<height; j++)
        {
            vis[j][i]=false;
        }

    queue <pair<int, int> > q;
    pair<int, int> p1; /*= make_pair(src2[0][0], src2[0][1]);*/
    //q.push(p1);
    int a=0;
    while(a<srcCount)
    {
        p1 = make_pair(src2[a][0], src2[a][1]);
        q.push(p1);
        parent[src2[a][0]][src2[a][1]] = -1;
        a++;
        //break;
    }
    //parent[src2[0][0]][src2[0][1]] = -1;
    while(!q.empty())
    {
        p1 = q.front();
        q.pop();
        int y = p1.first;
        int x = p1.second;
        if(vis[y][x]==true||x<0 || x>=width || y<0 || y>=height)
            continue;

        vis[y][x] = true;
        if(edgeVal[y][x][0]>0&&y-1>=0&&vis[y-1][x]==false)
        {
            pair<int, int> p2 = make_pair(y-1, x);
            q.push(p2);
            parent[y-1][x] = 1;
        }

        if(edgeVal[y][x][1]>0&&y+1<height&&vis[y+1][x]==false)
        {
            pair<int, int> p2 = make_pair(y+1, x);
            q.push(p2);
            parent[y+1][x] = 0;
        }

        if(edgeVal[y][x][2]>0&&x-1>=0&&vis[y][x-1]==false)
        {
            pair<int, int> p2 = make_pair(y, x-1);
            q.push(p2);
            parent[y][x-1] = 3;
        }

        if(edgeVal[y][x][3]>0&&x+1<width&&vis[y][x+1]==false)
        {
            pair<int, int> p2 = make_pair(y, x+1);
            q.push(p2);
            parent[y][x+1] = 2;
        }

        for(int i=0; i<snkCount; i++)
        {
            if(vis[snk2[i][0]][snk2[i][1]]==true)
                 return true;
        }
    }

    // if(vis[snk2[0][0]][snk2[0][1]]==true)
    //     return true;
    // else
    //     {
    //         return false;
    //     }
    for(int i=0; i<snkCount; i++)
    {
        if(vis[snk2[i][0]][snk2[i][1]]==true)
             return true;
    }
    return false;
}

void findCut(int x[], int y[], int t[], int n, int width, int height)
{
    int src[2], snk[2];
    for(int i=0; i<n; i++)
    {
        if(t[i]==1)
        {
            src2[srcCount][0] = y[i];
            src2[srcCount][1] = x[i];
            srcCount++;
        }
        else
        {
            snk2[snkCount][0] = y[i];
            snk2[snkCount][1] = x[i];
            snkCount++;
        }   
    }

    // src[0] = y[0];
    // src[1] = x[0];
    // snk[0] = y[1];
    // snk[1] = x[1];
    while(hasPath(width, height))
    {
        int x1, y1;
        for(int i=0; i<snkCount; i++)
        {
            if(vis[snk2[i][0]][snk2[i][1]]==true)
            {
                x1 = snk2[i][1];
                y1 = snk2[i][0];
                break;
            }
        }
        //x1 = snk[1];
        //y1 = snk[0];
        int flow=999;
        //while(x1!=src[1]||y1!=src[0])
        while(parent[y1][x1]!=-1)
        {
            
            int par = parent[y1][x1];
            if(par==0)
            {
                if(flow>edgeVal[y1][x1][par])
                    flow = edgeVal[y1][x1][par];
                y1--;
            }
            else if(par==1)
            {
                if(flow>edgeVal[y1][x1][par])
                    flow = edgeVal[y1][x1][par];
                y1++;
            }
            else if(par==2)
            {
                if(flow>edgeVal[y1][x1][par])
                    flow = edgeVal[y1][x1][par];
                x1--;
            }
            else
            {
                if(flow>edgeVal[y1][x1][par])
                    flow = edgeVal[y1][x1][par];
                x1++;
            }   
        }

        for(int i=0; i<snkCount; i++)
        {
            if(vis[snk2[i][0]][snk2[i][1]]==true)
            {
                x1 = snk2[i][1];
                y1 = snk2[i][0];
                break;
            }
        }

        //while(x1!=src[1]||y1!=src[0])
        while(parent[y1][x1]!=-1)
        {
            
            int par = parent[y1][x1];
            if(par==0)
            {
                edgeVal[y1][x1][par]+=flow;
                y1--;
            //    cout<<y1<<x1<<" ";
                edgeVal[y1][x1][1]-=flow;
            }
            else if(par==1)
            {
                edgeVal[y1][x1][par]+=flow;
                y1++;
            //    cout<<y1<<x1<<" ";
                edgeVal[y1][x1][0]-=flow;
            }
            else if(par==2)
            {
                edgeVal[y1][x1][par]+=flow;
                x1--;
            //    cout<<y1<<x1<<" ";
                edgeVal[y1][x1][3]-=flow;
            }
            else
            {
                edgeVal[y1][x1][par]+=flow;
                x1++;
            //    cout<<y1<<x1<<" ";
                edgeVal[y1][x1][2]-=flow;
            }   
        }        
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
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;
    //GaussianBlur( src, src, Size(5,5), 0, 0, BORDER_DEFAULT );
    Mat grad_x, grad_y, grad;
    Mat abs_grad_x, abs_grad_y;
    Scharr( src, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
    //Sobel( src, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );
    Scharr( src, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
    //Sobel( src, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
    out_image= grad.clone();
    int maxVal = 9999;
    out_image = grad.clone();
    int p[height+1][width+1];
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
                p[j][i]=1;
            }
        }
    
    for(int i=0; i<width; i++)
    {
        for(int j=0; j<height; j++)
        {
            //i, j, i, j-1 U 0
            if(j-1>=0)
            {
                edgeVal[j][i][0] = (p[j-1][i]-p[j][i]);
                if(edgeVal[j][i][0]<0)
                    edgeVal[j][i][0]=-edgeVal[j][i][0];
                edgeVal[j][i][0] = maxVal - edgeVal[j][i][0];
            }
            else
            {
                edgeVal[j][i][0] = 0;
            }
            

            //i, j, i, j+1 D 1
            if(j+1<height)
            {
                edgeVal[j][i][1] =  (p[j+1][i]-p[j][i]);
                if(edgeVal[j][i][1]<0)
                    edgeVal[j][i][1] = - edgeVal[j][i][1];
                edgeVal[j][i][1] = maxVal - edgeVal[j][i][1];
            }

            else
            {
                edgeVal[j][i][1] = 0;
            }

            //i, j, i-1, j L 2
            if(i-1>=0)
            {
                edgeVal[j][i][2] = (p[j][i-1]-p[j][i]);
                if(edgeVal[j][i][2]<0)
                    edgeVal[j][i][2] = -edgeVal[j][i][2];
                edgeVal[j][i][2] = maxVal - edgeVal[j][i][2];
            }

            else
            {
                edgeVal[j][i][2] = 0;
            }

            //i, j, i+1, j R 3
            if(i+1<width)
            {
                edgeVal[j][i][3] = (p[j][i+1]-p[j][i]);
                if(edgeVal[j][i][3]<0)
                    edgeVal[j][i][3] = -edgeVal[j][i][3];
                edgeVal[j][i][3] = maxVal - edgeVal[j][i][3];
            }

            else
            {
                edgeVal[j][i][3] = 0;
            }
        }
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

    bool vis[height][width];
    for(int i=0; i<width; i++)
        for(int j=0; j<height; j++)
        {
            vis[j][i] = false;
        }
    queue <pair<int, int> > q;
    int i=0;
    while(t1[i]!=1)
        i++;
    pair<int, int> p1 = make_pair(y1[i], x1[i]);
    q.push(p1);
    int x, y;
    Mat out_image2 = Mat::zeros(out_image.rows, out_image.cols, CV_8UC3);
    while(!q.empty())
    {
            p1 = q.front();
            q.pop();
            y = p1.first;
            x = p1.second;
            if(vis[y][x]==true||x<0 || x>=width || y<0 || y>=height)
                continue;
            Vec3b pixel;
            pixel[1] = 255;
            pixel[2] = 255;
            pixel[0] = 255; 
            out_image2.at<Vec3b>(y, x) = pixel;
            vis[y][x] = true;
            if(edgeVal[y][x][1]>0)
            {
                p1.first = y+1;
                q.push(p1);
            }

            if(edgeVal[y][x][0]>0)
            {
                p1.first = y-1;
                q.push(p1);
            }

            if(edgeVal[y][x][3]>0)
            {
                p1.first = y;
                p1.second = x+1;
                q.push(p1);
            }

            if(edgeVal[y][x][2]>0)
            {
                p1.second = x-1;
                q.push(p1);
            }
    }

    // while(t1[i]==1)
    // {
    //         x = x1[i];
    //         y = y1[i];
    //         queue <pair<int, int> > q;
    //         pair<int, int> p1 = make_pair(y, x);
    //         q.push(p1);            
    //         while(!q.empty())
    //         {
    //             p1 = q.front();
    //             q.pop();
    //             y = p1.first;
    //             x = p1.second;
    //             if(p[y][x]==0||x<0 || x>=width || y<0 || y>=height)
    //             {
    //                 continue;
    //             }
    //             Vec3b pixel;
    //             pixel[1] = 255;
    //             pixel[2] = 255;
    //             pixel[0] = 255; 
    //             out_image2.at<Vec3b>(y, x) = pixel;
    //             p[y][x] = 0;
    //             p1.first = y+1;
    //             q.push(p1);
    //             p1.first = y-1;
    //             q.push(p1);
    //             p1.first = y;
    //             p1.second = x+1;
    //             q.push(p1);
    //             p1.second = x-1;
    //             q.push(p1);
                
    //         }
    // i++;
    //     }
    
    // write it on disk
    imwrite( argv[3], out_image2);
    
    // also display them both
    namedWindow( "Original image", WINDOW_AUTOSIZE );
    namedWindow( "Show Marked Pixels", WINDOW_AUTOSIZE );
    imshow( "Original image", in_image );
    imshow( "Show Marked Pixels", out_image2 );
    waitKey(0);


    return 0;
}
