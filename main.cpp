/*
DESCRIPTION : to optimal vertex cover via primal dual algorithm
AUTHOR      : AJAY SINGH
NOTE        :vertices should be labeled 0, 1, 2, ... n
            : Assumption is made that graph is connected
*/
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <queue>
#include <cmath>
using namespace std;

int numVertices = 0; //default
int numEdges = 0;
enum vertexState  { DISCOVERED, UNDISCOVERED, PROCESSED};
vector <pair<int, int> > edgeSequence;

struct adjacenylist{
int vertexLabel;
adjacenylist *adjacentVertex;
}; //list for each vertex in vector

//GRAPH
struct vertex{
int vertexLabel;
vertexState vertexStatus; //IN_VERTEX_SET, NOT_IN_VERTEX_SET
bool inVC;
int parent; //parent of this vertex in BFS tree
int distance; //distance from source
int w;  //weight of vertex
int s;
adjacenylist *adjacentVertex;
};
vector <vertex> vertexList;
vector <int> parentList;
int** yij;

/*
DESCRIPTION : UTILITY FUNCTION TO PRINT GRAPH TO CONSOLE.  NOT USED BY DEFAULT
AUTHOR      : AJAY SINGH
NOTE        :vertices should be labeled 0, 1, 2, ... n
*/
void printGraph()
{
    adjacenylist *nextVertex;
    for(int i = 0; i < numVertices; i++)
    {
        cout<<vertexList[i].vertexLabel<<" -> ";
        nextVertex = vertexList[i].adjacentVertex;
        while(nextVertex != nullptr)
        {
            cout<<nextVertex->vertexLabel<<" -> ";
            nextVertex = nextVertex->adjacentVertex;
        }
        cout<<endl;
    }
}

/*
DESCRIPTION : to PRINT VC
AUTHOR      : AJAY SINGH
NOTE        :vertices should be labeled 0, 1, 2, ... n
*/
void printVC()
{
   int cost = 0;
   for(int i = 0; i < numVertices; i++)
    {
        if(vertexList[i].inVC){

            cost = cost + vertexList[i].w;
            cout<<vertexList[i].vertexLabel<<"  ";
            }
    }
     cout<<endl;
     cout<<"Cost : "<<cost<<endl;
}
void printEdgeSequence()
{
    for(int i = 0; i < numEdges; i++)
    {
        cout<<"("<<edgeSequence[i].first <<"-"<<edgeSequence[i].second<<")   ";
    }
    cout<<endl;
}

/*
DESCRIPTION : primal dual algo for vertex cover edges covered in BFS order
AUTHOR      : AJAY SINGH
NOTE        :
*/
void vertexCoverBFS(int v)
{
    queue <vertex> BFSQueue;
    vertex vert;
    adjacenylist *y;
    int x;
//init the source vertex
    vertexList[v].parent = -1;
    vertexList[v].distance = 0;

    vert = vertexList[v];
    vert.vertexStatus = DISCOVERED;
    //vert.parent = -1;
    //vert.distance = 0;



    BFSQueue.push(vert);
    int i = 0;
    while(!BFSQueue.empty())
    {
        x = BFSQueue.front().vertexLabel;
        y = vertexList[x].adjacentVertex;

        while(y != nullptr)// looking for all adjacent edges cahanging their status discovered if not yet discovered
        {
            if(vertexList[y->vertexLabel].vertexStatus == UNDISCOVERED)
            {
                vertexList[y->vertexLabel].vertexStatus = DISCOVERED;

                vertexList[y->vertexLabel].parent = vertexList[x].vertexLabel;
                vertexList[y->vertexLabel].distance = vertexList[x].distance + 1;
                BFSQueue.push(vertexList[y->vertexLabel]);

               // cout<<"edge "<< x <<"-"<<y->vertexLabel<<endl;

                if(!(vertexList[x].inVC || vertexList[y->vertexLabel].inVC)){
                    int delta = (vertexList[x].w - vertexList[x].s) < (vertexList[y->vertexLabel].w - vertexList[y->vertexLabel].s)?(vertexList[x].w - vertexList[x].s):(vertexList[y->vertexLabel].w - vertexList[y->vertexLabel].s);
                    yij[x][y->vertexLabel] = yij[x][y->vertexLabel] + delta;
                    vertexList[x].s = vertexList[x].s + delta;
                    vertexList[y->vertexLabel].s = vertexList[y->vertexLabel].s + delta;

                    //cout<<"edge "<< x <<"-"<<y->vertexLabel<<endl;
                    edgeSequence[i].first = x;
                    edgeSequence[i].second = y->vertexLabel;
                        i++;
                    numEdges = i;
                    if(vertexList[x].s == vertexList[x].w)
                    {
                      // cout << x <<"added to VC"<<endl;
                        vertexList[x].inVC = 1;

                    }
                    if(vertexList[y->vertexLabel].w == vertexList[y->vertexLabel].s)
                    {
                        //cout << y->vertexLabel <<"added to VC"<<endl;
                        vertexList[y->vertexLabel].inVC = 1;
                    }


                }
            }
            y = y->adjacentVertex;
        }
        vertexList[x].vertexStatus = PROCESSED;
        BFSQueue.pop();
    }

}

