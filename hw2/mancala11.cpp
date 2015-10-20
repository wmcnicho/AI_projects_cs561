#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <climits>
#include <vector>

class MancalaBoard;
struct TreeItem;
class GameTree;

void run_greedy(int p, std::vector<int>&p2_state, std::vector<int>&p1_state, int p2S, int p1S, std::ofstream &next_state);
void run_minimax(int p, std::vector<int>&p2_state, std::vector<int>&p1_state, int p2S, int p1S, int depth, std::ofstream &next_state, std::ofstream &log);
void run_ab(int p, std::vector<int>&p2_state, std::vector<int>&p1_state, int p2S, int p1S, int depth, std::ofstream &output, std::ofstream &log);
TreeItem *minimax_value(MancalaBoard& board, int current_depth, int max_depth, int player_num, bool is_max, TreeItem *node, int root_player, bool print, std::ofstream &log, GameTree &g);
TreeItem *minimax_value_ab(MancalaBoard& board, int current_depth, int max_depth, int player_num, bool is_max, TreeItem* cur_node, int root_player, std::ofstream &log, int alpha, int beta, GameTree &g);


int main(int argc, char *argv[]){
    /* Input file format:
     <Task#> Greedy=1, MiniMax=2, Alpha-Beta=3, Competition=4
     <Your player: 1 or 2>
     <Cutting off depth>
     <Board state for player-2> 
     <Board state for player-1> 
     <#stones in player-2’s mancala> 
     <#stones in player-1’s mancala>
     */
    
    int task_num;
    int player_num;
    int cutoff;
    //top player is player 2, bottom is player 1
    std::vector<int> top_player;
    std::vector<int> bot_player;
    int p1_score;
    int p2_score;
    
    if(argc < 2){
        std::cout << "Fail, not enough args" << std::endl;
        return 1;
    }
    
    std::ifstream input_file(argv[2]);
    //std::ifstream input_file("myTests.txt");
    if(!input_file.is_open()){
        std::cout << "File not found" << std::endl;
        return 1;
    }
    //Output stream
    std::ofstream next_state("next_state.txt");
    input_file >> task_num;
    input_file.ignore(INT_MAX, '\n');
    input_file >> player_num;
    input_file.ignore(INT_MAX, '\n');
    input_file >> cutoff;
    input_file.ignore(INT_MAX, '\n');

    std::string line;
    int count = 0;
    if(std::getline(input_file, line)){
        std::istringstream iss(line);
        int stones;
        while(iss >> stones){
            top_player.push_back(stones);
            count++;
        }
    }
    
    if(std::getline(input_file, line)){
        std::istringstream iss(line);
        int stones;
        while(iss >> stones){
            bot_player.push_back(stones);
        }
    }
    
    input_file >> p2_score;
    input_file.ignore(INT_MAX, '\n');
    input_file >> p1_score;
    input_file.ignore(INT_MAX, '\n');
    
    if(task_num == 1){
        run_greedy(player_num, top_player, bot_player, p2_score, p1_score, next_state);
    }
    else if(task_num == 2){
        std::ofstream log("traverse_log.txt");
        run_minimax(player_num, top_player, bot_player, p2_score, p1_score, cutoff, next_state, log);
    }
    else if(task_num == 3){
        std::ofstream log("traverse_log.txt");
        run_ab(player_num, top_player, bot_player, p2_score, p1_score, cutoff, next_state, log);
    }
    else if (task_num == 4){
        std::cout << "Haven't written competition code yet" << std::endl;
    }
    else{
        std::cout << "Invalid task code" << std::endl;
    }
    
    
    return 0;
}

//Data structure headers
struct TreeItem{
    bool max;//true=MAX false=MIN
    TreeItem *parent;
    TreeItem *next_move;//NULL if last in a series of moves
    std::string move_name;
    std::vector<TreeItem *> children;
    int value;
    int player_num;
    int move_index;
    int depth;
    bool bonus;
    
    TreeItem(bool type, int value, int player_num, int cell_num, int depth);
    TreeItem(std::string name);//used for root node
    
