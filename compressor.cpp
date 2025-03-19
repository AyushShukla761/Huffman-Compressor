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

//     h.encode("test.txt");
//     return 0;
// }

#include <iostream>
#include <fstream>
#include "Huffman.cpp"
#include "Node.cpp"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    string inputFile = argv[1]; // Get the file name from the argument
    huffman h;
    h.encode(inputFile); // Compress the file

    cout << inputFile << ".hdta"; // Print output filename for the Python script to capture
    return 0;
}
