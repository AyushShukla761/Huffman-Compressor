#include <bits/stdc++.h>
#include "huffman.h"
#include "Node.h"

using namespace std;
vector<vector<pair<char, int>>> v(300);

void print(node *root, int i, vector<vector<pair<char, int>>> &v)
{
    if (root == NULL)
        return;
    v[i].push_back({root->ch, root->getfreq()});
    print(root->left, i + 1, v);
    print(root->right, i + 1, v);
}

huffman ::huffman()
{
    encodingMap = new char_stringintpair_m;
    frequencies = NULL;
    datafile = NULL;
    tablefile = NULL;
}

huffman ::~huffman()
{
    delete encodingMap;
    delete frequencies;
    delete datafile;
    delete tablefile;
}

void huffman ::encode(string filename)
{

    char *data;
    int s = 0;
    stringintpair_v::iterator it;
    ifstream file(filename, ios::binary);

    file.seekg(0, ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    if (frequencies)
        delete frequencies;

    if (!file)
    {
        cout << "Could not open file: " << filename << "\n";
        file.close();
        return;
    }

    data = new char[size];
    file.read(data, size);

    frequencies = getFrequencies(data, size);
    buildTree();
    showTree();

    encodedResult = buildEncodedResult(data, size);

    createDataFile(encodedResult);
    createTableFile(*encodingMap);

    if (!writeDataFile(filename))
        return;
    if (!writeTableFile(filename))
        return;

    for (it = encodedResult.begin(); it != encodedResult.end(); it++)
        s += it->second;

    float comp = 1 - ((float)s / (float)(size * 8));
    cout << "\n\nAmount of bits in regular string: " << size * 8;
    cout << "\nAmount of bits in encoded string: " << s;
    cout << "\nCompression: " << comp * 100 << "%";
}

void huffman::decode(string filename, string tablename)
{
    readTableFile(tablename);
}

char_int_m *huffman::getFrequencies(char *data, int size)
{
    char_int_m *x = new char_int_m;
    char_int_m::iterator it;

    for (int i = 0; i < size; i++)
    {
        it = x->find(data[i]);
        if (it == x->end())
            x->insert(pair<char, int>{data[i], 1});
        else
            (*x)[data[i]]++;
    }

    return x;
}

vector<pair<string, int>> huffman::buildEncodedResult(char *data, int size)
{
    char_stringintpair_m ::iterator it;
    stringintpair_v result;

    for (int i = 0; i < size; i++)
    {
        it = encodingMap->find(data[i]);
        result.push_back(it->second);
    }

    return result;
}

void huffman::Sort(node_v &v)
{
    bool s = true;
    int n = v.size(), i;

    while (s)
    {
        s = false;
        for (i = 1; i <= n - 1; i++)
        {
            if (v[i - 1]->getfreq() > v[i]->getfreq())
            {
                swap(v[i - 1], v[i]);
                s = true;
            }
        }
        --n;
    }
}

void huffman::buildTree()
{
    node_v nodes;
    char_int_m temp = *frequencies;
    char_int_m ::iterator it, lw;

    while (!temp.empty())
    {
        it = temp.begin();
        lw = it;

        while (it != temp.end())
        {
            if (it->second < lw->second)
                lw = it;
            it++;
        }

        nodes.push_back(new node(lw->first, lw->second));
        temp.erase(lw);
    }

    while (nodes.size() != 1)
    {

        nodes.push_back(new node(nodes.at(0), nodes.at(1)));
        nodes.erase(nodes.begin(), nodes.begin() + 2);
        Sort(nodes);
    }

    tree = nodes.front();
    tree->fill(*encodingMap, "", 0);

    // print(tree,1,v);

    // for(int i=0;i<300;i++){
    //     char_stringintpair_m:: iterator it;
    //     if(!v[i].empty()){
    //         for(auto [a,b]: v[i]){

    //             it=encodingMap->find(a);
    //             if(a){
    //                 cout<<a<< " ----->>>>"<< b<<"( "<<it->second.first<<" )"<<"    ";
    //             }
    //             else cout<<b<<"    ";
    //         }
    //         cout<<endl;
    //     }
    // }
}

void huffman::showTree()
{
    char_stringintpair_m::iterator it;

    for (it = encodingMap->begin(); it != encodingMap->end(); it++)
    {
        cout << it->first << ": (" << it->second.second << ") " << it->second.first << endl;
    }
}

void huffman::createDataFile(stringintpair_v &v)
{
    stringintpair_v ::iterator it;
    int size, poff, bitpos, charpos, charbitpos;
    int bit;
    if (datafile)
        delete datafile;
    datafile = new DataFile;
    datafile->magicNumber = 0xA0;

    size = 0;

    for (it = v.begin(); it != v.end(); it++)
        size += it->second;

    size = (size + 8) / 8;

    datafile->size = size;
    datafile->data = new char[datafile->size];
    datafile->paddingOffset = 8 - (size % 8);

    charpos = 0;
    charbitpos = 7;

    for (it = v.begin(); it != v.end(); it++)
    {

        for (bitpos = it->second - 1; bitpos >= 0; bitpos--)
        {

            bit = (it->first[bitpos]) == '0' ? 0 : 1;

            if (charbitpos < 0)
            {
                charbitpos = 7;
                charpos++;
            }

            if (charbitpos == 7)
            {
                datafile->data[charpos] = 0x0;
            }

            datafile->data[charpos] = (datafile->data[charpos]) | bit << charbitpos;
            charbitpos--;
        }
    }
}

void huffman::createTableFile(char_stringintpair_m &m)
{
    if (tablefile)
        delete tablefile;
    tablefile = new TableFile;
    TableEntry temp;
    char_stringintpair_m::iterator it;

    tablefile->magicNumber = 0xA0;
    std::cout << "Writing magic number: " << std::hex << (int)tablefile->magicNumber << std::endl;

    for (it = encodingMap->begin(); it != encodingMap->end(); it++)
    {
        temp.data = it->first;
        temp.size = it->second.second;
        temp.bits = std::stoi(it->second.first, nullptr, 2);
        tablefile->data.push_back(temp);
    }
}

bool huffman::readTableFile(string fstr)
{
    char charbuff;
    char buff[4];
    TableEntry temp;

    if (tablefile)
        delete tablefile;
    tablefile = new TableFile;

    ifstream file(fstr, ios::binary);

    if (!file)
    {
        cout << "Could not open tablefile." << endl;
        file.close();
        return false;
    }

    file.read(&charbuff, 1);
    std::cout << "Read magic number: " << std::hex << (int)charbuff << std::endl;
    // if (charbuff != 0xA0) {
    // 		std::cout << "Invalid tablefile\n";
    // 		file.close();
    // 		return false;
    // }

    try
    {

        while (!file.eof())
        {
            // Read the char in question
            file.read(&charbuff, 1);
            temp.data = charbuff;
            if (file.fail())
                break;

            // Read the size (number of bits) this encoded char uses
            file.read(&charbuff, 1);
            temp.size = charbuff;
            if (file.fail())
                throw "2 Error reading tablefile";

            // Read the bits for this encoded char
            file.read((char *)&temp.bits, 4);

            // Append the new entry into the data vector
            tablefile->data.push_back(temp);
        }

        for (auto it : tablefile->data)
        {

            cout << it.data << "--->>>";
            cout << it.bits << "   " << it.size << endl;
        }
    }

    catch (const char *err)
    {
        file.close();
        std::cout << err << "\n";
        return false;
    }
    file.close();
    return true;
}

bool huffman::writeTableFile(string fstr)
{

    fstr += ".htbl";

    ofstream file(fstr, ios::binary);
    tablentry_v ::iterator it;

    if (!file)
    {
        cout << "Could nt write Tablefile for encoded data" << endl;
        return false;
    }
    else
    {

        file.write(&tablefile->magicNumber, 1);
        std::cout << "Writing magic number: " << std::hex << (int)tablefile->magicNumber << std::endl;

        for (it = tablefile->data.begin(); it != tablefile->data.end(); it++)
        {
            file.write(&it->data, 1);
            file.write(&it->size, 1);
            file.write((char *)&it->bits, 4);
        }
    }
    file.close();
    return true;
}

bool huffman::writeDataFile(string fstr)
{
    fstr += ".hdta";

    ofstream file(fstr, ios::binary);

    if (!file)
    {
        std::cout << "Could not write Datafile for encoded data\n";
        file.close();
        return false;
    }
    else
    {
        file.write(&datafile->magicNumber, 1);
        file.write((char *)&datafile->size, 4);
        file.write(&datafile->paddingOffset, 1);
        file.write((char *)datafile->data, datafile->size);
    }
    file.close();
    return true;
}

// bool readDataFile(const string& filename, DataFile& datafile) {
//     ifstream file(filename + ".htda", ios::binary);

//     if (!file) {
//         cout << "Error: Could not open file " << filename << ".htda" << endl;
//         return false;
//     }

//     // Read magic number
//     file.read(&datafile.magicNumber, 1);
//     if (datafile.magicNumber != 0xA0) {
//         cout << "Error: Invalid file format." << endl;
//         file.close();
//         return false;
//     }

//     // Read size of data
//     file.read(reinterpret_cast<char*>(&datafile.size), 4);

//     // Read padding offset
//     file.read(&datafile.paddingOffset, 1);

//     // Read actual compressed data
//     datafile.data.resize(datafile.size);
//     file.read(datafile.data.data(), datafile.size);

//     file.close();
//     return true;
// }