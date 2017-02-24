#include <math.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>

#include "prcfile.hpp"
#include "lz78.hpp"

bool compressLZ78 (const char *fileName, FileInfo *myFile) {
    /* Open input file in binary mode */
    std::ifstream FileIn(fileName, std::ifstream::in | std::ifstream::binary);
    if (!FileIn.good()) return false;

    /* Open output file in binary mode */
    std::ofstream FileOut(changeFileExtension(fileName, "hu3").c_str(), std::ifstream::out | std::ifstream::binary);
    if (!FileOut.good()) return false;

    unsigned char byte, id;
    std::vector<std::string> dict;
    std::string chain = "", aux = "";

    /* Inserts the empty string to the dict */
    dict.push_back(chain);

    /* Since the LZ78 will always be the last method of compression utilized, we need to save the head of information */
    FileOut.write((const char*) myFile, sizeof(FileInfo));

    while (FileIn.good()) {
        byte = FileIn.get();

        if (FileIn.good()) {
            /* The pseudocode can be found at Wikipedia.com */

            /* The auxiliary string is the actual chain concatenated with the read byte */
            aux = chain + static_cast<char>(byte);
            /* If this auxiliary string is found in the dict, we search for a bigger sequence */
            if (std::find(dict.begin(), dict.end(), aux) != dict.end())
                chain = aux;
            else {
                /* Find the position of the chain in the dict and puts writes in the output file */
                id = std::distance(dict.begin(), std::find(dict.begin(), dict.end(), chain));
                FileOut.write((const char*) &id, 1);
                FileOut.write((const char*) &byte, 1);

                /* Limits the dictionary to 256 entries */
                if (dict.size() < 256)
                    dict.push_back(aux);

                /* Reset the variables */
                chain = "";
            }
        }
    }

    FileIn.close();
    FileOut.close();

    return true;
}

bool decompressLZ78 (const char *fileName, FileInfo *myFile) {
    /* Open input file in binary mode */
    std::ifstream FileIn(fileName, std::ifstream::in | std::ifstream::binary);
    if (!FileIn.good()) return false;

    /* Open output file in binary mode */
    std::string extension = (myFile->method == 3) ? myFile->fileExtension : "huf";
    std::ofstream FileOut(changeFileExtension(fileName, extension).c_str(), std::ifstream::out | std::ifstream::binary);
    if (!FileOut.good()) return false;

    unsigned char byte, id;
    std::vector<std::string> dict;
    std::string chain = "";

    /* In the decompression the dict is constructed again */
    dict.push_back(chain);

    /* Reads the information about the original file */
    FileIn.read((char*) myFile, sizeof(FileInfo));

    while (FileIn.good()) {
        byte = FileIn.get();

        if (FileIn.good()) {
            /* find string in the dict that corresponds to the read byte */
            if (byte < dict.size())
                FileOut.write((const char*) dict[byte].c_str(), dict[byte].size());

            /* Save the string */
            chain = dict[byte];

            /* Read the next byte */
            byte = FileIn.get();

            FileOut.write((const char*) &byte, 1);

            /* Put the chain into the dict */
            chain += static_cast<char>(byte);
            dict.push_back(chain);
        }
    }

    FileIn.close();
    FileOut.close();

    return true;
}
