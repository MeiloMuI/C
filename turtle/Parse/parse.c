#include "parse.h"
#define FILE_ONE "./Testing/parsertest1.ttl"

int main(int argc, char* argv[]){
  command_test(argc);
  //test();
  Program* prog = initial_program();

  FILE* fp = fopen(argv[argc-1], "r");
  if(fp == NULL){
    fprintf(stderr, "Cannot open the file.\n");
    exit(EXIT_FAILURE);
  }
  int count = 0;
  while(fscanf(fp, "%s", prog->wds[count]) == 1){
    count++;
    if(count >= MAXNUMTOKENS){
      fprintf(stderr, "Too many tokens to read.\n");
      exit(EXIT_FAILURE);
    }
  }
  fclose(fp);
  Prog(prog);
  free(prog);
  return 0;
}

Program* initial_program(void){
  Program* prog = (Program*) calloc(1, sizeof(Program));
  if(prog == NULL){
    fprintf(stderr, "Cannot calloc the Program.\n");
    exit(EXIT_FAILURE);
  }
  prog->cw = 0;
  return prog;
}

void command_test(int argc){
  if(argc != 2){
    fprintf(stderr, "Retry it using a file as argv[1].\n");
    exit(EXIT_FAILURE);
  }
}

void Prog(Program *p){
  if(strcmp(p->wds[p->cw], "START") != 0){
    fprintf(stderr, "Error No START statement occurred\n");
    exit(EXIT_FAILURE);
  }
  p->cw = p->cw + 1;
  Inslst(p);
}

void Inslst(Program *p){
  if(strcmp(p->wds[p->cw], "END") == 0){
    return;
  }
  Ins(p);
  p->cw = p->cw + 1;
  Inslst(p);
}

void Ins(Program *p){
  if(strcmp(p->wds[p->cw], "FORWARD") == 0){
    Fwd(p);
  } else if (strcmp(p->wds[p->cw], "RIGHT") == 0){
    Rgt(p);
  } else if (strcmp(p->wds[p->cw], "COLOUR") == 0){
    Col(p);
  } else if (strcmp(p->wds[p->cw], "LOOP") == 0){
    Loop(p);
  } else if (strcmp(p->wds[p->cw], "SET") == 0){
    Set(p);
  } else {
    fprintf(stderr, "Error No correct INS statement occurred\n");
    exit(EXIT_FAILURE);
  }
}

void Fwd(Program *p){
  p->cw = p->cw + 1;
  VarNum(p);
}

void Rgt(Program *p){
  p->cw = p->cw + 1;
  VarNum(p);
}

void Col(Program *p){
  p->cw = p->cw + 1;
  //VAR or WORD
  if(p->wds[p->cw][0] == '$'){
    Var(p);
  } else {
    Word_Col(p);
  }
}

void Loop(Program *p){
  p->cw = p->cw + 1;
  Ltr(p);
  p->cw = p->cw + 1;
  if(strcmp(p->wds[p->cw], "OVER") != 0){
    fprintf(stderr, "Error No correct OVER statement occurred\n");
    exit(EXIT_FAILURE);
  }
  p->cw = p->cw + 1;
  Lst(p);
  p->cw = p->cw + 1;
  Inslst(p);
}

void Set(Program *p){
  p->cw = p->cw + 1;
  Ltr(p);
  p->cw = p->cw + 1;
  if(strcmp(p->wds[p->cw], "(") != 0){
    fprintf(stderr, "Error No '(' statement in SET occurred\n");
    exit(EXIT_FAILURE);
  }
  p->cw = p->cw + 1;
  Pfix(p);
}

void VarNum(Program *p){
  //var or num
  if(p->wds[p->cw][0] == '$'){
    Var(p);
  } else if (isNum(p->wds[p->cw])){
    Num(p);
  } else {
    fprintf(stderr, "Error No correct VarNum statement occurred\n");
    exit(EXIT_FAILURE);
  }
}

