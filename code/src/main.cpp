#include <opencv2/opencv.hpp>
#include<iostream>
#include<fstream>

using namespace cv;
using namespace std;
#define V 6 

class Graph{

	public:
	bool hasPath(int g[V][V], int parent[], int s, int t) 
	{ 
		bool vis[V]; 
		for(int i=0; i<V; i++)
			vis[i]=false;
		queue <int> q; 
		q.push(s); 
		vis[s] = true; 
		parent[s] = -1; 
		while (!q.empty()) 
		{ 
			int u = q.front(); 
			q.pop(); 

			for (int v=0; v<V; v++) 
			{ 
				if (vis[v]==false && g[u][v] > 0) 
				{ 
					q.push(v); 
					parent[v] = u; 
					vis[v] = true; 
				} 
			} 
		} 

		return (vis[t] == true); 
	} 
	int findMinCut(int g[V][V], int src, int snk) 
	{ 
		int u,v;
		int graph[V][V];
		for (int i = 0; i < V; i++) 
			for (int j = 0; j < V; j++) 
				graph[i][j] = g[i][j]; 
		int parent[V]; 
		int flow = 0; 
		while (hasPath(graph, parent, src, snk)) 
		{ 
			int path_flow = 9999;
			for (v=snk; v!=src; v=parent[v]) 
			{ 
				u = parent[v]; 
				path_flow = min(path_flow, graph[u][v]); 
			} 
			for (v=snk; v != src; v=parent[v]) 
			{ 
				u = parent[v]; 
				graph[u][v] -= path_flow; 
				graph[v][u] += path_flow; 
			} 
			flow += path_flow; 
		} 
		return flow; 
	}
} ;


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
    
    //1. Set the sink and source for these points
    //2. Make blur(Optional)
    //3. Make weights for the edges correspondingly
    //4. Find maxflow/mincut
    //5. Output the results
        

    Mat src;
    Mat grad;
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;
    src = imread( argv[1] );
    //cvtColor( src, src_gray, COLOR_RGB2GRAY );
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;
    Scharr( src, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );
    Scharr( src, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
        
    out_image = grad.clone();
    int n;
    f>>n;

    // get the initial pixels
    for(int i=0;i<n;++i){
        
        int x, y, t;
        f>>x>>y>>t;
        
        if(x<0 || x>=width || y<0 || y>=height){
            cout<<"Invalid pixel mask!"<<endl;
            return -1;
        }
        
        
        Vec3b pixel;
        
        if(t==1){
            pixel[1] = 255;
            pixel[2] = 255;
            pixel[0] = 255;
        } else {

            pixel[1] = 0;
            pixel[2] = 0;
            pixel[0] = 0;        }
        
        out_image.at<Vec3b>(y, x) = pixel;
    }

    int p[height][width];
    for(int i=0; i<width; i++)
        for(int j=0; j<height; j++)
        {
            Vec3b pixel= out_image.at<Vec3b>(j, i);
            if(pixel[0]==0||pixel[1]==0||pixel[2]==0)
            {
                pixel[1] = 0;
                pixel[2] = 255;
                pixel[0] = 0; 
                out_image.at<Vec3b>(j, i) = pixel;
                p[j][i] = 9999;
            }            
        }
        ofstream fout;
        fout.open("test.txt"); 
        for(int i=0; i<width; i++)
        {
            for(int j=0; j<height; j++)
            {
                fout<<p[j][i];
            }
            fout<<"\n";
        }
    
    // write it on disk
    imwrite( argv[3], out_image);
    
    // also display them both
    
    namedWindow( "Original image", WINDOW_AUTOSIZE );
    namedWindow( "Show Marked Pixels", WINDOW_AUTOSIZE );
   // namedWindow( "test", WINDOW_AUTOSIZE );
    imshow( "Original image", in_image );
    imshow( "Show Marked Pixels", out_image );
   // imshow( "test", in_image );
    waitKey(0);
    return 0;
}
