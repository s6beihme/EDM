#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>

// Implementation of Kruskals Algorithm using a Matrix class and making kruskal a method

//Find function to use in Union-Find-Functions 
//using a given "parent-vector" (using call by reference)
//(the Union function is coded directly in Kruskals Algorithm)
int Find(int x, std::vector<int> &parent) {
	if(x!=parent[x]) {
		parent[x]=Find(parent[x], parent);
	}
	return parent[x];
}



//Edge class to be used in Kruskals Algorithm
//An edge contains its starting point and its target point and its weight
//note that start and target are interchangable for undirected Graph
class Edge{
	public:
		int start;
		int target;
		double weight;
		
		//function to print edge (to be used in "print_graph")
		void print_edge();
		
		friend class Graph;
};

//function to print edge (to be used in "print_graph")
void Edge::print_edge() {
	std::cout << "{" << start << ", " << target << "}, weight: " << weight << "\n";
}

//function to compare two edges via their weight
//(to be used to sort the edges)
bool comp_edges(Edge e1, Edge e2) {
	return e1.weight < e2.weight;
}


//Graph class for Kruskals Algorithm
class Graph{
	public:
		Graph(int numb_nodes);
		
		//function to read the edges from a file.
		//format of file: 
		//first line: number of nodes in graph
		//after that each line represents an edge: start target weight
		void read_from_file(std::string filename);
		
		//print a graph in the same format as the file (with ',' between the entries of edges)
		void print_graph();
		
		//gives number of edges in graph by returning the size of edges
		unsigned int num_edges();
		
		//function to sort the edges via their weight (to be used in kruskal)
		void sort_edges();
		
		//Kruskals Algorithm to find a MTS in the Graph:		
		//Using Union-Find-Datastructures and Path-Compression
		void Kruskal();
		
		
	private:
		int n;
		std::vector<Edge> edges;
		
};

//initialize Graph with number of noder (default is 0)
Graph::Graph(int numb_nodes=0) { 
	n=numb_nodes;		
}


 //read a file representing a graph and create a graph object accordingly		
void Graph::read_from_file(std::string filename) {
	std::ifstream myfile; //filestream object to read the file
	myfile.open(filename); //open the file
	if(!myfile.is_open()) { //if file isnt open Warning and exit
		std::cout<<"FILE NOT OPENED!\n";
		exit(0);
	}
	myfile>>n; //save number of nodes
	
	//check if given number for number of nodes is valid
	if(n<0) {
		std::cout<< "number of nodes has to be >=0";
	}
	
	while(myfile.good()) { //go through whole file and add all the edges with respective weight
	
		//edge to fill and append to edges
		Edge e;
		
		//fill edge
		myfile>>e.start;
		myfile>>e.target;
		myfile>>e.weight;
		
		//check if edge is valid
		if(e.start<0 || e.start>=n || e.target<0 || e.target>=n ) {
			std::cout << "edge is incident with nonexistent node";
			exit(0);
		}
		
		if(e.start==e.target) {
			std::cout << "no edge from one node to itself allowed";
			exit(0);
		}
		
		//append edge to edges
		edges.push_back(e);
	}
	myfile.close(); 
}

//print the graph (first the number of nodes and then each edge)
void Graph::print_graph() {
	std::cout << "Number of nodes in Graph: " << n << std::endl;
	for(unsigned int i=0; i<edges.size(); i++) {
		edges[i].print_edge();
	}
}

//gives number of edges in graph by returning the size of edges
unsigned int Graph::num_edges() {
	return edges.size();
}

//function to sort the edges via their weight (to be used outside of methods)
void Graph::sort_edges(){
	std::sort(edges.begin(), edges.end(), comp_edges);
}

//Kruskals Algorithm to find a MTS in the Graph:		
//Using Union-Find-Datastructures and Path-Compression
void Graph::Kruskal(){
	// first make a vector representing each nodes parent (parent(vi)=parent[vi])
	std::vector<int> parent(n);
	
	//initialize parent (parent(vi)=vi)
	for(int i=0; i<n; i++) {
		parent[i]=i;
	}
	
	// make vector representing the rank of node
	std::vector<int> rank(n,0);
	
	//make vector of type Edge to put edges in there when they are added to the tree
	std::vector<Edge> Tree;
	
	// variable in which to store current weight of tree 
	double curr_weight=0;
	
	//sort the edges according to their weight
	std::sort(edges.begin(), edges.end(), comp_edges);
	
	//set variable for number of connected components,
	//decrease it every time two are being merged,
	//to check if graph contains Tree
	int num_con_comp=n;
	
	//now go through all edges (from lightest to heaviest)
	//if they dont close a circle add them to Tree
	for(unsigned int i=0; i<num_edges(); i++) {
		Edge e=edges[i];
		
		//find parent of start and target of edge
		int X=Find(e.start, parent);
		int Y=Find(e.target, parent);
		
		//if they are not in the same Set, add the edge to Tree,
		//add e.weight to curr_weight, merge the two Sets
		// and decrease num_con_comp
		if(X!=Y) {
			Tree.push_back(e);
			curr_weight+=e.weight;
			num_con_comp-=1;
			
			//now do Union(X,Y)
			if(rank[X]>rank[Y]) {
			parent[Y]=X;
			}
			else {
				parent[X]=Y;
			}
			if(rank[Y]==rank[Y]) {
				rank[Y]+=1;
			}
		}
	}
	
	//now print the output
	if(num_con_comp!=1) {
		std::cout << "Graph is not connected!\n";
	}
	else {
		std::cout << "The weight of the MST is " << curr_weight << "\n";
		for(unsigned int i=0; i<Tree.size(); i++) {
			Tree[i].print_edge();
		}
	}
}

int main() {
	Graph g;
	g.read_from_file("file1.txt");
	std::cout<<g.num_edges()<<"\n";
	g.Kruskal();
	
	return 0;
}
