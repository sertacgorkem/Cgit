// Write CPP code here

#include "HelperFunctions.h"

#include <openssl/sha.h>

#define SA struct sockaddr

typedef struct structObj {
    char* ip;
    char* port;
}serverStruct;

/*write to server*/
void func(int sockfd, char* message)
{
    write(sockfd, message, (strlen(message) + 1) * sizeof(char));

}



/*in the works*/
char* hash(char* str) {
    unsigned char temp[SHA_DIGEST_LENGTH];
    char buf[SHA_DIGEST_LENGTH * 2];

    //if ( argc != 2 ) {
    //printf("Usage: %s string\n", argv[0]);
    //return -1;
    //}


    memset(buf, 0x0, SHA_DIGEST_LENGTH * 2);
    memset(temp, 0x0, SHA_DIGEST_LENGTH);

    SHA1((unsigned char*)str, strlen(str), temp);
    int i = 0;
    for (i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf((char*)&(buf[i * 2]), "%02x", temp[i]);
    }

    //printf("SHA1 of %s is %s\n", str, buf);
    return buf;
}


/*
Write to .configure to save server info
*/
void configure(char* _ip, char* _port) {
    //printf("%s\n", _port);
    int writeLen = strlen(_ip) + 1 + strlen(_port);
    char* str = (char*)malloc(sizeof(char) * writeLen);
    strcpy(str, _ip);
    str[strlen(_ip)] = '\n';
    int count = strlen(_ip) + 1;
    int i = 0;
    for (i = 0; i < strlen(_port); i++) {
        str[count] = _port[i];
        count++;
    }

    //str[writeLen-1] = '\0';
    fileWriter("./.configure", str, writeLen, '1');

}

/*Unix connection*/
char* serverConnect(serverStruct* server, char* msg) {

    struct addrinfo hints;
    struct addrinfo * res;
    int sockfd;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(server->ip, server->port, &hints, &res);


    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    int serverConnected = 0;
    while (serverConnected == 0) {
        if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
            printf("connection with the server failed sleeping 3 seconds...\n");
            sleep(3);
            //exit(0);
        }
        else {
            printf("connected to the server..\n");
            serverConnected = 1;
            func(sockfd, msg);
        }
    }

    // function for chat

    char* readString = NULL;
    if (serverConnected == 1) {
        readString = sockReader(sockfd);
        printf("From Server : %s", readString);
    }
    close(sockfd);
    return readString;
}


serverStruct* ServerStringReader(char* str) {
    serverStruct* server = (serverStruct*)malloc(sizeof(serverStruct));
    char* ptr = strchr(str, '\n');
    if (ptr) {
        int index = ptr - str;
        int i = 0;
        char* ip = (char*)malloc(sizeof(char) * (index + 1));

        int portLen = (strlen(str) - index);

        char* port = (char*)malloc(sizeof(char) * portLen);
        for (i = 0; i < index; i++) {
            ip[i] = str[i];
        }
        ip[i] = '\0';
        i++;
        int count = 0;
        for (; i < strlen(str); i++) {
            port[count] = str[i];
            count++;
        }

        port[count] = '\0';
        server->ip = ip;
        server->port = port;
        //printf("%s\n%s\n",ip, port);
    }
    return server;
}


