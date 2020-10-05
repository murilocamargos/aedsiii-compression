#include <stdlib.h>
#include <math.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <map>

#include "prcfile.hpp"
#include "huffman.hpp"

HuffmanNode *createNode (std::pair<int, unsigned char> info) {
    /* Dynamically allocate the node */
    HuffmanNode *node = (HuffmanNode*) malloc (sizeof(HuffmanNode));
    if (!node) return NULL;

    /* Put the info into the node */
    node->info = info;
    node->left = node->right = NULL;

    return node;
}

HuffmanNode *joinNode (HuffmanNode *node1, HuffmanNode *node2) {
    /* Creates a new node */
    HuffmanNode *node = (HuffmanNode*) malloc (sizeof(HuffmanNode));
    if (!node) return NULL;

    /* This node will be the root of the node1 and node2 */
    /* And the probability of it, is the summation of the other two */
    node->info = std::make_pair(node1->info.first + node2->info.first, NULL);
    node->left = node1;
    node->right = node2;

    return node;
}

std::vector<HuffmanNode* > sortTree (std::vector<HuffmanNode* > nodes) {
    /* The tree is always sorted, but when we join two nodes, the last element may be out of place */
    int i = nodes.size() - 2, j;
    /* Stores the last node inserted in an auxiliary node */
    HuffmanNode *node = nodes[i], *aux = nodes[i + 1];

    /* From the penultimate node, all nodes with value smaller then the last node will be pushed one position */
    j = i;
    while (i >= 0 and aux->info.first > node->info.first) {
        nodes[i + 1] = nodes[i];
        node = nodes[--i];
    }
    /* When a node with value greater then the last node is found, put the last node one position from there. */
    if (j != i) nodes[i + 1] = aux;

    return nodes;
}

HuffmanNode *buildHuffmanTree (std::vector<HuffmanNode* > nodes) {
    /* The tree is build joining every two more probable nodes until theres only two nodes to be joined */
    int lastNode;

    /* Since the vector is already sorted, we have to join the last two nodes and sort again */
    while (nodes.size() != 1) {
        /* Join the two nodes from the bottom */
        lastNode = nodes.size() - 1;
        nodes[lastNode - 1] = joinNode(nodes[lastNode - 1], nodes[lastNode]);
        /* Remove the last node */
        nodes.pop_back();
        /* Sorts the vector again */
        nodes = sortTree(nodes);
    }

    /* With all that done, the first and only element of the vector will be the root of the tree */
    return nodes[0];
}

void generateCodes (HuffmanNode *node, unsigned long long code, CT *codeTable) {
    /* If the node is a leaf, then it is a representation of a byte
       then we need to insert it's code into the codeTable */
    if (!node->left)
        (*codeTable).insert(std::make_pair(node->info.second, code));
    else {
        /* Recursively visit the subtrees */
        generateCodes(node->left, code*10 + 1, codeTable);
        generateCodes(node->right, code*10 + 2, codeTable);
    }
}

void invertCodes (CT *codeTable){
    unsigned long long n, cpy;

    /* Mathematically reverse a number */
    for (CT::iterator m = (*codeTable).begin(); m != (*codeTable).end(); ++m) {
        n = m->second;
        cpy = 0;
        while (n > 0) {
            cpy = cpy * 10 + n %10;
            n /= 10;
        }
        (*codeTable)[m->first] = cpy;
    }
}

HuffmanNode *generateTree (BMP bitMap) {
    /* Creates the vector of huffman nodes */
    std::vector<HuffmanNode* > HuffmanTree;

    /* Every char on the file is a node initially */
    for (unsigned int i = 0; i < bitMap.size(); i++)
        HuffmanTree.push_back(createNode(bitMap[i]));

    /* Build the huffman tree and returns it's root */
    return buildHuffmanTree(HuffmanTree);
}

