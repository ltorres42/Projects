/**
* Name: Luis Torres
* Compiler Project
* Date: 10/26/16
**/

/*

s-expressions calculator

program ::= program s-expr

s-expr ::= number | ( func s-expr ) | ( func s-expr s-expr )

number ::= [ + | - ] digit+ [ . digit+ ] 

digit ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9

*/

%{
#include "cilisp.h"
%}

%union
{
   double dval;
   char *sval;
   struct ast_node *astNode;
};

%token <sval> FUNC
%token <dval> NUMBER
%token <sval> SYMBOL
%token LPAREN RPAREN EOL QUIT
%token LET

%type <astNode> s_expr
%type <astNode> let_list
%type <astNode> let_elem

%%

program:/* empty */ { 
                       printf("> ");
                    }
        | program s_expr EOL {
                              // printf("yacc: program expr\n"); 
                              printf("%lf", eval($2, 0));
                              freeNode($2);
                              printf("\n> "); 
                           }
        ;

s_expr:
        NUMBER { 
                  //printf("yacc: NUMBER%lf", $1); 
                  $$ = number($1); 
               }
        | SYMBOL {
                  //printf("yacc: SYMBOL%lf", $1);
                  $$ = symbol($1);
                }
        | LPAREN FUNC s_expr RPAREN { 
                                     // printf("yacc: LPAREN FUNC expr RPAREN\n"); 
                                     $$ = function($2, $3, 0);  
                                     //printf("%s(%lf)", $2, $3);
                                  }
        | LPAREN FUNC s_expr s_expr RPAREN {
                                          // printf("LPAREN FUNC expr expr RPAREN\n"); 
                                          // $$ = calc($2, $3, $4); 
                                          $$ = function($2, $3, $4);
                                       }
        | LPAREN LPAREN LET let_list RPAREN s_expr RPAREN {
                                                          // printf("LPAREN LPAREN LET let_list RPAREN s_expr RPAREN\n");
                                                          // $$ = calc($2, $3, $4);
                                                          $$ = let_stmt($4, $6);
                                                        }
        | QUIT { 
                  //printf("QUIT\n"); 
                  exit(0);
               }
        
        | error { 
                        printf("error\n"); 
                        //printf("> ");
                    }

        ;

let_list:
        let_elem {
                  //printf("yacc: let_list\n");
                  $$ = let_list(0, $1);
                }
        | let_list let_elem {
                            //printf("yacc: let_elem\n");
                            $$ = let_list($1, $2);
                          }
        ;

let_elem:
        LPAREN SYMBOL s_expr RPAREN {
                                    //printf("yacc: LPAREN SYMBOL s_expr RPAREN\n");
                                    $$ = let_elem($2, $3);
                                  }
        ;

%%

