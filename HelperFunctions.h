#ifndef HelperFunctions

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include <ftw.h>

#include <netdb.h>
#include <dirent.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <signal.h>

#define CHUNK 16384

#define MAX 20

char* _read(char* path);

char* concat(char* str1, char* str2, char delimeter);

char* subString(char* str, char delimeter, char begin);

char* subIndexer(char* fileStr, char* word, char endChar);

int fileWriter(char* fpath, char* string, int writeLen, char create);

char* digitToString(int number, int length);

int digitCounter(int number);

char* msgPreparer(char* msg);

char* sockReader(int sockf);

char* fileReader(char* path);

void createDir(char* path);

char* folderFinder(char* path);

#endif
