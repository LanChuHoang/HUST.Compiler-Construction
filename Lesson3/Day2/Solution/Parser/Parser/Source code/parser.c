/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"

Token *currentToken;
Token *lookAhead;

void scan(void) {
    Token* tmp = currentToken;
    currentToken = lookAhead;
    lookAhead = getValidToken();
    free(tmp);
}

void eat(TokenType tokenType) {
    if (lookAhead->tokenType == tokenType) {
        printToken(lookAhead);
        scan();
    } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void) {
    assert("Parsing a Program ....");
    eat(KW_PROGRAM);
    eat(TK_IDENT);
    eat(SB_SEMICOLON);
    compileBlock();
    eat(SB_PERIOD);
    assert("Program parsed!");
}

void compileBlock(void) {
    assert("Parsing a Block ....");
    if (lookAhead->tokenType == KW_CONST) {
        eat(KW_CONST);
        compileConstDecl();
        compileConstDecls();
        compileBlock2();
    } 
    else compileBlock2();
    assert("Block parsed!");
}

void compileBlock2(void) {
    if (lookAhead->tokenType == KW_TYPE) {
        eat(KW_TYPE);
        compileTypeDecl();
        compileTypeDecls();
        compileBlock3();
    } 
    else compileBlock3();
}

void compileBlock3(void) {
    if (lookAhead->tokenType == KW_VAR) {
        eat(KW_VAR);
        compileVarDecl();
        compileVarDecls();
        compileBlock4();
    } 
    else compileBlock4();
}

void compileBlock4(void) {
    compileSubDecls();
    compileBlock5();
}

void compileBlock5(void) {
    eat(KW_BEGIN);
    compileStatements();
    eat(KW_END);
}

void compileConstDecls(void) {
    // TODO
    if (lookAhead->tokenType == TK_IDENT) {
        compileConstDecl();
        compileConstDecls();
    }
}

void compileConstDecl(void) {
    // TODO
    eat(TK_IDENT);
    eat(SB_EQ);
    compileConstant();
    eat(SB_SEMICOLON);
}

void compileTypeDecls(void) {
    // TODO
    if (lookAhead->tokenType == TK_IDENT) {
        compileTypeDecl();
        compileTypeDecls();
    }
}

void compileTypeDecl(void) {
    // TODO
    eat(TK_IDENT);
    eat(SB_EQ);
    compileType();
    eat(SB_SEMICOLON);
}

void compileVarDecls(void) {
    // TODO
    if (lookAhead->tokenType == TK_IDENT) {
        compileVarDecl();
        compileVarDecls();
    }
}

void compileVarDecl(void) {
    // TODO
    eat(TK_IDENT);
    eat(SB_COLON);
    compileType();
    eat(SB_SEMICOLON);
}

void compileSubDecls(void) {
    assert("Parsing subtoutines ....");
    // TODO
    if (lookAhead->tokenType == KW_FUNCTION) {
        compileFuncDecl();
        compileSubDecls();
    }
    else if (lookAhead->tokenType == KW_PROCEDURE) {
        compileProcDecl();
        compileSubDecls();
    }
    assert("Subtoutines parsed ....");
}

void compileFuncDecl(void) {
    assert("Parsing a function ....");
    // TODO
    eat(KW_FUNCTION);
    eat(TK_IDENT);
    compileParams();
    eat(SB_COLON);
    compileBasicType();
    eat(SB_SEMICOLON);
    compileBlock();
    eat(SB_SEMICOLON);
    assert("Function parsed ....");
}

void compileProcDecl(void) {
    assert("Parsing a procedure ....");
    // TODO
    eat(KW_PROCEDURE);
    eat(TK_IDENT);
    compileParams();
    eat(SB_SEMICOLON);
    compileBlock();
    eat(SB_SEMICOLON);
    assert("Procedure parsed ....");
}

void compileUnsignedConstant(void) {
    // TODO
}

void compileConstant(void) {
    // TODO
    switch (lookAhead->tokenType) {
        case SB_PLUS:
            eat(SB_PLUS);
            compileConstant2();
            break;
        case SB_MINUS:
            eat(SB_MINUS);
            compileConstant2();
            break;
        case TK_CHAR:
            eat(TK_CHAR);
            break;
        default:
            compileConstant2();
            break;
    }
}

