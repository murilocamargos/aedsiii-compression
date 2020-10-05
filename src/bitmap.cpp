#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <bitset>

#include "prcfile.hpp"
#include "bitmap.hpp"

bool compressBitMap (const char *fileName, std::vector<std::pair<int, unsigned char> > bitMap, FileInfo *myFile) {
    unsigned char byte, key = 0;
    std::string diff = "";
    int leftBits = 8;

    /* Open input file in binary mode */
    std::ifstream FileIn(fileName, std::ifstream::in | std::ifstream::binary);
    if (!FileIn.good()) return false;

    /* Open output file in binary mode */
    std::string extension = (myFile->method == 1) ? "hu3" : "bmp";
    std::ofstream FileOut(changeFileExtension(fileName, extension).c_str(), std::ifstream::out | std::ifstream::binary);
    if (!FileOut.good()) return false;

    /* If the chosen method is just the bitmap, we need to save a head of information */
    if (myFile->method == 1)
        FileOut.write((const char*) myFile, sizeof(FileInfo));

    /* Read the file byte by byte */
    while (FileIn.good()) {
        byte = FileIn.get();

        if (FileIn.good()) {
            /* Key is the 8 bit sequence generated */
            key = key << 1;

            /* If the byte is equal to the most occurring char, then put 1 into the sequence */
            if (byte == bitMap[0].second) key |= 1;
            else diff += byte;

            leftBits -= 1;

            if (leftBits == 0) {
                /* When the sequence reaches 8 bits, write on the output file */
                FileOut.write((const char*) &key, 1);
                FileOut.write(diff.c_str(), diff.size());
                /* Reset the control variables */
                key = 0;
                leftBits = 8;
                diff = "";
            }
        }
    }

    /* If mod(number of bytes % 8) != 0, then we need to compress the final bytes */
    if (leftBits != 8) {
        key = key << leftBits;
        FileOut.write((const char*) &key, 1);
        FileOut.write(diff.c_str(), diff.size());
    }

    FileIn.close();
    FileOut.close();

    return true;
}

bool decompressBitMap (const char *fileName, FileInfo *myFile) {
    unsigned char byte;
    std::string diff = "", unkeystr;
    std::bitset<8> unkey;

    /* Open input and output file in binary mode */
    std::ifstream FileIn(fileName, std::ifstream::in | std::ifstream::binary);
    if (!FileIn.good()) return false;

    std::string extension = myFile->fileExtension;
    std::ofstream FileOut(changeFileExtension(fileName, extension).c_str(), std::ifstream::out | std::ifstream::binary);
    if (!FileOut.good()) return false;

    /* If this is the only method of compression utilized, then there is a head to read */
    if (myFile->method == 1)
        FileIn.read((char*) myFile, sizeof(FileInfo));

    while (FileIn.good()) {
        /* Read the first byte, it is a key */
        byte = FileIn.get();

        if (FileIn.good()) {
            /* Generate an 8-bit sequence */
            unkey = (int) byte;
            unkeystr = unkey.to_string();
            /* Loop through the sequence */
            for (std::string::iterator s = unkeystr.begin(); s != unkeystr.end(); ++s) {
                /* If the bit is one, put the compressed byte in the output file, if not, read the next byte */
                if (*s == '1')
                    FileOut.write((const char*) &myFile->compressedChar, 1);
                else {
                    byte = FileIn.get();
                    if (FileIn.good())
                        FileOut.write((const char*) &byte, 1);
                    else
                        break;
                }
            }
        }
    }

    FileIn.close();
    FileOut.close();

    return true;
}
