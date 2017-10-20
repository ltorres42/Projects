/**
* Name: Luis Torres
* Compiler Project
* Date: 10/26/16
**/

#include "cilisp.h"

SCOPE_AST_NODE *head;
SCOPE_AST_NODE *tail; 

int main(void)
{
    yyparse();
    return 0;
}

void yyerror(char *s)
{
    fprintf(stderr, "%s\n", s);
}

// find out which function it is
int resolveFunc(char *func)
{
   char *funcs[] = { "neg", "abs", "exp", "sqrt", "add", "sub", "mult", "div", "remainder", "log", "pow", "max", "min", "exp2", "cbrt", "hypot", ""};
   
   int i = 0;
   while (funcs[i][0] !='\0')
   {
      if (!strcmp(funcs[i], func))
         return i;
         
      i++;
   }
   yyerror("invalid function"); // paranoic -- should never happen
   return -1;
}

// create a node for a number
AST_NODE *number(double value)
{
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed size and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(NUMBER_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = NUM_TYPE;
    p->data.number.value = value;

    return p;
}

// create a node for a function
AST_NODE *function(char *funcName, AST_NODE *op1, AST_NODE *op2)
{
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed size and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(FUNCTION_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = FUNC_TYPE;
    p->data.function.name = funcName;
    p->data.function.op1 = op1;
    p->data.function.op2 = op2;

    return p;
}

//create a node for a symbol
AST_NODE *symbol(char *symName)
{
    AST_NODE *p;
    size_t nodeSize;

    //allocate space for the fixed size and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(SYMBOL_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = SYM_TYPE;
    p->data.symbol.name = symName;

    return p;
}

//create a node for a let_list
AST_NODE *let_list(AST_NODE *letList, AST_NODE *elem)
{
    AST_NODE *p;
    size_t nodeSize;

    //allocate space for the fixed size and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(LIST_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = LIST_TYPE;
    p->data.list.letList = letList;
    p->data.list.elem = elem;

    return p;
}

//create a node for a let_elem
AST_NODE *let_elem(char *sym_sign, AST_NODE *expr)
{
    AST_NODE *p;
    size_t nodeSize;

    //allocate space for the fixed size and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(ELEMENT_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = ELEM_TYPE;
    p->data.element.expr = expr;
    p->data.element.sym_sign = symbol(sym_sign);    

    return p;
}

//create a node for a let_stmt
AST_NODE *let_stmt(AST_NODE *list, AST_NODE *expr)
{
    AST_NODE *p;
    size_t nodeSize;

    //allocate space for the fixed size and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(LET_STMT_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = LET_STMT_TYPE;
    p->data.letStmt.list = list;
    p->data.letStmt.expr = expr;   

    SCOPE_AST_NODE *scope = malloc(sizeof(SCOPE_AST_NODE));
   // size_t nodeSize;
    //nodeSize = sizeof(SCOPE_AST_NODE);
    if (scope == NULL)
        yyerror("out of memory");

    //scope->symTab = p->data.letStmt.list;
    //scope->symTab = &(p->data.letStmt.list->data.list);
    scope->symTab = list;
    p->data.letStmt.scope = scope;

    if(!head)
    {
      head = scope;
      tail = scope;
      tail->previous = scope;
    }
    else
    {
      tail->next = scope;
      tail->previous = tail;
      tail = scope;
    }

    return p;
}

// free a node
void freeNode(AST_NODE *p)
{
    if (!p)
       return;
       
    if (p->type == FUNC_TYPE)
    {
        free(p->data.function.name);
        freeNode(p->data.function.op1);
        freeNode(p->data.function.op2);
    }
        
    free (p);
}

double myLog(int base, double value)
{
  if(base == 10)
  {
    return log10(value);
  }
  else if(base == 2)
  {
    return log2(value);
  }
  else
  {
    printf("error: please choose base 10 or 2\n");
    exit(1);
    return 0;
  }
}

AST_NODE *symbolLookUp(char *symName, SCOPE_AST_NODE *level)
{
  AST_NODE *value = NULL;
  LIST_AST_NODE *let_list = &level->symTab->data.list;
  
  do{
    if(strcmp(symName, let_list->elem->data.element.sym_sign->data.symbol.name) == 0) //level->symTab->elem->data.element.sym_sign->data.symbol.name
    {
      value = let_list->elem->data.element.expr;
    }
  }while(let_list->letList != NULL && (let_list = &let_list->letList->data.list));

  return value;
}

double eval(AST_NODE *p, SCOPE_AST_NODE *scope)
{
   if (!p)
      return 0.0;
      
    // char *funcs[] = { "neg", "abs", "exp", "sqrt", "add", "sub", "mult", "div", "remainder", "log", "pow", "max", "min", "exp2", "cbrt", "hypot", ""};
    if(p->type == LET_STMT_TYPE)
    {
      return eval(p->data.letStmt.expr, p->data.letStmt.scope);
    }
    else if(p->type == SYM_TYPE)
    {
      //search scope for symbol use that s_expr for the eval
      AST_NODE *var = symbolLookUp((p->data.symbol.name), scope);

      if(var != NULL)
      {
      return eval(var, scope);
      }
      else
      {
        printf("Compilation error.\n");
        exit(1);
      }
      //searching function for symbolLookUp
      //look at elem keep searching
      //while something.next != null

      //scope->symTab.letList
    }
    else if (p->type == FUNC_TYPE)
    {
      //prefix notation for function
      int index = resolveFunc(p->data.function.name);
      //char operator = ' ';
      //double result = 0.0;

      switch(index)
      {
        case 0: return -1*eval(p->data.function.op1, scope); break;//neg
        case 1: return fabs(eval(p->data.function.op1, scope)); break;//abs
        case 2: return exp(eval(p->data.function.op1, scope)); break;//exp
        case 3: return sqrt(eval(p->data.function.op1, scope)); break;//sqrt
        case 9: return myLog((eval(p->data.function.op1, scope)), (eval(p->data.function.op2, scope)));//log
        case 13: return exp2(eval(p->data.function.op1, scope)); break;//exp2
        case 14: return cbrt(eval(p->data.function.op1, scope)); break; //cbrt
        case 4: return eval(p->data.function.op1, scope)+eval(p->data.function.op2, scope); break;//add 
        case 5: return eval(p->data.function.op1, scope)-eval(p->data.function.op2, scope); break;//sub
        case 6: return eval(p->data.function.op1, scope)*eval(p->data.function.op2, scope); break;//mult
        case 7: return eval(p->data.function.op1, scope)/eval(p->data.function.op2, scope); break;//div
        case 8: return fmod((eval(p->data.function.op1, scope)), (eval(p->data.function.op2, scope))); break;//remainder 
        case 10: return pow((eval(p->data.function.op1, scope)), (eval(p->data.function.op2, scope))); break;//pow
        case 11: return fmax((eval(p->data.function.op1, scope)), (eval(p->data.function.op2, scope))); break;//max
        case 12: return fmin((eval(p->data.function.op1, scope)), (eval(p->data.function.op2, scope))); break;//min
        case 15: return hypot((eval(p->data.function.op1, scope)), (eval(p->data.function.op2, scope))); break;//hypot
      }

    }
    else if (p->type == NUM_TYPE)
    {
      //infix notation for arithmetic operation
      return p->data.number.value;
    }
    
    else 
    {
      return 0.0;
    }
    return 0.0;
}  



double calc(char *func, double op1, double op2)
{
  double result = 0.0;
  int index = resolveFunc(func);

  switch(index)
  {
    case 0: result = (-1) * op1; break;
    case 1: result = fabs(op1); break;
    case 2: result = exp(op1); break;
    case 3: result = sqrt(op1); break;
    case 4: result = op1 + op2; break;
    case 5: result = op1 - op2; break;
    case 6: result = op1 * op2; break;
    case 7: result = op1 / op2; break;
    case 8: result = fmod(op1, op2); break;
    case 9: result = myLog(op1, op2); break;
    case 10: result = pow(op1, op2); break;
    case 11: if(op1 > op2){
                result = op1;
              }
              else{
                result = op2;
              }
              break;
    case 12: if(op1 > op2){
                result = op2;
              }
              else{
                result = op1;
              }
              break;
    case 13: result = exp2(op1); break;
    case 14: result = cbrt(op1); break;
    case 15: result = hypot(op1, op2); break;
    default: break;
  }
   return result;

}  
