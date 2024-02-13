#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define MAXNUMTOKENS 1000
#define MAXTOKENSIZE 20
#define VALIDCOLOURNUM 8
#define OPERATORNUM 5

struct prog{
  char wds[MAXNUMTOKENS][MAXTOKENSIZE];
  int cw;
};
typedef struct prog Program;

Program* initial_program(void);
void command_test(int arg);
void test(void);
void Prog(Program *p);
void Inslst(Program *p);
void Ins(Program *p);
void Fwd(Program *p);
void Rgt(Program *p);
void Col(Program *p);
void Loop(Program *p);
void Set(Program *p);

void VarNum(Program *p);
bool isNum(char str[]);
void Num(Program *p);
void Var(Program *p);
void Word_Col(Program *p);
void Ltr(Program *p);
void Lst(Program *p);
void Items(Program *p);
void Item(Program *p);
void Word(Program *p);
void Pfix(Program *p);
bool isOp(Program *p);
void Op(Program *p);
