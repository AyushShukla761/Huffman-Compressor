#include <bits/stdc++.h>
#include "Node.h"


node :: node(char c, int f, NODE_TYPE t){
    type=t;
    freq=f;
    ch=c;
    left=right=NULL;
}

node :: node(node* l, node* r, NODE_TYPE t){
    type=t;
    left=l;
    right =r;
    freq= l->freq+ r->freq;
}

node:: ~node(){
    delete left;
    delete right;
}

int node :: getfreq(){
    return freq;
}


void node:: fill(map<char, pair<string,int>> &mp, string bits, int nbits){
    if(type== LEAF){
        mp[ch]= pair<string,int> {bits,nbits};
    }
    else{
        // mp[ch]= pair<int,int> {bits,nbits};
        bits+='0';
        left->fill(mp, bits, nbits + 1);

        // Recur for right subtree with '1' appended
        bits.back()='1';
        right->fill(mp, bits, nbits + 1);
    }
}