    void print_to_log(std::ofstream &log, bool print);//for minimax false if greedy
    void print_to_log(std::ofstream &log, int alpha, int beta);//used in alpha-beta
};

class GameTree{
public:
    GameTree();
    ~GameTree();
    bool empty();
    int size();
    void addChild(TreeItem *parent, TreeItem *child);
    void addNode(TreeItem *node);
private:
    TreeItem *root;
    std::vector<TreeItem *> nodes;
};

//Game representation
class MancalaBoard{
    /**Represents the board of the game. Does this all through a single array and an interaction interface**/
public:
    MancalaBoard(std::vector<int>&p2_state, std::vector<int>&p1_state, int p2S, int p1S);
    ~MancalaBoard();//delete cells
    //copy constructor
    MancalaBoard(const MancalaBoard& source);
    //assignment operator
    MancalaBoard& operator=(const MancalaBoard& source);
    
    bool make_move(int player_num, int cell_num);//true if there's a bonus move
    
    int moves_available(int player_num);
    
    int util_value(int player_turn, int root_player);
    
    //returns an array of the indicies of the next valid moves for the player, BE SURE TO DELETE THIS
    int *next_moves(int player_num);
    
    void print();
    void print_to_file(std::ofstream &output);
private:
    int *cells;
    int p1_mancala;
    int p2_mancala;
    int p1_cell_start;
    int p2_cell_start;
    int p1_cell_end;
    int p2_cell_end;
    int num_cells;//number of cells for each player
    
    
};


//Algorithms
void run_greedy(int p, std::vector<int>&p2_state, std::vector<int>&p1_state, int p2S, int p1S, std::ofstream &next_state){
    run_minimax(p, p2_state, p1_state, p2S, p1S, 1, next_state, next_state);
}

void run_minimax(int p, std::vector<int>&p2_state, std::vector<int>&p1_state, int p2S, int p1S, int depth, std::ofstream &next_state, std::ofstream &log){
    bool print = true;
    if(&next_state == &log){
        print = false;//greedy scenario
    }
    else{
        std::cout << "Node,Depth,Value" << std::endl;
        log << "Node,Depth,Value" << std::endl;
    }
    //TODO clean up all the TreeItem memory
    GameTree tree = GameTree();
    TreeItem *root = new TreeItem("root");
    tree.addChild(NULL, root);
    tree.addNode(root);

    TreeItem *best_move = root;
    MancalaBoard initialBoard = MancalaBoard(p2_state, p1_state, p2S, p1S);
    
    int *next_moves = initialBoard.next_moves(p);
    int count = initialBoard.moves_available(p);
    root->print_to_log(log, print);
    for (int i=0; i < count; i++) {
        MancalaBoard traverseBoard = initialBoard;
        bool bonus = traverseBoard.make_move(p, next_moves[i]);
        int cur_depth = 1;
        bool is_max = bonus;
        int next_player = p;
        if(!bonus){
            if(p == 1){
                next_player = 2;
            }
            else{
                next_player = 1;
            }
        }
        TreeItem *next_node;
        if(is_max){
            next_node = new TreeItem(is_max, INT32_MIN, p, next_moves[i], cur_depth);
            tree.addNode(next_node);
        }
        else{
            next_node = new TreeItem(is_max, INT32_MAX, p, next_moves[i], cur_depth);
            tree.addNode(next_node);
        }
        next_node->bonus = bonus;
        
        TreeItem *best_child = minimax_value(traverseBoard, cur_depth, depth, next_player, is_max, next_node, p, print, log, tree);
        if(bonus){
            next_node->next_move = best_child;
        }
        next_node->value = best_child->value;
        if(next_node->value > best_move->value){
            best_move = next_node;
            root->value = best_move->value;
        }
        root->print_to_log(log, print);
    }
    delete [] next_moves;
    //at this point best_move should correspond to the TreeItem of the best move
    
    //Make the series of moves to update the board to the new state
    initialBoard.make_move(best_move->player_num, best_move->move_index);
    TreeItem *move = best_move->next_move;
    while(move != NULL){
        initialBoard.make_move(move->player_num, move->move_index);
        move = move->next_move;
    }
    
    initialBoard.print_to_file(next_state);
    
    
    //delete best_move;
    //Clear up memory made by the tree!
}

