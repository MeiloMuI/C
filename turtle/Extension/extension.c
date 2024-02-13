#include "extension.h"


#define RECTSIZE 20
#define COMMANDX 10
#define COMMANDY WHEIGHT-30
#define MILLISECONDDELAY 20

int main(int argc, char* argv[]){
  command_test(argc, argv);
  //test();
  Program* prog = initial_program(argc);
  //char array used to save the input
  char input[MAXNUMTTL][MAXSIZETTL];
  
  int count = 0;

  SDL_Simplewin sw;
  SDL_Rect rectangle;
  rectangle.w = RECTSIZE;
  rectangle.h = RECTSIZE;
  
  Neill_SDL_Init(&sw);
  char command[MAXSIZETTL] = "";
  fntrow fontdata[FNTCHARS][FNTHEIGHT];
  Neill_SDL_ReadFont(fontdata, "mode7.fnt");
  Neill_SDL_ReadFont(prog->fontdata, "mode7.fnt");
  prog->sw = &sw;
  SDL_SetRenderDrawColor(sw.renderer, 255, 255, 255, 255);
  SDL_RenderClear(sw.renderer);
  do{

    SDL_Delay(30);
    
    SDL_StartTextInput();
    
    SDL_Event event;
    
    while(SDL_PollEvent(&event)){
      switch(event.type){
        case SDL_QUIT:
          sw.finished = 1;
          break;
        case SDL_TEXTINPUT:
          strcat(command, event.text.text);
          break;
        case SDL_KEYDOWN:
          if(event.key.keysym.sym == SDLK_BACKSPACE && strlen(command) > 0){
            command[strlen(command) - 1] = '\0';
            SDL_SetRenderDrawColor(sw.renderer, 255, 255, 255, 255);
            SDL_RenderClear(sw.renderer);
          }
          if(event.key.keysym.sym == SDLK_RETURN && strlen(command) > 0){
            strcpy(input[count++], command);
            //Update command statement
            strcpy(command, " ");
            SDL_SetRenderDrawColor(sw.renderer, 255, 255, 255, 255);
            SDL_RenderClear(sw.renderer);
            Neill_SDL_DrawString(&sw, fontdata, command, COMMANDX, COMMANDY);
            strcpy(command, "");
          }
          if(event.key.keysym.sym == SDLK_BACKQUOTE){
            if(count > 0){
              count--;
            }
            strcpy(input[count], "END");
            SDL_SetRenderDrawColor(sw.renderer, 255, 255, 255, 255);
            SDL_RenderClear(sw.renderer);
          }          
          break;
      }
    }

    /* print the input instruction of the user to the screen */
    rectangle.x = COMMANDX;
    rectangle.y = COMMANDY;
    
    Neill_SDL_DrawString(&sw, fontdata, command, rectangle.x, rectangle.y);
    /* print the existing instructions of the user to the screen */
    rectangle.x = 20;
    rectangle.y = 20;
    for(int i = 0; i < count; i++){
      Neill_SDL_DrawString(&sw, fontdata, input[i], rectangle.x, rectangle.y);
      rectangle.y += 20;
    }
    /* restart the Program* */
    free(prog);
    prog = initial_program(argc);
    Neill_SDL_ReadFont(prog->fontdata, "mode7.fnt");
    prog->sw = &sw;
    /* get command from input[][] */
    input_to_prog(prog, input, count);
    prog->cw = 0;
    Prog(prog);
    SDL_StopTextInput();
 
    SDL_SetRenderDrawColor(sw.renderer, 255, 255, 255, 255);
    Neill_SDL_UpdateScreen(&sw);

  }while(!sw.finished);

  /* Clear up graphics subsystems */
  SDL_DestroyTexture(sw.display);
  SDL_DestroyRenderer(sw.renderer);
  SDL_DestroyWindow(sw.win);
  SDL_Quit();
  atexit(SDL_Quit);
  free(prog);
  if(argc == 1){
    result_print_ttl(input, "input.ttl");
  } else {
    result_print_ttl(input, argv[1]);
  }
  return 0;
}

Program* initial_program(int argc){
  Program* prog = (Program*) calloc(1, sizeof(Program));
  if(prog == NULL){
    fprintf(stderr, "Cannot calloc the Program.\n");
    exit(EXIT_FAILURE);
  }
  for(int i = 0; i < MAXNUMTOKENS; i++){
    strcpy(prog->wds[i] ,"END");//copy the "END" to the end.
  }
  prog->cw = 0;
  prog->var_num = -1;//unused when it is -1
  prog->set_count = 0;
  prog->argc = argc;
  prog->colour = 'W';
  prog->x = RESULTWIDE / 2;
  prog->y = RESULTHEIGHT / 2;
  for(int i = 0; i < RESULTHEIGHT; i++){
    for(int j = 0; j < RESULTWIDE; j++){
      prog->res[i][j] = ' '; 
    }
  }
  prog->ps_x = STARTVALUEX;
  prog->ps_y = STARTVALUEY;
  prog->ex_x = EXTENSIONX;
  prog->ex_y = EXTENSIONY;
  return prog;
}

void command_test(int argc, char** argv){
  if(argc != 1 && argc != 2){
    fprintf(stderr, "Retry it using: 1. %s 2. %s <argv1>. <argv1> is the .ttl file you want\n", argv[0], argv[0]);
    exit(EXIT_FAILURE);
  } 
}

void Prog(Program *p){
  if(strcmp(p->wds[p->cw], "START") != 0){
    Neill_SDL_DrawString(p->sw, p->fontdata, "Error No START statement occurred", 640, 360);
    //fprintf(stderr, "Error No START statement occurred\n");
    return;
  }
  if(p->fp != NULL){
    fputs("0.2 setlinewidth\n"
          "10 10 scale\n", p->fp);
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
    Neill_SDL_DrawString(p->sw, p->fontdata, "Error No correct INS statement occurred", 640, 360);
    //fprintf(stderr, "Error No correct INS statement occurred\n");
    return;
  }
}

