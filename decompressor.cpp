#include <iostream>
#include <fstream>
#include "Huffman.cpp"
#include "Node.cpp"

using namespace std;


void unbindFile(const string &inputFile, const string &tableOut, const string &dataOut) {
    ifstream inFile(inputFile, ios::binary);
    if (!inFile) {
        cout << "Error opening combined file.\n";
        return;
    }


    char magic[4];
    inFile.read(magic, 4);

    if (inFile.gcount() != 4 || strncmp(magic, "HUFF", 4) != 0) {
        cout << "Error: The provided file is not a valid bound .huf file.\n";
        inFile.close();
        return;
    }

    uint32_t tableSize, dataSize;

    inFile.read(reinterpret_cast<char*>(&tableSize), sizeof(tableSize));
    inFile.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));

    ofstream tFile(tableOut, ios::binary);
    vector<char> buffer(tableSize);
    inFile.read(buffer.data(), tableSize);
    tFile.write(buffer.data(), tableSize);
    tFile.close();

    ofstream dFile(dataOut, ios::binary);
    buffer.resize(dataSize);
    inFile.read(buffer.data(), dataSize);
    dFile.write(buffer.data(), dataSize);
    dFile.close();

    inFile.close();

}

void deleteFiles(const string &inputFile, const string &tableOut, const string &dataOut){
    if (!remove(tableOut.c_str()) == 0)
    {
        perror(("Error deleting file: " + tableOut).c_str());
    }
    if (!remove(dataOut.c_str()) == 0)
    {
        perror(("Error deleting file: " + dataOut).c_str());
    }
    if (!remove(inputFile.c_str()) == 0)
    {
        perror(("Error deleting file: " + inputFile).c_str());
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    string inputFile = argv[1]; // Get the file name from the argument
    huffman h;

    string dataFile= inputFile.substr(0,inputFile.size()-5)+ ".hdta";
    string tableFile= inputFile.substr(0,inputFile.size()-5)+ ".htbl";
    unbindFile(inputFile, tableFile,dataFile);

    h.decode(dataFile,tableFile); // Decompress the file

    deleteFiles(inputFile,tableFile,dataFile);
    return 0;
}