TreeItem *minimax_value(MancalaBoard& board, int current_depth, int max_depth, int player_num, bool is_max, TreeItem* cur_node, int root_player, bool print, std::ofstream &log, GameTree &g){
    //if terminal test/ depth test
    if (!cur_node->bonus && (board.moves_available(player_num) == 0 || current_depth >= max_depth) ){
        cur_node->value = board.util_value(player_num, root_player);
        cur_node->print_to_log(log, print);
        return cur_node;
    }
    //else if MAX's move
    else if(is_max){
        //return highest minimax_value of successors
        cur_node->print_to_log(log, print);
        TreeItem *max_child_node = NULL;
        int *next_moves = board.next_moves(player_num);
        int count = board.moves_available(player_num);
        for (int i=0; i<count; i++) {
            MancalaBoard nextBoard = MancalaBoard(board);
            bool bonus = nextBoard.make_move(player_num, next_moves[i]);
            int depth = cur_node->bonus ? current_depth : current_depth + 1;
            bool is_max = bonus;//stays true if a bonus move
            int next_player = player_num;
            if(!bonus){
                if(player_num == 1){
                    next_player = 2;
                }
                else{
                    next_player = 1;
                }
            }
            
            TreeItem *next_node;
            if(is_max){
                next_node = new TreeItem(is_max, INT32_MIN, player_num, next_moves[i], depth);
                g.addNode(next_node);
            }
            else{
                next_node = new TreeItem(is_max, INT32_MAX, player_num, next_moves[i], depth);
                g.addNode(next_node);
            }
            next_node->bonus = bonus;
            
            
            TreeItem *best_child = minimax_value(nextBoard, depth, max_depth, next_player, is_max, next_node, root_player, print, log, g);
            if(bonus){//if it's a part of a string of moves
                next_node->next_move = best_child;
            }
            
            if(!max_child_node){
                max_child_node = next_node;
                cur_node->value = max_child_node->value;
            }
            else if(next_node->value > max_child_node->value){
                max_child_node = next_node;
                cur_node->value = max_child_node->value;
            }
            cur_node->print_to_log(log, print);
        }
        delete [] next_moves;
        return max_child_node;
    }
    //else (i.e. MIN's move)
    else{
        //return lowest minimax_value of successors
        cur_node->print_to_log(log, print);
        TreeItem *min_child_node = NULL;
        int *next_moves = board.next_moves(player_num);
        int count = board.moves_available(player_num);
        for (int i=0; i<count; i++) {
            MancalaBoard nextBoard = MancalaBoard(board);
            bool bonus = nextBoard.make_move(player_num, next_moves[i]);
            int depth = cur_node->bonus ? current_depth : current_depth + 1;
            bool is_max = !bonus;//MIN && bonus -> false,  MIN && !bonus -> true
            int next_player = player_num;
            if(!bonus){
                if(player_num == 1){
                    next_player = 2;
                }
                else{
                    next_player = 1;
                }
            }
            
            TreeItem *next_node;
            if(is_max){
                next_node = new TreeItem(is_max, INT32_MIN, player_num, next_moves[i], depth);
                g.addNode(next_node);
            }
            else{
                next_node = new TreeItem(is_max, INT32_MAX, player_num, next_moves[i], depth);
                g.addNode(next_node);
            }
            next_node->bonus = bonus;
            
            //next_node->print_to_log(log, print);
            TreeItem *best_child = minimax_value(nextBoard, depth, max_depth, next_player, is_max, next_node, root_player, print, log, g);
            if(bonus){//if it's a part of a string of moves
                next_node->next_move = best_child;
            }
            next_node->value = best_child->value;
            if(!min_child_node){
                min_child_node = next_node;
                cur_node->value = min_child_node->value;
            }
            else if(next_node->value < min_child_node->value){
                min_child_node = next_node;
                cur_node->value = min_child_node->value;
            }
            
           cur_node -> print_to_log(log, print);
        }
        delete [] next_moves;
        return min_child_node;
    }
}
void run_ab(int p, std::vector<int>&p2_state, std::vector<int>&p1_state, int p2S, int p1S, int depth, std::ofstream &next_state, std::ofstream &log){
    
    std::cout << "Node,Depth,Value,Alpha,Beta" << std::endl;
    log << "Node,Depth,Value,Alpha,Beta" << std::endl;

    //TODO clean up all the TreeItem memory
    GameTree tree = GameTree();
    TreeItem *root = new TreeItem("root");
    tree.addChild(NULL, root);
    tree.addNode(root);
    
    TreeItem *best_move = root;
    MancalaBoard initialBoard = MancalaBoard(p2_state, p1_state, p2S, p1S);
    
    int *next_moves = initialBoard.next_moves(p);
    int count = initialBoard.moves_available(p);
    int alpha = INT32_MIN;
    int beta = INT32_MAX;
    root->print_to_log(log, alpha, beta);
    for (int i=0; i < count; i++) {
        MancalaBoard traverseBoard = initialBoard;
        bool bonus = traverseBoard.make_move(p, next_moves[i]);
        int cur_depth = 1;
        bool is_max = bonus;
        int next_player = p;
        if(!bonus){
            if(p == 1){
                next_player = 2;
            }
            else{
                next_player = 1;
            }
        }
        TreeItem *next_node;
        if(is_max){
            next_node = new TreeItem(is_max, INT32_MIN, p, next_moves[i], cur_depth);
            tree.addNode(next_node);
        }
        else{
            next_node = new TreeItem(is_max, INT32_MAX, p, next_moves[i], cur_depth);
            tree.addNode(next_node);
        }
        next_node->bonus = bonus;
        TreeItem *best_child = minimax_value_ab(traverseBoard, cur_depth, depth, next_player, is_max, next_node, p, log, alpha, beta, tree);
        if(bonus){//if it's a part of a string of moves, needs fixing
            next_node->next_move = best_child;
        }
        next_node->value = best_child->value;
        if(next_node->value > best_move->value){
            best_move = next_node;
            root->value = best_move->value;
        }
        if(best_move->value >= beta){
            break;//found! this should never hit
        }
        if(best_move->value > alpha){
            alpha = best_move->value;
        }
        
        root->print_to_log(log, alpha, beta);
    }
    delete [] next_moves;
    //at this point best_move should correspond to the TreeItem of the best move
    
    //Make the series of moves to update the board to the new state
    initialBoard.make_move(best_move->player_num, best_move->move_index);
    TreeItem *move = best_move->next_move;
    while(move != NULL){
        initialBoard.make_move(move->player_num, move->move_index);
        move = move->next_move;
    }
    
    initialBoard.print_to_file(next_state);
}

