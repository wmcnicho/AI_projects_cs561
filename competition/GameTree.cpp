//
//  GameTree.cpp
//  mancala_competition
//
//  Created by Hunter McNichols on 11/9/15.
//  Copyright © 2015 hunter_mc. All rights reserved.
//

#include "GameTree.h"
#include <iostream>

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

void GameTree::addNode(TreeItem *node){
    nodes.push_back(node);
}