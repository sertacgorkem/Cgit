#include "helperFunctions.h"

/*Reads from the given file path*/
char* _read(char* path) {
    int fd = open(path, O_RDONLY);
    char buffer[1024];
    char* readString;
    int buffer_len = 0;
    int readMessageLen = 0;

    if (fd > 0) {
        while ((buffer_len = read(fd, buffer, 1023)) > 0) {
            readMessageLen += buffer_len;
            readString = realloc(readString, (readMessageLen + 1) * sizeof(char));
            strncat(readString, buffer, buffer_len);
            readString[readMessageLen] = '\0';
        }
        close(fd);
        return readString;
    }
    else {
        return NULL;
    }
}


void getLine(char* str, char eol, int n) {//eol is the end of line signifier normally '\n' but can be changed to anything
//n is the num of eol to skip, do 0 to get the first line
    char* ptr = str;
    while (n > 0) {
        ptr = strchr(ptr, eol);
        if (ptr + 1 == NULL)
            return;
        ptr++;
        n--;
    }
    return;
}

/*
Wrote myself a String.concat 
*/
char* concat(char* str1, char* str2, char delimeter) {
    int length = strlen(str1) + 1 + strlen(str2) + 1;
    int count = strlen(str1) + 1;
    if (delimeter == '\0') {
        length -= 1;
        count -= 1;
        //val[strlen(str1)] = delimeter;
    }
    char* val = (char*)malloc(sizeof(char) * length);

    strcpy(val, str1);
    if (delimeter != '\0') {
        val[strlen(str1)] = delimeter;
    }

    int i = 0;
    for (i = 0; i < strlen(str2); i++) {
        val[count] = str2[i];
        count++;
    }
    val[count] = '\0';
    //printf("%d len out concat\t", length);
    return val;
}

/*
Wrote myself a String.substring
*/
char* subString(char* str, char delimeter, char begin) {
    char* ptr = strchr(str, delimeter);
    char* sub = NULL;

    if (ptr) {
        if (begin == '1') {
            int index = ptr - str;
            int i = index + 1;

            int subLen = (strlen(str) - index);

            sub = (char*)malloc(sizeof(char) * subLen);
            int count = 0;
            for (; i < strlen(str); i++) {
                sub[count] = str[i];
                count++;
            }
            sub[count] = '\0';
        }
        else {
            int index = ptr - str;;
            int i = 0;

            int subLen = index + 1;

            sub = (char*)malloc(sizeof(char) * subLen);
            for (i = 0; i < subLen - 1; i++) {
                sub[i] = str[i];
            }
            sub[i] = '\0';
        }



    }
    return sub;
}

/*
Finds the word in the given string
*/
char* subIndexer(char* fileStr, char* word, char endChar) {
    char* begin = strstr(fileStr, word);
    char* val = NULL;
    if (begin != NULL) {
        int count = 0;
        count = strlen(word);
        int end = count;
        int start = count;
        char hasStarted = '0';
        for (count = count; count < strlen(begin); count++) {
            if (begin[count] == endChar && hasStarted == '1') {
                break;
            }
            if (hasStarted == '1') {
                end++;
            }
            if (isalnum(begin[count]) > 0 && hasStarted == '0') {
                hasStarted = '1';
                start = count;
                end = start;
            }

        }
        //printf("%d", end);
        if (end > 0) {
            val = (char*)malloc(sizeof(char) * (end + 1));
            int i = 0;
            for (start = start; start <= end; start++) {
                val[i] = begin[start];
                i++;
            }
            val[i] = '\0';
            //printf("%s\t",val);
        }
    }
    return val;
}
/*
Write the string to desired path
*/
int fileWriter(char* fpath, char* string, int writeLen, char create) {

    if (create == '1') {

        int fileDir = open(fpath, O_CREAT | O_RDWR, 0666);
        if (fileDir != -1) {

            write(fileDir, string, (strlen(string)) * sizeof(char));
        }
        close(fileDir);
    }

    else {
        FILE* fileSaver = fopen(fpath, "w");
        fprintf(fileSaver, string);
        fclose(fileSaver);
    }
    /*
        FILE *fp = fopen(fpath, "w");
        fprintf(fp, string);
    */

    return 0;
}

/*
Finds the number inside of a string
*/
char* digitToString(int number, int length) {
    char* fileSizeStr = (char*)malloc(sizeof(char) * length + 1);
    int count = 0;
    while (count != length) {
        fileSizeStr[length - count - 1] = number % 10 + '0';
        number /= 10;
        count++;
    }
    fileSizeStr[length] = '\0';
    return fileSizeStr;
}

