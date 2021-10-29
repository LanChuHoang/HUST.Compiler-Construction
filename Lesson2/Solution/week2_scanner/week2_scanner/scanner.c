/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];



/***************************************************************/

void skipBlank(void) {
    while (currentChar != EOF && charCodes[currentChar] == CHAR_SPACE) {
        readChar();
    }
}

void skipComment(void) {
    // Note: currentChar = '*'
    int isEndOfComment = 0;
    int prevChar = 0;
    readChar();
    while (currentChar != EOF && !isEndOfComment) {
        if (charCodes[currentChar] == CHAR_RPAR && charCodes[prevChar] == CHAR_TIMES) {
            isEndOfComment = 1;
            break;
        }
        prevChar = currentChar;
        readChar();
    }
    if (!isEndOfComment) {
        error(ERR_ENDOFCOMMENT, lineNo, colNo);
    } else {
        readChar();
    }
}

Token* readIdentKeyword(void) {
    // Read token and store it
    Token* token = makeToken(TK_IDENT, lineNo, colNo);
    int i = 0;
    while (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT) {
        token->string[i] = currentChar;
        readChar();
        i++;
    }
    token->string[i] = '\0';
    // Check the length of the token and check wether token is keyword or not
    if (i >= MAX_IDENT_LEN) {
        error(ERR_IDENTTOOLONG, lineNo, colNo);
    } else {
        TokenType type = checkKeyword(token->string);
        if (type != TK_NONE) {
            token->tokenType = type;
        }
    }
    return token;
}

Token* readNumber(void) {
    Token* token = makeToken(TK_NUMBER, lineNo, colNo);
    int i = 0;
    while (charCodes[currentChar] == CHAR_DIGIT) {
        token->string[i] = currentChar;
        readChar();
        i++;
    }
    token->string[i] = '\0';
    token->value = atoi(token->string);
    return token;
}

Token* readConstChar(void) {
    // Note: currentChar = '
    Token* token = makeToken(TK_CHAR, lineNo, colNo);
    
    // Read the next char
    // If the next char is EOF -> ERROR: ERR_INVALIDCHARCONSTANT
    readChar();
    if (currentChar == EOF) {
        token->tokenType = TK_NONE;
        error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
        return token;
    }
    // Else -> store the char
    token->string[0] = currentChar;
    token->string[1] = '\0';
    
    // Read the next char
    // Check wether it is ' or not
    readChar();
    if (charCodes[currentChar] == CHAR_SINGLEQUOTE) {
        readChar();
    } else {
        token->tokenType = TK_NONE;
        error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    }
    return token;
}

Token* getToken(void) {
    Token *token;
    
    if (currentChar == EOF) {
        return makeToken(TK_EOF, lineNo, colNo);
    }
    
    switch (charCodes[currentChar]) {
        case CHAR_SPACE: skipBlank(); return getToken();
        case CHAR_LETTER: return readIdentKeyword();
        case CHAR_DIGIT: return readNumber();
        case CHAR_SINGLEQUOTE: return readConstChar();
            
        // Group 1: Symbols that have 1 case
        case CHAR_PLUS:             // "+"
            token = makeToken(SB_PLUS, lineNo, colNo);
            break;
        case CHAR_MINUS:            // "-"
            token = makeToken(SB_MINUS, lineNo, colNo);
            break;
        case CHAR_TIMES:            // "*"
            token = makeToken(SB_TIMES, lineNo, colNo);
            break;
        case CHAR_SLASH:            // "/"
            token = makeToken(SB_SLASH, lineNo, colNo);
            break;
        case CHAR_EQ:               // "="
            token = makeToken(SB_EQ, lineNo, colNo);
            break;
        case CHAR_COMMA:            // ","
            token = makeToken(SB_COMMA, lineNo, colNo);
            break;
        case CHAR_SEMICOLON:        // ";"
            token = makeToken(SB_SEMICOLON, lineNo, colNo);
            break;
        case CHAR_RPAR:             // ")"
            token = makeToken(SB_RPAR, lineNo, colNo);
            break;
            
        // Group 2: Symbols that have n cases
        case CHAR_LT:               // "<" or "<="
            token = makeToken(SB_LT, lineNo, colNo);
            readChar();
            if (charCodes[currentChar] == CHAR_EQ) {
                // "<="
                token->tokenType = SB_LE;
                readChar();
            }
            return token;
        case CHAR_GT:               // ">" or ">="
            token = makeToken(SB_GT, lineNo, colNo);
            readChar();
            if (charCodes[currentChar] == CHAR_EQ) {
                // ">="
                token->tokenType = SB_GE;
                readChar();
            }
            return token;
        case CHAR_PERIOD:           // "." or ".)"
            token = makeToken(SB_PERIOD, lineNo, colNo);
            readChar();
            if (charCodes[currentChar] == CHAR_RPAR) {
                // ".)"
                token->tokenType = SB_RSEL;
                readChar();
            }
            return token;
        case CHAR_COLON:            // ":" or ":="
            token = makeToken(SB_COLON, lineNo, colNo);
            readChar();
            if (charCodes[currentChar] == CHAR_EQ) {
                // ":="
                token->tokenType = SB_ASSIGN;
                readChar();
            }
            return token;
        case CHAR_EXCLAIMATION:     // "!" or "!="
            token = makeToken(TK_NONE, lineNo, colNo);
            readChar();
            if (charCodes[currentChar] == CHAR_EQ) {
                // "!="
                token->tokenType = SB_NEQ;
                readChar();
            } else {
                // invalid "!"
                error(ERR_INVALIDSYMBOL, token->lineNo, token->colNo);
            }
            return token;
        case CHAR_LPAR:             // "(" or "(." or "(*"
            token = makeToken(SB_LPAR, lineNo, colNo);
            readChar();
            switch (charCodes[currentChar]) {
                case CHAR_PERIOD:   // "(."
                    token->tokenType = SB_LSEL;
                    break;
                case CHAR_TIMES:    // "(*"
                    skipComment();
                    return getToken();
                default:            // "("
                    return token;
            }
            break;
        
        default:
            token = makeToken(TK_NONE, lineNo, colNo);
            error(ERR_INVALIDSYMBOL, lineNo, colNo);
            break;
    }
    readChar();
    return token;
}


