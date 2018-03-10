#ifndef EFANNA_MEMORY_H_
#define EFANNA_MEMORY_H_

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> 

int getMemory();
int parseLine(char* line);
float file_size(char* filename)  ;

int getMemory(){ //Note: this value is in MB!
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmRSS:", 6) == 0){
            result = parseLine(line);
            break;
        }
    }
    fclose(file);
    return result/1024;
}

int parseLine(char* line){
    int i = strlen(line);
    const char* p = line;
    while (*p<'0'||*p>'9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

float file_size(char* filename)  
{  //Note: this value is in MB!
    struct stat statbuf;  
    stat(filename,&statbuf);  
    float size=statbuf.st_size;  
  
    return size/1048576;  
} 

#endif