void checkout(char* fileName) {
    char* serverInfo = fileReader("./.configure");
    //printf("%s\n", serverInfo);
    serverStruct* server = ServerStringReader(serverInfo);
    char* msg = concat("checkout", fileName, ':');
    if (strlen(msg) > 0) {
        char* total = msgPreparer(msg);
        free(msg);
        if (total != NULL) {
            char* response = serverConnect(server, total);
            free(total);

            if (response != NULL) {

                char* charEnd = subString(response, '>', '1');
                char* front = subString(response, '>', '0');
                if (strcmp(front, "success") == 0) {
                    int pass = 0;
                    while (strchr(charEnd, '>') != NULL) {
                        char* path = subIndexer(charEnd, "<", '>');
                        char* c1 = subString(charEnd, '>', '1');
                        char* strs = folderFinder(path);

                        char* content = subString(c1, '<', '1');
                        if (strchr(content, '>') != NULL) {
                            char* fileContent = subString(content, '>', '0');
                            char* _tmpName_ = subString(c1, '>', '1');
                            free(content);
                            content = _tmpName_;
                            fileWriter(strs, fileContent, 0, '1');
                            free(fileContent);
                        }
                        else {
                            fileWriter(strs, content, 0, '1');
                            free(content);
                        }
                        free(charEnd);
                        free(path);
                        free(c1);
                        free(strs);
                        charEnd = content;
                        pass++;
                    }
                    if (pass > 1) {
                        free(charEnd);
                    }
                }
                else {

                }
                if (front != NULL) {
                    free(front);
                }
                free(response);
            }
            else {
                printf("Error while preparing message");
            }
        }
    }

    free(serverInfo);
    free(server->ip);
    free(server->port);
    free(server);

}

/*
Sends the file info to the server to create
*/
void clientCreate(char* fileName) {
    char* serverInfo = fileReader("./.configure");
    //printf("%s\n", serverInfo);
    serverStruct* server = ServerStringReader(serverInfo);
    char* msg = concat("create", fileName, ':');

    if (strlen(msg) > 0) {
        char* total = msgPreparer(msg);
        if (total != NULL) {

            char* response = serverConnect(server, total);

            char* charEnd = subString(response, '>', '1');
            char* front = subString(response, '>', '0');
            if (strcmp(front, "success") == 0) {
                char* path = subIndexer(charEnd, "<", '>');
                char* c1 = subString(charEnd, '>', '1');
                char* content = subString(c1, '<', '1');

                //char *strs = folderFinder(path);
                createDir(fileName);
                char* strs = concat(fileName, ".manifest", '/');
                fileWriter(strs, content, 0, '1');


                free(path);
                free(c1);
                free(content);
            }
            if (charEnd != NULL) {
                free(charEnd);
            }
            if (front != NULL) {
                free(front);
            }


            free(total);
            //free(response);
        }
        else {
            printf("Error while preparing message");
        }
    }


    free(msg);
    free(serverInfo);
    free(server->ip);
    free(server->port);
    free(server);
}

/*
Sends command to destroy the file in the server
*/
void clientDestroy(char* fileName) {
    char* serverInfo = fileReader("./.configure");
    //printf("%s\n", serverInfo);
    serverStruct* server = ServerStringReader(serverInfo);
    char* msg = concat("destroy", fileName, ':');
    if (strlen(msg) > 0) {
        char* total = msgPreparer(msg);

        if (total != NULL) {
            //printf("%s\n", total);
            char* response = serverConnect(server, total);
            free(total);
            free(response);
        }
        else {
            printf("Error while preparing message");
        }
    }
    free(serverInfo);
    free(server->ip);
    free(server->port);
    free(server);
}
/*
Adds client file to the .manifest
*/
void clientAdd(char* projectName, char* fname) {
    struct stat stats;
    if (stat(projectName, &stats) == -1) {
        printf("Project name doesn't exist\n");
        return;
    }
    char* path = concat(projectName, fname, '/');
    char* manipath = concat(projectName, ".manifest", '/');
    char* manistr = fileReader(manipath);
    char* fileEntryTmp = concat(fname, "\t1\t", '\0');
    char* fileStr = fileReader(path);
    if (fileStr != NULL) {
        char* fileHash = hash(fileStr);
        char* content0 = concat(fileEntryTmp, fileHash, '\0');

        char* manicontent = concat(content0, "\n", '\0');
        char* newfile = concat(manistr, manicontent, '\0');
        free(content0);
        free(manicontent);
        free(newfile);
        fileWriter(manipath, newfile, 0, '1');
    }
    else {
        printf("File doesnt exist in project\n");

    }


    //char *fpath = concat(path, fname
    free(path);
    free(manipath);
    free(manistr);
    free(fileEntryTmp);
    free(fileStr);



}

