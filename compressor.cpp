// #include <iostream>
// #include <fstream>
// #include "Huffman.cpp"
// // #include "Huffman.h"
// // #include "Node.h"
// #include "Node.cpp"

// using namespace std;

// int main(int argc, char* argv[]) {
//     ifstream file;
//     huffman h;

//     // h.encode("test.txt");

//     h.decode("test.txt.hdta","test.txt.htbl");

//     return 0;
// }

#include <iostream>
#include <cstdio>
#include <fstream>
#include "Huffman.cpp"
#include "Node.cpp"

using namespace std;

void bindFiles(const string &tableFile, const string &dataFile, const string &outputFile)
{
    ifstream tFile(tableFile, ios::binary);
    ifstream dFile(dataFile, ios::binary);
    ofstream outFile(outputFile, ios::binary);

    if (!tFile || !dFile || !outFile)
    {
        cout << "Error opening files.\n";
        return;
    }



    tFile.seekg(0, ios::end);
    uint32_t tableSize = tFile.tellg();
    tFile.seekg(0, ios::beg);

    dFile.seekg(0, ios::end);
    uint32_t dataSize = dFile.tellg();
    dFile.seekg(0, ios::beg);

    // Write header
    outFile.write("HUFF", 4);
    outFile.write(reinterpret_cast<char *>(&tableSize), sizeof(tableSize));
    outFile.write(reinterpret_cast<char *>(&dataSize), sizeof(dataSize));

    // Write table
    vector<char> buffer(tableSize);
    tFile.read(buffer.data(), tableSize);
    outFile.write(buffer.data(), tableSize);

    // Write data
    buffer.resize(dataSize);
    dFile.read(buffer.data(), dataSize);
    outFile.write(buffer.data(), dataSize);

    tFile.close();
    dFile.close();
    outFile.close();

    if (remove(tableFile.c_str()) == 0)
    {
        cout << "Successfully created compressed file: " << outputFile;
    }
    else
    {
        perror(("Error deleting file: " + tableFile).c_str());
    }

    if (remove(dataFile.c_str()) == 0)
    {
        cout << endl;
    }
    else
    {
        perror(("Error deleting file: " + dataFile).c_str());
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    string inputFile = argv[1];
    huffman h;
    h.encode(inputFile);

    bindFiles(inputFile + ".htbl", inputFile + ".hdta", inputFile + ".huff");

    return 0;
}