bool compressHuffman (const char *fileName, BMP bitMap, FileInfo *myFile) {
    CT codeTable;
    int length, bitsLeft = 8, bmpSize = bitMap.size();
    unsigned long long n;
    unsigned char bit, byte, x = 0;

    /* Build huffman tree trough the bitMap, generate the codes and invert them */
    HuffmanNode *root = generateTree(bitMap);
    generateCodes(root, 0, &codeTable);
    invertCodes(&codeTable);

    /* Open input file in binary mode */
    std::ifstream FileIn(fileName, std::ifstream::in | std::ifstream::binary);
    if (!FileIn.good()) return false;

    /* Open output file in binary mode */
    std::string extension = (myFile->method == 2 || myFile->method == 4) ? "hu3" : "huf";
    std::ofstream FileOut(changeFileExtension(fileName, extension).c_str(), std::ifstream::out | std::ifstream::binary);

    /* If the method of compression is 3 or 4, then the Huffman algorithm is the last to be used,
       so, it's necessary to save the head of information about the original file. */
    if (myFile->method == 2 || myFile->method == 4)
        FileOut.write((const char*) myFile, sizeof(FileInfo));

    /* Writes the size and the bitMap on the file */
    FileOut.write((const char*) &bmpSize, sizeof(int));
    FileOut.write((const char*) &bitMap[0], sizeof(std::pair<int, unsigned char>) * bmpSize);

    /* Read the file byte by byte */
    while (FileIn.good()) {
        byte = FileIn.get();

        if (FileIn.good()) {
            /* Calculates the distance between the root and the byte on the huffman tree */
            length = (int)log10(codeTable[byte]) + 1;
            /* Gets the code that represents the read byte */
            n = codeTable[byte];

            while (length > 0) {
                /* Generates the bits that corresponds to the read byte */
                bit = n % 10 - 1;
                n /= 10;
                x = x | bit;
                bitsLeft--;
                length--;
                if (bitsLeft == 0) {
                    /* Write the byte on the file if there are 8 bits on 'x' */
                    FileOut.write((const char*) &x, 1);
                    /* Reset the variables */
                    x = 0;
                    bitsLeft = 8;
                }
                x = x << 1;
            }
        }
    }

    /* If there are bits left */
    if (bitsLeft != 8){
        /* Complete them with zeros on the right */
        x = x << (bitsLeft-1);
        FileOut.write((const char*) &x, 1);
    }

    FileIn.close();
    FileOut.close();

    return true;
}

bool decompressHuffman (const char *fileName, FileInfo *myFile) {
    /* Open the input file in binary mode for reading */
    std::ifstream FileIn(fileName, std::ifstream::in | std::ifstream::binary);
    if (!FileIn.good()) return false;

    /* Open the output file in binary mode for writing */
    std::string extension = (myFile->method == 2) ? myFile->fileExtension : "bmp";
    std::ofstream FileOut(changeFileExtension(fileName, extension).c_str(), std::ifstream::out | std::ifstream::binary);

    unsigned char byte, bit, mask = 1 << 7;
    int bmpSize = 0, i;
    BMP bitMap;
    HuffmanNode *node, *root;
    std::vector<HuffmanNode* > HuffmanTree;

    /* If the method of last compression was the huffman algorithm, then we need to read the head struct */
    if (myFile->method == 2 || myFile->method == 4)
        FileIn.read((char*) myFile, sizeof(FileInfo));

    /* Reads the size and the bitMap */
    FileIn.read((char*) &bmpSize, sizeof(int));
    bitMap.resize(bmpSize);

    FileIn.read((char*) &bitMap[0], sizeof(std::pair<int, unsigned char>) * bmpSize);

    /* Generate the tree based on the bitMar read */
    root = generateTree(bitMap);

    node = root;
    while (FileIn.good()) {
        byte = FileIn.get();
        if (FileIn.good()) {
            for (i = 0; i < 8; i++) {
                /* This is made to get the first bit of an 8-bit sequence */
                bit = byte & mask;
                byte = byte << 1;

                node = (bit == 0) ? node->left : node->right;
                /* If the node is a leaf, then write it's char value on the file */
                if (!node->left) {
                    FileOut.write((const char*) &node->info.second, 1);
                    node = root;
                }
            }
        }
    }


    FileIn.close();
    FileOut.close();

    return true;
}
