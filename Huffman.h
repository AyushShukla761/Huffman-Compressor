#ifndef HUUFMAN_H
#define HUFFMAN_H

#include <bits/stdc++.h>
#include "Node.h"
using namespace std;


struct DataFile{
    char  magicNumber;
    int   size;
    char  paddingOffset;
    char* data;
};
struct TableEntry{
    char data;
    char size;
    int  bits;
};
struct TableFile{
    char magicNumber;
    vector<TableEntry> data;
};

typedef vector<node*>          node_v;
typedef vector<int>            int_v;
typedef pair<int, int>         intpair;
typedef pair<string, int>      stringintpair;
typedef vector<intpair>        intpair_v;
typedef vector<stringintpair>  stringintpair_v;
typedef map<char, intpair>     char_intpair_m;
typedef map<char, stringintpair>     char_stringintpair_m;
typedef map<char, int>         char_int_m;
typedef vector<TableEntry>     tablentry_v;

class huffman{
    private:

    node*           tree;
    DataFile*       datafile;
    TableFile*      tablefile;
    std::string     decodedResult;
    stringintpair_v       encodedResult;
    char_stringintpair_m* encodingMap;
    char_int_m*     frequencies;

    char_int_m* getFrequencies(char* data, int size);
    stringintpair_v   buildEncodedResult(char* data, int size);
    
    void createDataFile(stringintpair_v& v);
    void createTableFile(char_stringintpair_m& m);
    
    bool readDataFile(string fstr);
    bool readTableFile(string fstr);
    
    bool writeTableFile(string fstr);
    bool writeDataFile(string fstr);
    
    void Sort(node_v& v);
    void buildTree();
    void showTree();
    void showIntVector(int_v& v);


    public:

    huffman();
    ~huffman();
    void encode(string filename);
    void decode(string data_filename, string table_filename);
};

#endif