/*
Counts how many digits there are in the number
*/
int digitCounter(int number) {
    int digit = 0;
    if (number != 0) {
        while (number / 10 != 0) {
            digit++;
            number /= 10;
        }
        digit++;
    }
    return digit;
}

/*
Prepares the message that is about to be sent to the server or client
*/
char* msgPreparer(char* msg) {
    size_t msgLen = strlen(msg) + 1;
    char* total = NULL;;
    int size = digitCounter(msgLen);
    if (size > 0) {

        char* sendMsg = concat("<", msg, '\0');
        char* send = concat(sendMsg, ">", '\0');
        msgLen = strlen(send) + 1;//
        msgLen += size + 2;
        int size = digitCounter(msgLen);
        char* meta1 = digitToString(msgLen, size);

        char* meta2 = concat("<", meta1, '\0');
        char* meta3 = concat(meta2, ">", '\0');

        total = concat(meta3, send, '\0');
        printf("%s\n", total);
        free(meta1);
        free(meta2);
        free(meta3);
        free(sendMsg);
        free(send);
    }
    return total;
}

/*
Reads the message that comes from the socket
*/
char* sockReader(int sockfd) {
    char buff[MAX];

    char* readString = NULL;
    bzero(buff, MAX);

    // read the message from client and copy it in buffer
    int readSize = -1;
    char limitFound = '0';
    int limit = 0;
    int start = 0;
    int totalSize = 0;
    int charCount = 0;
    while ((readSize = read(sockfd, buff, sizeof(buff))) > 0) {
        //printf("%s\n", buff);
        int k = 0;
        if (limitFound == '0') {

            if (buff[0] != '<') {
                printf("ERROR: message does not agree to protocol\n");
                exit(0);
            }
            char endCount = 0;
            for (start = 1; start < MAX; start++) {
                if (isdigit(buff[start]) == 0) {
                    break;
                }
                endCount = start;
            }
            int _size = endCount + 1;
            int _count = 0;
            char* fzstr = (char*)malloc(sizeof(char) * _size);
            for (start = 1; start <= endCount; start++) {
                fzstr[_count] = buff[start];
                _count++;
            }
            fzstr[_count] = '\0';
            limit = atoi(fzstr);
            limitFound = '1';
            if (buff[start] == '>') {
                start += 2;
            }
            totalSize = start;
            readString = (char*)malloc(sizeof(char) * (limit - start - 1));
            k = start;
        }

        if (limitFound == '1') {

            while (k < readSize && totalSize < limit - 2) {
                readString[charCount] = buff[k];
                charCount++;
                totalSize++;
                k++;
            }

        }
        if (totalSize == limit - 2) {
            break;
        }


    }
    readString[charCount] = '\0';

    return readString;
}


/*
Reads the file present and returns a string
*/
char* fileReader(char* fpath) {
    int fd = open(fpath, O_RDONLY);
    char* fileStr = NULL;
    if (fd != -1) {
        off_t currentPos = lseek(fd, (size_t)0, SEEK_CUR);
        int size = lseek(fd, 0, SEEK_END);
        lseek(fd, currentPos, SEEK_SET);

        fileStr = (char*)malloc(sizeof(char) * size);
        read(fd, fileStr, size);
    }
    else {
        printf("file could not be opened\n");
        //exit(0);
    }
    close(fd);
    return fileStr;
}

/*
Finds the folder in the UNIX file directory.
*/
char* folderFinder(char* path) {
    char* pPosition;
    char* _temp;
    char* parent;
    char* ptemp;
    char* val;
    char p = '0';
    while (strchr(path, '/') != NULL && (pPosition != NULL || p == '0')) {
        //printf("HI");

        if (p == '0') {
            parent = subString(path, '/', '0');
            createDir(parent);
            _temp = subString(path, '/', '1');
            p = '1';
            pPosition = _temp;
        }
        else {

            if (strchr(pPosition, '/')) {
                char* pparent = subString(pPosition, '/', '0');
                ptemp = concat(parent, pparent, '/');
                free(parent);
                parent = ptemp;
                createDir(parent);

                _temp = subString(pPosition, '/', '1');
                free(pPosition);
                pPosition = _temp;
            }
            else {
                break;
            }
        }
    }
    if (pPosition != NULL && parent != NULL) {
        val = concat(parent, pPosition, '/');
    }
    free(parent);
    free(pPosition);
    return val;
}

/*
Creates the directory
*/
void createDir(char* str) {
    struct stat stats;

    if (stat(str, &stats) == -1) {
        mkdir(str, 0700);
       
    }
}



