/*
Network Routing Simulator
Author: N Sumant

Features:
- Dijkstra Algorithm
- Bellman-Ford Algorithm
- Routing Table Generation
- Dynamic Link Addition/Removal
- Performance Comparison
*/
#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <climits>
#include <chrono>
using namespace std;
struct Edge
{
    int u,v,w;
};
void createGraph(vector<vector<pair<int,int>>>& graph,vector<Edge>& edges, int m)
{
    cout<<"Enter the edges (u v w): "<<endl;
    for(int i=0;i<m;i++)
    { 
        int u,v,w;
        cin>>u>>v>>w;
        graph[u].push_back({v,w});
        graph[v].push_back({u,w}); // Assuming undirected graph 
        edges.push_back({u,v,w});
        edges.push_back({v,u,w}); // For Bellman-Ford, we need both directions
    }
}
void addlink(vector<vector<pair<int,int>>>& graph, vector<Edge>& edges, int u, int v, int w)
{
    graph[u].push_back({v,w});
    graph[v].push_back({u,w});
    edges.push_back({u,v,w});
    edges.push_back({v,u,w});
}
void removelink(vector<vector<pair<int,int>>>& graph, vector<Edge>& edges, int u, int v)
{
    bool found = false;

    for(const auto& edge : graph[u])
    {
        if(edge.first == v)
        {
        found = true;
        break;
        }
    }

    if(!found)
    {
    cout << "Link does not exist!\n";
    return;
    }
    graph[u].erase(
    remove_if(
        graph[u].begin(),
        graph[u].end(),
        [v](pair<int,int>& edge)
        {
            return edge.first == v;
        }
    ),
    graph[u].end()
    );
    graph[v].erase(
    remove_if(
        graph[v].begin(),
        graph[v].end(),
        [u](pair<int,int>& edge)
        {
            return edge.first == u;
        }
    ),
    graph[v].end()
);
    edges.erase(remove_if(edges.begin(), edges.end(), [u,v](const Edge& edge){ return (edge.u==u && edge.v==v) || (edge.u==v && edge.v==u); }), edges.end());
    cout<<"Link removed successfully.\n";
}
void displayGraph(const vector<vector<pair<int,int>>>& graph)
{
    cout<<"the graph is: "<<endl;
    for(int i=0;i<graph.size();i++)
    {
        cout<<"Router "<<i<<" : "<<endl;
        for(const auto &edge: graph[i])        
        {
            cout<<"  "<<"Router "<<edge.first<<" Cost "<<edge.second<<endl;
        }
    }    
    cout<<endl;
}
void printPath(const vector<int>& parent, int v)
{
    if(parent[v]==v) 
    {
        cout<<v;
        return;
    }
    printPath(parent,parent[v]);
    cout<<" -> "<<v;
}
int nexthop(const vector<int>& parent, int des,int src)
{
    if(parent[des]==src) return des;
    int current = des;
    while (parent[current]!=src)
    {
        current = parent[current];
    }
    return current;
}
void printRoutingTable(const vector<int>& dist, const vector<int>& parent, int src)
{
    cout<<"Routing Table for Router "<<src<<":\n";
    cout<<"Destination\tCost\tNext Hop\n";
    for(int i=0;i<dist.size();i++)
    {
        cout<<i<<"\t\t";
        if(dist[i]==INT_MAX) cout<<"Unreachable\t-"<<endl;
        else 
        {
            cout<<dist[i]<<"\t\t";
            if(i==src) cout<<"LOCAL"<<endl;
            else cout<<nexthop(parent,i,src)<<endl;
        }
    }
}
void dijkstra(const vector<vector<pair<int,int>>>& graph, int src)
{
    int n = graph.size();
    vector<int> dist(n, INT_MAX);
    vector<int> parent(n, -1);
    dist[src] = 0;
    parent[src] = src;
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
    pq.push({0, src});
    while(!pq.empty())
    {
        auto [cost,u]=pq.top();
        pq.pop();
        if(cost>dist[u]) continue;
        for(const auto &edge: graph[u])
        {
            int v = edge.first;
            int w = edge.second;
            if(dist[u]+w<dist[v])
            {
                dist[v] = dist[u]+w;
                parent[v] = u;
                pq.push({dist[v],v});
            }
        }
    }
    cout<<"Dijkstra's Algorithm Results:"<<endl;
    printRoutingTable(dist, parent, src);
}
void belmanford(const vector<Edge>& edges, int src,int n)
{
    vector<int> dist(n,INT_MAX);
    vector<int> parent(n,-1);
    dist[src]=0;
    for(int i=0;i<n-1;i++)
    {
        for(const auto &edge: edges)
        {
            int u = edge.u;
            int v = edge.v;
            int w = edge.w;
            if(dist[u]!=INT_MAX && dist[u]+w<dist[v])
            {
                dist[v] = dist[u]+w;
                parent[v] = u;
            }
        }
    }
    for(auto &edge: edges)
    {
        int u = edge.u;
        int v = edge.v;
        int w = edge.w;
        if(dist[u]!=INT_MAX && dist[u]+w<dist[v])
        {
            cout<<"Negative weight cycle detected!"<<endl;
            return;
        }
    }
    cout<<"Bellman-Ford Algorithm Results:"<<endl;
    printRoutingTable(dist, parent, src);
}

