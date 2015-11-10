//
//  GameTree.h
//  mancala_competition
//
//  Created by Hunter McNichols on 11/9/15.
//  Copyright © 2015 hunter_mc. All rights reserved.
//

#ifndef GameTree_h
#define GameTree_h

#include <stdio.h>
#include <vector>
#include "TreeItem.h"

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

#endif /* GameTree_h */
