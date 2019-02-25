#include <iostream> 
#include <queue> 

using namespace std; 

#define V 6 

class Graph{

	public:
	int g[V][V];
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

int main() 
{ 
	// Let us create a graph shown in the above example 
	int graph[V][V] = { {0, 16, 13, 0, 0, 0}, 
						{0, 0, 10, 12, 0, 0}, 
						{0, 4, 0, 0, 14, 0}, 
						{0, 0, 9, 0, 0, 20}, 
						{0, 0, 0, 7, 0, 4}, 
						{0, 0, 0, 0, 0, 0} 
					}; 

	Graph g;
	cout << "The maximum possible flow is " << g.findMinCut(graph, 0, 5); 

	return 0; 
} 
