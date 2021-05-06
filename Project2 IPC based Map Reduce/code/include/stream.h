#ifndef STREAM_H
#define STREAM_H

#include "utils.h"

int mapperID;
char *inputFileDir;
char *ipFdr = "MapperInput/Mapper";
int interDS[MaxWordLength];

void emit(char *line);
void stream(char * inputFileName);

#endif