int main()
{
    int n,m;

    cout<<"Enter the number of nodes and edges: ";
    cin>>n>>m;

    vector<vector<pair<int,int>>> graph(n);
    vector<Edge> edges;      // <-- important change

    createGraph(graph,edges,m);

    while(true)
    {
        cout<<"\n===== NETWORK ROUTING SIMULATOR =====\n";
        cout<<"1. Display Network\n";
        cout<<"2. Run Dijkstra\n";
        cout<<"3. Run Bellman-Ford\n";
        cout<<"4. Add Link\n";
        cout<<"5. Remove Link\n";
        cout<<"6. Compare Algorithms\n";
        cout<<"7. Display Routing Tables\n";
        cout<<"8. Exit\n";

        int choice;
        cout<<"Enter choice: ";
        cin>>choice;

        switch(choice)
        {
            case 1:
            {
                displayGraph(graph);
                break;
            }

            case 2:
            {
                int src;
                cout<<"Enter source router: ";
                cin>>src;

                dijkstra(graph,src);
                break;
            }

            case 3:
            {
                int src;
                cout<<"Enter source router: ";
                cin>>src;

                belmanford(edges,src,n);
                break;
            }

            case 4:
            {
                int u,v,w;

                cout<<"Enter u v w: ";
                cin>>u>>v>>w;

                addlink(graph,edges,u,v,w);

                cout<<"Link added successfully.\n";
                break;
            }

            case 5:
            {
                int u,v;

                cout<<"Enter u v: ";
                cin>>u>>v;

                if(u < 0 || u >= n || v < 0 || v >= n)
                {
                    cout<<"Invalid router number!\n";
                    break;
                }

                 removelink(graph,edges,u,v);

                
                break;
            }

            case 6:
            {
                int src;
                cout<<"Enter source router: ";
                cin>>src;

                auto start =
                chrono::high_resolution_clock::now();

                dijkstra(graph,src);

                auto end =
                chrono::high_resolution_clock::now();

                auto duration =
                chrono::duration_cast
                <chrono::microseconds>
                (end-start);

                auto start_bf =
                chrono::high_resolution_clock::now();

                belmanford(edges,src,n);

                auto end_bf =
                chrono::high_resolution_clock::now();

                auto duration_bf =
                chrono::duration_cast
                <chrono::microseconds>
                (end_bf-start_bf);

                cout<<"\n===== PERFORMANCE =====\n";

                cout<<"Dijkstra : "
                    <<duration.count()
                    <<" microseconds\n";

                cout<<"Bellman-Ford : "
                    <<duration_bf.count()
                    <<" microseconds\n";

                break;
            }
            case 7:
            {
                for(int i=0;i<n;i++)
                {
                    cout<<"\nRouting Table for Router "<<i<<":\n";
                    dijkstra(graph,i);
                }
                break;
            }
            case 8:
            {
                cout<<"Exiting...\n";
                return 0;
            }

            default:
            {
                cout<<"Invalid Choice!\n";
            }
        }
    }
}