bool isNum(char str[]){
  if(str == NULL){
    return false;
  }
  int i = 0;
  //skip '-' when the number is negative
  if(str[i] == '-'){
    i++;
  }
  //To ensure 0 before '.'
  if(str[i] == '.'){
    return false;
  }
  //if string only has a '-' or it is ''
  if(str[i] == '\0'){
    return false;
  }
  int dotCount = 0;
  //number has 0 or 1 dot.
  while(str[i] != '\0'){
    if(!isdigit(str[i])){
      if(str[i] == '.'){
        dotCount++;
        if(dotCount > 1){
          return false;
        }
      } else {
        return false;
      }
    }
    i++;
  }
  return true;
}

void Num(Program *p){
  if(isNum(p->wds[p->cw])){
    return;
  }
}

void Var(Program *p){
  if(p->wds[p->cw][0] != '$'){
    fprintf(stderr, "Error No correct Var statement occurred($A.$B...$Z)\n");
    exit(EXIT_FAILURE);
  }
  if(!isupper(p->wds[p->cw][1])){
    fprintf(stderr, "Error No correct Var statement occurred($A.$B...$Z)\n");
    exit(EXIT_FAILURE);
  }
}

void Word_Col(Program *p){
  //if the word is a valid colour
  char colours[VALIDCOLOURNUM][MAXTOKENSIZE] = {"\"BLACK\"", "\"RED\"", "\"GREEN\"", "\"BLUE\"", "\"YELLOW\"", "\"CYAN\"", "\"MAGENTA\"", "\"WHITE\""};
  for(int i = 0; i < VALIDCOLOURNUM; i++){
    if(strcmp(colours[i], p->wds[p->cw]) == 0){
      return;
    }
  }
  fprintf(stderr, "Error No valid colour statement in word occurred\n");
  exit(EXIT_FAILURE);
}

void Ltr(Program *p){
  if(!isupper(p->wds[p->cw][0])){
    fprintf(stderr, "Error No correct Ltr statement occurred(A, B, C... Z)\n");
    exit(EXIT_FAILURE);
  }
}

void Lst(Program *p){
  if(strcmp(p->wds[p->cw], "{") != 0){
    fprintf(stderr, "Error No '{' statement occurred\n");
    exit(EXIT_FAILURE);
  }
  p->cw = p->cw + 1;
  Items(p);
}

void Items(Program *p){
  if(strcmp(p->wds[p->cw], "}") == 0){
    return;
  }
  Item(p);
  p->cw = p->cw + 1;
  Items(p);
}

void Item(Program *p){
  if(p->wds[p->cw][0] == '"'){
    Word(p);
  } else {
    VarNum(p);
  }
}

void Word(Program *p){
  int length = strlen(p->wds[p->cw]);
  //A word has double-quotes around it.
  if(p->wds[p->cw][0] != '"' || p->wds[p->cw][length-1] != '"'){
    fprintf(stderr, "Error No correct WORD statement occurred\n");
    exit(EXIT_FAILURE);
  }
}

void Pfix(Program *p){
  if(strcmp(p->wds[p->cw], ")") == 0){
    return;
  }
  if(isOp(p)){
    Op(p);
  } else {
    VarNum(p);
  }
  p->cw = p->cw + 1;
  Pfix(p);
}

bool isOp(Program *p){
  if(strlen(p->wds[p->cw]) != 1){
    return false;
  }
  char c = p->wds[p->cw][0];
  char operator[OPERATORNUM] = {'=', '+', '-', '/', '*'};
  for(int i = 0; i < OPERATORNUM; i++){
    if(operator[i] == c){
      return true;
    }
  }
  return false;
}

void Op(Program *p){
  if(strcmp(p->wds[p->cw], "=") == 0){
    return;
  }
  if(strcmp(p->wds[p->cw], "+") == 0){
    return;
  }
  if(strcmp(p->wds[p->cw], "-") == 0){
    return;
  }
  if(strcmp(p->wds[p->cw], "/") == 0){
    return;
  }
  if(strcmp(p->wds[p->cw], "*") == 0){
    return;
  }
  fprintf(stderr, "Error No correct OP statement occurred\n");
  exit(EXIT_FAILURE);
}

