#ifndef _HUFFMAN_HPP
#define _HUFFMAN_HPP

/**
 *  This Huffman compression implementation is based on this:
 *  http://www.programminglogic.com/implementing-huffman-coding-in-c/
 */

#include <map>

/* This structure holds the information of a node of the Huffman Tree.
   It contains the pointers to the nodes of the right and the left too. */
typedef struct _hfn {
    std::pair<int, unsigned char> info;
    struct _hfn *left, *right;
} HuffmanNode;

/* Used to abbreviate the original struct */
typedef std::map<unsigned char, unsigned long long> CT;

/**
  * \brief Creates a HuffmanNode;
  * \param info - A pair containing the the probability and the char that represents the node.
  * \return A pointer to a HuffmanNode allocated.
  */
HuffmanNode *createNode (std::pair<int, unsigned char> info);

/**
  * \brief Joins two HuffmanNodes;
  * \param node1
  * \param node2
  * \return A pointer to a new HuffmanNode created through the given nodes.
  */
HuffmanNode *joinNode (HuffmanNode *node1, HuffmanNode *node2);

/**
  * \brief Build the Huffman tree;
  * \param nodes - A vector with all the HuffmanNodes created.
  * \return A pointer to the root of this tree.
  */
HuffmanNode *buildHuffmanTree (std::vector<HuffmanNode* > nodes);

/**
  * \brief Sort the tree by the probability of each node.
  * \param nodes - A vector with all the HuffmanNodes created.
  * \return The vector ordered.
  */
std::vector<HuffmanNode* > sortTree (std::vector<HuffmanNode* > nodes);

/**
  * \brief Creates the tree and put each chars of the bitmap as a node, then calls buildHuffmanTree.
  * \param bitMap - The vector containing each char and the number of occurrences.
  * \return A pointer to the root of the tree.
  */
HuffmanNode *generateTree (BMP bitMap);

/**
  * \brief Generate the codes of each node of the tree and put it on a table recursively.
  * \param node - A pointer to a node, initially the root.
  * \param code - The code of each node, initially zero.
  * \param codeTable - A map linking each code to it's char.
  * \return none.
  */
void generateCodes (HuffmanNode *node, unsigned long long code, CT *codeTable);

/**
  * \brief Invert the codes to be used on the compression.
  * \param codeTable - A map linking each code to it's char.
  * \return none.
  */
void invertCodes (CT *codeTable);

/**
  * \brief Compress a file using the generated tree of Huffman.
  * \param fileName - A string that holds the file name to be compressed.
  * \param bitMap - The vector containing each char of the file and the number of occurrences.
  * \param myFile - The informations about the original file.
  * \return true if the compression goes right, else otherwise.
  */
bool compressHuffman (const char *fileName, BMP bitMap, FileInfo *myFile);

/**
  * \brief Decompress a file using the tree of Huffman saved on the compressed file.
  * \param fileName - A string that holds the file name to be decompressed.
  * \param myFile - The informations about the original file.
  * \return true if the decompression goes right, else otherwise.
  */
bool decompressHuffman (const char *fileName, FileInfo *myFile);

#endif