TreeItem *minimax_value_ab(MancalaBoard& board, int current_depth, int max_depth, int player_num, bool is_max, TreeItem* cur_node, int root_player, std::ofstream &log, int alpha, int beta, GameTree &g){
    if (!cur_node->bonus && (board.moves_available(player_num) == 0 || current_depth >= max_depth) ){
        cur_node->value = board.util_value(player_num, root_player);
        cur_node->print_to_log(log, alpha, beta);
        return cur_node;
    }
    //else if MAX's move
    else if(is_max){
        //return highest minimax_value of successors
        cur_node->print_to_log(log, alpha, beta);
        TreeItem *max_child_node = NULL;
        int *next_moves = board.next_moves(player_num);
        int count = board.moves_available(player_num);
        for (int i=0; i<count; i++) {
            MancalaBoard nextBoard = MancalaBoard(board);
            bool bonus = nextBoard.make_move(player_num, next_moves[i]);
            int depth = cur_node->bonus ? current_depth : current_depth + 1;
            bool is_max = bonus;//stays true if a bonus move
            int next_player = player_num;
            if(!bonus){
                if(player_num == 1){
                    next_player = 2;
                }
                else{
                    next_player = 1;
                }
            }
            
            TreeItem *next_node;
            if(is_max){
                next_node = new TreeItem(is_max, INT32_MIN, player_num, next_moves[i], depth);
                g.addNode(next_node);
            }
            else{
                next_node = new TreeItem(is_max, INT32_MAX, player_num, next_moves[i], depth);
                g.addNode(next_node);
            }
            next_node->bonus = bonus;
            
            
            TreeItem *best_child = minimax_value_ab(nextBoard, depth, max_depth, next_player, is_max, next_node, root_player, log, alpha, beta, g);
            if(bonus){//if it's a part of a string of moves
                next_node->next_move = best_child;
            }
            
            if(!max_child_node){
                max_child_node = next_node;
                cur_node->value = max_child_node->value;
            }
            else if(next_node->value > max_child_node->value){
                max_child_node = next_node;
                cur_node->value = max_child_node->value;
            }
            if(max_child_node->value >= beta){
                cur_node->print_to_log(log, alpha, beta);
                delete [] next_moves;
                return max_child_node;
            }
            if(max_child_node->value > alpha){
                alpha = max_child_node->value;
            }
            cur_node->print_to_log(log, alpha, beta);
        }
        delete [] next_moves;
        return max_child_node;
    }
    //else (i.e. MIN's move)
    else{
        //return lowest minimax_value of successors
        cur_node->print_to_log(log, alpha, beta);
        TreeItem *min_child_node = NULL;
        int *next_moves = board.next_moves(player_num);
        int count = board.moves_available(player_num);
        for (int i=0; i<count; i++) {
            MancalaBoard nextBoard = MancalaBoard(board);
            bool bonus = nextBoard.make_move(player_num, next_moves[i]);
            int depth = cur_node->bonus ? current_depth : current_depth + 1;
            bool is_max = !bonus;//MIN && bonus -> false,  MIN && !bonus -> true
            int next_player = player_num;
            if(!bonus){
                if(player_num == 1){
                    next_player = 2;
                }
                else{
                    next_player = 1;
                }
            }
            
            TreeItem *next_node;
            if(is_max){
                next_node = new TreeItem(is_max, INT32_MIN, player_num, next_moves[i], depth);
                g.addNode(next_node);
            }
            else{
                next_node = new TreeItem(is_max, INT32_MAX, player_num, next_moves[i], depth);
                g.addNode(next_node);
            }
            next_node->bonus = bonus;
            
            //next_node->print_to_log(log, print);
            TreeItem *best_child = minimax_value_ab(nextBoard, depth, max_depth, next_player, is_max, next_node, root_player, log, alpha, beta, g);
            if(bonus){//if it's a part of a string of moves
                next_node->next_move = best_child;
            }
            next_node->value = best_child->value;
            if(!min_child_node){
                min_child_node = next_node;
                cur_node->value = min_child_node->value;
            }
            else if(next_node->value < min_child_node->value){
                min_child_node = next_node;
                cur_node->value = min_child_node->value;
            }
            
            if(min_child_node->value <= alpha){
                cur_node -> print_to_log(log, alpha, beta);
                delete [] next_moves;
                return min_child_node;
            }
            if(min_child_node->value < beta){
                beta = min_child_node->value;
            }
            cur_node -> print_to_log(log, alpha, beta);
        }
        delete [] next_moves;
        return min_child_node;
    }
}