void compileConstant2(void) {
    // TODO
    if (lookAhead->tokenType == TK_IDENT || lookAhead->tokenType == TK_NUMBER) {
        eat(lookAhead->tokenType);
    } else {
        error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
    }
}

void compileType(void) {
    // TODO
    switch (lookAhead->tokenType) {
        case KW_INTEGER:
        case KW_CHAR:
        case TK_IDENT:
        case KW_FLOAT:
            eat(lookAhead->tokenType);
            break;
        case KW_ARRAY:
            eat(KW_ARRAY);
            eat(SB_LBRACKET);
            eat(TK_NUMBER);
            eat(SB_RBRACKET);
            eat(KW_OF);
            compileType();
            break;
        default:
            error(ERR_INVALIDTYPE, lookAhead->lineNo, lookAhead->colNo);
            break;
    }
}

void compileBasicType(void) {
    // TODO
    if(lookAhead->tokenType == KW_INTEGER || lookAhead->tokenType == KW_CHAR)
        eat(lookAhead->tokenType);
    else
        error(ERR_INVALIDBASICTYPE, lookAhead->lineNo, lookAhead->colNo);
}

void compileParams(void) {
    // TODO
    if(lookAhead->tokenType == SB_LPAR) {
        eat(SB_LPAR);
        compileParam();
        compileParams2();
        eat(SB_RPAR);
    }
}

void compileParams2(void) {
    // TODO
    if(lookAhead->tokenType == SB_SEMICOLON) {
        eat(SB_SEMICOLON);
        compileParam();
        compileParams2();
    }
}

void compileParam(void) {
    // TODO
    if (lookAhead->tokenType == TK_IDENT) {
        eat(TK_IDENT);
        eat(SB_COLON);
        compileBasicType();
    } else if (lookAhead->tokenType == KW_VAR) {
        eat(KW_VAR);
        eat(TK_IDENT);
        eat(SB_COLON);
        compileBasicType();
    } else {
        error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
    }
}

void compileStatements(void) {
    // TODO
//    assert("Parsing statements");
    compileStatement();
    compileStatements2();
//    assert("Parsed statements");
}

void compileStatements2(void) {
    // TODO
//    assert("Parsing statements 2");
    if(lookAhead->tokenType != KW_END) {
        eat(SB_SEMICOLON);
        compileStatement();
        compileStatements2();
    }
//    assert("Parsed statements 2");
}

void compileStatement(void) {
//    assert("Parsing statement");
    switch (lookAhead->tokenType) {
        case TK_IDENT:
            compileAssignSt();
            break;
        case KW_CALL:
            compileCallSt();
            break;
        case KW_BEGIN:
            compileGroupSt();
            break;
        case KW_IF:
            compileIfSt();
            break;
        case KW_WHILE:
            compileWhileSt();
            break;
        case KW_FOR:
            compileForSt();
            break;
            // EmptySt needs to check FOLLOW tokens
        case SB_SEMICOLON:
        case KW_END:
        case KW_ELSE:
            break;
            // Error occurs
        default:
            error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
            break;
    }
//    assert("Parsed statement");
}

void compileAssignSt(void) {
    assert("Parsing an assign statement ....");
    // TODO
    eat(TK_IDENT);
    compileIndexes();
    eat(SB_ASSIGN);
    compileExpression();
    assert("Assign statement parsed ....");
}

void compileCallSt(void) {
    assert("Parsing a call statement ....");
    // TODO
    eat(KW_CALL);
    eat(TK_IDENT);
    compileArguments();
    assert("Call statement parsed ....");
}

void compileGroupSt(void) {
    assert("Parsing a group statement ....");
    // TODO
    eat(KW_BEGIN);
    compileStatements();
    eat(KW_END);
    assert("Group statement parsed ....");
}

void compileIfSt(void) {
    assert("Parsing an if statement ....");
    eat(KW_IF);
    compileCondition();
    eat(KW_THEN);
    compileStatement();
    compileElseSt();
    assert("If statement parsed ....");
}

