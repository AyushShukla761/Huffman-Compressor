#include <bits/stdc++.h>
#include <filesystem>
#include "huffman.h"
#include "Node.h"

using namespace std;
namespace fs = std::filesystem;
vector<vector<pair<char, int>>> v(300);

void print(node *root, int i)
{
    if (root->type == LEAF)
    {
        cout << root->ch << endl;
        return;
    }
    print(root->left, i + 1);
    print(root->right, i + 1);
}

void buildTreeFromMap(node *&root, const map<char, pair<string, int>> &mp)
{

    for (const auto &entry : mp)
    {
        char ch = entry.first;
        string bits = entry.second.first;

        node *curr = root;

        for (char bit : bits)
        {
            if (bit == '0')
            {
                if (!curr->left)
                    curr->left = new node();
                curr = curr->left;
            }
            else
            {
                if (!curr->right)
                    curr->right = new node();
                curr = curr->right;
            }
        }

        curr->type = LEAF;
        curr->ch = ch;
    }
}

std::string bitsToString(uint32_t bits, int bit_length)
{
    std::string s;
    for (int i = bit_length - 1; i >= 0; i--)
    {
        s += ((bits >> i) & 1) ? '1' : '0';
    }
    return s;
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
    // showTree();
    // print(tree,0);

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
    cout << "\nAmount of bits in regular file: " << size * 8;
    cout << "\nAmount of bits in encoded file: " << s;
    cout << "\nCompression: " << comp * 100 << "%"<<endl;
}

void huffman::decode(string filename, string tablename)
{

    if (!readTableFile(tablename))
    {
        cout << "Failed to read table file: " << tablename << endl;
        return;
    }

    node *root = new node();
    buildTreeFromMap(root, *encodingMap);

    tree = root;

    if (!readDataFile(filename))
    {
        cout << "Failed to read compressed file: " << filename << endl;
    }
    else
    {
        cout << "File Decompressed" << endl;
    }
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
}

void huffman::showTree()
{
    char_stringintpair_m::iterator it;

    for (it = encodingMap->begin(); it != encodingMap->end(); it++)
    {
        cout << it->first << ": (" << it->second.second << ") " << it->second.first << endl;
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
    // std::cout << "Writing magic number: " << std::hex << (int)tablefile->magicNumber << std::endl;

    for (it = encodingMap->begin(); it != encodingMap->end(); it++)
    {
        temp.data = it->first;
        temp.size = it->second.second;
        temp.bits = std::stoi(it->second.first, nullptr, 2);
        tablefile->data.push_back(temp);
    }
}

void huffman::createDataFile(stringintpair_v &v)
{
    if (datafile)
        delete datafile;
    datafile = new DataFile;
    datafile->magicNumber = 0xA0;

    // Calculate total bits to encode
    int total_bits = 0;
    for (auto &it : v)
        total_bits += it.second;

    int byte_size = (total_bits + 7) / 8;
    datafile->size = byte_size;
    datafile->data = new char[byte_size];
    memset(datafile->data, 0, byte_size);

    datafile->paddingOffset = (8 - (total_bits % 8)) % 8;

    int bit_pos = 0; // Total bits written

    for (auto &it : v)
    {
        const string &bits = it.first;
        int nbits = it.second;
        for (int i = 0; i < nbits; i++)
        {
            int bit = (bits[i] == '1') ? 1 : 0;
            int byte_idx = bit_pos / 8;
            int bit_idx = 7 - (bit_pos % 8);
            if (bit)
            {
                datafile->data[byte_idx] |= (1 << bit_idx);
            }
            bit_pos++;
        }
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
    }

    catch (const char *err)
    {
        file.close();
        std::cout << err << "\n";
        return false;
    }
    encodingMap->clear();
    for (auto Entry : tablefile->data)
    {
        int x = Entry.size;
        std::string s = bitsToString(Entry.bits, x);
        (*encodingMap)[Entry.data] = {s, Entry.size};
    }

    file.close();
    return true;
}

bool huffman::readDataFile(string filename)
{
    if (datafile)
    {
        delete datafile;
    }

    datafile = new DataFile;

    ifstream dataFile(filename, ios::binary);
    if (!dataFile)
    {
        cout << "Could not open data file: " << filename << endl;
        return false;
    }

    dataFile.seekg(0, ios::end);
    int file_size = dataFile.tellg();
    dataFile.seekg(0, ios::beg);

    unsigned char magic;
    dataFile.read((char *)&magic, 1);
    if (magic != 0xA0)
    {
        cout << "Invalid data file format." << endl;
        dataFile.close();
        return false;
    }

    unsigned int dataSize;
    dataFile.read(reinterpret_cast<char *>(&dataSize), 4);
    datafile->size = dataSize;

    char paddingOffset;
    dataFile.read(&paddingOffset, 1);
    datafile->paddingOffset = paddingOffset;

    char *compressedData = new char[dataSize];
    dataFile.read(compressedData, dataSize);
    datafile->data = compressedData;
    dataFile.close();

    if (!datafile || !tree)
    {
        cout << "Datafile or Huffman tree missing.\n";
        return false;
    }

    string decoded = "";
    node *current = tree;
    int total_bits = datafile->size * 8 - datafile->paddingOffset;
    int bit_pos = 0;
    int symbols_decoded = 0;

    while (bit_pos < total_bits)
    {
        int byte_idx = bit_pos / 8;
        int bit_idx = 7 - (bit_pos % 8);
        char current_byte = datafile->data[byte_idx];
        int bit = (current_byte >> bit_idx) & 1;

        if (bit == 0)
        {
            if (current->left)
                current = current->left;
        }
        else
        {
            if (current->right)
                current = current->right;
        }

        if (current->type == LEAF)
        {
            decoded += current->ch;
            symbols_decoded++;
            current = tree;
        }
        bit_pos++;
    }

    fs::path inPath(filename);

    // Step 2: Extract parent directory, stem, and construct new filename
    fs::path parentDir = inPath.parent_path(); // "uploads"
    std::string stem = inPath.stem().string(); // "test"

    fs::path outputPath = parentDir / ("decompressed_" + stem);    
    
    ofstream outputFile(outputPath, ios::binary);
    if (!outputFile)
    {
        cout << "Could not create output file: " << outputPath << endl;
    }
    else
    {
        outputFile.write(decoded.c_str(), decoded.size());
        outputFile.close();
    }
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
