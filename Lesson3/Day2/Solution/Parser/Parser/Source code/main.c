/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"
#include "parser.h"

/******************************************************************/

void printFile(char* fileName) {
    FILE *fp;
    if ((fp = fopen(fileName, "r")) == NULL) {
        printf("Cannot open %s\n", fileName);
        return;
    }
    
    char c;
    while((c = fgetc(fp)) != EOF) {
        printf("%c", c);
    }
    printf("\n");
    
    fclose(fp);
}

void compileAndWriteToFile(char* filePath, char* myResultPath) {
    FILE *fp = freopen(myResultPath, "w", stdout);
    compile(filePath);
    fclose(fp);
}

int compareLineByLine(char* fileName1, char* fileName2) {
    FILE* file1 = fopen(fileName1, "r");
    FILE* file2 = fopen(fileName2, "r");
    if (file1 == NULL) {
        printf("Cannot open %s\n", fileName1);
        return 0;
    }
    if (file2 == NULL) {
        printf("Cannot open %s\n", fileName2);
        return 0;
    }
    
    int lineNo = 0;
    int isEqual = 1;
    char line1[1000];
    char line2[1000];
    while(fgets(line1, 1000, file1) && fgets(line2, 1000, file2)) {
        lineNo++;
        isEqual = strcmp(line1, line2) == 0;
        if (!isEqual) {
            printf("%d\n%s%s-->Not Equal\n\n", lineNo, line1, line2);
        }
    }
    
    while (fgets(line1, 1000, file1)) {
        printf("FILE1: %sFILE2: NULL\n-->Not Equal\n\n", line1);
    }
    while (fgets(line2, 1000, file2)) {
        printf("FILE1: NULLFILE2: %s-->Not Equal\n\n", line1);
    }
    
    if (isEqual) printf("==>Same\n"); else printf("==>Not same\n");
    return isEqual;
}

void test(void) {
    char examplePath[] = "/Users/lanchu/OneDrive/Hust/20211/Compiler Lab/HUST.Compiler-Construction/Lesson3/Day2/Solution/Parser/Parser/Test Cases/example7.kpl";
    char resultPath[] = "/Users/lanchu/OneDrive/Hust/20211/Compiler Lab/HUST.Compiler-Construction/Lesson3/Day2/Solution/Parser/Parser/Test Cases/result7.txt";
    char myResultPath[] = "/Users/lanchu/OneDrive/Hust/20211/Compiler Lab/HUST.Compiler-Construction/Lesson3/Day2/Solution/Parser/Parser/Test Cases/tempResult.txt";
//    compileAndWriteToFile(examplePath, myResultPath);
//    compareLineByLine(myResultPath, resultPath);
    compile(examplePath);
}

int main(int argc, char *argv[]) {
//    if (argc <= 1) {
//        printf("parser: no input file.\n");
//        return -1;
//    }
//
//    if (compile(argv[1]) == IO_ERROR) {
//        printf("Can\'t read input file!\n");
//        return -1;
//    }
    test();
    return 0;
}