/*
Updates the files
*/
void clientUpdate(char* updateName) {
    char* serverInfo = fileReader("./.configure");
    serverStruct* server = ServerStringReader(serverInfo);

    char* tmp = concat("update", updateName, ':');
    char* msg = msgPreparer(tmp);

    char* updatePath = concat(updateName, ".update", '/');
    char* _tmp = concat(updateName, ".manifest", '/');

    char* manifest_s = serverConnect(server, msg);//manifest server
    char* manifest_c = _read(tmp);
    buildUpdate(updateName, updatePath, manifest_s, manifest_c);

    free(msg);
    free(tmp);
    free(_tmp);
    free(updatePath);
    free(manifest_s);
}

/*
Removes file from the .manifest file
*/
void clientRemove(char* projName, char* fileName) {
    char* path = concat(projName, ".manifest", '/');
    char* manifestString = fileReader(path);

    if (manifestString == NULL) {
        printf("Error: _read() error\n");
        free(path);
        return;
    }
    else {
        char* tmp;
        char* content;
        char* search = concat(fileName, "\t", '\0');
        if ((tmp = strstr(manifestString, search)) != NULL) {
            int size = tmp - manifestString;
            int z = 0;
            char* kk = (char*)malloc(sizeof(char) * size);
            for (z = 0; z < size - 1; z++) {
                kk[z] = manifestString[z];
            }
            kk[z] = '\0';
            char* resttmp = subString(tmp, '\n', '1');
            content = concat(kk, resttmp, '\n');

            fileWriter(path, content, 0, '0');
            free(content);
            free(kk);
            free(resttmp);
        }
        else {
            printf("Not found \n");
        }

        free(search);


    }
}

/*
Retuns the current version the file is at
*/
void currentVersion(char* proj) {
    char* serverInfo = fileReader("./.configure");
    serverStruct* server = ServerStringReader(serverInfo);
    char* msg = concat("currentversion", proj, ':');
    if (strlen(msg) > 0) {
        char* total = msgPreparer(msg);

        if (total != NULL) {
            char* response = serverConnect(server, total);
            printf("%s\n", response);
            free(total);
            //printf(response);
            free(response);
        }
        else {
            printf("Error while preparing message");
        }
        free(msg);
    }
}

/*
Rollback to the previous version
*/
void clientRollback(char* projectName, char* versionNumber) {
    char* serverInfo = fileReader("./.configure");
    //printf("%s\n", serverInfo);
    serverStruct* server = ServerStringReader(serverInfo);
    char* tmp = concat("rollback", projectName, ':');
    char* msg = concat(tmp, versionNumber, ':');
    if (strlen(msg) > 0) {
        char* total = msgPreparer(msg);
        if (total != NULL) {
            //printf("%s\n", total);
            char* response = serverConnect(server, total);
            free(total);
            free(response);
        }
        else {
            printf("Error while preparing message");
        }
        free(msg);
    }
    free(tmp);
}


/*
Gets the commit history from the server
*/
void history(char* proj) {
    char* serverInfo = fileReader("./.configure");
    serverStruct* server = ServerStringReader(serverInfo);
    char* msg = concat("history", proj, ':');
    if (strlen(msg) > 0) {
        char* total = msgPreparer(msg);

        if (total != NULL) {
            char* response = serverConnect(server, total);

            printf(response);

            free(total);
            free(response);
        }
        else {
            printf("Error while preparing message");
        }
        free(msg);
    }
    free(msg);
}

