#ifndef NODE_H
#define NODE_H

#include <bits/stdc++.h>
using namespace std;
enum NODE_TYPE {LEAF, BIND};

class node{
    private:
        int freq;
        
        public:
        NODE_TYPE type;
        
        node* left;
        node *right;
        char ch;
        node();
        node(char c, int f, NODE_TYPE t=LEAF);
        node(node* l, node* r, NODE_TYPE t=BIND);
        ~node();

        int getfreq();
        void fill(map<char, pair<string,int>> &mp, string bits, int nbits);

};



#endif