#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <climits>
#include <vector>



int main(int argc, char *argv[]){
    /*<Task#> Greedy=1, MiniMax=2, Alpha-Beta=3, Competition=4
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
    //std::ofstream output("output.txt");
    
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
    
    
    
    std::cout << "Hello world" << std::endl;
    
    return 0;
}