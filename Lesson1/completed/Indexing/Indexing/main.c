//
//  main.c
//  Indexing
//
//  Created by Lan Chu on 9/30/21.
//

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char* VAN_BAN_PATH = "/Users/lanchu/OneDrive/Hust/20211/Compiler Lab/Students/Lesson1/completed/vanban.txt";
char* STOPW_PATH = "/Users/lanchu/OneDrive/Hust/20211/Compiler Lab/Students/Lesson1/completed/stopw.txt";
const int maxLines = 1000;
const int maxCols = 1000;
const int maxWords = 1000;
const int maxWordLength = 50;

char* bannedWords[maxWords];
char* words[maxWords];
int lineIndices[maxWords][maxLines];
int colIndices[maxWords][maxCols];

void toLower(char *str) {
    for(int i = 0; i < strlen(str); ++i) {
       str[i] = tolower(str[i]);
    }
}

void cleanLine(char* str) {
    for (int i = 0; i < strlen(str); i++) {
        if (!isalnum(str[i]) && str[i] != '.' && str[i] != ',' && str[i] != '-') {
            str[i] = ' ';
        }
    }
}

void cleanWord(char* str) {
    for (int i = 0; i < strlen(str); i++) {
        if (!isalnum(str[i]) && str[i] != '-') {
            str[i] = '\0';
            break;
        }
    }
}

bool isWord(char* word) {
    for (int i = 0; i < strlen(word); i++) {
        if (!isalpha(word[i])) {
            return false;
        }
    }
    return true;
}

bool isLastWord(char* word) {
    for (int i = 0; i < strlen(word); i++) {
        if (isupper(word[i])) {
            return false;
        }
    }
    return word[strlen(word) - 1] == '.';
}

bool isNameWord(char* word, char* prevWord) {
    if (strlen(prevWord) != 0 && isupper(word[0]) && !isLastWord(prevWord)) {
        return true;
    }
    return false;
}

bool isBanned(char* word) {
    toLower(word);
    for (int i = 0; i < maxWords && bannedWords[i] != NULL; i++) {
        if (strcmp(word, bannedWords[i]) == 0) {
            return true;
        }
    }
    return false;
}

void appendWordPosition(int wordIndex, int lineIndex, int colIndex) {
    // Append to LineIndices and ColIndices array
    for (int i = 0; i < maxLines; i++) {
        if (lineIndices[wordIndex][i] == 0) {
            lineIndices[wordIndex][i] = lineIndex;
            colIndices[wordIndex][i] = colIndex;
            return;
        }
    }
}

void appendToWords(char word[], int lineIndex, int colIndex) {
//    printf("%s - len: %d\n", word, strlen(word));
    for (int i = 0; i < maxWords; i++) {
        if (words[i] == NULL) {
            words[i] = malloc(strlen(word) + 1);
            strcpy(words[i], word);
            appendWordPosition(i, lineIndex, colIndex);
            return;
        } else if (strcmp(words[i], word) == 0){
            appendWordPosition(i, lineIndex, colIndex);
            return;
        }
    }
}

int numberOfOccurences(int wordIndex) {
    int count = 0;
    for (int i = 0; i < maxLines; ++i) {
        if (lineIndices[wordIndex][i] != 0) {
            count++;
        } else {
            break;
        }
    }
    return count;
}

void readFileAndConvertToTokens(char* fileName, int numLines) {
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL) {
        printf("Cannot open file %s\n", fileName);
        return;
    }

    int wordIndex = 0;
    char word[maxWordLength];
    while (fscanf(fp, "%s", word) != EOF) {
        bannedWords[wordIndex] = malloc(strlen(word) * sizeof(char));
        strcpy(bannedWords[wordIndex], word);
        wordIndex++;
    }

    fclose(fp);

}

void buildTable(void) {
    readFileAndConvertToTokens(STOPW_PATH, maxWords);
    FILE *fp = fopen(VAN_BAN_PATH, "r");
    if (fp == NULL) {
        printf("Could not open file %s", VAN_BAN_PATH);
        return;
    }

    int lineIndex = 1;
    char line[maxCols];
    while(fgets(line, maxCols, fp) != NULL) {
        cleanLine(line);
        
        char prevWord[maxWordLength];
        prevWord[0] = '\0';
        char word[maxWordLength];
        
        int colIndex = 1;
        int k = 0;
        for (int i = 0; i < strlen(line); ++i) {
            if (line[i] == ' ') {
                word[k] = '\0';
                char cleanedWord[maxWordLength];
                strcpy(cleanedWord, word);
                cleanWord(cleanedWord);

                if (strlen(cleanedWord) != 0 && isWord(cleanedWord) && !isNameWord(cleanedWord, prevWord) && !isBanned(cleanedWord)) {
                    appendToWords(cleanedWord, lineIndex, colIndex);
                }

                colIndex = i + 2;
                k = 0;
                strcpy(prevWord, word);
            } else {
                word[k] = line[i];
                k++;
            }
        }
        lineIndex++;
    }

    fclose(fp);
}

void swap(int i, int j) {
    char* temp = words[i];
    words[i] = words[j];
    words[j] = temp;
    
    for (int k = 0; k < maxLines; ++k) {
        int tempNum = lineIndices[i][k];
        lineIndices[i][k] = lineIndices[j][k];
        lineIndices[j][k] = tempNum;
    }
    
    for (int k = 0; k < maxCols; ++k) {
        int tempNum = colIndices[i][k];
        colIndices[i][k] = colIndices[j][k];
        colIndices[j][k] = tempNum;
    }
}

void sortTable(void) {
    for (int i = 0; i < maxWords && words[i] != NULL; i++) {
        for (int j = i + 1; j < maxWords && words[j] != NULL; j++) {
            if (strcmp(words[j], words[i]) < 0) {
                swap(i, j);
            }
        }
    }
}

void printTable(void) {
    for (int i = 0; i < maxWords; i++) {
        if (words[i] == NULL) {
            break;
        } else {
            printf("%-20s (occurs: %3d) \t==>\t ", words[i], numberOfOccurences(i));
            for (int j = 0; j < maxLines; ++j) {
                if (lineIndices[i][j] == 0) {
                    break;
                }
                printf("(%d, %d) ", lineIndices[i][j], colIndices[i][j]);
            }
            printf("\n");
        }
    }
}

int main(int argc, const char * argv[]) {
    buildTable();
    sortTable();
    printTable();
    
    return 0;
}