//DataStructure implementations, is this even needed?
void GameTree::addNode(TreeItem *node){
    nodes.push_back(node);
}

void TreeItem::print_to_log(std::ofstream &log, int alpha, int beta){
    std::string s_alpha;
    std::string s_beta;
    if(alpha == INT32_MAX){
        s_alpha = "Infinity";
    }
    else if(alpha == INT32_MIN){
        s_alpha = "-Infinity";
    }
    else{
        std::ostringstream s;
        s << alpha;
        std::string convert(s.str());
        s_alpha = convert;
    }
    if(beta == INT32_MAX){
        s_beta = "Infinity";
    }
    else if(beta == INT32_MIN){
        s_beta = "-Infinity";
    }
    else{
        std::ostringstream s;
        s << beta;
        std::string convert(s.str());
        s_beta = convert;
    }
    
    if(value == INT32_MAX){
        log << move_name << "," << depth << ",Infinity," << s_alpha << "," << s_beta << std::endl;
        std::cout << move_name << "," << depth << ",Infinity," << s_alpha << "," << s_beta << std::endl;
    }
    else if(value == INT32_MIN){
        log << move_name << "," << depth << ",-Infinity," << s_alpha << "," << s_beta << std::endl;
        std::cout << move_name << "," << depth << ",-Infinity," << s_alpha << "," << s_beta << std::endl;
    }
    else{
        log << move_name << "," << depth << "," << value << "," << s_alpha << "," << s_beta << std::endl;
        std::cout << move_name << "," << depth << "," << value << "," << s_alpha << "," << s_beta << std::endl;
    }
}

