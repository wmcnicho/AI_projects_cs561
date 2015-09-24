//Author: Hunter McNichols
//CS561 Homework 1 : Uninformed Search

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <queue>
#include <stack>
#include <set>
#include <algorithm>
#include <climits>
#include <list>

class Edge;
struct Node;
class Graph;
class UCS_Node;
enum Node_type {SOURCE, DEST, MID};

void run_BFS(Graph &g, int start_time, std::ofstream &output);
void run_DFS(Graph &g, int start_time, std::ofstream &output);
void run_UCS(Graph &g, int start_time, std::ofstream &output);


class Graph{
    Node *source;
    std::vector<Node*> nodes;
    std::vector<Edge*> edges;
public:
    ~Graph();
    void create_node(std::string name, Node_type type);
    void add_edge(std::string start_name, std::string end_name, int length, int hours[]);
    Node *getSource();
    
};

class Edge
{
public:
    Node* source;
    Node* dest;
    int length;
    std::vector<int> active_hours;
    int num_active_hours;
    
    
    Edge(Node* source, Node* dest, int length, int *hourData);
};


class Node
{
    std::vector<Edge*> edges;
    std::vector<Node*> children;
public:
    std::vector<int> validHours;
    bool isActive(int time);
    void setValidHours(std::vector<int> hours);
    int pathCost;
    Node(Node_type n, std::string name);
    Node_type type;
    std::string name;
    void add_edge(Edge* e);
    std::vector<Node*> &getChildren();
    std::vector<Node*> getUCSChildren(int currentCost);
    //std::vector<UCS_Node> getUCSChildren(int currentCost);//yeah it's slow but better than storing it here
    int getNumChildren();
};

class UCS_Node{
    
public:

    Node_type type;
    int cost;
    std::string name;
    Node *origin;
    std::vector<int> validHours;
    
    UCS_Node(Node *node);
    bool isActive(int time);
};

bool compareNodeName(const Node* lhs, const Node* rhs){
    return lhs->name < rhs->name;
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
    
    int num_sims;
    input_file >> num_sims;
    input_file.ignore(INT_MAX, '\n');
    
    std::ofstream output("output.txt");
    
    for(int i=0; i<num_sims; i++){
        
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
            
            int hours[24] = {};
            for (int j=0; j<num_periods; j++) {
                std::string period;
                input_file >> period;
                
                std::stringstream ss(period);
                std::string item1;
                std::string item2;
                std::getline(ss, item1, '-');
                std::getline(ss, item2, '-');
                int s = atoi(item1.c_str());
                int f = atoi(item2.c_str());
                for (int k=s; k<=f; k++) {
                    hours[k]++;
                }
            }
            g->add_edge(start, end, length, hours);
            input_file.ignore(INT_MAX, '\n');
        }
        
        int start_time;
        input_file >> start_time;
        input_file.ignore(INT_MAX, '\n');
        start_time = start_time%24;
        
        //run simulation
        if(alg == "BFS"){
            run_BFS(*g, start_time, output);
        }
        else if(alg == "DFS")
        {
            run_DFS(*g, start_time, output);
        }
        else if(alg == "UCS"){
            run_UCS(*g, start_time, output);
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
    explored.insert(n);
    
    while(!frontier.empty()){
        n = frontier.front();
        frontier.pop();
        
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
                explored.insert(children[i]);
            }
        }
        //increment time
        current_time++;
        current_time = current_time%24;

    }
    //if this while loop finishes then all nodes were checked and no suceess was found
    std::cout << "None" << std::endl;
    output << "None" << std::endl;
    return;
    
    
}

void run_DFS(Graph &g, int start_time, std::ofstream& output){
    int current_time = start_time;
    Node *n = g.getSource();
    if(n->type == DEST){
        std::cout << n->name << " " << current_time << std::endl;
        output << n->name << " " << current_time << std::endl;
        return;
    }
    std::stack<Node*> frontier;
    frontier.push(n);
    
    std::set<Node*> explored;
    explored.insert(n);
    
    while(!frontier.empty()){
        n = frontier.top();
        frontier.pop();

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
                explored.insert(children[i]);
            }
        }
        //increment time
        current_time++;
        current_time = current_time%24;
        
    }
    //if this while loop finishes then all nodes were checked and no suceess was found
    std::cout << "None" << std::endl;
    output << "None" << std::endl;
    return;
    
    
}