void test(void){
  Program* testProgram = initial_program();
  //test Program* initial_program(void);
  assert(testProgram != NULL);
  //use 
  //    START
  //    END
  //to test if Prog() will call Inslst() and return successfully.
  strcpy(testProgram->wds[0], "START");
  strcpy(testProgram->wds[1], "END");
  Prog(testProgram);
  assert(testProgram->cw == 1);
  //use 
  //    START
  //      FORWARD 5
  //    END
  //To test if Prog() will call Ins() successfully.
  //And to test if Ins() will call Fwd(), which will call the VarNum() 
  //and then call the Num() in the VarNum(), and return successfully.
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "START");
  strcpy(testProgram->wds[1], "FORWARD");
  strcpy(testProgram->wds[2], "5");
  strcpy(testProgram->wds[3], "END");
  Prog(testProgram);
  assert(testProgram->cw == 3);
  //use 
  //    START
  //      FORWARD $A
  //    END
  //To test if Prog() will call Ins() successfully.
  //And to test if Ins() will call Fwd(), which will call the VarNum() 
  //and then call the Var() in the VarNum(), and return successfully.
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "START");
  strcpy(testProgram->wds[1], "FORWARD");
  strcpy(testProgram->wds[2], "$A");
  strcpy(testProgram->wds[3], "END");
  Prog(testProgram);
  assert(testProgram->cw == 3);
  //use 
  //    START
  //      RIGHT 45
  //    END
  //To test if Ins() will call Rgt() and return successfully.
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "START");
  strcpy(testProgram->wds[1], "RIGHT");
  strcpy(testProgram->wds[2], "90");
  strcpy(testProgram->wds[3], "END");
  Prog(testProgram);
  assert(testProgram->cw == 3);
  //use 
  //    START
  //      COLOUR BLUE
  //    END
  //To test if Ins() will call Col(), which will call Word_Col() and then return successfully.
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "START");
  strcpy(testProgram->wds[1], "COLOUR");
  strcpy(testProgram->wds[2], "\"BLUE\"");
  strcpy(testProgram->wds[3], "END");
  Prog(testProgram);
  assert(testProgram->cw == 3);
  //use 
  //    START
  //      COLOUR $C
  //    END
  //To test if Ins will call Col(), which will call Var() and then return successfully.
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "START");
  strcpy(testProgram->wds[1], "COLOUR");
  strcpy(testProgram->wds[2], "$C");
  strcpy(testProgram->wds[3], "END");
  Prog(testProgram);
  assert(testProgram->cw == 3);
  //use 
  //    START
  //      SET A ( 0 )
  //      SET B ( $A 1 + ) 
  //    END
  //To test if Ins() will call Set(), which will call Ltr() and Pfix(), and then will call Op() and 
  //VarNum() successfully. The Pfix() will call itself recursively.
  //And to test if Inslst() will be called many times recursively and successfully.
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "START");
  strcpy(testProgram->wds[1], "SET");
  strcpy(testProgram->wds[2], "A");
  strcpy(testProgram->wds[3], "(");
  strcpy(testProgram->wds[4], "3");
  strcpy(testProgram->wds[5], ")");
  strcpy(testProgram->wds[6], "SET");
  strcpy(testProgram->wds[7], "B");
  strcpy(testProgram->wds[8], "(");
  strcpy(testProgram->wds[9], "$A");
  strcpy(testProgram->wds[10], "1");
  strcpy(testProgram->wds[11], "+");
  strcpy(testProgram->wds[12], ")");
  strcpy(testProgram->wds[13], "END");
  Prog(testProgram);
  assert(testProgram->cw == 13);
  //To test if Item() will call Word() and VarNum() and return successfully.
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "$A");
  Item(testProgram);
  strcpy(testProgram->wds[0], "3");
  Item(testProgram);
  strcpy(testProgram->wds[0], "\"TEST\"");
  Item(testProgram);
  //To test if VarNum() will call Var() and Num() and return successfully.
  strcpy(testProgram->wds[0], "$A");
  VarNum(testProgram);
  strcpy(testProgram->wds[0], "124");
  VarNum(testProgram);
  //To test if Word() will be called and return successfully.
  strcpy(testProgram->wds[0], "\"TESTWORD\"");
  Word(testProgram);
  //To test if Word_Col() will be called and return successfully.
  strcpy(testProgram->wds[0], "\"YELLOW\"");
  Word_Col(testProgram);
  //Test isNum()
  assert(isNum("7"));
  assert(isNum("-7"));
  assert(isNum("7.76463"));
  assert(isNum("-7.76463"));
  assert(!isNum("asdfsd4"));
  assert(!isNum(".7"));
  assert(!isNum("-"));
  assert(!isNum(""));
  assert(!isNum("-.4687"));
  assert(!isNum("-."));
  //Test isOp()
  strcpy(testProgram->wds[0], "+");
  assert(isOp(testProgram));
  strcpy(testProgram->wds[0], "+--");
  assert(!isOp(testProgram));
  strcpy(testProgram->wds[0], "+1321");
  assert(!isOp(testProgram));
  //Test Var()
  strcpy(testProgram->wds[0], "$L");
  Var(testProgram);
  //Test Ltr()
  strcpy(testProgram->wds[0], "L");
  Ltr(testProgram);
  //Test Op()
  strcpy(testProgram->wds[0], "*");
  Op(testProgram);
  //use 
  //    START
  //      LOOP A OVER { 1 2 3 4 }
  //        FORWARD 3
  //        RIGHT 45
  //      END  
  //    END
  //To test if Ins() will call Loop() successfully.
  //To test if Loop() will call Ltr(), Lst() and Inslst() successfully.
  //To test if Lst() will call Items() successfully.
  //To test if Items() will call Item() and call itself successfully.
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "START");
  strcpy(testProgram->wds[1], "LOOP");
  strcpy(testProgram->wds[2], "A");
  strcpy(testProgram->wds[3], "OVER");
  strcpy(testProgram->wds[4], "{");
  strcpy(testProgram->wds[5], "1");
  strcpy(testProgram->wds[6], "2");
  strcpy(testProgram->wds[7], "3");
  strcpy(testProgram->wds[8], "4");
  strcpy(testProgram->wds[9], "}");
  strcpy(testProgram->wds[10], "FORWARD");
  strcpy(testProgram->wds[11], "3");
  strcpy(testProgram->wds[12], "RIGHT");
  strcpy(testProgram->wds[13], "45");
  strcpy(testProgram->wds[14], "END");
  strcpy(testProgram->wds[15], "END");
  Prog(testProgram);
  assert(testProgram->cw == 15);
  //use 
  //    START
  //      LOOP C OVER { "BLUE" "RED" "BLACK" }
  //        COLOUR $C
  //        FORWARD 5
  //      END  
  //    END
  //A test just like the former.
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "START");
  strcpy(testProgram->wds[1], "LOOP");
  strcpy(testProgram->wds[2], "A");
  strcpy(testProgram->wds[3], "OVER");
  strcpy(testProgram->wds[4], "{");
  strcpy(testProgram->wds[5], "\"BLUE\"");
  strcpy(testProgram->wds[6], "\"RED\"");
  strcpy(testProgram->wds[7], "\"BLACK\"");
  strcpy(testProgram->wds[8], "\"CYAN\"");
  strcpy(testProgram->wds[9], "}");
  strcpy(testProgram->wds[10], "COLOUR");
  strcpy(testProgram->wds[11], "$C");
  strcpy(testProgram->wds[12], "FORWARD");
  strcpy(testProgram->wds[13], "5");
  strcpy(testProgram->wds[14], "END");
  strcpy(testProgram->wds[15], "END");
  Prog(testProgram);
  assert(testProgram->cw == 15);
  //To test the parser can read commands from the .txt file successfully with fscanf()
  //and run the Prog() successfully.
  testProgram->cw = 0;
  FILE* test_fp = fopen(FILE_ONE, "r");
  if(test_fp == NULL){
    fprintf(stderr, "Cannot open the file in test().\n");
    exit(EXIT_FAILURE);
  }
  int count = 0;
  while(fscanf(test_fp, "%s", testProgram->wds[count]) == 1){
    count++;
    if(count >= MAXNUMTOKENS){
      fprintf(stderr, "Too many tokens to read in test().\n");
      exit(EXIT_FAILURE);
    }
  }
  fclose(test_fp);
  Prog(testProgram);
  assert(testProgram->cw == 19);
  
  free(testProgram);
}