void TreeItem::print_to_log(std::ofstream &log, bool print){
    if(!print){
        return;
    }
    if(value == INT32_MAX){
        log << move_name << "," << depth << ",Infinity" << std::endl;
        std::cout << move_name << "," << depth << ",Infinity" << std::endl;
    }
    else if(value == INT32_MIN){
        log << move_name << "," << depth << ",-Infinity" << std::endl;
        std::cout << move_name << "," << depth << ",-Infinity" << std::endl;
    }
    else{
        log << move_name << "," << depth << "," << value << std::endl;
        std::cout << move_name << "," << depth << "," << value << std::endl;
    }
}

TreeItem::TreeItem(std::string name) : move_name(name){
    value = INT32_MIN;
    max = true;//Root is always a max
    next_move = NULL;
    player_num = 0;
    move_index = 0;
    depth = 0;//root
    bonus = false;
}

TreeItem::TreeItem(bool type, int value, int player_num, int cell_num, int depth):max(type), value(value), player_num(player_num), move_index(cell_num), depth(depth){
    next_move = NULL;
    if(player_num==1){
        std::ostringstream s;
        s << "B" << (cell_num + 2);
        std::string convert(s.str());
        move_name = convert;
    }
    else if(player_num==2){
        std::ostringstream s;
        s << "A" << (cell_num + 2);
        std::string convert(s.str());
        move_name = convert;
    }
    bonus = false;
}

GameTree::GameTree(){
    root = NULL;
}

GameTree::~GameTree(){
    for (int i=0; i<nodes.size(); i++) {
        delete nodes[i];
    }
}
bool GameTree::empty(){
    return !root;
}
void GameTree::addChild(TreeItem *parent, TreeItem* child){
    if(!root && !parent){//empty tree case call addChild(NULL, new_root);
        root = child;
        return;
    }
    if(root && !parent){
        std::cout << "Error please specify a parent node" << std::endl;
        return;
    }
    if(!child){
        std::cout << "No child specified" << std::endl;
        return;
    }
    child->parent = parent;
    parent->children.push_back(child);
}

void MancalaBoard::print(){
    for (int i=p2_cell_start; i<=p2_cell_end; i++) {
        if(i == p2_cell_end){//no space and endl
            std::cout << cells[i] << std::endl;
        }
        else{//print and space
            std::cout << cells[i] << " ";
        }
    }
    for (int i=p1_cell_start; i<=p1_cell_end; i++) {
        if(i == p1_cell_end){//no space and endl
            std::cout << cells[i] << std::endl;
        }
        else{//print and space
            std::cout << cells[i] << " ";
        }
    }
    std::cout << p2_mancala << std::endl;
    std::cout << p1_mancala << std::endl;
}
void MancalaBoard::print_to_file(std::ofstream &output){
    for (int i=p2_cell_start; i<=p2_cell_end; i++) {
        if(i == p2_cell_end){//no space and endl
            std::cout << cells[i] << std::endl;
            output << cells[i] << std::endl;
        }
        else{//print and space
            std::cout << cells[i] << " ";
            output << cells[i] << " ";
        }
    }
    for (int i=p1_cell_start; i<=p1_cell_end; i++) {
        if(i == p1_cell_end){//no space and endl
            std::cout << cells[i] << std::endl;
            output << cells[i] << std::endl;
        }
        else{//print and space
            std::cout << cells[i] << " ";
            output << cells[i] << " ";
        }
    }
    std::cout << p2_mancala << std::endl;
    output << p2_mancala << std::endl;
    std::cout << p1_mancala << std::endl;
    output << p1_mancala << std::endl;
}