void run_UCS(Graph &g, int start_time, std::ofstream& output){
    int current_time = start_time;
    Node *n = g.getSource();
    n->pathCost = 0;
    int init_hours[24] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 };
    n->setValidHours(std::vector<int>(init_hours, init_hours + sizeof(init_hours) / sizeof(int)));
    std::list<Node*> frontier;

    frontier.push_front(n);
    
    std::set<Node*> explored;
    
    while(!frontier.empty()){
        n = frontier.front();
        frontier.pop_front();
        
        //checks to see if n is a valid node, if not it will pull the next valid node
        while(!n->isActive(current_time)){
            if(frontier.empty()){
                //remaining children are not accessible
                std::cout << "None" << std::endl;
                output << "None" << std::endl;
                return;
            }
            n = frontier.front();
            frontier.pop_front();
        }
        
    
        if(n->type == DEST){
            //success
            //total time is the starting time + the total path cost
            std::cout << n->name << " " << (start_time + n->pathCost)%24 << std::endl;
            output << n->name << " " << (start_time + n->pathCost)%24 << std::endl;
            return;
        }
        explored.insert(n);
        
        //queue children into frontier
        int current_cost = n->pathCost;
        std::vector<Node*> children = n->getUCSChildren(current_cost);
        for (int i=0; i<children.size(); ++i) {
            //if not in explored or frontier == !explored && !frontier
                //add to frontier
            //if in frontier with larger cost
                //remove node currently in frontier and insert this node
            if(explored.find(children[i]) == explored.end()){//if not in explored
                std::list<Node*>::iterator search = std::find(frontier.begin(), frontier.end(), children[i]);
                if(search != frontier.end()){//not in explored but in frontier
                      frontier.remove(*search);//remove and then add new child value
                }
                
                

                //not in explored or frontier
                bool found_place = false;
                std::list<Node*>::iterator iterator;
                for (iterator = frontier.begin(); iterator != frontier.end(); ++iterator) {
                    if( children[i]->pathCost < (*iterator)->pathCost){
                        //reached correct spot to insert in prioirty queue
                        frontier.insert(iterator, children[i]);
                        found_place = true;
                    }
                    else if(children[i]->pathCost == (*iterator)->pathCost){
                        if(children[i]->name >= (*iterator)->name){
                            frontier.insert(iterator, children[i]);
                            found_place = true;
                        }
                    }
                }
                if(!found_place){
                    frontier.push_back(children[i]);
                }
    
            }
        
        }
        //increment time will not be +1 but instead be start + total pathCost
        current_time = start_time + n->pathCost;
        current_time = current_time%24;
        
    }
    //if this while loop finishes then all nodes were checked and no suceess was found
    std::cout << "None" << std::endl;
    output << "None" << std::endl;
    return;
    
    
}
//void run_UCS(Graph &g, int start_time, std::ofstream& output){
//    int current_time = start_time;
//    Node *temp = g.getSource();
//    temp->pathCost = 0;
//    int init_hours[24] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 };
//    temp->setValidHours(std::vector<int>(init_hours, init_hours + sizeof(init_hours) / sizeof(int)));
//    
//    UCS_Node n = UCS_Node(temp);
//    std::list<UCS_Node> frontier;
//    
//    frontier.push_front(n);
//    
//    std::set<Node*> explored;
//    
//    while(!frontier.empty()){
//        n = frontier.front();
//        frontier.pop_front();
//        
//        //checks to see if n is a valid node, if not it will pull the next valid node
//        while(!n.isActive(current_time)){
//            if(frontier.empty()){
//                //remaining children are not accessible
//                std::cout << "None" << std::endl;
//                output << "None" << std::endl;
//                return;
//            }
//            n = frontier.front();
//            frontier.pop_front();
//        }
//        
//        
//        if(n.type == DEST){
//            //success
//            //total time is the starting time + the total path cost
//            std::cout << n.name << " " << (start_time + n.cost)%24 << std::endl;
//            output << n.name << " " << (start_time + n.cost)%24 << std::endl;
//            return;
//        }
//        explored.insert(n.origin);
//        
//        //queue children into frontier
//        int current_cost = n.cost;
//        std::vector<UCS_Node> children = n.origin->getUCSChildren(current_cost);
//        for (int i=0; i<children.size(); ++i) {
//            //if not in explored or frontier == !explored && !frontier
//            //add to frontier
//            //if in frontier with larger cost
//            //remove node currently in frontier and insert this node
//            UCS_Node expansion = UCS_Node(children[i]);
//            if(explored.find(expansion.origin) == explored.end()){//if not in explored
//                std::list<UCS_Node>::iterator it;
//                for (it = frontier.begin(); it != frontier.end(); ++it) {
//                    if(expansion.name == it->name){
//                        if( it->cost > expansion.cost){
//                            frontier.erase(it);//remove and then add new child value
//                        }
//                    }
//                }
//            
//                //not in explored or frontier
//                bool found_place = false;
//                std::list<UCS_Node>::iterator iterator;
//                for (iterator = frontier.begin(); iterator != frontier.end(); ++iterator) {
//                    if( expansion.cost < (iterator)->cost){
//                        //reached correct spot to insert in prioirty queue
//                        frontier.insert(iterator, expansion);
//                        found_place = true;
//                    }
//                    else if(expansion.cost == (iterator)->cost){
//                        if(expansion.name >= (iterator)->name){
//                            frontier.insert(iterator, expansion);
//                            found_place = true;
//                        }
//                    }
//                }
//                if(!found_place){
//                    frontier.push_back(expansion);
//                }
//                
//            }
//            
//        }
//        //increment time will not be +1 but instead be start + total pathCost
//        current_time = start_time + n.cost;
//        current_time = current_time%24;
//        
//    }
//    //if this while loop finishes then all nodes were checked and no suceess was found
//    std::cout << "None" << std::endl;
//    output << "None" << std::endl;
//    return;
//    
//    
//}