void compileElseSt(void) {
    if (lookAhead->tokenType == KW_ELSE) {
        eat(KW_ELSE);
        compileStatement();
    }
}

void compileWhileSt(void) {
    assert("Parsing a while statement ....");
    // TODO
    eat(KW_WHILE);
    compileCondition();
    eat(KW_DO);
    compileStatement();
    assert("While statement parsed ....");
}

void compileForSt(void) {
    assert("Parsing a for statement ....");
    // TODO
    eat(KW_FOR);
    eat(TK_IDENT);
    eat(SB_ASSIGN);
    compileExpression();
    eat(KW_TO);
    compileExpression();
    eat(KW_DO);
    compileStatement();
    assert("For statement parsed ....");
}

void compileArguments(void) {
    // TODO
    if(lookAhead->tokenType == SB_LPAR) {
        eat(SB_LPAR);
        compileExpression();
        compileArguments2();
        eat(SB_RPAR);
    }
}

void compileArguments2(void) {
    // TODO
    if(lookAhead->tokenType == SB_COMMA) {
        eat(SB_COMMA);
        compileExpression();
        compileArguments2();
    }
}

void compileCondition(void) {
    // TODO
    compileExpression();
    compileCondition2();
}

void compileCondition2(void) {
    // TODO
    switch (lookAhead->tokenType) {
        case SB_EQ:
        case SB_NEQ:
        case SB_LE:
        case SB_LT:
        case SB_GE:
        case SB_GT:
            eat(lookAhead->tokenType);
            compileExpression();
            break;
        default:
            error(ERR_INVALIDCOMPARATOR, lookAhead->lineNo, lookAhead->colNo);
            break;
    }
}

void compileExpression(void) {
    assert("Parsing an expression");
    // TODO
    switch (lookAhead->tokenType) {
        case SB_PLUS:
        case SB_MINUS:
            eat(lookAhead->tokenType);
            compileExpression2();
            break;
        default:
            compileExpression2();
            break;
    }
    assert("Expression parsed");
}

void compileExpression2(void) {
    // TODO
    compileTerm();
    compileExpression3();
}

void compileExpression3(void) {
    // TODO
    if(lookAhead->tokenType == SB_PLUS || lookAhead->tokenType == SB_MINUS) {
        eat(lookAhead->tokenType);
        compileTerm();
        compileExpression3();
    }
}

void compileTerm(void) {
    // TODO
    compileFactor();
    compileTerm2();
}

void compileTerm2(void) {
    // TODO
    if(lookAhead->tokenType == SB_TIMES || lookAhead->tokenType == SB_SLASH) {
        eat(lookAhead->tokenType);
        compileFactor();
        compileTerm2();
    }
}

void compileFactor(void) {
    // TODO
    switch (lookAhead->tokenType) {
        case TK_NUMBER:
        case TK_CHAR:
            eat(lookAhead->tokenType);
            break;
        case TK_IDENT:
            eat(TK_IDENT);
            if(lookAhead->tokenType == SB_LBRACKET)
                compileIndexes();
            else if (lookAhead->tokenType == SB_LPAR)
                compileArguments();
            break;
        case SB_LPAR:
            compileExpression();
            eat(SB_RPAR);
            break;
        case TK_FLOAT:
            eat(lookAhead->tokenType);
            break;
        default:
            error(ERR_INVALIDFACTOR, lookAhead->lineNo, lookAhead->colNo);
            break;
    }
}

void compileIndexes(void) {
    // TODO
    if (lookAhead->tokenType == SB_LBRACKET) {
        eat(SB_LBRACKET);
        compileExpression();
        eat(SB_RBRACKET);
        compileIndexes();
    }
}

int compile(char *fileName) {
    if (openInputStream(fileName) == IO_ERROR)
        return IO_ERROR;
    
    currentToken = NULL;
    lookAhead = getValidToken();
    
    compileProgram();
    
    free(currentToken);
    free(lookAhead);
    closeInputStream();
    return IO_SUCCESS;
    
}