int* MancalaBoard::next_moves(int player_num){
    int count = moves_available(player_num);
    int *result = new int[count];
    if(player_num == 1){
        int j = 0;
        for(int i=0; i <= p1_cell_end; i++){
            if(cells[i] != 0){
                result[j] = i;
                j++;
            }
        }
    }
    else if(player_num == 2){
        int j = 0;
        for(int i=p2_cell_start; i <= p2_cell_end; i++){
            if(cells[i] != 0){
                result[j] = i - num_cells;
                j++;
            }
        }
    }
    return result;
}

int MancalaBoard::util_value(int player_turn, int root_player){
    if(moves_available(player_turn) == 0){
        //If this is the case then the game is over and you need to add all stones from the opposing side to their mancala
        if(player_turn == 1){
            for(int i=p2_cell_start; i <= p2_cell_end; i++){
                p2_mancala += cells[i];
                cells[i] = 0;
            }
        }
        else if(player_turn == 2){
            for(int i=0; i <= p1_cell_end; i++){
                p1_mancala += cells[i];
                cells[i] = 0;
            }
        }
    }
    
    if(root_player == 1){
        return p1_mancala - p2_mancala;
    }
    else{
        return p2_mancala - p1_mancala;
    }
    
}

int MancalaBoard::moves_available(int player_num){
    int count = 0;
    if(player_num == 1){
        for(int i=0; i <= p1_cell_end; i++){
            if(cells[i] != 0)
                count++;
        }
    }
    else if(player_num == 2){
        for(int i=p2_cell_start; i <= p2_cell_end; i++){
            if(cells[i] != 0)
                count++;
        }
    }
    return count;
}

MancalaBoard& MancalaBoard::operator=(const MancalaBoard& source){
    if(this == &source){//self pointer check
        return *this;
    }
    if(cells){
        delete [] cells;
    }
    //shallow copy stuff
    p1_mancala = source.p1_mancala;
    p2_mancala = source.p2_mancala;
    p1_cell_start = source.p1_cell_start;
    p2_cell_start = source.p2_cell_start;
    p1_cell_end = source.p1_cell_end;
    p2_cell_end = source.p2_cell_end;
    num_cells = source.num_cells;
    if(source.cells){//deep copy if cells exists which it should
        cells = new int[num_cells*2];
        for (int i=0; i < num_cells*2; i++) {
            cells[i] = source.cells[i];
        }
    }
    else{
        cells = NULL;
        std::cout << "Something went wrong in the copy" << std::endl;
    }
    return *this;
}

MancalaBoard::MancalaBoard(const MancalaBoard& source){
    p1_mancala = source.p1_mancala;
    p2_mancala = source.p2_mancala;
    p1_cell_start = source.p1_cell_start;
    p2_cell_start = source.p2_cell_start;
    p1_cell_end = source.p1_cell_end;
    p2_cell_end = source.p2_cell_end;
    num_cells = source.num_cells;

    if(source.cells){//deep copy if cells exists which it should
        cells = new int[num_cells*2];
        for (int i=0; i < num_cells*2; i++) {
            cells[i] = source.cells[i];
        }
    }
    else{
        cells = NULL;
        std::cout << "Something went wrong in the copy" << std::endl;
    }
}

