#include "mapper.h"

void writeInterDSToFiles(void) {
    char line[30];
    char filepath[maxFileNameLength];
    memset(filepath, '\0', maxFileNameLength);

    int i;
    for (i = 0; i < MaxWordLength; i++) {
        if (interDS[i] > 0) {
            memset(line, '\0', 30);
            sprintf(line, "%d %d\n", i+1, interDS[i]);
            sprintf(filepath, "%s/%d/m%d.txt", intermediateDir, (i+1), mapperID);
            writeLineToFile(filepath, line);
        }
    }
}

void printInterDS(void) {
    printf(" < Inter DS Contents >\n");
    int i;
    for (i = 0; i < MaxWordLength; i++) {
        printf("Key: %d - Value: %d\n", (i + 1), interDS[i]);
    }
}

void emit(char *line) {
    char* token;
    char* rest = line;
    while ((token = strtok_r(rest, " \n", &rest))) {
        interDS[strlen(token) - 1]++;
    }
}

void map(char * inputFileName) {
    FILE * fp;                                              // File pointer
    char *line = (char *)malloc(sizeof(char) * chunkSize);  // The variable line points to a chunk buffer where a new line is stored
    size_t len = chunkSize;                                 // The size of a chunk buffer
    ssize_t nread;                                          // Length of a new line

    if ((fp = getFilePointer(inputFileName)) == NULL) {
        exit(EXIT_FAILURE);
    }
    while((nread = getLineFromFile(fp, line, len)) != -1) {
        //TODO: Parse line & Count words by word length
        emit(line);
    }
    free(line);
    fclose(fp);
}

int main(int argc, char *argv[]) {

    if (argc != 4) {
        printf("Less number of arguments.\n");
        printf("./mapper mapperID nMappers inputFileDir\n");
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    mapperID = strtol(argv[1], NULL, 10);
    int nMappers = strtol(argv[2], NULL, 10);
    inputFileDir = argv[3];

    //getMapperTasks function returns a list of input file names that this mapper should process
    char *myTasks[MaxNumInputFiles] = {NULL};
    int nTasks = getMapperTasks(nMappers, mapperID, inputFileDir, &myTasks[0]);

    int tIdx;
    for (tIdx = 0; tIdx < nTasks; tIdx++) {
        printf("mapper[%d] - %s\n", mapperID, myTasks[tIdx]);
        map(myTasks[tIdx]);
        free(myTasks[tIdx]);
    }

//    printInterDS();
    writeInterDSToFiles();

    return EXIT_SUCCESS;
}