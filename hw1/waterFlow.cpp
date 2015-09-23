//Author: Hunter McNichols
//CS561 Homework 1 : Uninformed Search

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <queue>
#include <set>
#include <algorithm>
#include <climits>

class Edge;
struct Node;
class Graph;
enum Node_type {SOURCE, DEST, MID};

void run_BFS(Graph &g, int start_time, std::ofstream &output);
void run_DFS(Graph &g, int start_time);
void run_UCS(Graph &g, int start_time);


class Graph{
    Node *source;
    std::vector<Node*> nodes;
    std::vector<Edge*> edges;
public:
    ~Graph();
    void create_node(std::string name, Node_type type);
    void add_edge(std::string start_name, std::string end_name, int length);
    Node *getSource();
    Graph();
    
};

//Graph data structure, this may change significantly
class Edge
{
public:
    Node* source;
    Node* dest;
    int length;
    int *active_hours;
    
    
    Edge(Node* source, Node* dest, int length, int *active_hours);
    bool checkActive(int time);
};


class Node
{
    std::vector<Edge*> edges;
    std::vector<Node*> children;
    
public:
    Node(Node_type n, std::string name);
    Node_type type;
    std::string name;
    void add_edge(Edge* e);
    std::vector<Node*> &getChildren();
    int getNumChildren();
};


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
    
    int num_sims;
    input_file >> num_sims;
    input_file.ignore(INT_MAX, '\n');
    
    std::ofstream output("output.txt");
    
    while(!input_file.eof()){
        
        //parse a single simulation worth of data
        // <task> algorithm that you are supposed to use for this case
        std::string alg;
        input_file >> alg;
        input_file.ignore(INT_MAX, '\n');
        
        Graph *g = new Graph();
        // <source> name of the source node
        std::string source_name;
        input_file >> source_name;
        input_file.ignore(INT_MAX, '\n');
        g->create_node(source_name, SOURCE);
        // <destinations> names of the destination nodes
        std::string line;
        if(std::getline(input_file, line)){
            std::istringstream iss(line);
            std::string name;
            while(iss >> name){
                g->create_node(name, DEST);
            }
        }
        
        // <middle nodes> names of the middle nodes
        if(std::getline(input_file, line)){
            std::istringstream iss(line);
            std::string name;
            while(iss >> name){
                g->create_node(name, MID);
            }
        }
        
        
        // <#pipes> represents the number of pipes
        int num_pipes;
        input_file >> num_pipes;
        input_file.ignore(INT_MAX, '\n');
        
        // <graph> represents start-end nodes, lengths and off-times of pipes
        for (int i = 0; i < num_pipes; ++i)
        {
            //<start> <end> <length> <#off-periods> < period1 > .... <periodn>
            std::string start;
            std::string end;
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
        input_file.ignore(INT_MAX, '\n');
        
        //run simulation
        if(alg == "BFS"){
            run_BFS(*g, start_time, output);
        }
        else{
            std::cout << "Haven't written these yet" << std::endl;
        }
        delete g;
    }
    
    std::cout << "Finished" << std::endl;
    return 0;
}


void run_BFS(Graph &g, int start_time, std::ofstream& output){
    int current_time = start_time;
    Node *n = g.getSource();
    if(n->type == DEST){
        std::cout << n->name << " " << current_time << std::endl;
        output << n->name << " " << current_time << std::endl;
        return;
    }
    std::queue<Node*> frontier;
    frontier.push(n);
    
    std::set<Node*> explored;
    
    while(!frontier.empty()){
        n = frontier.front();
        frontier.pop();
        explored.insert(n);
        if(n->type == DEST){
            //success
            std::cout << n->name << " " << current_time << std::endl;
            output << n->name << " " << current_time << std::endl;
            return;
        }
        //queue children into frontier
        std::vector<Node*> children = n->getChildren();
        for (int i=0; i<children.size(); ++i) {
            if(explored.find(children[i]) == explored.end()){//If the node has not already been explored
                frontier.push(children[i]);
            }
        }
        //increment time
        current_time++;

    }
    //if this while loop finishes then all nodes were checked and no suceess was found
    std::cout << "None" << std::endl;
    output << "None" << std::endl;
    return;
    
    
}


//Graph implementation
Graph::Graph(){
    
}

Graph::~Graph(){
    //clean up nodes and edges created in graph
    for(std::vector<Node*>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        delete *it;
    }
    for(std::vector<Edge*>::iterator it = edges.begin(); it != edges.end(); ++it) {
        delete *it;
    }

}

void Graph::create_node(std::string name, Node_type type){
    Node *new_node = new Node(type, name);
    if(type == SOURCE){
        source = new_node;
    }
    //Does ordering matter?
    nodes.push_back(new_node);
}

void Graph::add_edge(std::string start_name, std::string end_name, int length){
    Node *start = NULL;
    Node *end = NULL;
    for (int i = 0; i < nodes.size(); ++i)
    {
        if(nodes[i]->name == start_name){
            start = nodes[i];
        }
        if(nodes[i]->name == end_name){
            end = nodes[i];
        }
    }
    if(!start || !end){
        std::cout << "Problem in adding edge" << std::endl;
        return;
    }
    Edge *e = new Edge(start, end, length, NULL);
    edges.push_back(e);
    start->add_edge(e);
}



//Edge Implementation
Edge::Edge(Node* source, Node* dest, int length, int *active_hours):
source(source), dest(dest), length(length), active_hours(active_hours) 
{
    
}


bool Edge::checkActive(int time){
    std::cout << "Write this" << std::endl;
    return false;
}


//Node implementation
Node::Node(Node_type n, std::string name): type(n), name(name){
}


Node* Graph::getSource(){
    return source;
}

std::vector<Node*> &Node::getChildren(){
    //This might be unnessary you could probably just return and then in the add_edge deal with sorting
    children.clear();
    for (int i=0; i<edges.size(); ++i) {
        children.push_back(edges[i]->dest);
    }
    //TODO: sort children alphabetically before returning
    return children;
}

int Node::getNumChildren(){
    return static_cast<int>(edges.size());
}

void Node::add_edge(Edge* e){
    edges.push_back(e);
}