MancalaBoard::MancalaBoard(std::vector<int>&p2_state, std::vector<int>&p1_state, int p2S, int p1S){
    int size = (int)p2_state.size() * 2;
    num_cells = (int)p1_state.size();
    cells = new int[size];
    p1_cell_start = 0;
    for (int i = 0; i < p1_state.size(); ++i) {
        cells[i] = p1_state[i];
    }
    p1_cell_end = num_cells - 1;
    p2_cell_start = num_cells;
    for (int i = num_cells; i < p2_state.size()*2; ++i) {
        cells[i] = p2_state[i - num_cells];
    }
    p2_cell_end = 2*num_cells -1;
    
    p1_mancala = p1S;
    p2_mancala = p2S;
}

MancalaBoard::~MancalaBoard(){
    delete[] cells;
}

//Enocdes rules of the game, pretty crazy function
//excepts a 0-indexed move relative to the player's cells from left to right
bool MancalaBoard::make_move(int player, int cell){
    if(player == 1){
        if(cell > p1_cell_end || cell < 0){
            std::cout << "Invalid move for p1" << std::endl;
            return false;
        }
        
        int num_moves = cells[cell];
        cells[cell] = 0;
        int pos = cell; //current position during loop
        while(num_moves > 0){
            if(pos < p1_cell_end){//iterating through own player's cells just move to next cell
                pos++;
                if(num_moves == 1){//ending on own side (check if cell is empty)
                    if(cells[pos] == 0){
                        p1_mancala += cells[pos + num_cells];
                        cells[pos + num_cells] = 0;
                        cells[pos] = 0;
                        p1_mancala ++;
                    }
                    else{
                        cells[pos]++;
                    }
                }
                else{
                    cells[pos]++;
                }
            }
            else if(pos == p1_cell_end){//at the end of player's row, add to mancala and begin iterating backwards
                p1_mancala++;
                pos = p2_cell_end + 1;
            }
            else{//at other player's side add to their cell and iterate
                if(pos == p2_cell_start){//reset to bottom cell
                    pos = p1_cell_start;
                    if(num_moves == 1){//ending on own side (check if cell is empty)
                        if(cells[pos] == 0){
                            p1_mancala += cells[pos + num_cells];
                            cells[pos + num_cells] = 0;
                            cells[pos] = 0;
                            p1_mancala ++;
                        }
                        else{
                            cells[pos]++;
                        }

                    }
                    else{
                        cells[pos]++;
                    }
                }
                else{
                    pos--;
                    cells[pos]++;
                }
            }
            num_moves--;
        }
        if(pos == p2_cell_end + 1)//bonus move!
        {
            return true;
        }
    }
    else if(player == 2){
        if(cell >= num_cells || cell < 0){
            std::cout << "Invalid move for p2" << std::endl;
        }
        
        cell = cell + num_cells;//convert to upper cell location in internal state representation
        int num_moves = cells[cell];
        cells[cell] = 0;
        int pos = cell;//current position during iteration
        //A move involves placing a stone into the next cell and updating position accordingly
        while(num_moves > 0){
            if(pos > p2_cell_start){//iterating through player's own cells (CCW => negative)
                pos--;
                if(num_moves == 1){
                    if(cells[pos] == 0){//ending in void cell on own side
                        p2_mancala += cells[pos - num_cells];
                        cells[pos - num_cells] = 0;
                        cells[pos] = 0;
                        p2_mancala++;
                    }
                }
                else{
                    cells[pos]++;
                }
            }
            else if(pos == p2_cell_start){//at the end of the player's row, add to mancala and begin iterating forwards
                p2_mancala++;
                pos = p1_cell_start - 1;
            }
            else{//on p1's side increment forwards
                if(pos == p1_cell_end){//reset to top cell
                    pos = p2_cell_end;
                    if(num_moves == 1){
                        if(cells[pos] == 0){
                            p2_mancala += cells[pos - num_cells];
                            cells[pos - num_cells] = 0;
                            cells[pos] = 0;
                            p2_mancala++;
                        }
                        else{
                            cells[pos]++;
                        }
                    }
                    else{
                        cells[pos]++;
                    }
                }
                else{
                    pos++;
                    cells[pos]++;
                }
            }
            num_moves--;
        }
        if(pos == p1_cell_start - 1){//bonus move!
            return true;
        }
    }
    return false;//performed action with no bonus move
}