int main()
{
    ifstream input_file;
    string delimiter1 = ":", delimiter2 = ",", str_item,inputLine;
    size_t pos = 0;
    string::size_type sz;

    adjacenylist *newVertex, *adjListhead, *lastNode;
    cout<<"make sure input file has no spaces \n Undirected graph has a edge twice in adjacency list\n GRAPH IS :"<<endl;

    input_file.open("inp-params.txt", ifstream::in);

    //#if 0
    /*Block parsing input file*/
    if (input_file.is_open())
    {

        //first calc number of vertices
        while(getline(input_file, inputLine))
        {
            cout <<inputLine<<endl;
            numVertices++;
        }

        cout <<"#vertices = " <<numVertices <<endl;

            //dynamically allocated memoization table for dual variables
        yij = new int*[numVertices];
        for(int row = 0; row < numVertices; row++)
        {
            yij[row] = new int[numVertices];
        }

        for(int row = 0; row < numVertices; row++)
        {
            for(int col = 0; col < numVertices; col++)
            {
                yij[row][col] = 0;
            }
        }


        vertexList.resize(numVertices);
        edgeSequence.resize(numVertices);
        int cap = vertexList.capacity();

        //moving again to beginning of file
        input_file.clear();
        input_file.seekg(0, ios::beg);

        int i = 0;
        while(getline(input_file, inputLine))
        {
            pos = inputLine.find(delimiter1);
            str_item = inputLine.substr(0, pos);
            vertexList[i].vertexLabel = stod(str_item, &sz);
            vertexList[i].adjacentVertex = nullptr;
            vertexList[i].vertexStatus = UNDISCOVERED;
            vertexList[i].parent = -1;
            vertexList[i].distance = INFINITY;// means infinity
            vertexList[i].s = 0;
            vertexList[i].w = 0;
            vertexList[i].inVC = 0;
            inputLine.erase(0, pos + delimiter1.length());

            //cout<<str_item<<":";
            if(inputLine == "")// no adjacents present fill adj list for another vertex
            {
                i++;
                continue;//no adjacents present
            }

            int j = 0;
            while(pos != string::npos)
            {
                pos = inputLine.find(delimiter2);// if no more delimiter is found then returns npos

                str_item = inputLine.substr(0, pos);

                newVertex = new adjacenylist;
                newVertex->vertexLabel = stod(str_item, &sz);
                if(newVertex->vertexLabel >= numVertices)
                {
                    cout <<"invalid :p"<<endl;
                    return 0;
                }
                newVertex->adjacentVertex = nullptr;
                //newVertex->vertexStatus = UNDISCOVERED;

                if(0 == j)
                {
                    adjListhead = newVertex;
                    lastNode = adjListhead;
                }
                else
                {
                    lastNode->adjacentVertex = newVertex; // last node keeps track of adjacent list last node so far.
                    lastNode = lastNode->adjacentVertex;
                }
                j++;

                vertexList[i].adjacentVertex = adjListhead;
                inputLine.erase(0, pos + delimiter2.length());
                //cout<<str_item<<",";
            }
            i++;
        }
    }
    else{
    cout<<"inp-params.txt not open";
    }

    //printGraph();
    for(int i = 0; i < numVertices; i++)
    {
        cout<<endl;
        cout<<"enter vertex "<<i<<"weight";
        cin>>vertexList[i].w ;
        }

    int v;
//    cout <<"Enter begin vertex (0-" <<numVertices<<") "<<endl;
//    cin >>v;
    vertexCoverBFS(0);
    cout<<"edge sequence : ";
    printEdgeSequence();
    cout<<"Vertex Cover : ";
    printVC();

    input_file.close();
    return 0;
}
