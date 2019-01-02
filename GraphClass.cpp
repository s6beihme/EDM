#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <fstream>

//I am assuming, there are no parallel edges

//class bodies
#define INF 2<<29

//to be used in dijkstra
class Node {
public:
	Node(int _index, int _dist) : index(_index), dist(_dist) {}

	//for priority queue
	bool operator > (Node v2);
	int index;
	int dist;
};


class Edge {
public:
	Edge(int _start, int _target, int _cost) : start(_start), target(_target), cost(_cost) {}
	//copy constructor
	Edge(const Edge&other) : start(other.start), target(other.target), cost(other.cost) {}
	//copy assignment
	Edge& operator = (const Edge& other) { start = other.start; target = other.target; cost = other.cost; return *this; }
	int start;
	int target;
	int cost;
};

class Graph {
public:
	//reads a graph in format from exercise sheet from file
	//adds source and sink and fills adj_list accordingly 
	//(adj_list[node_count] are the edges leaving source and adj_list[node_count+1] are the edges leaving sink.
	//otherwise adj_list is according to index)
	Graph(std::string filename);
	void print();

	//to be used in the first step to compute first potential (NOTE: graph is acyclic, therefore a valid potential exists)
	void moore_bellman_ford(int source, std::vector<int>& predecessor, std::vector<int>& distance);

	//finds the shortest path from node with index source to all other noder using Dijkstras Algorithm with reduced costs
	//stores the paths in predecessor and distance and for each node v1 it stores the position in the adj_list of 
	//v1's predecessor v2, where the edge (v1,v2) is situated, in position_of_edge_in_adj_list_of_predecessor
	void dijkstra(int source, std::vector<int>& predecessor, std::vector<int>& position_of_edge_in_adj_list_of_predecessor, std::vector<int>& distance);

	//turnes an edge around:
	//deletes the edge adj_list[start][position_in_adj_list] and creates a new one( in adj_list of the target
	//of the original edge) with start as the target
	void turn_edge(int start, unsigned int position_in_adj_list);

	//successive shortest path algorithm 
	//returns true if a perfect matching exists, otherwise it returns false
	//this implementation is only usable for instances of the "zuordnungsproblem" (with source and sink already
	//having been added to the graph), because it 
	//uses the balance (b) only implicitly by trying to create a flow from source to sink with 
	//maximal value. It also doesnt add a special node to update the potential, because in this case (if a perfect
	//matching does exist)all nodes can be reached from the source as long as the sink can be reached 
	//from the source, and when that is no longer possible, the algorithm has to end.
	//NOTE: this algorithm changes the graph (eg after the algorithm some edges might have been turned around)
		  //so if the graph is to be used again later, this algorithm should be aplyied on a copy of the graph 
	bool successive_shortest_path();
	void print_min_cost_perfect_matching();
private:
	int node_count;
	std::vector<std::vector<Edge>> adj_list;
	std::vector<int> potential;
};

class ComparisonClass {
public:
	bool operator() (Node v1, Node v2) {
		if (v1.dist >= v2.dist) return true;
		else return false;
	}
};


//Definitions

bool Node::operator > (Node v2) {
	if (dist > v2.dist) return true;
	else return false;
}

Graph::Graph(std::string filename) {
	std::ifstream myfile;
	myfile.open(filename);

	if (!myfile.is_open()) { 
		std::cout << "FILE DIDNT OPEN!\n";
		exit(0);
	}
	
	int start, target, cost;
	myfile >> node_count;
	adj_list = std::vector<std::vector<Edge>>(node_count + 2);
	for (int i = 0; i <= (node_count / 2)-1; i++) adj_list[node_count].push_back(Edge(node_count, i, 0));
	for (int i = node_count / 2; i <= node_count - 1; i++) adj_list[i].push_back(Edge(i, node_count + 1, 0));
	while (myfile.good()) {
		myfile >> start >> target >> cost;
		if (start >= node_count / 2 || start < 0 || target<node_count / 2 || target>node_count-1) {
			std::cout << "\nError: invalid node index in file";
			exit(0);
		}
		adj_list[start].push_back(Edge(start, target, cost));
	}
	node_count+=2;
	potential = std::vector<int>(node_count, 0);
	myfile.close();
}

void Graph::print() {
	for (int i = 0; i < node_count; i++) std::cout << i << ": " << adj_list[i].size() << "\n";
	std::cout << node_count << " Nodes\nEdges:\n";
	for (int i = 0; i < node_count; i++) {
		for (unsigned int j = 0; j < adj_list[i].size(); j++) {
			std::cout << adj_list[i][j].start << " " << adj_list[i][j].target << " " << adj_list[i][j].cost << std::endl;
		}
	}
	std::cout << "\nPotentials:\n";
	for (int i = 0; i < node_count; i++) {
		std::cout << i << " " << potential[i] << std::endl;
	}
}

