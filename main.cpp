#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>

#include "lib/prcfile.hpp"
#include "lib/bitmap.hpp"
#include "lib/huffman.hpp"
#include "lib/lz78.hpp"

#define COMPRESS   1
#define DECOMPRESS 0

int main(int argc, char *argv[]) {
    char fileName[50];
    int action, option, method;

    /** Get an option and a method of compression from the user. **/

    printf("Choose an action:\n[1] Compress\n[2] Decompress\n");
    scanf("%d", &option);

    action = (option == 1) ? COMPRESS : DECOMPRESS;

    /* The methods of compression will be asked only if action = 1 = COMPRESS */
    if (option == 1) {
        system("CLS");

        printf("Choose a method:\n[1] Bit Map\n[2] Huffman\n[3] LZ78\n[4] Bit Map + Huffman\n[5] Bit Map + Huffman + LZ78\n");
        scanf("%d", &method);

        if (method < 1 || method > 5) method = 1;
    }

    /* Get a valid file name from the user */
    inputFileName(fileName);


    /** Action **/

    FileInfo myFile, myFileAux;

    if (action == COMPRESS) {
        /* Generate the bit map from the file and compose the FileInfo myFile.
           myFile have the file extension, the most occurring char more and the method of compression */
        BMP bitMap = generateBitMap(fileName, &myFile);
        myFile.method = method;

        if (method == 1)
            compressBitMap(fileName, bitMap, &myFile);
        else if (method == 2)
            compressHuffman(fileName, bitMap, &myFile);
        else if (method == 3)
            compressLZ78(fileName, &myFile);
        else if (method == 4) {
            compressBitMap(fileName, bitMap, &myFile);

            bitMap = generateBitMap(changeFileExtension(fileName, "bmp").c_str(), &myFileAux);

            compressHuffman(changeFileExtension(fileName, "bmp").c_str(), bitMap, &myFile);
            remove(changeFileExtension(fileName, "bmp").c_str());
        }
        else {
            compressBitMap(fileName, bitMap, &myFile);

            bitMap = generateBitMap(changeFileExtension(fileName, "bmp").c_str(), &myFileAux);

            compressHuffman(changeFileExtension(fileName, "bmp").c_str(), bitMap, &myFile);
            remove(changeFileExtension(fileName, "bmp").c_str());

            compressLZ78(changeFileExtension(fileName, "huf").c_str(), &myFile);
            remove(changeFileExtension(fileName, "huf").c_str());
        }
    } else {
        /* Read the head of the file. The head contains information about the compressed file */
        std::ifstream FileIn(fileName, std::ifstream::in | std::ifstream::binary);
        FileIn.read((char*) &myFile, sizeof(FileInfo));
        FileIn.close();

        if (myFile.method == 1)
            decompressBitMap(fileName, &myFile);
        else if (myFile.method == 2)
            decompressHuffman(fileName, &myFile);
        else if (myFile.method == 3)
            decompressLZ78(fileName, &myFile);
        else if (myFile.method == 4) {
            decompressHuffman(fileName, &myFile);
            decompressBitMap(changeFileExtension(fileName, "bmp").c_str(), &myFile);
            remove(changeFileExtension(fileName, "bmp").c_str());
        }
        else {
            decompressLZ78(fileName, &myFile);
            decompressHuffman(changeFileExtension(fileName, "huf").c_str(), &myFile);
            remove(changeFileExtension(fileName, "huf").c_str());
            decompressBitMap(changeFileExtension(fileName, "bmp").c_str(), &myFile);
            remove(changeFileExtension(fileName, "bmp").c_str());
        }
    }

    return 0;
}
