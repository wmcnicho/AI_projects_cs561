//
//  MancalaBoard.cpp
//  mancala_competition
//
//  Created by Hunter McNichols on 11/9/15.
//  Copyright © 2015 hunter_mc. All rights reserved.
//

#include "MancalaBoard.h"

bool MancalaBoard::checkMoveValid(int move, int player_num){
    int count = moves_available(player_num);
    int *options = next_moves(player_num);
    
    for (int i=0; i<count; ++i) {
        if(options[i] == move){
            delete options;
            return true;
        }
    }
    delete options;
    return false;
}
bool MancalaBoard::checkGameOver(int player_turn){
    if(moves_available(player_turn) == 0){
        //If this is the case then the game is over and you need to add all stones from the opposing side to their mancala
        if(player_turn == 1){
            for(int i=p2_cell_start; i <= p2_cell_end; i++){
                p2_mancala += cells[i];
                cells[i] = 0;
                return true;
            }
        }
        else if(player_turn == 2){
            for(int i=0; i <= p1_cell_end; i++){
                p1_mancala += cells[i];
                cells[i] = 0;
                return true;
            }
        }
    }
    return false;
}

std::string MancalaBoard::toString(){
    std::stringstream output;
    for (int i=p2_cell_start; i<=p2_cell_end; i++) {
        if(i == p2_cell_end){//no space and endl
            output << cells[i] << std::endl;
            
        }
        else{//print and space
            output << cells[i] << " ";
        }
    }
    for (int i=p1_cell_start; i<=p1_cell_end; i++) {
        if(i == p1_cell_end){//no space and endl
            output << cells[i] << std::endl;
        }
        else{//print and space
            output << cells[i] << " ";
        }
    }
    output << p2_mancala << std::endl;
    output << p1_mancala << std::endl;
    return output.str();
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
            // std::cout << cells[i] << std::endl;
            output << cells[i] << std::endl;
        }
        else{//print and space
            //std::cout << cells[i] << " ";
            output << cells[i] << " ";
        }
    }
    for (int i=p1_cell_start; i<=p1_cell_end; i++) {
        if(i == p1_cell_end){//no space and endl
            //std::cout << cells[i] << std::endl;
            output << cells[i] << std::endl;
        }
        else{//print and space
            //std::cout << cells[i] << " ";
            output << cells[i] << " ";
        }
    }
    //std::cout << p2_mancala << std::endl;
    output << p2_mancala << std::endl;
    //std::cout << p1_mancala << std::endl;
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
    checkGameOver(player);
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
            if(moves_available(player) > 0){
                return true;
            }
            else{
                checkGameOver(player);
            }
        }
    }
    else if(player == 2){
        if(cell >= num_cells || cell < 0){
            std::cout << "Invalid move for p2" << std::endl;
            return false;
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
                    else{
                        cells[pos]++;
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
            if(moves_available(player) > 0){
                return true;
            }
            else{
                checkGameOver(player);
            }
        }
    }
    return false;//performed action with no bonus move
}

int MancalaBoard::remaining_stones(){
    int count = 0;
    for(int i=0; i <= p2_cell_end; i++){
        if (cells[i] > 0) {
            count++;
        }
    }
    return count;
}

double MancalaBoard::cells_per_player(){
    return static_cast<double>(num_cells);
}