/*
Commit a file. In the works
*/
void clientCommit(char* fname) {

    char* serverInfo = fileReader("./.configure");
    //printf("%s\n", serverInfo);
    serverStruct* server = ServerStringReader(serverInfo);
    char* updatePath = concat(fname, ".update", '/');
    char* updateStr = fileReader(updatePath);
    char hasFailed = '0';
    if (updateStr != NULL) {
        if (strlen(updateStr) > 0) {
            printf("project has a non empty .update file\n");
            hasFailed = '1';
        }
        free(updateStr);
    }
    free(updatePath);

    if (hasFailed == '0') {
        char* msg = concat("commit", fname, ':');
        if (strlen(msg) > 0) {
            char* total = msgPreparer(msg);

            if (total != NULL) {

                char* response = serverConnect(server, total);
                if (response != NULL) {
                    char* charEnd = subString(response, '>', '1');
                    char* front = subString(response, '>', '0');
                    if (strcmp(front, "success") == 0) {
                        //char *path = subIndexer(charEnd, "<", '>');
                        char* c1 = subString(charEnd, '>', '1');
                        char* serverContent = subString(c1, '<', '1');

                        char* manipath = concat(fname, ".manifest", '/');
                        char* manistr = fileReader(manipath);
                        if (manistr != NULL) {
                            //char versionCheck = '0';
                            char* serverVersion = subString(serverContent, '\n', '0');
                            char* selfVersion = subString(manistr, '\n', '0');
                            if (strcmp(serverVersion, selfVersion) == 0) {
                                char* tmp = subString(serverContent, '\n', '1');
                                if (tmp != NULL) {
                                    while (strchr(tmp, '\n')) {
                                        char* file = subString(tmp, '\t', '0');
                                        if (strstr(manistr, file) != NULL) {

                                        }
                                    }
                                }
                            }
                            else {
                                printf("Version Number does not match aborted\n");
                            }

                            free(manistr);
                            free(selfVersion);
                            free(serverVersion);
                        }
                        free(manipath);
                        free(serverContent);
                        free(c1);

                    }
                    free(charEnd);
                    free(front);
                    free(total);
                    free(response);
                }
            }
            else {
                printf("Error while preparing message");
            }
        }
    }
    free(serverInfo);
    free(server->ip);
    free(server->port);
    free(server);
}



int main(int argc, char** argv)
{

    if (argc > 1) {
        if (strcmp(argv[1], "configure") == 0) {
            char* ip = argv[2];
            char* port = argv[3];
            configure(ip, port);
        }
        if (strcmp(argv[1], "commit") == 0) {
            char* pname = argv[2];
            clientCommit(pname);
        }
        else if (strcmp(argv[1], "checkout") == 0) {
            //printf("Hi");
            char* checkoutName = argv[2];
            checkout(checkoutName);
        }
        else if (strcmp(argv[1], "update") == 0) {//WIP
            char* updateName = argv[2];
            clientUpdate(updateName);
        }
        else if (strcmp(argv[1], "create") == 0) {
            //printf("Hi");
            char* createName = argv[2];
            clientCreate(createName);
        }
        else if (strcmp(argv[1], "destroy") == 0) {
            //printf("Hi");
            char* destroyName = argv[2];
            clientDestroy(destroyName);
        }
        else if (strcmp(argv[1], "add") == 0) {
            char* projectName = argv[2];
            if (argv[3] != NULL) {
                char* fname = argv[3];
                clientAdd(projectName, fname);
            }
        }
        else if (strcmp(argv[1], "remove") == 0) {
            char* projName = argv[2];
            char* fileName = argv[3];
            clientRemove(projName, fileName);
        }
        else if (strcmp(argv[1], "currentversion") == 0) {//WIP
            char* proj = argv[2];
            currentVersion(proj);
        }
        else if (strcmp(argv[1], "rollback") == 0) {
            char* projectName = argv[2];
            if (argv[3] != NULL) {
                char* versionNumber = argv[3];
                clientRollback(projectName, versionNumber);
            }
        }
    }
    return 0;

}