/******************************************************************/

void printToken(Token *token) {
    
    printf("%d-%d:", token->lineNo, token->colNo);
    
    switch (token->tokenType) {
        case TK_NONE: printf("TK_NONE\n"); break;
        case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
        case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
        case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
        case TK_EOF: printf("TK_EOF\n"); break;
            
        case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
        case KW_CONST: printf("KW_CONST\n"); break;
        case KW_TYPE: printf("KW_TYPE\n"); break;
        case KW_VAR: printf("KW_VAR\n"); break;
        case KW_INTEGER: printf("KW_INTEGER\n"); break;
        case KW_CHAR: printf("KW_CHAR\n"); break;
        case KW_ARRAY: printf("KW_ARRAY\n"); break;
        case KW_OF: printf("KW_OF\n"); break;
        case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
        case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
        case KW_BEGIN: printf("KW_BEGIN\n"); break;
        case KW_END: printf("KW_END\n"); break;
        case KW_CALL: printf("KW_CALL\n"); break;
        case KW_IF: printf("KW_IF\n"); break;
        case KW_THEN: printf("KW_THEN\n"); break;
        case KW_ELSE: printf("KW_ELSE\n"); break;
        case KW_WHILE: printf("KW_WHILE\n"); break;
        case KW_DO: printf("KW_DO\n"); break;
        case KW_FOR: printf("KW_FOR\n"); break;
        case KW_TO: printf("KW_TO\n"); break;
            
        case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
        case SB_COLON: printf("SB_COLON\n"); break;
        case SB_PERIOD: printf("SB_PERIOD\n"); break;
        case SB_COMMA: printf("SB_COMMA\n"); break;
        case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
        case SB_EQ: printf("SB_EQ\n"); break;
        case SB_NEQ: printf("SB_NEQ\n"); break;
        case SB_LT: printf("SB_LT\n"); break;
        case SB_LE: printf("SB_LE\n"); break;
        case SB_GT: printf("SB_GT\n"); break;
        case SB_GE: printf("SB_GE\n"); break;
        case SB_PLUS: printf("SB_PLUS\n"); break;
        case SB_MINUS: printf("SB_MINUS\n"); break;
        case SB_TIMES: printf("SB_TIMES\n"); break;
        case SB_SLASH: printf("SB_SLASH\n"); break;
        case SB_LPAR: printf("SB_LPAR\n"); break;
        case SB_RPAR: printf("SB_RPAR\n"); break;
        case SB_LSEL: printf("SB_LSEL\n"); break;
        case SB_RSEL: printf("SB_RSEL\n"); break;
    }
}

int scan(char *fileName) {
    Token *token;
    
    if (openInputStream(fileName) == IO_ERROR)
        return IO_ERROR;
    
    token = getToken();
    while (token->tokenType != TK_EOF) {
        printToken(token);
        free(token);
        token = getToken();
    }
    
    free(token);
    closeInputStream();
    return IO_SUCCESS;
}

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

void scanAndWriteResult(int number, char* filePath, char* myResultPath) {
    filePath[strlen(filePath) - 5] = number;
    myResultPath[strlen(myResultPath) - 5] = number;
    FILE *fp = freopen(myResultPath, "w", stdout);
    scan(filePath);
    fclose(fp);
}

int compareLineByLine(char*  fileName1, char* fileName2) {
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
    
    int isEqual = 1;
    char line1[1000];
    char line2[1000];
    while(fgets(line1, 1000, file1) && fgets(line2, 1000, file2)) {
        isEqual = strcmp(line1, line2) == 0;
        if (!isEqual) {
            printf("%s%s-->Not Equal\n\n", line1, line2);
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
    char myResultPath[] = "/Users/lanchu/OneDrive/Hust/20211/Compiler Lab/HUST.Compiler-Construction/Lesson2/Solution/week2_scanner/week2_scanner/test/myResult1.txt";
    char filePath[] = "/Users/lanchu/OneDrive/Hust/20211/Compiler Lab/HUST.Compiler-Construction/Lesson2/Solution/week2_scanner/week2_scanner/test/example7.kpl";
    char solutionPath[] = "/Users/lanchu/OneDrive/Hust/20211/Compiler Lab/HUST.Compiler-Construction/Lesson2/Solution/week2_scanner/week2_scanner/test/result7.txt";
    
//    for(char i = '1'; i <= '7'; ++i) {
//        scanAndWriteResult(i, filePath, myResultPath);
//    }
    
    for(char i = '1'; i <= '7'; ++i) {
        myResultPath[strlen(myResultPath) - 5] = i;
        solutionPath[strlen(solutionPath) - 5] = i;

        printf("\nCompare %c\n", i);
        compareLineByLine(myResultPath, solutionPath);
    }
}

int main(int argc, char *argv[]) {
//    if (argc <= 1) {
//        printf("scanner: no input file.\n");
//        return -1;
//    }
//
//    if (scan(argv[1]) == IO_ERROR) {
//        printf("Can\'t read input file!\n");
//        return -1;
//    }
    test();
    return 0;
}
