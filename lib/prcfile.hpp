#ifndef _PRCFILE_HPP
#define _PRCFILE_HPP

#include <vector>
#include <string>

/* This structure holds some information about the original file, like the
   file extension, the most occurred char and the method of compression. */
typedef struct {
    unsigned char compressedChar;
    char fileExtension[4];
    int method;
} FileInfo;

/* This structure was created to suppress the truly structure, witch is vary large. */
typedef std::vector<std::pair<int, unsigned char> > BMP;

/**
  * \brief verifies if a given file exists in the current path.
  * \param name - the file name to be checked.
  * \return true if the file exists, false if not.
  */
bool fileExists (const char *fileName);

/**
  * \brief get from the user a name of a file.
  * \param fileName - pointer to the string that will hold the file name.
  * \return none.
  */
void inputFileName (char *fileName);

/**
  * \brief gets the extension of a given file.
  * \param fileName.
  * \return the string with the file extension.
  */
std::string getFileExtension (std::string fileName);

/**
  * \brief changes a file extension.
  * \param fileName.
  * \param newExtension.
  * \return the new file name.
  */
std::string changeFileExtension (std::string fileName, std::string newExtension);

/**
  * \brief Counts how many times each char occurs in a file.
  * \param fileName.
  * \param myFile - pointer to the struct that will hold the information of the file.
  * \return a vector with pairs of number of occurrences and the char.
  */
BMP generateBitMap (const char *fileName, FileInfo *myFile);

#endif


