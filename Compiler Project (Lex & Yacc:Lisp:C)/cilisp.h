/**
* Name: Luis Torres
* Compiler Project
* Date: 10/26/16
**/

#ifndef __cilisp_h_
#define __cilisp_h_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "cilisp.tab.h"

int yyparse(void);
int yylex(void);
void yyerror(char *);

typedef enum { NUM_TYPE, FUNC_TYPE, SYM_TYPE, LIST_TYPE, ELEM_TYPE, LET_STMT_TYPE} AST_NODE_TYPE;

typedef struct
{
    double value;
} NUMBER_AST_NODE;

typedef struct
{
   char *name;
   struct ast_node *op1;
   struct ast_node *op2;
} FUNCTION_AST_NODE;

typedef struct symbol_ast_node
{
   char *name;
} SYMBOL_AST_NODE;

typedef struct list_ast_node
{
   struct ast_node *letList;
   struct ast_node *elem;
} LIST_AST_NODE;

typedef struct element_ast_node
{
   struct ast_node *sym_sign;
   struct ast_node *expr;
} ELEMENT_AST_NODE;

typedef struct scope_ast_node
{
   struct ast_node *symTab; 
   struct scope_ast_node *next;
   struct scope_ast_node *previous;
} SCOPE_AST_NODE;

typedef struct 
{
   struct ast_node *list;
   struct ast_node *expr;
   struct scope_ast_node *scope;
} LET_STMT_AST_NODE;

typedef struct ast_node
{
   AST_NODE_TYPE type;
   union
   {
      NUMBER_AST_NODE number;
      FUNCTION_AST_NODE function;
      SYMBOL_AST_NODE symbol;
      LIST_AST_NODE list;
      ELEMENT_AST_NODE element;
      LET_STMT_AST_NODE letStmt;
   } data;
} AST_NODE;

AST_NODE *number(double value);
AST_NODE *function(char *funcName, AST_NODE *op1, AST_NODE *op2);
AST_NODE *symbol(char *symName);
AST_NODE *let_list(AST_NODE *letList, AST_NODE *elem);
AST_NODE *let_elem(char *sym_sign, AST_NODE *expr);
AST_NODE *let_stmt(AST_NODE *letList, AST_NODE *elem);
void freeNode(AST_NODE *p);
AST_NODE *symbolLookUp(char *symName, SCOPE_AST_NODE *level);
double eval(AST_NODE *ast, SCOPE_AST_NODE *scope);

#endif