void Graph::moore_bellman_ford(int source, std::vector<int>& predecessor, std::vector<int>& distance) {
	predecessor = std::vector<int>(node_count, -1);
	distance = std::vector<int>(node_count, INF);
	predecessor[source] = source;
	distance[source] = 0;
	Edge e(0, 0, 0);
	for (int i = 0; i <= node_count - 1; i++) {
		for (int j = 0; j < node_count; j++) {
			for (unsigned int k = 0; k < adj_list[j].size(); k++) {
				e = adj_list[j][k];
				if (distance[e.target] > distance[j] + e.cost) {
					distance[e.target] = distance[j] + e.cost;
					predecessor[e.target] = j;
				}
			}
		}
	}
}

void Graph::dijkstra(int source, std::vector<int>& predecessor, std::vector<int>& position_of_edge_in_adj_list_of_predecessor, std::vector<int>& distance) {
	std::priority_queue<Node, std::vector<Node>, ComparisonClass > pq; //CHECK THIS IN TESTING
	predecessor = std::vector<int>(node_count, -1);
	position_of_edge_in_adj_list_of_predecessor = std::vector<int>(node_count, -1);
	std::vector<bool> shortest_path_found(node_count, false);
	distance = std::vector<int>(node_count, INF);

	predecessor[source] = source;
	distance[source] = 0;
	pq.push(Node(source, 0));

	int v;
	Edge e(0,0,0);
	while (!pq.empty()) {
		v = pq.top().index;
		pq.pop();

		//do the following so no decrease key is necessary
		//(because priority_queue doesnt have decrease key)
		//and add copies of nodes into queue instead of decreasing key (NOTE: O(logE)=O(logV))
		if (shortest_path_found[v] == true) continue;
		else {
			shortest_path_found[v] = true;
			for (unsigned int i = 0; i < adj_list[v].size(); i++) {
				e = adj_list[v][i];
				//here the reduced costs are used
				if (distance[e.target] > distance[v] + (e.cost + potential[v] - potential[e.target])) {
					distance[e.target] = distance[v] + (e.cost + potential[v] - potential[e.target]);
					predecessor[e.target] = v;
					position_of_edge_in_adj_list_of_predecessor[e.target] = i;
					//add copy
					pq.push(Node(e.target, distance[e.target]));
				}
			}
		}
	}
}

void Graph::turn_edge(int start, unsigned int position_in_adj_list) {
	int target = adj_list[start][position_in_adj_list].target;
	int cost = adj_list[start][position_in_adj_list].cost;
	adj_list[start].erase(adj_list[start].begin() + position_in_adj_list);
	adj_list[target].push_back(Edge(target, start, -cost));
}

bool Graph::successive_shortest_path() {
	std::vector<int> predecessor;
	std::vector<int> position_of_edge_in_adj_list_of_predecessor;
	std::vector<int> distance;

	//find potential
	moore_bellman_ford(node_count - 2, predecessor, distance);
	//if one node cannot be reached from the source, then a perfect matching doesnt exist
	for (int i = 0; i < node_count; i++) { if (predecessor[i] == -1) return false; }
	for (int i = 0; i < node_count; i++) potential[i] = distance[i];
	
	//find optimal solution recursively (if possible)
	int current_flow = 0;
	int current_node, current_predecessor;
	while (current_flow != (node_count - 2) / 2) {
		dijkstra(node_count - 2, predecessor, position_of_edge_in_adj_list_of_predecessor, distance);
		if (predecessor[node_count - 1] == -1) return false;
		//gamma (from algorithm in lecture) is always 1
		//augment along shortest source-sink path
		current_node = node_count - 1;
		current_predecessor = predecessor[current_node];
		while (current_node != current_predecessor) {
			turn_edge(current_predecessor, position_of_edge_in_adj_list_of_predecessor[current_node]);
			current_node = current_predecessor;
			current_predecessor = predecessor[current_node];
		}
		current_flow++;
		//compute potential for new residual graph
		dijkstra(node_count - 2, predecessor, position_of_edge_in_adj_list_of_predecessor, distance);
		for (int i = 0; i < node_count; i++) potential[i] = potential[i] + distance[i];
	}
	return true;
}

void Graph::print_min_cost_perfect_matching() {
	bool success = successive_shortest_path();
	if (!success) std::cout << "\nA perfect matching does not exist\n";
	else {
		int cost_of_matching=0;
		for (int i = (node_count - 2) / 2; i <= node_count - 3; i++) {
			cost_of_matching += adj_list[i][0].cost;
		}
		std::cout << -cost_of_matching << std::endl;
		for (int i = (node_count - 2) / 2; i <= node_count - 3; i++) {
			std::cout << adj_list[i][0].target << " " << i << std::endl;
		}
	}
}
int main() {
	
	Graph g("graphfile.txt");

	g.print_min_cost_perfect_matching();
	
}