//Graph implementation

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

void Graph::add_edge(std::string start_name, std::string end_name, int length, int hours[]){
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
    
    Edge *e = new Edge(start, end, length, hours);
    edges.push_back(e);
    start->add_edge(e);
}



//Edge Implementation
Edge::Edge(Node* source, Node* dest, int length, int *hour_data):
source(source), dest(dest), length(length)
{
    for (int i = 0; i<24; i++) {
    if(hour_data[i] == 0)
            active_hours.push_back(i);
    }
    
}



//Node implementation
Node::Node(Node_type n, std::string name): type(n), name(name){
    pathCost = -1;
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
    std::sort(children.begin(), children.end(), compareNodeName);
    return children;
}

std::vector<Node*> Node::getUCSChildren(int current_cost){
    std::vector<Node*> result;
    for (int i=0; i<edges.size(); ++i) {
        //set path cost & validHours before insertion
        Node* next_node = edges[i]->dest;
        next_node->setValidHours(edges[i]->active_hours);
        next_node->pathCost = current_cost + edges[i]->length;
        result.push_back(next_node);
    }
    return result;
}

//std::vector<UCS_Node> Node::getUCSChildren(int current_cost){
//    std::vector<UCS_Node> result;
//    for (int i=0; i<edges.size(); ++i) {
//        //set path cost & validHours before insertion
//        UCS_Node next_node = UCS_Node(edges[i]->dest);
//        next_node.validHours = edges[i]->active_hours;
//        next_node.cost = current_cost + edges[i]->length;
//        result.push_back(next_node);
//    }
//    return result;
//}

int Node::getNumChildren(){
    return static_cast<int>(edges.size());
}

void Node::add_edge(Edge* e){
    edges.push_back(e);
}

void Node::setValidHours(std::vector<int>activeHours){
    validHours = activeHours;
}

bool Node::isActive(int time){
    for (int i=0; i<validHours.size(); ++i) {
        if(validHours[i] == time){
            return true;
        }
    }
    return false;
}

//UCS Node used to update copy
UCS_Node::UCS_Node(Node *node){
    name = node->name;
    type = node->type;
    cost = node->pathCost;
    validHours = node->validHours;
    origin = node;
}

bool UCS_Node::isActive(int time){
    for (int i=0; i<validHours.size(); ++i) {
        if(validHours[i] == time){
            return true;
        }
    }
    return false;
}


