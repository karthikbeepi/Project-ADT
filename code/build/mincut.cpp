#include<iostream>
#include<queue>
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
    parent[src[0]][src[1]] = -1;
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
    }

    if(vis[snk[0]][snk[1]]==true)
        return true;
        else
        {
            return false;
        }
        
}

void findCut(int x[], int y[], int n, int width, int height)
{
    int src[2], snk[2];
    src[0] = y[0];
    src[1] = x[0];
    snk[0] = y[1];
    snk[1] = x[1];
    int y1=snk[0], x1=snk[1];
    while(hasPath(src, snk, width, height))
    {
        int x1, y1;
        x1 = snk[1];
        y1 = snk[0];
        int flow=999;
        while(x1!=src[1]||y1!=src[0])
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
        x1 = snk[1], y1 = snk[0];
        while(x1!=src[1]||y1!=src[0])
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

int main(int argc, char * argv[])
{

    int p[5][5] = {     {3, 1, 1, 1, 1}, 
						{1, 1, 1, 1, 1}, 
						{1, 1, 1, 1, 1}, 
						{1, 1, 1, 1, 1}, 
						{1, 1, 1, 1, 3} 
					}; 
    int maxVal = 3;
    int width = 5, height = 5;
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
    int x[2] = {4, 0};
    int y[2] = {4, 0};
    

    findCut(x, y, 5, width, height);

    
    int sr[2] = {4, 4};
    int sn[2] = {0, 1};
    cout<<hasPath(sr, sn, width, height);
    
}