void Fwd(Program *p){
  p->cw = p->cw + 1;
  VarNum(p);
  //pseudo:
  //1. get the x and y from forward and angle
  //2. set the value according to colour, x and y to the result array
  //3. update the x and y in prog

  //1. argc == 2, output the result using ASNI
  //2. argc == 3, output the result using .txt

  //Postscript
  if(p->fp != NULL){
    char code[200];
    sprintf(code, "newpath\n"
          "%lf %lf moveto\n", p->ps_x, p->ps_y);
    fputs(code, p->fp);
    p->ps_x += p->forward * sin_degree(p->angle);
    p->ps_y += p->forward * cos_degree(p->angle);
    //string rgb
    char colour[30];
    get_rgb(p, colour);
    sprintf(code, "%lf %lf lineto\n"
          "%s setrgbcolor\n"
          "stroke\n", p->ps_x, p->ps_y, colour);
    fputs(code, p->fp);
  }
  //Interpreter
  for(int i = 0; i < (int)p->forward; i++){
    if((int)round(p->x) >= 0 && (int)round(p->y) >= 0 && (int)round(p->x) < RESULTWIDE && (int)round(p->y) < RESULTHEIGHT){
      p->res[(int)round(p->y)][(int)round(p->x)] = p->colour;
    }
    p->x += sin_degree(p->angle);
    p->y -= cos_degree(p->angle);
    // if(p->x < 0.0 || p->y < 0.0 || (int)round(p->x) >= RESULTWIDE || (int)round(p->y) >= RESULTHEIGHT){
    //   if(p->argc == 2){
    //     neillreset();
    //   }
    //   fprintf(stderr, "Error. The result should be placed in 51x33. Out of Bound.\n");
    //   exit(EXIT_FAILURE);
    // }
  }
  //ANSI  
  // if(p->argc == 2){
  //   ANSI_print(p);
  //   neillbusywait(1.0);//print the result using ANSI and wait 1 second.
  // }
  //extension
  SDL_SetColour(p);
  int tempx = (int)round(p->ex_x);
  int tempy = (int)round(p->ex_y);
  p->ex_x += p->forward * sin_degree(p->angle) * 20;
  p->ex_y -= p->forward * cos_degree(p->angle) * 20;
  SDL_RenderDrawLine(p->sw->renderer, tempx, tempy, (int)round(p->ex_x), (int)round(p->ex_y));
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
  int ltr = p->wds[p->cw][0];
  p->cw = p->cw + 1;
  if(strcmp(p->wds[p->cw], "OVER") != 0){
    Neill_SDL_DrawString(p->sw, p->fontdata, "Error No correct OVER statement occurred", 640, 360);
    //fprintf(stderr, "Error No correct OVER statement occurred\n");
    return;
  }
  p->cw = p->cw + 1;

  int lst_start = p->cw;//"{"

  Lst(p);
  
  int loop_times = p->cw - lst_start - 1;//"}"

  p->cw = p->cw + 1;

  int loop_start = p->cw;
  for(int i = 0; i < loop_times; i++){
    //1. update varibles
    update_var(p, ltr, lst_start + i + 1);
    //2. update p->cw to loop_times
    p->cw = loop_start;
    //3. call Inslst()
    Inslst(p);
  }

}

