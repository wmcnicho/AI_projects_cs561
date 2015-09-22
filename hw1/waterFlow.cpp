//Author: Hunter McNichols
//CS561 Homework 1 : Uninformed Search

#include <iostream>
#include <fstream>
#include <vector>

class Edge;
struct Node;
class Graph;
enum Node_type {SOURCE, DEST, MID};

void run_BFS(Graph &g, int start_time);
void run_DFS(Graph &g, int start_time);
void run_UCS(Graph &g, int start_time);


class Graph{
public:
	Node *source;
	Graph();
	~Graph();

	void create_node(const char *name, Node_type type);
	void add_edge(const char *start_name, char *end_name, int length);

	std::vector<Node> nodes;
};

Graph::Graph(){

}

Graph::~Graph(){
	//going to need to write this to clean up memory
}

void Graph::create_node(const char *name, Node_type type){

}

void Graph::add_edge(const char *start_name, char *end_name, int length){
	
}


int main(int argc, char *argv[]){
	if(argc < 2){
		std::cout << "Fail, not enough args" << std::endl;
		return 1;
	}
	
	std::ifstream input_file(argv[2]);
	if(!input_file.is_open()){
		std::cout << "File not found" << std::endl;
		return 1;
	}

	while(!input_file.eof()){

		//parse a single simulation worth of data
		// <task> algorithm that you are supposed to use for this case
		std::string alg;
		input_file >> alg;

		Graph *g = new Graph();
		// <source> name of the source node
		char *source_name;
		input_file >> source_name;
		g->create_node(source_name, SOURCE);
		// <destinations> names of the destination nodes
		std::string name;
		while(std::getline(input_file, name, ' ')){
			g->create_node(name.c_str(), DEST);
		}

		// <middle nodes> names of the middle nodes
		while(std::getline(input_file, name, ' ')){
			g->create_node(name.c_str(), MID);
		}

		// <#pipes> represents the number of pipes
		int num_pipes;
		input_file >> num_pipes;

		// <graph> represents start-end nodes, lengths and off-times of pipes
		for (int i = 0; i < num_pipes; ++i)
		{
			//<start> <end> <length> <#off-periods> < period1 > .... <periodn>
			char *start;
			char *end;
			int length;
			int num_periods;
			input_file >> start >> end >> length >> num_periods;

			g->add_edge(start, end, length);

			//ignoring off-periods for now because not starting UCS yet
			std::string buff;
			std::getline(input_file, buff);
		}

		int start_time;
		input_file >> start_time;

		//run simulation
		if(alg == "BFS"){
			run_BFS(*g, start_time);
		}
		else{
			std::cout << "Haven't written these yet" << std::endl;
		}

		//write to file and clean up memory

	}

std::cout << "Hello World" << std::endl;
return 0;
}


void run_BFS(Graph &g, int start_time){

}




//Graph data structure, this may change significantly
class Edge
{
	Edge(Node* source, Node* dest, int length, int *active_hours);
	~Edge();
	bool checkActive(int time);

	Node* source;
	Node* dest;
	int length;
	int *active_hours;

public:
};

Edge::Edge(Node* source, Node* dest, int length, int *active_hours):
 source(source), dest(dest), length(length), active_hours(active_hours) 
{

}

Edge::~Edge(){
	//?
}

bool Edge::checkActive(int time){
	std::cout << "Write this" << std::endl;
	return false;
}

class Node
{
	Node(Node_type n, char* name);

	std::vector<Edge*> edges;
	Node_type type;
	char* name;

public:
	void add_edge(Edge* e);
};

Node::Node(Node_type n, char* name): type(n), name(name){
}

void Node::add_edge(Edge* e){
	//edges.push_back(e);
}
