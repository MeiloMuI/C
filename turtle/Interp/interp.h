#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

#define MAXNUMTOKENS 1000
#define MAXTOKENSIZE 20
#define VALIDCOLOURNUM 8
#define OPERATORNUM 5
#define RESULTWIDE 51
#define RESULTHEIGHT 33
#define SETSIZE 3
#define PI 3.1415926
#define LIGHTWIHTE 97
#define STARTVALUEX 30
#define STARTVALUEY 40

struct prog{
  char wds[MAXNUMTOKENS][MAXTOKENSIZE];
  int cw;
  //variable set(used to record the value of variables $A-$Z)
  char vars[26][MAXTOKENSIZE];
  //the number of variable
  int var_num;
  //the integers used to save the information of Set
  int set_count;
  int set_var;
  //the type of output
  int argc;
  //the result 2-D array
  char res[RESULTHEIGHT][RESULTWIDE];
  double y;
  double x;
  double forward;
  double angle;
  char colour;
  FILE* fp;
  double ps_x;
  double ps_y;
};
typedef struct prog Program;

Program* initial_program(int argc);
void command_test(int argc, char** argv);
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
bool isOp(char str[MAXNUMTOKENS]);
void Op(Program *p);

void update_var(Program *p, int ltr, int var_pos);
bool isColour(char str[MAXNUMTOKENS]);
double adjust_angle_value(double angle);
double cos_degree(double degree);
double sin_degree(double degree);
void result_print(Program *p, char filename[]);
void ANSI_print(Program *p);
void file_to_prog(Program *p, char *file_name);

void ps_file_initial(Program* p, char file_name[]);
void get_rgb(Program *p, char colour[]);