void Set(Program *p){
  p->cw = p->cw + 1;
  Ltr(p);
  p->set_var = p->wds[p->cw][0] - 'A';//update set_var 
  p->cw = p->cw + 1;
  if(strcmp(p->wds[p->cw], "(") != 0){
    Neill_SDL_DrawString(p->sw, p->fontdata, "Error No '(' statement in SET occurred", 640, 360);
    //fprintf(stderr, "Error No '(' statement in SET occurred\n");
    return;
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
    p->var_num = -1;
  } else {
    Neill_SDL_DrawString(p->sw, p->fontdata, "Error No correct VarNum statement occurred", 640, 360);
    //fprintf(stderr, "Error No correct VarNum statement occurred\n");
    return;
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
  //Rgt
  if(p->cw != 0 && strcmp(p->wds[p->cw - 1], "RIGHT") == 0){
    //adjust the value to 0 <= x < 360.0
    double value = atof(p->wds[p->cw]);
    p->angle = adjust_angle_value(p->angle + value);
  }
  //Fwd
  if(p->cw != 0 && strcmp(p->wds[p->cw - 1], "FORWARD") == 0){
    //set the value to program->forward
    p->forward = atof(p->wds[p->cw]);
  }
}

void Var(Program *p){
  if(p->wds[p->cw][0] != '$'){
    Neill_SDL_DrawString(p->sw, p->fontdata, "Error No correct Var statement occurred($A.$B...$Z)", 640, 360);
    // fprintf(stderr, "Error No correct Var statement occurred($A.$B...$Z)\n");
    return;
  }
  if(!isupper(p->wds[p->cw][1])){
    Neill_SDL_DrawString(p->sw, p->fontdata, "Error No correct Var statement occurred($A.$B...$Z)", 640, 360);
    // fprintf(stderr, "Error No correct Var statement occurred($A.$B...$Z)\n");
    return;
  }
  //A~Z == 0~25
  p->var_num = p->wds[p->cw][1] - 'A';//update prog->var_num
  //Rgt
  if(p->cw != 0 && strcmp(p->wds[p->cw - 1], "RIGHT") == 0){
    if(!isNum(p->vars[p->var_num])){
      Neill_SDL_DrawString(p->sw, p->fontdata, "Error No correct number statement occurred in Var", 640, 360);
      //fprintf(stderr, "Error No correct number statement occurred in Var\n");
      return;
    }
    //adjust the value to 0 <= x < 360.0
    double value = atof(p->vars[p->var_num]);
    p->angle = adjust_angle_value(p->angle + value);
  }
  //Fwd
  if(p->cw != 0 && strcmp(p->wds[p->cw - 1], "FORWARD") == 0){
    if(!isNum(p->vars[p->var_num])){
      Neill_SDL_DrawString(p->sw, p->fontdata, "Error No correct number statement occurred in Var", 640, 360);
      //fprintf(stderr, "Error No correct number statement occurred in Var\n");
      return;
    }
    //set the value to program->forward
    p->forward = atof(p->vars[p->var_num]);
  }
  //Col
  if(p->cw != 0 && strcmp(p->wds[p->cw - 1], "COLOUR") == 0){
    if(strcmp(p->vars[p->var_num], "\"BLACK\"") == 0){
      p->colour = 'K';
    } else if(strcmp(p->vars[p->var_num], "\"RED\"") == 0){
      p->colour = 'R';
    } else if(strcmp(p->vars[p->var_num], "\"GREEN\"") == 0){
      p->colour = 'G';
    } else if(strcmp(p->vars[p->var_num], "\"YELLOW\"") == 0){
      p->colour = 'Y';
    } else if(strcmp(p->vars[p->var_num], "\"BLUE\"") == 0) {
      p->colour = 'B';
    } else if(strcmp(p->vars[p->var_num], "\"MAGENTA\"") == 0) {
      p->colour = 'M';
    } else if(strcmp(p->vars[p->var_num], "\"CYAN\"") == 0) {
      p->colour = 'C';
    } else if(strcmp(p->vars[p->var_num], "\"WHITE\"") == 0) {
      p->colour = 'W';
    } else {
      Neill_SDL_DrawString(p->sw, p->fontdata, "Error No correct colour occurred in Col.", 640, 360);
      //fprintf(stderr, "Error No correct colour occurred in Col.\n");
      return;
    }
  }
}

void Word_Col(Program *p){
  //if the word is a valid colour
  if(isColour(p->wds[p->cw])){
    if(strcmp(p->wds[p->cw], "\"BLACK\"") == 0){
      p->colour = 'K';
    } else if(strcmp(p->wds[p->cw], "\"RED\"") == 0){
      p->colour = 'R';
    } else if(strcmp(p->wds[p->cw], "\"GREEN\"") == 0){
      p->colour = 'G';
    } else if(strcmp(p->wds[p->cw], "\"YELLOW\"") == 0){
      p->colour = 'Y';
    } else if(strcmp(p->wds[p->cw], "\"BLUE\"") == 0) {
      p->colour = 'B';
    } else if(strcmp(p->wds[p->cw], "\"MAGENTA\"") == 0) {
      p->colour = 'M';
    } else if(strcmp(p->wds[p->cw], "\"CYAN\"") == 0) {
      p->colour = 'C';
    } else if(strcmp(p->wds[p->cw], "\"WHITE\"") == 0) {
      p->colour = 'W';
    } 
    return;
  }
  Neill_SDL_DrawString(p->sw, p->fontdata, "Error No valid colour statement in word occurred", 640, 360);
  //fprintf(stderr, "Error No valid colour statement in word occurred\n");
  return;
}

void Ltr(Program *p){
  if(!isupper(p->wds[p->cw][0])){
    Neill_SDL_DrawString(p->sw, p->fontdata, "Error No correct Ltr statement occurred(A, B, C... Z)", 640, 360);
    // fprintf(stderr, "Error No correct Ltr statement occurred(A, B, C... Z)\n");
    return;
  }
}

void Lst(Program *p){
  if(strcmp(p->wds[p->cw], "{") != 0){
    Neill_SDL_DrawString(p->sw, p->fontdata, "Error No '{' statement occurred", 640, 360);
    //fprintf(stderr, "Error No '{' statement occurred\n");
    return;
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
    Neill_SDL_DrawString(p->sw, p->fontdata, "Error No correct WORD statement occurred", 640, 360);
    //fprintf(stderr, "Error No correct WORD statement occurred\n");
    return;
  }
}

void Pfix(Program *p){
  if(strcmp(p->wds[p->cw], ")") == 0){
    if(p->set_count == 0 || p->set_count == 2){
      Neill_SDL_DrawString(p->sw, p->fontdata, "Error No correct Pfix statement occurred. Please input valid Pfix command in Set", 640, 360);
      //fprintf(stderr, "Error No correct Pfix statement occurred. Please input valid Pfix command in Set.\n");
      return;
    }
    if(p->set_count == 1 && p->var_num != -1){
      //bug found, need to test if the p->wds[p->cw - 1] is Var or Num. fixed.
      snprintf(p->vars[p->set_var], MAXTOKENSIZE, "%s", p->vars[p->var_num]);
    } else if (p->set_count == 1 && p->var_num == -1){
      snprintf(p->vars[p->set_var], MAXTOKENSIZE, "%s", p->wds[p->cw - 1]);
    }
    p->set_count = 0;//reset the set_count to 0
    return;
  }
  p->set_count = p->set_count + 1;
  //set_count == 1 || set_count == 2 (VarNum)
  //set_count == 3 (Op)
  if(p->set_count == 3){
    Op(p);
  } else if(p->set_count == 1 || p->set_count == 2) {
    VarNum(p);
    // test if the value of Var is a number
    if(p->var_num != -1){
      if(!isNum(p->vars[p->var_num])){
        Neill_SDL_DrawString(p->sw, p->fontdata, "Error. The value in Pfix is not a number.", 640, 360);
        //fprintf(stderr, "Error. The value in Pfix is not a number.\n");
        return;
      }
    }
  } else {
    Neill_SDL_DrawString(p->sw, p->fontdata, "Error. Interpreter can only understand Set command in the formats such as \"Set Ltr ( VarNum )\" or \"Set Ltr ( VarNum VarNum Op )\"", 640, 360);
    //fprintf(stderr, "Error. Interpreter can only understand Set command in the formats such as \"Set Ltr ( VarNum )\" or \"Set Ltr ( VarNum VarNum Op )\"\n");
    return;
  }
  p->cw = p->cw + 1;
  Pfix(p);
}

bool isOp(char str[MAXNUMTOKENS]){
  if(strlen(str) != 1){
    return false;
  }
  char c = str[0];
  char operator[OPERATORNUM] = {'=', '+', '-', '/', '*'};
  for(int i = 0; i < OPERATORNUM; i++){
    if(operator[i] == c){
      return true;
    }
  }
  return false;
}

void Op(Program *p){
  if(!isOp(p->wds[p->cw])){
    Neill_SDL_DrawString(p->sw, p->fontdata, "Error No correct OP statement occurred in Pfix", 640, 360);
    //fprintf(stderr, "Error No correct OP statement occurred in Pfix\n");
    return;
  }
  if(strcmp(p->wds[p->cw], "=") == 0){
    Neill_SDL_DrawString(p->sw, p->fontdata, "Error. Cannot interpret the \"=\" in Set", 640, 360);
    //fprintf(stderr, "Error. Cannot interpret the \"=\" in Set\n");
    return;
  }
  //get the number before Op
  double num1, num2;
  if(p->cw >= 2 && !isNum(p->wds[p->cw - 2])){
    int ltr = p->wds[p->cw - 2][1] - 'A';
    num1 = atof(p->vars[ltr]);
  } else {
    num1 = atof(p->wds[p->cw - 2]);
  }
  if(p->cw >= 2 && !isNum(p->wds[p->cw - 1])){
    int ltr = p->wds[p->cw - 1][1] - 'A';
    num2 = atof(p->vars[ltr]);
  } else {
    num2 = atof(p->wds[p->cw - 1]);
  }
  if(strcmp(p->wds[p->cw], "+") == 0){
    snprintf(p->vars[p->set_var], MAXTOKENSIZE, "%lf", num1 + num2);
    return;
  }
  if(strcmp(p->wds[p->cw], "-") == 0){
    snprintf(p->vars[p->set_var], MAXTOKENSIZE, "%lf", num1 - num2);
    return;
  }
  if(strcmp(p->wds[p->cw], "/") == 0){
    snprintf(p->vars[p->set_var], MAXTOKENSIZE, "%lf", num1 / num2);
    return;
  }
  if(strcmp(p->wds[p->cw], "*") == 0){
    snprintf(p->vars[p->set_var], MAXTOKENSIZE, "%lf", num1 * num2);
    return;
  }
  Neill_SDL_DrawString(p->sw, p->fontdata, "Error No correct OP statement occurred", 640, 360);
  //fprintf(stderr, "Error No correct OP statement occurred\n");
  return;
}

void update_var(Program *p, int ltr, int var_pos){
  int n = ltr - 'A';
  strcpy(p->vars[n], p->wds[var_pos]);
}

bool isColour(char str[MAXNUMTOKENS]){
  char colours[VALIDCOLOURNUM][MAXTOKENSIZE] = {"\"BLACK\"", "\"RED\"", "\"GREEN\"", "\"BLUE\"", "\"YELLOW\"", "\"CYAN\"", "\"MAGENTA\"", "\"WHITE\""};
  for(int i = 0; i < VALIDCOLOURNUM; i++){
    if(strcmp(colours[i], str) == 0){
      return true;
    }
  }
  return false;
}

double adjust_angle_value(double angle){
  while(angle < 0){
    angle += 360.0;
  }
  while(angle >= 360.0){
    angle -= 360.0;
  }
  return angle;
}

double cos_degree(double degree){
  return cos(degree * PI / 180.0);
}

double sin_degree(double degree){
  return sin(degree * PI / 180.0);
}

void result_print(Program *p, char filename[]){
  FILE *fp = fopen(filename, "w");
  if(fp == NULL){
    Neill_SDL_DrawString(p->sw, p->fontdata, "Error. Cannot write .txt file.", 640, 360);
    //fprintf(stderr, "Error. Cannot write .txt file.\n");
    exit(EXIT_FAILURE);
  }
  for(int i = 0; i < RESULTHEIGHT; i++){
    for(int j = 0; j < RESULTWIDE; j++){
      fprintf(fp, "%c", p->res[i][j]);
    }
    fprintf(fp, "\n");
  }
  fclose(fp);
}

// void ANSI_print(Program *p){
//   neillbgcol(LIGHTWIHTE);
//   neillcursorhome();
//   for(int i = 0; i < RESULTHEIGHT; i++){
//     for(int j = 0; j < RESULTWIDE; j++){
//       switch(p->res[i][j]){
//         case 'K':
//           neillfgcol(black);
//           break;
//         case 'R':
//           neillfgcol(red);
//           break;
//         case 'G':
//           neillfgcol(green);
//           break;
//         case 'Y':
//           neillfgcol(yellow);
//           break;
//         case 'B':
//           neillfgcol(blue);
//           break;
//         case 'M':
//           neillfgcol(magenta);
//           break;
//         case 'C':
//           neillfgcol(cyan);
//           break;
//         case 'W':
//           neillfgcol(white);
//           break;
//         case ' ':
//           neillfgcol(white);
//           break;
//         default:
//           Neill_SDL_DrawString(p->sw, p->fontdata, "Error. Cannot identify the colour when using ANSI", 640, 360);
//           //fprintf(stderr, "Error. Cannot identify the colour when using ANSI.\n");
//           return;
//       }
//       printf("%c", p->res[i][j]);
//     }
//     printf("\n");
//   }
// }

void file_to_prog(Program *p, char *file_name){
  FILE* fp = fopen(file_name, "r");
  if(fp == NULL){
    fprintf(stderr, "Cannot open the file.\n");
    exit(EXIT_FAILURE);
  }
  int count = 0;
  while(fscanf(fp, "%s", p->wds[count]) == 1){
    count++;
    if(count >= MAXNUMTOKENS){
      fprintf(stderr, "Too many tokens to read.\n");
      exit(EXIT_FAILURE);
    }
  }
  fclose(fp);
}

void input_to_prog(Program *p, char input[MAXNUMTTL][MAXSIZETTL], int row_num){
  int word_num = 0;
  char temp[MAXNUMTTL][MAXSIZETTL];
  for(int i = 0; i < row_num; i++){
    strcpy(temp[i], input[i]);
  }
  for(int i = 0; i < row_num; i++){
    char* res = NULL;
    res = strtok(temp[i], " ");
    while(res != NULL && word_num < MAXNUMTOKENS){
      //printf("%s+++\n", res);
      strcpy(p->wds[word_num++], res);
      res = strtok(NULL, " ");
    }
  }
  if(word_num < MAXNUMTOKENS){
    strcpy(p->wds[word_num] ,"END");//copy the "END" to the end.
  }
}

void ps_file_initial(Program* p, char file_name[]){
  FILE* fp = fopen(file_name, "w");
  if(fp == NULL){
    fprintf(stderr, "Cannot write information into the file.\n");
    exit(EXIT_FAILURE);
  }
  p->fp = fp;
}

void get_rgb(Program *p, char colour[]){
  switch(p->colour){
        case 'K':
          strcpy(colour, "0 0 0");
          break;
        case 'R':
          strcpy(colour, "1 0 0");
          break;
        case 'G':
          strcpy(colour, "0 1 0");
          break;
        case 'Y':
          strcpy(colour, "1 1 0");
          break;
        case 'B':
          strcpy(colour, "0 0 1");
          break;
        case 'M':
          strcpy(colour, "1 0 1");
          break;
        case 'C':
          strcpy(colour, "0 1 1");
          break;
        case 'W':
          strcpy(colour, "0.8 0.8 0.8");
          break;
        default:
          Neill_SDL_DrawString(p->sw, p->fontdata, "Error. Cannot identify the colour when using ps function.", 640, 360);
          //fprintf(stderr, "Error. Cannot identify the colour when using ps function.\n");
          return;
  }
}

void SDL_SetColour(Program *p){
  switch(p->colour){
        case 'K':
          SDL_SetRenderDrawColor(p->sw->renderer, 0, 0, 0, 255);
          break;
        case 'R':
          SDL_SetRenderDrawColor(p->sw->renderer, 255, 0, 0, 255);
          break;
        case 'G':
          SDL_SetRenderDrawColor(p->sw->renderer, 0, 255, 0, 255);
          break;
        case 'Y':
          SDL_SetRenderDrawColor(p->sw->renderer, 255, 255, 0, 255);
          break;
        case 'B':
          SDL_SetRenderDrawColor(p->sw->renderer, 0, 0, 255, 255);
          break;
        case 'M':
          SDL_SetRenderDrawColor(p->sw->renderer, 255, 0, 255, 255);
          break;
        case 'C':
          SDL_SetRenderDrawColor(p->sw->renderer, 0, 255, 255, 255);
          break;
        case 'W':
          SDL_SetRenderDrawColor(p->sw->renderer, 210, 210, 210, 255);
          break;
        default:
          Neill_SDL_DrawString(p->sw, p->fontdata, "Error. Cannot identify the colour when using ps function.", 640, 360);
          //fprintf(stderr, "Error. Cannot identify the colour when using ps function.\n");
          return;
  }
} 
void result_print_ttl(char userinput[MAXNUMTTL][MAXSIZETTL], char filename[]){
  FILE *fp = fopen(filename, "w");
  if(fp == NULL){
    fprintf(stderr, "Error. Cannot write .txt file.\n");
    exit(EXIT_FAILURE);
  }
  for(int i = 0; i < MAXNUMTTL; i++){
    for(int j = 0; j < MAXSIZETTL; j++){
      fprintf(fp, "%c", userinput[i][j]);
    }
    fprintf(fp, "\n");
  }
  fclose(fp);
}


void test(void){
  //interpreter test part.

  Program* testProgram = initial_program(3);
  //test Program* initial_program(void);
  //using the argc in the Program to test the Program* is initialised successfully.
  assert(testProgram != NULL);
  assert(testProgram->argc == 3);
  
  //1. test the new functions in interpreter

  //update_var(), test if this function can update the content of variables
  //The update_var() will copy the string in the position(parameter 3) to the array in the Program* according to the parameter 2
  strcpy(testProgram->wds[0], "TEST_UPDATE");
  update_var(testProgram, 'A', 0);
  assert(strcmp(testProgram->vars[0], "TEST_UPDATE") == 0);
  strcpy(testProgram->wds[3], "TEST_UPDATE_2");
  update_var(testProgram, 'D', 3);
  assert(strcmp(testProgram->vars[3], "TEST_UPDATE_2") == 0);
  
  //isColour()
  //The isColour() will output true if the string is the same as eight colours.
  assert(isColour("\"BLACK\""));
  assert(!isColour("135454"));
  assert(isColour("\"CYAN\""));

  //double adjust_angle_value(double angle), 0.0 <= return value < 360.0
  //test if this function can control the return value in the range 0.0~360.0
  assert(adjust_angle_value(7345.0) < 360.0);
  assert(adjust_angle_value(360.0) < 360.0);
  assert(adjust_angle_value(-230.0) > 0.0);
  assert(adjust_angle_value(-729.0) > 0.0);

  //double cos_degree(double degree)
  //test this function using special degrees to ensure the return value is correct.
  assert((int)cos_degree(0.0) == 1);
  assert((int)cos_degree(90.0) == 0);
  assert(cos_degree(59.0) > 0.5);
  assert((int)cos_degree(-90.0) == 0);

  //double sin_degree(double degree)
  //test this function using special degrees to ensure the return value is correct.
  assert((int)sin_degree(0.0) == 0);
  assert((int)round(sin_degree(90.0)) == 1);
  assert(sin_degree(36.0) > 0.5);
  assert((int)round(sin_degree(-90.0)) == -1);

  //void result_print(Program *p, char filename[]), void ANSI_print(Program *p)
  //are tested by running the interpreter.
  
  //void file_to_prog()
  //test this function by reading files from Testing folder, and test if the string in the Program* is correct.
  file_to_prog(testProgram, "./Testing/onlyend.ttl");
  assert(strcmp(testProgram->wds[0], "END") == 0);
  //this function will continue to be tested below

  //void Prog(Program *p)
  //read the onlyend.ttl file which has only one command "END" to ensure the program will printf the error information
  //Prog(testProgram);

  //void void Inslst(Program *p)
  //read the onlystart.ttl file which has only one command "START" to ensure the program will printf the error information
  file_to_prog(testProgram, "./Testing/onlystart.ttl");
  assert(strcmp(testProgram->wds[0], "START") == 0);
  //Inslst(testProgram);

  //test if the Prog() will call Inslst() and return correctly.
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "START");
  strcpy(testProgram->wds[1], "END");
  Prog(testProgram);
  assert(testProgram->cw == 1);

  //void Num(Program *p)
  //using RIGHT 79 and FORWARD 16 to test if this funtion will change the value in the Program*
  testProgram->cw = 1;
  strcpy(testProgram->wds[0], "RIGHT");
  strcpy(testProgram->wds[1], "79");
  Num(testProgram);
  assert((int)testProgram->angle == 79);
  strcpy(testProgram->wds[0], "FORWARD");
  strcpy(testProgram->wds[1], "16");
  Num(testProgram);
  assert((int)testProgram->forward == 16);

  free(testProgram);
  testProgram = initial_program(3);

  //void Var(Program *p)
  //1. The Var() will output the error information if p->wds[p->cw] is not a variable.
  // strcpy(testProgram->wds[0], "A");
  // Var(testProgram);
  // strcpy(testProgram->wds[0], "$a");
  // Var(testProgram);
  //2. Using RIGHT $R, FORWARD $F, COLOUR $C to test if Var() will change the value in the Program*
  testProgram->cw = 1;
  strcpy(testProgram->vars['T'-'A'], "trash");
  strcpy(testProgram->vars['R'-'A'], "102");
  strcpy(testProgram->vars['F'-'A'], "77");
  strcpy(testProgram->vars['C'-'A'], "\"BLACK\"");
  strcpy(testProgram->wds[0], "RIGHT");
  strcpy(testProgram->wds[1], "$R");
  Var(testProgram);
  assert((int)testProgram->angle == 102);
  strcpy(testProgram->wds[0], "FORWARD");
  strcpy(testProgram->wds[1], "$F");
  Var(testProgram);
  assert((int)testProgram->forward == 77);
  strcpy(testProgram->wds[0], "COLOUR");
  strcpy(testProgram->wds[1], "$C");
  Var(testProgram);
  assert(testProgram->colour == 'K');
  //3. test if the Var will output error information when the content of variable is "trash".
  // strcpy(testProgram->wds[0], "FORWARD");
  // strcpy(testProgram->wds[1], "$T");
  // Var(testProgram);
  // strcpy(testProgram->wds[0], "RIGHT");
  // strcpy(testProgram->wds[1], "$T");
  // Var(testProgram);
  // strcpy(testProgram->wds[0], "COLOUR");
  // strcpy(testProgram->wds[1], "$T");
  // Var(testProgram);

  free(testProgram);
  testProgram = initial_program(3);

  //void Word_Col(Program *p)
  //Test the Word_Col using "BLACK", "GREEN", "RED", "CYAN", "WHITE"and "TRASH".
  //If the word is colour, it will change the value of p->colour
  //If the word is not a colour, it will output errror.
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "\"BLACK\"");
  Word_Col(testProgram);
  assert(testProgram->colour == 'K');
  strcpy(testProgram->wds[0], "\"GREEN\"");
  Word_Col(testProgram);
  assert(testProgram->colour == 'G');
  strcpy(testProgram->wds[0], "\"RED\"");
  Word_Col(testProgram);
  assert(testProgram->colour == 'R');
  strcpy(testProgram->wds[0], "\"CYAN\"");
  Word_Col(testProgram);
  assert(testProgram->colour == 'C');
  strcpy(testProgram->wds[0], "\"WHITE\"");
  Word_Col(testProgram);
  assert(testProgram->colour == 'W');
  // strcpy(testProgram->wds[0], "\"TRASH\"");
  // Word_Col(testProgram);
  
  //void VarNum(Program *p)
  //using $A and $K to test if VarNum() will call Var() and change the value of var_num in Program*
  //using 13 and 79.2 to test if VarNum() will call Num() and change the value of var_num to -1 in Program*
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "$A");
  VarNum(testProgram);
  assert(testProgram->var_num == 0);
  strcpy(testProgram->wds[0], "13");
  VarNum(testProgram);
  assert(testProgram->var_num == -1);
  strcpy(testProgram->wds[0], "$K");
  VarNum(testProgram);
  assert(testProgram->var_num == 'K'-'A');
  strcpy(testProgram->wds[0], "79.2");
  VarNum(testProgram);
  assert(testProgram->var_num == -1);
  //using trash string to test if the VarNum() will output error message
  // strcpy(testProgram->wds[0], "asdfs55");
  // VarNum(testProgram);

  //void Ltr(Program *p)
  //test if this function will output error information if the string is not a uppercase letter
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "a");
  // Ltr(testProgram);
  
  //void Word(Program *p)
  //test if this function will output error information if the string is not a word
  strcpy(testProgram->wds[0], "abcbc");
  // Word(testProgram);

  //void Item(Program *p)
  //test if this function will call Word() and output an error information
  strcpy(testProgram->wds[0], "\"a");
  // Item(testProgram);
  //test if this function will call VarNum()
  strcpy(testProgram->wds[0], "$T");
  Item(testProgram);
  assert(testProgram->var_num == 'T' - 'A');
  testProgram->var_num = -1;

  //void Items(Program *p)
  //test if the Items() will call Item() and call itself recursively through the value of var_num and cw
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "$A");
  strcpy(testProgram->wds[1], "$A");
  strcpy(testProgram->wds[2], "$A");
  strcpy(testProgram->wds[3], "$A");
  strcpy(testProgram->wds[4], "}");
  Items(testProgram);
  assert(testProgram->var_num == 0);
  assert(testProgram->cw == 4);
  
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "$A");
  strcpy(testProgram->wds[1], "$Z");
  strcpy(testProgram->wds[2], "124");
  strcpy(testProgram->wds[3], "12");
  strcpy(testProgram->wds[4], "}");
  strcpy(testProgram->wds[5], "}");
  Items(testProgram);
  assert(testProgram->var_num == -1);
  assert(testProgram->cw == 4);

  //void Lst(Program *p)
  //1.test if the Lst() will call Items() through the value of var_num and cw
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "{");
  strcpy(testProgram->wds[1], "$Z");
  strcpy(testProgram->wds[2], "124");
  strcpy(testProgram->wds[3], "$S");
  strcpy(testProgram->wds[4], "}");
  strcpy(testProgram->wds[5], "}");
  Lst(testProgram);
  assert(testProgram->var_num == 'S'-'A');
  assert(testProgram->cw == 4);

  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "{");
  strcpy(testProgram->wds[1], "3424");
  strcpy(testProgram->wds[2], "145.1");
  strcpy(testProgram->wds[3], "654548");
  strcpy(testProgram->wds[4], "454");
  strcpy(testProgram->wds[5], "}");
  Lst(testProgram);
  assert(testProgram->var_num == -1);
  assert(testProgram->cw == 5);
  //2.test if this function will output a error information when the first string is not "{"
  // testProgram->cw = 0;
  // strcpy(testProgram->wds[0], "$A");
  // strcpy(testProgram->wds[1], "$Z");
  // Lst(testProgram);

  //void Pfix(Program *p) 
  //Pfix() will change the content of variable according to the var_num and ensure 
  //the format is SET <LTR> (<VarNum>) or SET <LTR> (<VarNum> <VarNum> <Op>).
  //1. test if the Pfix() will change the content of variable correctly
  //The Pfix() will call Op() and VarNum()
  testProgram->cw = 0;
  testProgram->set_var = 2;//C
  strcpy(testProgram->wds[0], "34");
  strcpy(testProgram->wds[1], "7");
  strcpy(testProgram->wds[2], "+");
  strcpy(testProgram->wds[3], ")");
  Pfix(testProgram);
  assert((int)(atof(testProgram->vars[2])) == 41);
  assert(testProgram->set_count == 0);
  //find a bug in Op(). Fixed.
  testProgram->cw = 0;
  testProgram->set_var = 1;//B
  strcpy(testProgram->wds[0], "52");
  strcpy(testProgram->wds[1], ")");
  Pfix(testProgram);
  assert((int)round(atof(testProgram->vars[1])) == 52);
  //find a bug the Pfix() can copy "$C" as the content of $A. Fixed
  testProgram->cw = 0;
  testProgram->set_var = 0;
  strcpy(testProgram->wds[0], "$C");
  strcpy(testProgram->wds[1], ")");
  Pfix(testProgram);
  assert((int)round(atof(testProgram->vars[0])) == 41);

  testProgram->cw = 0;
  testProgram->set_var = 0;
  strcpy(testProgram->wds[0], "$C");
  strcpy(testProgram->wds[1], "7");
  strcpy(testProgram->wds[2], "-");
  strcpy(testProgram->wds[3], ")");
  Pfix(testProgram);
  assert((int)round(atof(testProgram->vars[0])) == 34);
  
  //2. test if the Pfix() will output error information when given a wrong format
  //format SET <LTR> (<VarNum>) or SET <LTR> (<VarNum> <VarNum> <Op>)
  testProgram->cw = 0;
  testProgram->set_var = 2;//C
  strcpy(testProgram->wds[0], "1");
  strcpy(testProgram->wds[1], "+");
  strcpy(testProgram->wds[2], "+");
  strcpy(testProgram->wds[3], ")");
  //Pfix(testProgram);

  testProgram->cw = 0;
  testProgram->set_var = 2;//C
  strcpy(testProgram->wds[0], "1");
  strcpy(testProgram->wds[1], "+");
  strcpy(testProgram->wds[2], "1");
  strcpy(testProgram->wds[3], ")");
  //Pfix(testProgram);
  
  testProgram->cw = 0;
  testProgram->set_var = 2;//C
  strcpy(testProgram->wds[0], "1");
  strcpy(testProgram->wds[1], "1");
  strcpy(testProgram->wds[2], "+");
  strcpy(testProgram->wds[3], "3");
  strcpy(testProgram->wds[4], "+");
  strcpy(testProgram->wds[5], ")");
  //Pfix(testProgram);
  
  //if Pfix is empty or has only two parameters
  testProgram->cw = 0;
  testProgram->set_var = 2;//C
  strcpy(testProgram->wds[0], ")");
  //Pfix(testProgram);

  testProgram->cw = 0;
  testProgram->set_var = 2;//C
  strcpy(testProgram->wds[0], "1");
  strcpy(testProgram->wds[1], "1");
  strcpy(testProgram->wds[2], ")");
  //Pfix(testProgram);

  //bool isOp(char str[MAXNUMTOKENS])
  assert(!isOp("S"));
  assert(isOp("+"));
  assert(isOp("="));
  assert(isOp("-"));
  assert(isOp("/"));
  assert(isOp("*"));
  
  free(testProgram);
  testProgram = initial_program(3);

  //void Op(Program *p)
  //1. test if Op() can perform addition, subtraction, multiplication and division
  // test the result which is saved in a variable
  testProgram->cw = 2;
  testProgram->set_var = 2;//C
  strcpy(testProgram->wds[0], "3");
  strcpy(testProgram->wds[1], "3");
  strcpy(testProgram->wds[2], "+");
  Op(testProgram);
  assert((int)(atof(testProgram->vars[2])) == 6);
  assert(testProgram->set_count == 0);
  
  testProgram->cw = 2;
  testProgram->set_var = 1;//B
  strcpy(testProgram->wds[0], "13");
  strcpy(testProgram->wds[1], "11");
  strcpy(testProgram->wds[2], "-");
  Op(testProgram);
  assert((int)(atof(testProgram->vars[1])) == 2);
  assert(testProgram->set_count == 0);

  testProgram->cw = 2;
  testProgram->set_var = 0;//A
  strcpy(testProgram->wds[0], "$C");
  strcpy(testProgram->wds[1], "$B");
  strcpy(testProgram->wds[2], "*");
  Op(testProgram);
  assert((int)(atof(testProgram->vars[0])) == 12);
  assert(testProgram->set_count == 0);

  testProgram->cw = 2;
  testProgram->set_var = 0;//A
  strcpy(testProgram->wds[0], "$C");
  strcpy(testProgram->wds[1], "$B");
  strcpy(testProgram->wds[2], "/");
  Op(testProgram);
  assert((int)(atof(testProgram->vars[0])) == 3);
  assert(testProgram->set_count == 0);

  //2. test if Op() will output error information when p->wds[p->cw] is "=" or "trash"
  testProgram->cw = 2;
  strcpy(testProgram->wds[0], "$C");
  strcpy(testProgram->wds[1], "$B");
  strcpy(testProgram->wds[2], "=");
  //Op(testProgram);

  testProgram->cw = 2;
  strcpy(testProgram->wds[0], "$C");
  strcpy(testProgram->wds[1], "$B");
  strcpy(testProgram->wds[2], "trashdffsdfsf");
  //Op(testProgram);
  
  free(testProgram);
  testProgram = initial_program(3);

  //void Loop(Program *p)
  //1. test the keyword "OVER"
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "LOOP");
  strcpy(testProgram->wds[1], "A");
  strcpy(testProgram->wds[2], "fjsdlkfjsjflsdfj");
  //Loop(testProgram);
  
  //2. test the Loop() will call Ltr() and Lst()
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "LOOP");
  strcpy(testProgram->wds[1], "787");
  //Loop(testProgram);

  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "LOOP");
  strcpy(testProgram->wds[1], "D");
  strcpy(testProgram->wds[2], "OVER");
  strcpy(testProgram->wds[3], "trash");
  //Loop(testProgram);

  //3.using looptest1.ttl to test if Loop() could call update_var() and Inslst() several times
  //to get the correct result.(And testing if the number of cycles is correct)
  file_to_prog(testProgram, "./Testing/looptest1.ttl");
  Prog(testProgram);
  assert(testProgram->res[12][25] == ' ');
  for(int i = 0; i < 4; i++){
    assert(testProgram->res[16-i][25] == 'W');
  }
  assert(strcmp(testProgram->vars[0], "4") == 0);

  //4.using looptest2.ttl to test if loop() could skip when loop_times == 0(fail_parse_OK_Interp)
  testProgram->cw = 0;
  file_to_prog(testProgram, "./Testing/looptest2.ttl");
  Prog(testProgram);
  
  free(testProgram);
  testProgram = initial_program(3);

  //void Set(Program *p)
  //1. test if Set() could call ltr()
  strcpy(testProgram->wds[0], "SET");
  strcpy(testProgram->wds[1], "ABBCC");
  //Set(testProgram);

  //2. test if Set could control the format
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "SET");
  strcpy(testProgram->wds[1], "A");
  strcpy(testProgram->wds[2], "{");
  //Set(testProgram);
  
  //3. test if Set could call Pfix() and set the value to variable
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "SET");
  strcpy(testProgram->wds[1], "Z");
  strcpy(testProgram->wds[2], "(");
  strcpy(testProgram->wds[3], "7");
  strcpy(testProgram->wds[4], "8");
  strcpy(testProgram->wds[5], "*");
  strcpy(testProgram->wds[6], ")");
  Set(testProgram);
  assert((int)(atof(testProgram->vars['Z'-'A'])) == 56);
  
  //void Col(Program *p)
  //test if Col() could call Var()(this is tested by the final test) or Word_Col() to change the p->colour
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "COLOUR");
  strcpy(testProgram->wds[1], "\"CYAN\"");
  Col(testProgram);
  assert(testProgram->colour == 'C');

  //void Rgt(Program *p)
  //test if Rgt() could call VarNum() and change the p->angle
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "RIGHT");
  strcpy(testProgram->wds[1], "57");
  Rgt(testProgram);
  assert((int)testProgram->angle == 57);
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "RIGHT");
  strcpy(testProgram->wds[1], "-57");
  Rgt(testProgram);
  //void Fwd(Program *p)
  //test if Fwd() could call VarNum() and change the result array.
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "FORWARD");
  strcpy(testProgram->wds[1], "5");
  Fwd(testProgram);
  for(int i = 0; i < 5; i++){
    assert(testProgram->res[16 - i][25] == 'C');
  }
  //test if Fwd() could change the x and y in Program* correctly.
  assert((int)round(testProgram->x) == 25);
  assert((int)round(testProgram->y) == 11);

  //void Ins(Program *p)
  //test if Ins() will output error information when the keyword is wrong
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "WHILE");
  strcpy(testProgram->wds[1], "5");
  //Ins(testProgram);

  //test all command using progtest.ttl to ensure all the functions are correct.
  free(testProgram);
  testProgram = initial_program(3);
  file_to_prog(testProgram, "./Testing/progtest.ttl");
  Prog(testProgram);
  assert((int)atof(testProgram->vars[0]) == 3);
  assert((int)atof(testProgram->vars[3]) == 1);
  assert(testProgram->res[16][25] == 'B');
  assert(testProgram->res[15][25] == 'K');
  assert(testProgram->res[15][26] == 'R');
  
  free(testProgram);
  testProgram = initial_program(3);
  
  //void ps_file_initial(Program* p, char file_name[])
  //test if this function could open a file in the way of writing
  ps_file_initial(testProgram, "./Testing/writefiletest.ps");
  assert(testProgram->fp != NULL);
  
  //test if the Fwd() could adjust the ps_x and ps_y correctly when writing .ps file
  testProgram->cw = 0;
  strcpy(testProgram->wds[0], "START");
  strcpy(testProgram->wds[1], "FORWARD");
  strcpy(testProgram->wds[2], "6");
  strcpy(testProgram->wds[3], "RIGHT");
  strcpy(testProgram->wds[4], "-90");
  strcpy(testProgram->wds[5], "FORWARD");
  strcpy(testProgram->wds[6], "6");
  strcpy(testProgram->wds[7], "END");
  Prog(testProgram);
  assert((int)round(testProgram->ps_x) == STARTVALUEX - 6);
  assert((int)round(testProgram->ps_y) == STARTVALUEY + 6);
  
  fclose(testProgram->fp);
  free(testProgram);
}
