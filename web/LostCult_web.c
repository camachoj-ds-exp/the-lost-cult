#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/* ============================================================
   Web / WebAssembly compatibility layer.
   This file is AUTO-GENERATED from LostCult.c for the browser
   build (Emscripten). Do not edit by hand: edit LostCult.c and
   re-run web/port.py. The game logic below is unchanged.
   ============================================================ */
#include <emscripten.h>

/* Blocking single keystroke, raw / no echo (replaces conio getch). */
EM_ASYNC_JS(int, em_input_getch, (void), {
  return await Module.inputGetch();
});

/* Blocking read of one character from line-buffered (echoed) stdin. */
EM_ASYNC_JS(int, em_input_getchar, (void), {
  return await Module.inputGetChar();
});

/* Push one character back onto the stdin stream. */
EM_JS(void, em_unget_char, (int c), {
  Module.inputUnget(c);
});

/* Emulate the handful of Windows console commands passed to system(). */
static void em_system(const char *cmd) {
  if (!cmd) return;
  if (strcmp(cmd, "cls") == 0) {
    printf("\x1b[2J\x1b[3J\x1b[H");
    fflush(stdout);
  } else if (strncmp(cmd, "color ", 6) == 0) {
    const char *c = cmd + 6;                 /* Windows: BG then FG nibble  */
    if      (strstr(c, "7F")) printf("\x1b[?5h");            /* flash / reverse screen */
    else if (strstr(c, "0F")) printf("\x1b[?5l\x1b[0m");    /* normal white on black  */
    else if (strstr(c, "4F")) printf("\x1b[?5l\x1b[41;97m");/* red background         */
    else if (strstr(c, "2F")) printf("\x1b[?5l\x1b[42;97m");/* green background       */
    fflush(stdout);
  }
}

/* scanf("%d") replacement: skip leading whitespace, read an integer,
   and leave the terminating character in the stream (matches libc so
   that the game's follow-up getchar() calls still consume the newline). */
static int em_read_int(void) {
  int c;
  do { c = em_input_getchar(); } while (c==' '||c=='\n'||c=='\r'||c=='\t');
  int neg = 0;
  if (c == '-') { neg = 1; c = em_input_getchar(); }
  else if (c == '+') { c = em_input_getchar(); }
  int val = 0;
  while (c >= '0' && c <= '9') { val = val*10 + (c-'0'); c = em_input_getchar(); }
  em_unget_char(c);
  return neg ? -val : val;
}

/* scanf("%s") replacement: skip leading whitespace, read a whitespace-
   delimited word, leave the terminating whitespace in the stream. */
static void em_read_word(char *buf, int max) {
  int c, i = 0;
  do { c = em_input_getchar(); } while (c==' '||c=='\n'||c=='\r'||c=='\t');
  while (c != -1 && c!=' ' && c!='\n' && c!='\r' && c!='\t') {
    if (i < max - 1) buf[i++] = (char)c;
    c = em_input_getchar();
  }
  buf[i] = '\0';
  em_unget_char(c);
}

/* Route the Windows-specific calls used throughout the game to the layer above. */
#define Sleep(ms)   emscripten_sleep((ms))
#define system(cmd) em_system((cmd))
#define getch()     em_input_getch()
#undef  getchar
#define getchar()   em_input_getchar()
/* ==================== end web compatibility layer ==================== */


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// universal variables
  char Player_Name[50];
  int ghostescape_game_state = 0;
  int burgergame_game_state = 0;
  int life = 3;
  int player_deck[3], ghost_deck[3], g_deck[3]; // DECKS

void press_enter(){
    //printf("\n-------------------------------------------------------------------------------------------------------------------\n");
    printf("\n\n\n                                                                                        Press ENTER key to Continue\n");  
    getchar();    
    system("cls");
}

void flicker(){
  system("color 7F");
  system("color 0F");
  Sleep(15);
  system("color 7F");
  system("color 0F");
  Sleep(15);
  system("color 7F");
  system("color 0F");
  Sleep(15);
  system("color 7F");
  system("color 0F");
  Sleep(15);
  system("color 7F");
  system("color 0F");
  Sleep(100);
  system("color 7F");
  system("color 0F");
  Sleep(150);
  system("color 7F");
  system("color 0F");
  Sleep(200);
  system("color 7F");
  Sleep(200);
  system("color 0F");
}

void declare_time(){
  time_t t1;                  // declare time variable
  srand((unsigned)time(&t1)); // pass the srand() parameter
}

void board(char sqr[3][6]) {// Display the initial game board
  printf("\n\n\n\n\n");
  printf("\t\t\t\t\t.___________________________________.\n");
  printf("\t\t\t\t\t|     |     |     |     |     |     |\n");
  printf("\t\t\t\t\t|  %c  |  %c  |  %c  |  %c  |  %c  |  ", sqr[0][0], sqr[0][1], sqr[0][2], sqr[0][3], sqr[0][4]);
  printf("\e[32m"); printf("E"); printf("\e[0m"); printf("  |\n");  
  printf("\t\t\t\t\t|_____|_____|_____|_____|_____|_____|\n");
  printf("\t\t\t\t\t|     |     |     |     |     |     |\n");
  printf("\t\t\t\t\t|  %c  |  %c  |  %c  |  %c  |  %c  |  %c  |\n", sqr[1][0], sqr[1][1], sqr[1][2], sqr[1][3], sqr[1][4], sqr[1][5]);
  printf("\t\t\t\t\t|_____|_____|_____|_____|_____|_____|\n");
  printf("\t\t\t\t\t|     |     |     |     |     |     |\n");
  printf("\t\t\t\t\t|  %c  |  %c  |  %c  |  %c  |  %c  |  %c  |\n", sqr[2][0], sqr[2][1], sqr[2][2], sqr[2][3], sqr[2][4], sqr[2][5]);
  printf("\t\t\t\t\t|     |     |     |     |     |     |\n");
  printf("\t\t\t\t\t|___________________________________|\n\n\n\n\n");
}

void burgerheader_ascii(){
    FILE *file = fopen("burger.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void june_ascii(){
    FILE *file = fopen("june.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void cardpacks_ascii(){
    FILE *file = fopen("cards.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void cardsof3_ascii(){
    FILE *file = fopen("cardsof3.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void player_ascii(){
    FILE *file = fopen("player.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void radio_ascii(){
    FILE *file = fopen("radio.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void cardgame_ascii(){
    FILE *file = fopen("cardgame.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void title_ascii(){
    FILE *file = fopen("title.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void objectives_ascii(){
    FILE *file = fopen("objectives.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void may13_ascii(){
    FILE *file = fopen("may13.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void ghostescape_ascii(){
    FILE *file = fopen("ghostescape.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void ghost_ascii(){
    FILE *file = fopen("ghost.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void missingposter_ascii(){
    FILE *file = fopen("missingposter.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void notehint_ascii(){
    FILE *file = fopen("notehint.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void fardoor_ascii(){
    FILE *file = fopen("fardoor.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void cultleader_base_ascii(){
    FILE *file = fopen("cultleader_base.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void cultleader_smile_ascii(){
    FILE *file = fopen("cultleader_smile.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void cultleader_angry_ascii(){
    FILE *file = fopen("cultleader_angry.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void lives_0_ascii(){
    FILE *file = fopen("lives_0.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void lives_1_ascii(){
    FILE *file = fopen("lives_1.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void lives_2_ascii(){
    FILE *file = fopen("lives_2.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void lives_3_ascii(){
    FILE *file = fopen("lives_3.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void chef_angrier_ascii(){
    FILE *file = fopen("chef_angrier.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void chef_angry_ascii(){
    FILE *file = fopen("chef_angry.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void chef_base_ascii(){
    FILE *file = fopen("chef_base.txt", "r");
    char buffer_file[255];


    if (file == NULL) {
      printf("Unable to open file!\n");
    } else {
      while (fgets(buffer_file, 255, file) != NULL) {
        printf("%s", buffer_file);
      }
    }


    fclose(file);
}

void printline(){
  printf("\n-------------------------------------------------------------------------------------------------------------------\n");
}

// game_functions

void life_state(){
  if (life==3){
    lives_3_ascii();
  }
  else if (life == 2){
    lives_2_ascii();
  }
  else if (life == 1){
    lives_1_ascii();
  }
  Sleep(1000);
  flicker();
  life--;
  system("cls");
  if (life==3){
    lives_3_ascii();
  }
  else if (life == 2){
    lives_2_ascii();
  }
  else if (life == 1){
    lives_1_ascii();
  }
  else if (life == 0){
    lives_0_ascii();
  }
  Sleep(1000);
  system("cls");
}

void ghostescape_game(){
  char sqre[3][6] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};


    int x_player=2, y_player=0;
    sqre[x_player][y_player] = 'O';


    int x_ghostx = 0, y_ghostx = 2;
    sqre[x_ghostx][y_ghostx] = 'X';


    int x_ghosty = 1, y_ghosty = 0;
    sqre[x_ghosty][y_ghosty] = 'Y';


    int x_ghostz = 2, y_ghostz = 5;
    sqre[x_ghostz][y_ghostz] = 'Z';
 
    int x_player_old, y_player_old, x_ghosty_old,y_ghosty_old, x_ghostx_old, y_ghostx_old, x_ghostz_old, y_ghostz_old;


    int playermoved;
    int gy,gx,gz;
    char ch;

    ghostescape_ascii();
    printline();
    press_enter();
   
while (ghostescape_game_state == 0){
    playermoved = 0;
    system("cls");
   
    board(sqre);
    printf("Press W, A, S, D to move.\n");
    printf("Steer clear of the ghosts (X, Y, Z), lest you become one of them. \n\n");
 
    ch = getch();
    switch (ch) { //player move
        case 'w':
            if(x_player == 1 || x_player == 2){
            x_player_old = x_player;
            y_player_old = y_player;
            x_player--;
            playermoved = 1;
            }
            else
            printf(ANSI_COLOR_RED"Invalid Move\n"ANSI_COLOR_RESET);
            Sleep(750);
            break;
        case 's':
            if(x_player == 0 || x_player == 1){
            x_player_old = x_player;
            y_player_old = y_player;
            x_player++;
            playermoved = 1;
            }
            else
            printf(ANSI_COLOR_RED"Invalid Move\n"ANSI_COLOR_RESET);
            Sleep(750);
            break;
    case 'a':
            if(y_player >=1 && y_player <= 5){
            x_player_old = x_player;
            y_player_old = y_player;
            y_player--;
            playermoved = 1;
            }
            else
            printf(ANSI_COLOR_RED"Invalid Move\n"ANSI_COLOR_RESET);
            Sleep(750);
        break;
    case 'd':
        if(y_player >= 0 && y_player <= 4){
            x_player_old = x_player;
            y_player_old = y_player;
            y_player++;
            playermoved = 1;
            }
            else
            printf(ANSI_COLOR_RED"Invalid Move\n"ANSI_COLOR_RESET);
            Sleep(750);
        break;
  }
   
        if (playermoved == 1){


            if (x_ghostx == 0){
                gx = 1;
            }
            else if (x_ghostx == 2){
                gx = 0;
            }
            else
                gx = gx;
           
            if (y_ghosty == 0){
                gy = 1;
            }
            else if (y_ghosty == 5){
                gy = 0;
            }
            else
                gy = gy;
            if (y_ghostz == 0){
                gz = 1;
            }
            else if (y_ghostz == 5){
                gz = 0;
            }
            else
                gz = gz;


            switch (gx) {//ghostX move
                case 1:
                x_ghostx_old = x_ghostx;
                y_ghostx_old = y_ghostx;
                x_ghostx++;
                break;
           
                case 0:
                x_ghostx_old = x_ghostx;
                y_ghostx_old = y_ghostx;
                x_ghostx--;
                break;
            }
            switch (gy) {//ghostY move
                case 1:
                x_ghosty_old = x_ghosty;
                y_ghosty_old = y_ghosty;
                y_ghosty++;
                break;
           
                case 0:
                x_ghosty_old = x_ghosty;
                y_ghosty_old = y_ghosty;
                y_ghosty--;
                break;
        }
            switch (gz) {//ghostZ move
            case 1:
                x_ghostz_old = x_ghostz;
                y_ghostz_old = y_ghostz;
                y_ghostz++;
                break;
           
                case 0:
                x_ghostz_old = x_ghostz;
                y_ghostz_old = y_ghostz;
                y_ghostz--;
                break;
        }


        sqre[x_player_old][y_player_old] = ' ';
        sqre[x_ghostz_old][y_ghostz_old] = ' ';
        sqre[x_ghostx_old][y_ghostx_old] = ' ';
        sqre[x_ghosty_old][y_ghosty_old] = ' ';
        sqre[x_player][y_player] = 'O';
        sqre[x_ghostx][y_ghostx] = 'X';
        sqre[x_ghosty][y_ghosty] = 'Y';
        sqre[x_ghostz][y_ghostz] = 'Z';
       
    }


    if (x_player == x_ghostx && y_player == y_ghostx){
        ghostescape_game_state = 1; //lose
    }
    else if (x_player == x_ghosty && y_player == y_ghosty){
        ghostescape_game_state = 1; //lose
    }
    else if (x_player == x_ghostz && y_player == y_ghostz){
        ghostescape_game_state = 1; //lose
    }
    else if (x_player == 0 && y_player == 5){
        ghostescape_game_state = 2; //win
    }


}
        sqre[x_player_old][y_player_old] = ' ';
        sqre[x_ghostz_old][y_ghostz_old] = ' ';
        sqre[x_ghostx_old][y_ghostx_old] = ' ';
        sqre[x_ghosty_old][y_ghosty_old] = ' ';
        sqre[x_player][y_player] = 'O';
        sqre[x_ghostx][y_ghostx] = 'X';
        sqre[x_ghosty][y_ghosty] = 'Y';
        sqre[x_ghostz][y_ghostz] = 'Z';


        system("cls");
   
    board(sqre);
    printf("Press W, A, S, D to move.\n");
    printf("Steer clear of the ghosts (X, Y, Z), lest you become one of them. \n\n");
}

void burger_game(){
  system("cls");
  burgerheader_ascii();
  printline();
  press_enter();


  //#############################################################################
  printline();
  char Narration_14 [180] =  "You get dragged to the preparation table.\n\nYou notice red liquid spilled everywhere, and a bunch of ... meat?\n\nAnd...  ingredients? scattered throughout the surface.\n";


      for (int i = 0; i < strlen(Narration_14); i++) {
        printf("%c", Narration_14[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_19 [100] =  "I don't- \n";


      for (int i = 0; i < strlen(Player_Dialogue_19); i++) {
        printf("%c", Player_Dialogue_19[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  chef_base_ascii();
  printline();
  printf(ANSI_COLOR_MAGENTA"The Chef: "ANSI_COLOR_RESET);
  char thechef_Dialogue_3 [150] =  "Enough talking. Just cook.\n";


      for (int i = 0; i < strlen(thechef_Dialogue_3); i++) {
        printf("%c", thechef_Dialogue_3[i]);
        Sleep(15);
    }
  press_enter();


  char *ingredients[] = {"Onions", "Eyeballs", "Blood sauce", "Suspicious Meat", "Lettuce"};


  declare_time();


  int orderchoice[5];
  int orderlist[5];
 
  int a = rand() % 5;
  int b = rand() % 5;
  int c = rand() % 5;
  int d = rand() % 5;
  int e = rand() % 5;


  //#############################################################################
  chef_base_ascii();
  printline();
  printf(ANSI_COLOR_MAGENTA"The Chef: "ANSI_COLOR_RESET);
  char thechef_Dialogue_4 [150] =  " Listen carefully. I won't say it twice.\n";


      for (int i = 0; i < strlen(thechef_Dialogue_4); i++) {
        printf("%c", thechef_Dialogue_4[i]);
        Sleep(15);
    }


  printf(ANSI_COLOR_RED"\n\n\tThe Cult Leader wants.."ANSI_COLOR_RESET"\n");
  Sleep(100);
  printf("\n\t\t\t- %s",ingredients[a]);
  Sleep(100);
  printf("\n\t\t\t- %s",ingredients[b]);
  Sleep(100);
  printf("\n\t\t\t- %s",ingredients[c]);
  Sleep(100);
  printf("\n\t\t\t- %s",ingredients[d]);
  Sleep(100);
  printf("\n\t\t\t- %s",ingredients[e]);
  Sleep(100);
  printf("\n\n\t--This list is stacked from top to bottom.\n\n\nWell? Make it quick!\n");


  press_enter();
 
  //#############################################################################
  chef_base_ascii();
  printline();
  printf(ANSI_COLOR_MAGENTA"The Chef: "ANSI_COLOR_RESET);
  char thechef_Dialogue_5 [150] =  "Did you take note of what he wanted? Remember, burgers are stacked from the bottom to the top.\n";


      for (int i = 0; i < strlen(thechef_Dialogue_5); i++) {
        printf("%c", thechef_Dialogue_5[i]);
        Sleep(15);
    }


  press_enter();

  orderlist [0] = e;
  orderlist [1] = d;
  orderlist [2] = c;
  orderlist [3] = b;
  orderlist [4] = a;
 
  printline();
  printf("\n\nTime to make the burger.\n");
  Sleep(20);
  printf("What would you like to add? (stacked from bottom up)\n\n");
  Sleep(20);
  printf("\t0. Onions\n");
  Sleep(20);
  printf("\t1. Eyeballs\n");
  Sleep(20);
  printf("\t2. Blood Sauce\n");
  Sleep(20);
  printf("\t3. Suspicious Meat\n");
  Sleep(20);
  printf("\t4. Lettuce\n\n");
  Sleep(20);
  printf("Preparation Phase\n\n");
  Sleep(20);

  for (int n=0; n<5; n++){
    int burger_choice_state = 0;
    while(burger_choice_state == 0){
    printf("\tEnter the number of your choice for ingredient number %d: ", n+1);
    orderchoice[n] = em_read_int();
    if (orderchoice[n]>=0 && orderchoice[n]<=4){
      burger_choice_state = 1;
    }
    else
    printf("\n\n\tInvalid Input.\n");
    }
  }
    printf("\n\nChecking if ingredients are correct");
    Sleep(1000);
    printf(".");
    Sleep(1000);
    printf(".");
    Sleep(1000);
    printf(".\n");
    Sleep(1000);
    system("cls");


  if (orderchoice[0] == orderlist[0] &&
      orderchoice[1] == orderlist[1] &&
      orderchoice[2] == orderlist[2] &&
      orderchoice[3] == orderlist[3] &&
      orderchoice[4] == orderlist[4]){
    //give letter hints
    burgergame_game_state = 1; // win
   
      }
   
  else{
    burgergame_game_state = 2; // lose
  }
}

void password_game(){
  char guess[50], correctguess[10] = "*M****A*";
    int status = 1, rooms = 8, guessalpha, guesscheck, hold = 0, distance = (rooms + (life - 3)); // rooms up to 8

    //lock ascii art
    char locktop[12][25] = {"\n\t   .-------.", "\n\t  / .-----. \\", "\n\t ( (       ) )", "\n\t | |       | |", "\n\t | |       | |", "\n\t | |       | |", "\n\t | |       | |", "\n\t | |       | |", "\n\t | |       | |", "\n\t.|_|-'''''-|_|.", "\n       /               \\", "\n      |'\t       '|"};
    char lockbot[3][26] = {"\n      |.       |       .|", "\n       \\.\t      ./", "\n\t \'-----------\'"};


    char instructions[500] = "\n\nYou have to guess the password to get out. The cult leader is hunting you and can't be stopped.\n\nYou can guess one letter at a time or risk it all with a full attempt.\n\nYou have to enter the password to check.\n\n";


    system("cls");


    { //prints title ascii art
        FILE *title = fopen("password_title.txt", "r");
        char buffer[100];
        if (title == NULL) {
            printf("Unable to open file!\n");
        }
        else {
            while (fgets(buffer, 255, title) != NULL) {
                printf("%s", buffer);
            }
        }
        fclose(title);
    }


  press_enter();
  printline();
  char Narration_27 [202] =  "You inspect the lock.\n\nThe lock requires a combination of 8 letters.\n";


       for (int i = 0; i < strlen(Narration_27); i++) {
         printf("%c", Narration_27[i]);
         Sleep(15);
     }
  press_enter();


  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_24 [100] =  "It has to be something related to that freak. That guy's a narcissist.\n";


      for (int i = 0; i < strlen(Player_Dialogue_24); i++) {
        printf("%c", Player_Dialogue_24[i]);
        Sleep(15);
    }
  char Narration_28 [320] =  "\n\n\n\nAfter letting go of the lock, a slip of paper falls to the ground.\n";


        for (int i = 0; i < strlen(Narration_28); i++) {
          printf("%c", Narration_28[i]);
          Sleep(15);
      }
  press_enter();

  notehint_ascii();
  printline();
  press_enter();

  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_25 [100] =  "The key to victory, His ultimate goal... huh.\n";


      for (int i = 0; i < strlen(Player_Dialogue_25); i++) {
        printf("%c", Player_Dialogue_25[i]);
        Sleep(15);
    }
  press_enter();




  printline();
    for (int i = 0; i < strlen(instructions); i++) { //prints instructions lettter by letter
        printf("%c", instructions[i]);
        Sleep(15);
    }
    press_enter();


    do {

    char password[15] = "IMMORTAL";
        { // prints lock ascii art
            hold = 0;


            for (int i = 0; i < 12; i++) {
                for (int j = 0; j < 25; j++) {
                        printf("%c", locktop[i][j]);
                }
            }


            for (int i = 0; i < (strlen(password) / 2); i++) {
                printf("\n      |        |        |");
                printf("\n     {|   (%c)  |  (%c)   |}", correctguess[hold], correctguess[hold + 4]); //prints correct guesses on lock ascii art
                hold++;
            }


            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 26; j++) {
                        printf("%c", lockbot[i][j]);
                }
            }
        }


        puts("\n------------------------------------------------------------------------------------------------------------\n");
        printf("\t");


        for (int i = 0; i < strlen(password); i++) { //loop that prints the blanks and correct guesses
            if (isalpha(correctguess[i]) != 1)
                printf("__ ");
            else
                printf("%c ", correctguess[i]); //if a guess has been put into the correctguesses array, it will print the letter


            Sleep(15);
        }


        printf("\n\n");


        do {
            printf("\tGuess the password: ");
            em_read_word(guess, sizeof(guess));

            if (strlen(guess) > strlen(password)) {
                strcpy(guess, "wrong");
            }

            for (int i = 0; i <= strlen(guess) - 1; i++) {
                guess[i] = toupper(guess[i]); //makes characters of the guess uppercase
                if (isalpha(guess[i]) == 0) { //checks if input contains numbers and symbols and breaks if loop if it does
                    guessalpha = 0;
                    printf("\tThere are no options for numbers or symbols.\n");
                    Sleep(15);
                    break;
                }
                else if (isalpha(guess[i]) != 0){ //checks if input only contains letters
                    guessalpha = 1;
                }
            }
        } while (guessalpha == 0); //loops if previous guess contains numbers or symbols


        guesscheck = 0; //resets every loop


        if (strlen(guess) == 1) {
            for (int i = 0; i <= strlen(password) - 1; i++) { //loop that compares guess to every character in the password
                    if (password[i] == guess[0]) {
                        guesscheck = 1;
                        correctguess[i] = guess[0];
                    }
            }
        }
        else if (strlen(guess) > 1) {
            status = strcmp(guess, password); //compares guess to password
            if (status == 0) //win condition
                guesscheck = 1;
            else if (status != 0)
                guesscheck = 0;
        }


        //additional dialogue
        char right[100] = "\n\n\n\tYou hear a click.\n";
        char wrong[100] = "\n\n\n\tNothing happens...\n";


        if (guesscheck == 1) { //prints dialogue for a good guess
            system("cls");
            for (int i = 0; i < strlen(right); i++) {
                printf("%c", right[i]);
                Sleep(20);
            }
        }
        else if (guesscheck == 0) { //prints dialogue for a bad guess
            system("cls");
            for (int i = 0; i < strlen(wrong); i++) {
                printf("%c", wrong[i]);
                Sleep(20);
            }
        }


        Sleep(500);
        system("cls");


        if (status != 0) {
            printf("\033[0;31m"); //sets print color to red


            for (int i = 1; i <= 3; i++){ //loop to print ascii animation
                char file[50] = "", num[1] = "";
                system("cls");


                //concatinates the number of tries to a file name to check which frame to show
                sprintf(num, "%i", i);
                strcat(file, "kick_");
                strcat(file, num);
                strcat(file, ".txt");
                FILE *kick = fopen(file, "r");
                char buffer[100];


                if (kick == NULL) {
                    printf("Unable to open file!\n");
                }
                else {
                    while (fgets(buffer, 255, kick) != NULL) {
                        printf("%s", buffer);
                    }
                }


                Sleep(350);
                fclose(kick);
            }


            printf("\033[0m"); //sets print color back to white
            system("cls");


            //additional dialogue
            char crash[100] = "\n\n\tYou hear distant crashing...\n";
            char close[100] = "\n\n\tThe crashes are getting closer...\n";
            char knock[150] = "\n\n\tYou hear knocking behind you...\n\n\tYou can't run from me.\n\n\tI WONT LET MY VESSEL GET AWAY.\n\n\t- You have one last chance -\n";
            char dead[100] = "\n\n\tYou hear heavy breathing behind you...\n";


            if (distance >= 6)
                for (int i = 0; i < strlen(crash); i++) {
                    printf("%c", crash[i]);
                    Sleep(20);
                }
            else if (distance >= 3)
                for (int i = 0; i < strlen(close); i++) {
                    printf("%c", close[i]);
                    Sleep(20);
                }
            else {
                printf("\033[0;31m"); //sets print color to red


                if (distance == 2)
                    for (int i = 0; i < strlen(knock); i++) {
                        printf("%c", knock[i]);
                        Sleep(20);
                    }
                else
                    for (int i = 0; i < strlen(dead); i++) {
                        printf("%c", dead[i]);
                        Sleep(20);
                    }


                printf("\033[0m"); //sets print color back to white
            }


            Sleep(500);
            system("cls");
            distance--;
        }


    } while (distance != 0 && status != 0 );
   
    if (status == 0) { //prints winning dialogue  
 
        { //prints win ascii art
            FILE *win = fopen("win.txt", "r");
            char buffer[100];
            if (win == NULL) {
                printf("Unable to open file!\n");
            }
            else {
                while (fgets(buffer, 255, win) != NULL) {
                    printf("%s", buffer);
                }
            }
            fclose(win);
        }


        char win[500] = "\n\tThe lock unlocks and falls to the ground.\n\n\tYou quickly push the door open and run away.\n\n\tAs you run away, you hear the cult leader scream in anger.\n\n";


        for (int i = 0; i < strlen(win); i++) {
            printf("%c", win[i]);
            Sleep(20);
        }
      
    }
    else if (status != 0) { //prints losing dialogue


        printf("\033[0;31m"); //sets print color to red
       
        { //prints lose ascii art
            FILE *lose = fopen("lose.txt", "r");
            char buffer[100];
            if (lose == NULL) {
                printf("Unable to open file!\n");
            }
            else {
                while (fgets(buffer, 255, lose) != NULL) {
                    printf("%s", buffer);
                }
            }
            fclose(lose);
        }


        char lose[500] = "\n\tYou slowly turn around and see the cult leader behind you...\n\n\tYou scream in terror as they walk toward you and say...\n\n\t\"The cult life forevermore. Be proud to be my vessel.\"\n\n";


        for (int i = 0; i < strlen(lose); i++) {
            printf("%c", lose[i]);
            Sleep(20);
        }


        printf("\033[0m"); //sets print color back to white
    }
}

void shuffles(int array[], int length) 
{
  srand(time(NULL));
  for (int x = 0; x < length; x++) {
    int swap_index = rand() % length;
    int temp = array[x];
    array[x] = array[swap_index];
    array[swap_index] = temp;
  }
}
void ghostdeck(int g_deck[], int p_deck[], int s_deck[]) 
{
  int C = 0;
  int W = 0;
  for (int x = 0; x < 6; x++) {
    C = 0;
    for (int y = 0; y < 3; y++) {
      if (s_deck[x] != p_deck[y]) {
        C++;
        if (C == 3) {
          g_deck[W] = s_deck[x];
          W++;
        }
      }
    }
  }
}

void card_game(){
char *cards_name[] = {"The Plant", "The Spider",  "The Bat",
                        "The Snake", "The Vulture", "Death"};
  char *card_num[] = {"First", "Second", "Third"};
  char *shuffled_order[6];       // Shuffled deck of STRINGS
  int deck[6], shuffled_deck[6]; // Shuffled deck of NUMBER VALUES
  int unshuffled[6] = {0, 1, 2, 3, 4, 5};
  int player_input[3];                 // PLAYER INPUT
  int A, B, x, y, count; // counters or checkers
  int cor, cor2; // correlators (don't use unless necessary)
  int total_deck = 6; // Deck counter (if index deleter will be used)
  int card_play;      // card played by player
  int player_score = 0, ghost_score = 0; //
  int card_check[3] = {-1, -1, -1}; // -1 = not yet played (avoids matching a real card 1-3)

  cardgame_ascii();

  //#############################################################################
  printline();
  printf(ANSI_COLOR_CYAN"Stubborn Ghost: "ANSI_COLOR_RESET);
  char stubbornghost_Dialogue_4 [110] =  "The strong prey on the weak. Everything ends with death. Only life can bring you back.\n\n\tYou get it?\n";


      for (int i = 0; i < strlen(stubbornghost_Dialogue_4); i++) {
        printf("%c", stubbornghost_Dialogue_4[i]);
        Sleep(15);
    }

  printf("\n\tMechanics:\n\tThe Spider ingests The Plant\n\tThe Bat eats The Spider\n\tThe Snake swallows The Bat\n\tThe Vulture devours Snake\n\tNothing escapes death\n\tThe Plant consumes Death.\n\n\n\n");
  press_enter();
 
  printline();

  cardpacks_ascii();

  // DECK SHUFFLING PHASE
  for (x = 0; x < 6; x++) // Automatic assignment of values to deck (1....6)
  {
    deck[x] = x;
  }
  shuffles(deck, total_deck);
  for (x = 0; x < 6; x++) {
    shuffled_deck[x] = deck[x];
    cor = shuffled_deck[x];
    shuffled_order[x] = cards_name[cor];
  }

  //#############################################################################
  printline();
  printf(ANSI_COLOR_CYAN"Stubborn Ghost: "ANSI_COLOR_RESET);
  char stubbornghost_Dialogue_5 [150] =  "Pick three cards. I will take those that you don't choose.\n\n\nChoose carefully. Just like in life, you cannot take back the actions you have done.\n";


      for (int i = 0; i < strlen(stubbornghost_Dialogue_5); i++) {
        printf("%c", stubbornghost_Dialogue_5[i]);
        Sleep(15);
    }
  press_enter();

  // PLAYER PICKING PHASE
  A = 0;
  while (A == 0) 
    {
    for (count = 0; count < 3; count++) {
      B = 0;
      while (B == 0){
      system("cls");
      cardpacks_ascii();
      printline();
      printf("Pick your %s card: ", card_num[count]);
      player_input[count] = em_read_int(); 
      if (player_input[count] > 0 && player_input[count] < 7)
              {
                B = 1;
              }
      }
    }
    if (count == 3) 
    {
      if (player_input[0] != player_input[1] &&
          player_input[1] != player_input[2] &&
          player_input[2] != player_input[0]) 
      {
        if (player_input[0] > 0 && player_input[0] < 7 && player_input[1] > 0 &&
            player_input[1] < 7 && player_input[2] > 0 && player_input[2] < 7) 
        {
          for (x = 0; x < 3; x++) 
          {
            cor2 = player_input[x] - 1;
            cor2 = shuffled_deck[cor2];
            player_deck[x] = shuffled_deck[cor2];
            player_deck[x] = unshuffled[player_deck[x]];

            A = 1;
            
          }
        }
      } 
      else{
      printf("\n\nInvalid. Pick different cards.\n");
      getchar();
      press_enter();
      }
    }
  }
  press_enter();

  B = 0;

  // GHOST CARD ASSIGNING
  ghostdeck(ghost_deck, player_deck, shuffled_deck);
  
  // PLAYING SEQUENCE
  cardpacks_ascii();
  printline();    
  printf("\nYour Cards: \n");
    for (x = 0; x < 3; x++) 
      {
        printf("[%s]\n", cards_name[player_deck[x]]);
      }
  printf("\nThe Ghost's Cards \n");
    for (x = 0; x < 3; x++) 
      {
        printf("[%s]\n", cards_name[ghost_deck[x]]);
      }

  press_enter();

  int K = 0;
  int round = 0;
  x = 0;
  
  while(K == 0) 
  {
    cardsof3_ascii();
    printline();    
    printf("\nYour Cards: \n");
    printf("[%d][%s]   [%d][%s]   [%d][%s]", 
      1, cards_name[player_deck[0]], 
      2, cards_name[player_deck[1]], 
      3, cards_name[player_deck[2]]);

    B = 0;

    while (B == 0){
    printf("\n\nChoose a card. Remember, you may not reuse them: ");
    card_play = em_read_int();
     if (card_play > 0 && card_play < 4)
      {
        B = 1;
      }
      else
      {
        printf("\n\nChoose only from your cards.\n\n");
      }
    }

    card_check[x] = card_play;

    int already_used = 0;            // DUPLICATE CHECKER: compare against earlier picks only
    for (int j = 0; j < x; j++) {
        if (card_check[j] == card_play) already_used = 1;
    }
    if (already_used)
      {
        printf("\n\nAlready used that. Pick a different card.\n");
        card_check[x] = -1;          // undo this pick so the slot stays free
        getchar();
        press_enter();
        continue;
      }
    
    card_play--;
    x++;
    
     if (player_deck[card_play] == 0 && ghost_deck[round] == 5)
       {
         printf("\nThe Plant consumes Death.\n\n");
         player_score++;
       }
     else if (player_deck[card_play] == 5 && ghost_deck[round] == 0)
       {
         printf("\nThe Plant consumes Death.\n\n");
         ghost_score++;
       }
    else if (player_deck[card_play] > ghost_deck[round]) 
      {
        printf("\n%s beats %s\n\n", cards_name[player_deck[card_play]], cards_name[ghost_deck[round]]);
        printf("Not bad. But don't get too cocky. You win this round.\n");
        player_score++;
      }
    else if (player_deck[card_play] < ghost_deck[round]) 
      {
        printf("\n%s beats %s\n\n", cards_name[ghost_deck[round]], cards_name[player_deck[card_play]]);
        printf("You know my cards as I know yours. Think. You lost this round.\n");
        ghost_score++;
      }
    
    getchar();
    press_enter();
    
    round++;
      if ( round == 3)
        {
          K = 1;
        }
  }

  if (player_score > ghost_score) 
    {
  
  //#############################################################################
  ghost_ascii();
  printline();
  printf(ANSI_COLOR_CYAN"Stubborn Ghost: "ANSI_COLOR_RESET);
  char stubbornghost_Dialogue_6 [100] =  "Don't get too excited. In the game of life, nobody truly wins.\n";


      for (int i = 0; i < strlen(stubbornghost_Dialogue_6); i++) {
        printf("%c", stubbornghost_Dialogue_6[i]);
        Sleep(15);
    }
  press_enter();
  //#############################################################################
  ghost_ascii();
  printline();
  printf(ANSI_COLOR_CYAN"Stubborn Ghost: "ANSI_COLOR_RESET);
  char stubbornghost_Dialogue_7 [120] =  "Between you and me? I never understood why he wants to live forever.\nDeath is inevitable. But your escape? Well...\n";


      for (int i = 0; i < strlen(stubbornghost_Dialogue_7); i++) {
        printf("%c", stubbornghost_Dialogue_7[i]);
        Sleep(15);
    }
  press_enter();
  //#############################################################################
  ghost_ascii();
  printline();
  printf(ANSI_COLOR_CYAN"Stubborn Ghost: "ANSI_COLOR_RESET);
  char stubbornghost_Dialogue_8 [100] =  "Just keep your cards close to your chest.\n";


      for (int i = 0; i < strlen(stubbornghost_Dialogue_8); i++) {
        printf("%c", stubbornghost_Dialogue_8[i]);
        Sleep(15);
    }
  press_enter();
  
  }


  if (player_score < ghost_score) 
    {

  //#############################################################################
  ghost_ascii();
  printline();
  printf(ANSI_COLOR_CYAN"Stubborn Ghost: "ANSI_COLOR_RESET);
  char stubbornghost_Dialogue_4 [100] =  "Seems like luck wasn't on your side...\n";


      for (int i = 0; i < strlen(stubbornghost_Dialogue_4); i++) {
        printf("%c", stubbornghost_Dialogue_4[i]);
        Sleep(15);
    }
  press_enter();

  //#############################################################################
  printline();
  char Narration_4 [125] =  "An echo of footsteps start off faint, becoming louder with each step.\nYou turn to leave, but the ghost blocks your way.\n";
      for (int i = 0; i < strlen(Narration_4); i++) {
        printf("%c", Narration_4[i]);
        Sleep(15);
    }
  press_enter();

  //#############################################################################
  ghost_ascii();
  printline();
  printf(ANSI_COLOR_CYAN"Stubborn Ghost: "ANSI_COLOR_RESET);
  char stubbornghost_Dialogue_5 [100] =  "Did you think you could just walk away? There are consequences to losing.\n";


      for (int i = 0; i < strlen(stubbornghost_Dialogue_5); i++) {
        printf("%c", stubbornghost_Dialogue_5[i]);
        Sleep(15);
    }
  press_enter();

  flicker();

  life_state();
    }
}

int main(int argc, char const *argv[]) {
  setvbuf(stdout, NULL, _IONBF, 0); /* live output for the typing effect */
  system("color 0F");

{ //menu
  int menu_choice = 0, menu_state = 0;
  char N_1 [110] = "Your curiosity got the better of you, and now you have found yourself trapped with no certainty of escape.\n\n";
  char N_2 [150] = "Navigate through the halls of the church and gather as much information as you can.\n\nKnowledge is power. It can serve as the key to your freedom.\n\n";
  char N_3 [105] = "You chose to take the easy way out. That's fair.\n\nBut just because you quit, does not mean it's over.\n";
  char N_4 [25] = "What is your name? \n\n";
  char N_5 [80] = "\nSuch ignorance. You'll learn not to give out your name so easily, child.\n";
 
  while (menu_state == 0){ //Menu System
    if (menu_choice == 0){
      system("cls");
      title_ascii();
      printline();
      printf("1. Play\n");
      printf ("2. Objectives\n");
      printf ("3. Quit\n\n");

      printline();
      printf("type number of your choice: ");
      menu_choice = em_read_int();
    }
    else if(menu_choice == 1){ //play
      system("cls");
      title_ascii();
      printline();
      for (int i = 0; i < strlen(N_4); i++) {
          printf("%c", N_4[i]);
          Sleep(15);
      }
      printf("Enter a 1-word name: ");
      em_read_word(Player_Name, sizeof(Player_Name));
      getchar();  
      for (int i = 0; i < strlen(N_5); i++) {
          printf(ANSI_COLOR_RED"%c"ANSI_COLOR_RESET, N_5[i]);
          Sleep(15);
      }
      press_enter();
      menu_state = 1; // leave menu state
    }
    else if (menu_choice == 2)//objectives
    {
        system("cls");

        objectives_ascii();
        printf("\n");

        for (int i = 0; i < strlen(N_1); i++) {
          printf("%c", N_1[i]);
          Sleep(15);
      }


        for (int i = 0; i < strlen(N_2); i++) {
          printf("%c", N_2[i]);
          Sleep(15);
      }
        getchar();  
        press_enter();
        menu_choice = 0;
    }
    else if (menu_choice == 3)//quit
    {
        system("cls");
        title_ascii();
        printline();


        for (int i = 0; i < strlen(N_3); i++) {
          printf("%c", N_3[i]);
          Sleep(15);
      }
      flicker();
      system("color 4F");
      system("cls");


      //CultSmile Ascii
      cultleader_smile_ascii();
      Sleep(1000);
      flicker();
      system("cls");


      Sleep(2000);
      system("color 0F");


        exit(0);
    }
    else{
    printf("\n\nInvalid Input\n\n");
    Sleep(650);
    menu_choice=0;
    }
    }


    system("cls");
}
 
{ //STORY 1

  //date ascii
  may13_ascii();
  printline();
  press_enter();

  //#############################################################################
  june_ascii();
  printline();
  printf(ANSI_COLOR_YELLOW"June: "ANSI_COLOR_RESET);
  char June_Dialogue_1 [100] =  "Hey, want to come with me?\n";


      for (int i = 0; i < strlen(June_Dialogue_1); i++) {
        printf("%c", June_Dialogue_1[i]);
        Sleep(15);
    }
  press_enter();
 
  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_1 [100] =  "Where?\n";


      for (int i = 0; i < strlen(Player_Dialogue_1); i++) {
        printf("%c", Player_Dialogue_1[i]);
        Sleep(15);
    }
  press_enter();

  //#############################################################################
  june_ascii();
  printline();
  printf(ANSI_COLOR_YELLOW"June: "ANSI_COLOR_RESET);
  char June_Dialogue_2 [100] =  "Just nearby, you said you liked exploring?\n";


      for (int i = 0; i < strlen(June_Dialogue_2); i++) {
        printf("%c", June_Dialogue_2[i]);
        Sleep(15);
    }
  press_enter();

  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_2 [100] =  "I guess...\n";


      for (int i = 0; i < strlen(Player_Dialogue_2); i++) {
        printf("%c", Player_Dialogue_2[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  june_ascii();
  printline();
  printf(ANSI_COLOR_YELLOW"June: "ANSI_COLOR_RESET);
  char June_Dialogue_3 [100] =  "Great! Meet me at...\n";


      for (int i = 0; i < strlen(June_Dialogue_3); i++) {
        printf("%c", June_Dialogue_3[i]);
        Sleep(15);
    }
  press_enter();


  flicker();


  //#############################################################################
  printline();
  char Narration_1 [100] =  "You wake up in a dark room\n";


      for (int i = 0; i < strlen(Narration_1); i++) {
        printf("%c", Narration_1[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_3 [100] =  "Where am I? Where's June?\n";


      for (int i = 0; i < strlen(Player_Dialogue_3); i++) {
        printf("%c", Player_Dialogue_3[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  printline();
  char Narration_2 [100] =  "From a far corner, you hear the crackle of a radio.\n";


      for (int i = 0; i < strlen(Narration_2); i++) {
        printf("%c", Narration_2[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  radio_ascii();
  printline();
  printf(ANSI_COLOR_YELLOW"June: "ANSI_COLOR_RESET);
  printf("%s! %s! ", Player_Name, Player_Name);
  char June_Dialogue_4 [100] =  "Are you awake?\n";


      for (int i = 0; i < strlen(June_Dialogue_4); i++) {
        printf("%c", June_Dialogue_4[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_4 [100] =  "June! Where are you? Where am I?\n";


      for (int i = 0; i < strlen(Player_Dialogue_4); i++) {
        printf("%c", Player_Dialogue_4[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  radio_ascii();
  printline();
  printf(ANSI_COLOR_YELLOW"June: "ANSI_COLOR_RESET);
  char June_Dialogue_5 [100] =  "I don't know either, but I can see a map-!\n";


      for (int i = 0; i < strlen(June_Dialogue_5); i++) {
        printf("%c", June_Dialogue_5[i]);
        Sleep(15);
    }
  press_enter();


  flicker();


  //#############################################################################
  radio_ascii();
  printline();
  printf(ANSI_COLOR_RED"???: "ANSI_COLOR_RESET);
  char mystery_Dialogue_1 [100] =  "Enough of that yapping.\n";


      for (int i = 0; i < strlen(mystery_Dialogue_1); i++) {
        printf("%c", mystery_Dialogue_1[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  radio_ascii();
  printline();
  printf(ANSI_COLOR_RED"???: "ANSI_COLOR_RESET);
  printf("%s, ", Player_Name);
  char mystery_Dialogue_2 [100] =  "was it? It's about time you woke up.\n";


      for (int i = 0; i < strlen(mystery_Dialogue_2); i++) {
        printf("%c", mystery_Dialogue_2[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_5 [100] =  "Who are you? What have you done to June?!\n";


      for (int i = 0; i < strlen(Player_Dialogue_5); i++) {
        printf("%c", Player_Dialogue_5[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  radio_ascii();
  printline();
  printf(ANSI_COLOR_RED"???: "ANSI_COLOR_RESET);
  char mystery_Dialogue_3 [100] =  "I said enough. You will obey me and stay in that room.\n";


      for (int i = 0; i < strlen(mystery_Dialogue_3); i++) {
        printf("%c", mystery_Dialogue_3[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_6 [100] =  "Why should I listen to you?\n";


      for (int i = 0; i < strlen(Player_Dialogue_6); i++) {
        printf("%c", Player_Dialogue_6[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  radio_ascii();
  printline();
  printf(ANSI_COLOR_RED"???: "ANSI_COLOR_RESET);
  char mystery_Dialogue_4 [100] =  "Do you really think you have a choice?\n";


      for (int i = 0; i < strlen(mystery_Dialogue_4); i++) {
        printf("%c", mystery_Dialogue_4[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  printline();
  char Narration_3 [115] =  "The line goes dead.\n\nUpon inspection the radio seems to have been broken years ago\n\n\nYou deem it useless.\n";


      for (int i = 0; i < strlen(Narration_3); i++) {
        printf("%c", Narration_3[i]);
        Sleep(15);
    }
  press_enter();
}


 //CARD GAME
  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_7 [100] =  "What did I get myself into?\n";


      for (int i = 0; i < strlen(Player_Dialogue_7); i++) {
        printf("%c", Player_Dialogue_7[i]);
        Sleep(15);
    }
  press_enter();
  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_8 [100] =  "Anyway. No time to think about this, I need to find June-\n";


      for (int i = 0; i < strlen(Player_Dialogue_8); i++) {
        printf("%c", Player_Dialogue_8[i]);
        Sleep(15);
    }
  press_enter();
  flicker();
  //#############################################################################
  ghost_ascii();
  printline();
  printf(ANSI_COLOR_CYAN"???: "ANSI_COLOR_RESET);
  char stubbornghost_Dialogue_1 [100] =  "Do you believe in luck?\n";


      for (int i = 0; i < strlen(stubbornghost_Dialogue_1); i++) {
        printf("%c", stubbornghost_Dialogue_1[i]);
        Sleep(15);
    }
  press_enter();
  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_9 [100] =  "Wh-What?\n";


      for (int i = 0; i < strlen(Player_Dialogue_9); i++) {
        printf("%c", Player_Dialogue_9[i]);
        Sleep(15);
    }
  press_enter();

  //#############################################################################
  ghost_ascii();
  printline();
  printf(ANSI_COLOR_CYAN"???: "ANSI_COLOR_RESET);
  char stubbornghost_Dialogue_2 [100] =  "What were the chances that he chose YOU?\n";


      for (int i = 0; i < strlen(stubbornghost_Dialogue_2); i++) {
        printf("%c", stubbornghost_Dialogue_2[i]);
        Sleep(15);
    }
  press_enter();
  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_10 [100] =  "What do you mean?\n";


      for (int i = 0; i < strlen(Player_Dialogue_10); i++) {
        printf("%c", Player_Dialogue_10[i]);
        Sleep(15);
    }
  press_enter();
  //#############################################################################
  ghost_ascii();
  printline();
  printf(ANSI_COLOR_CYAN"???: "ANSI_COLOR_RESET);
  char stubbornghost_Dialogue_3 [100] =  "The game is simple.\n";


      for (int i = 0; i < strlen(stubbornghost_Dialogue_3); i++) {
        printf("%c", stubbornghost_Dialogue_3[i]);
        Sleep(15);
    }
  press_enter();

  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_11 [100] =  "I don't understand. I don't have time for-\n";


      for (int i = 0; i < strlen(Player_Dialogue_11); i++) {
        printf("%c", Player_Dialogue_11[i]);
        Sleep(15);
    }
  press_enter();

  flicker();
  card_game();

  //#############################################################################
  printline();
  char Narration_4 [115] =  "You hastily make your leave, passing door after door.\n";
      for (int i = 0; i < strlen(Narration_4); i++) {
        printf("%c", Narration_4[i]);
        Sleep(15);
    }
  press_enter();
  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_12 [100] =  "I really need to find June, but this place is a maze!\n";


      for (int i = 0; i < strlen(Player_Dialogue_12); i++) {
        printf("%c", Player_Dialogue_12[i]);
        Sleep(15);
    }
  press_enter();

  //#############################################################################
  printline();
  char Narration_5 [117] =  "You enter a room. A bang sounds from behind you. The room is coated with darkness.\n\nThe door behind you is locked.\n";


      for (int i = 0; i < strlen(Narration_5); i++) {
        printf("%c", Narration_5[i]);
        Sleep(15);
    }
  press_enter();

  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_13 [100] =  "What the hell is this place?\n";


      for (int i = 0; i < strlen(Player_Dialogue_13); i++) {
        printf("%c", Player_Dialogue_13[i]);
        Sleep(15);
    }
  press_enter();

  //#############################################################################
  fardoor_ascii();
  char Narration_6 [117] =  "You notice the faint outline of another door at the far end of the room.\n";


      for (int i = 0; i < strlen(Narration_6); i++) {
        printf("%c", Narration_6[i]);
        Sleep(15);
    }
  press_enter();

  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_14 [100] =  "That better be the exit.\n";


      for (int i = 0; i < strlen(Player_Dialogue_14); i++) {
        printf("%c", Player_Dialogue_14[i]);
        Sleep(15);
    }
  press_enter();

  flicker();

//GHOST ESCAPE GAME
  ghostescape_game();

  if (ghostescape_game_state == 1){


        flicker();
        system("color 4F");


  //#############################################################################
  printline();
  char Narration_7 [117] =  "You feel a chill down your spine.\n";


      for (int i = 0; i < strlen(Narration_7); i++) {
        printf("%c", Narration_7[i]);
        Sleep(15);
    }
  press_enter();
  system("color 0F");


  //#############################################################################
  ghost_ascii();
  printline();
  printf(ANSI_COLOR_CYAN"Stubborn Ghost: "ANSI_COLOR_RESET);
  char stubbornghost_Dialogue_7 [100] =  "My... how generous.... Your life.... is .... So good!\n";


      for (int i = 0; i < strlen(stubbornghost_Dialogue_7); i++) {
        printf("%c", stubbornghost_Dialogue_7[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  printline();
  char Narration_8 [117] =  "You begin to feel more fatigued.\n";


      for (int i = 0; i < strlen(Narration_8); i++) {
        printf("%c", Narration_8[i]);
        Sleep(15);
    }
  press_enter();


  // life DEDUCTION 1
  life_state();


    }
    else if (ghostescape_game_state == 2){
        flicker();
        system("color 2F");
  //#############################################################################
  printline();
  char Narration_9 [117] =  "You make it through the door.\n";


      for (int i = 0; i < strlen(Narration_9); i++) {
        printf("%c", Narration_9[i]);
        Sleep(15);
    }
  press_enter();


        system("color 0F");


  //#############################################################################
  ghost_ascii();
  printline();
  printf(ANSI_COLOR_CYAN"Stubborn Ghost: "ANSI_COLOR_RESET);
  char stubbornghost_Dialogue_8 [100] =  "I see now what makes him want you...\n";


      for (int i = 0; i < strlen(stubbornghost_Dialogue_8); i++) {
        printf("%c", stubbornghost_Dialogue_8[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_15 [100] =  "!!!\n\nWhat are you talking about!\n";


      for (int i = 0; i < strlen(Player_Dialogue_15); i++) {
        printf("%c", Player_Dialogue_15[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  ghost_ascii();
  printline();
  printf(ANSI_COLOR_CYAN"Stubborn Ghost: "ANSI_COLOR_RESET);
  char stubbornghost_Dialogue_9 [150] =  "You want to live. He wants to live. Live longer. Live forever.\n\nHe wants it so bad. He makes everything about it.\n\nDon't disappoint him...\n";


      for (int i = 0; i < strlen(stubbornghost_Dialogue_9); i++) {
        printf("%c", stubbornghost_Dialogue_9[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  printline();
  char Narration_10 [117] =  "The door closes behind you.\n";


      for (int i = 0; i < strlen(Narration_10); i++) {
        printf("%c", Narration_10[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_17 [100] =  "What? Live forever? That's impossible!\n";


      for (int i = 0; i < strlen(Player_Dialogue_17); i++) {
        printf("%c", Player_Dialogue_17[i]);
        Sleep(15);
    }
  press_enter();


    }
    else
    printf("\n\nerror");


  //#############################################################################
  printline();
  char Narration_11 [180] =  "You make your way through the hall. To no avail, all the doors are locked.\n\nBanging of pots and clinking of metal sounds through the next door.\n\nYou open to see...\n\nA chef?\n";


      for (int i = 0; i < strlen(Narration_11); i++) {
        printf("%c", Narration_11[i]);
        Sleep(15);
    }
  press_enter();

  //#############################################################################
  printline();
  char Narration_12 [180] =  "You step inside quietly while the chef has its back to you. You walk slowly, attempting to sneak past him.\n";


      for (int i = 0; i < strlen(Narration_12); i++) {
        printf("%c", Narration_12[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  printline();
  printf(ANSI_COLOR_MAGENTA"The Chef: "ANSI_COLOR_RESET);
  char thechef_Dialogue_1 [150] =  "I smell a rat.\n";


      for (int i = 0; i < strlen(thechef_Dialogue_1); i++) {
        printf("%c", thechef_Dialogue_1[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  printline();
  char Narration_13 [180] =  "You feel a breath over your ear.\n";


      for (int i = 0; i < strlen(Narration_13); i++) {
        printf("%c", Narration_13[i]);
        Sleep(15);
    }
  press_enter();

  //#############################################################################
  chef_base_ascii();
  printline();
  printf(ANSI_COLOR_MAGENTA"The Chef: "ANSI_COLOR_RESET);
  char thechef_Dialogue_2 [150] =  "Well hello there, rat.\n\nWhy don't you come join me and make His meal?\n";


      for (int i = 0; i < strlen(thechef_Dialogue_2); i++) {
        printf("%c", thechef_Dialogue_2[i]);
        Sleep(15);
    }
  press_enter();


  flicker();


 //BURGER GAME
  burger_game();


  if (burgergame_game_state == 1){// win
  //#############################################################################
    flicker();
    system("color 2F");
    printline();
    char Narration_19 [202] =  "The Chef smiles as you shakily place the burger in front of him.\n";
        for (int i = 0; i < strlen(Narration_19); i++) {
          printf("%c", Narration_19[i]);
          Sleep(15);
      }
    getchar();
    press_enter();
    system("color 0F");


 


  //#############################################################################
    chef_base_ascii();
    printline();
    printf(ANSI_COLOR_MAGENTA"The Chef: "ANSI_COLOR_RESET);
    char thechef_Dialogue_8 [150] =  "Well done, rat... Perhaps you are worthy to be His next host...\n";


        for (int i = 0; i < strlen(thechef_Dialogue_8); i++) {
          printf("%c", thechef_Dialogue_8[i]);
          Sleep(15);
      }
    press_enter();
   


  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_18 [100] =  "What? A host?\n";


      for (int i = 0; i < strlen(Player_Dialogue_18); i++) {
        printf("%c", Player_Dialogue_18[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
    chef_base_ascii();
    printline();
    printf(ANSI_COLOR_MAGENTA"The Chef: "ANSI_COLOR_RESET);
    char thechef_Dialogue_9 [150] =  "Don't tell me, you're unaware? He does not deserve to die, and so he must live...\n\nHe will live... through You.\n";


        for (int i = 0; i < strlen(thechef_Dialogue_9); i++) {
          printf("%c", thechef_Dialogue_9[i]);
          Sleep(15);
      }
    press_enter();


  //#############################################################################
    printline();
    char Narration_20 [202] =  "The Chef pours himself a glass of wine.\n\nYou take this opportunity to throw a plate at him and run.\n";


        for (int i = 0; i < strlen(Narration_20); i++) {
          printf("%c", Narration_20[i]);
          Sleep(15);
      }
    press_enter();


  //#############################################################################
    chef_angrier_ascii();
    printline();
    printf(ANSI_COLOR_MAGENTA"The Chef: "ANSI_COLOR_RESET);
    char thechef_Dialogue_10 [150] =  "YOU RAT!\n";


        for (int i = 0; i < strlen(thechef_Dialogue_10); i++) {
          printf("%c", thechef_Dialogue_10[i]);
          Sleep(15);
      }
    char Narration_21 [202] =  "\n\nYou slam the door behind you.\n";


        for (int i = 0; i < strlen(Narration_21); i++) {
          printf("%c", Narration_21[i]);
          Sleep(15);
      }
    press_enter();
  }
  else if(burgergame_game_state = 2){//lose
    
    flicker();
    system("color 4F");


  //#############################################################################
  printline();
  char Narration_14 [180] =  "The Chef carefully inspects the burger. He turns to you with a glare.\n";


      for (int i = 0; i < strlen(Narration_14); i++) {
        printf("%c", Narration_14[i]);
        Sleep(15);
    }
  press_enter();
  system("color 0F");


  //#############################################################################
  chef_angry_ascii();
  printline();
  printf(ANSI_COLOR_MAGENTA"The Chef: "ANSI_COLOR_RESET);
  char thechef_Dialogue_6 [150] =  "Wrong. Wrong. Wrong. ";


      for (int i = 0; i < strlen(thechef_Dialogue_6); i++) {
        printf("%c", thechef_Dialogue_6[i]);
        Sleep(15);
    }
  printf(ANSI_COLOR_RED"WRONG!\n\n"ANSI_COLOR_RESET);
  char thechef_Dialogue_7 [150] =  "Do you think this is worthy for a Leader?! A disgrace.";


      for (int i = 0; i < strlen(thechef_Dialogue_7); i++) {
        printf("%c", thechef_Dialogue_7[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  printline();
  char Narration_15 [180] =  "He takes your face and forcibly opens your mouth.\n\nYou struggle, but his grip only tightens.\n";


      for (int i = 0; i < strlen(Narration_15); i++) {
        printf("%c", Narration_15[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  chef_angrier_ascii();
  printline();
  printf(ANSI_COLOR_MAGENTA"The Chef: "ANSI_COLOR_RESET);
  char thechef_Dialogue_8 [150] =  "EAT. YOU RAT. EAT!\n";


      for (int i = 0; i < strlen(thechef_Dialogue_8); i++) {
        printf("%c", thechef_Dialogue_8[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  printline();
  char Narration_16 [202] =  "He shoves your creation into your mouth, and you feel the tears prick at your eyes.\n\nHe moves your jaw as if you chew the meat, and you have no choice to swallow.\n\nYour mind starts to feel fuzzy.\n";


      for (int i = 0; i < strlen(Narration_16); i++) {
        printf("%c", Narration_16[i]);
        Sleep(15);
    }
  press_enter();

  life_state();

  if(life==0){
    //#############################################################################
    printline();
    char Narration_17 [202] =  "The last thing you feel is your body hitting the floor. Darkness consumes you.\n";


        for (int i = 0; i < strlen(Narration_17); i++) {
          printf("%c", Narration_17[i]);
          Sleep(15);
      }
    press_enter();
    flicker();
    system("color 4F");
      system("cls");


      //CultSmile Ascii
      cultleader_smile_ascii();
      Sleep(1000);
      flicker();
      system("cls");


      Sleep(2000);
      system("color 0F");


        exit(0);
    //gameover
  }
  else if (life==1 || life == 2 || life == 3){
    //#############################################################################
    printline();
    char Narration_18 [202] =  "The Chef lets you go. You take this as a chance to run with what little consciousness you have left.\n\nFrom behind you hear him yell.\n";


        for (int i = 0; i < strlen(Narration_18); i++) {
          printf("%c", Narration_18[i]);
          Sleep(15);
      }
    press_enter();


    //#############################################################################
    chef_base_ascii();
    printline();
    printf(ANSI_COLOR_MAGENTA"The Chef: "ANSI_COLOR_RESET);
    char thechef_Dialogue_8 [150] =  "No matter how much you run, you can't ever escape Him!\n";


        for (int i = 0; i < strlen(thechef_Dialogue_8); i++) {
          printf("%c", thechef_Dialogue_8[i]);
          Sleep(15);
      }
    press_enter();
  }
  }

  //#############################################################################
    printline();
    char Narration_22 [300] =  "Pumped up with adrenaline, you keep running through the seemingly endless halls.\n\nFinally, you reach the Nave. The windows are boarded up with wood, but some bits of sunlight slips through.\n\nYou look towards the entrance.\n\nIt's blocked.\n";


        for (int i = 0; i < strlen(Narration_22); i++) {
          printf("%c", Narration_22[i]);
          Sleep(15);
      }
    press_enter();


  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_20 [100] =  "I need to find a way out before I get June.\n";


      for (int i = 0; i < strlen(Player_Dialogue_20); i++) {
        printf("%c", Player_Dialogue_20[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
    printline();
    char Narration_23 [300] =  "You make a round of the church. You have half a mind to smash through the wood and glass windows.\n\nYou spot a small doorway tucked into one of the corners of the church.\n\nInside, you assume it is a room of preparation. And...\n\nA door!\n\nYou rush towards it.\n\nThere's a lock.\n";


        for (int i = 0; i < strlen(Narration_23); i++) {
          printf("%c", Narration_23[i]);
          Sleep(15);
      }
    press_enter();


  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_21 [200] =  "A passcode? Just what kind of church is this?!\n\nWhatever, I need to find June.\n\nYou go back into the nave, planning to trace your steps to look for June.\n";


      for (int i = 0; i < strlen(Player_Dialogue_21); i++) {
        printf("%c", Player_Dialogue_21[i]);
        Sleep(15);
    }
  press_enter();
 
  //#############################################################################

  printline();
  printf(ANSI_COLOR_RED"???: "ANSI_COLOR_RESET);
  char mystery_Dialogue_5 [100] =  "Where do you think you're going?\n";


      for (int i = 0; i < strlen(mystery_Dialogue_5); i++) {
        printf("%c", mystery_Dialogue_5[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
    printline();
    char Narration_24 [202] =  "You turn to the altar, and there she is.\n\nJune.\n\nExcept... she didn't sound like June.\n";


        for (int i = 0; i < strlen(Narration_24); i++) {
          printf("%c", Narration_24[i]);
          Sleep(15);
      }
    press_enter();


  //#############################################################################
  printline();
  printf(ANSI_COLOR_RED"???: "ANSI_COLOR_RESET);
  char mystery_Dialogue_6 [100] =  "Didn't I tell you to stay in the room? \n";


      for (int i = 0; i < strlen(mystery_Dialogue_6); i++) {
        printf("%c", mystery_Dialogue_6[i]);
        Sleep(15);
    }
  char Narration_25 [202] =  "\n\nShe sounded like the man from the radio.\n";


        for (int i = 0; i < strlen(Narration_25); i++) {
          printf("%c", Narration_25[i]);
          Sleep(15);
      }
  press_enter();
 
  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_99 [100] =  "June? Why do you sound like that? Let's go!\n";


      for (int i = 0; i < strlen(Player_Dialogue_99); i++) {
        printf("%c", Player_Dialogue_99[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  printline();
  printf(ANSI_COLOR_RED"???: "ANSI_COLOR_RESET);
  char mystery_Dialogue_7 [150] =  "It seems I made a mistake with you. You're quite stupid.\n\nYour... friend, June... She merely served to be a vessel.\n";


      for (int i = 0; i < strlen(mystery_Dialogue_7); i++) {
        printf("%c", mystery_Dialogue_7[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  flicker();
  cultleader_base_ascii();
  printline();
  printf(ANSI_COLOR_RED"The Cult Leader: "ANSI_COLOR_RESET);
  char cultleader_Dialogue_1 [200] =  "Behold, mortal. You stand upon the sacred grounds of eternal life!\n\nYou, like many others, are the path to my destiny!\n";


      for (int i = 0; i < strlen(cultleader_Dialogue_1); i++) {
        printf("%c", cultleader_Dialogue_1[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_22 [100] =  "Why would I want to be part of your sick destiny!\n";


      for (int i = 0; i < strlen(Player_Dialogue_22); i++) {
        printf("%c", Player_Dialogue_22[i]);
        Sleep(15);
    }
  press_enter();


  //#############################################################################
  cultleader_base_ascii();
  printline();
  printf(ANSI_COLOR_RED"The Cult Leader: "ANSI_COLOR_RESET);
  char cultleader_Dialogue_2 [200] =  "Ungrateful vermin, people would offer themselves up to me! You are not above eternal life.\n";


      for (int i = 0; i < strlen(cultleader_Dialogue_2); i++) {
        printf("%c", cultleader_Dialogue_2[i]);
        Sleep(15);
    }


  char Narration_26 [202] =  "\n\n\n\nThey step closer and closer.\n";


        for (int i = 0; i < strlen(Narration_26); i++) {
          printf("%c", Narration_26[i]);
          Sleep(15);
      }
  press_enter();

  //#############################################################################
  player_ascii();
  printline();
  printf(ANSI_COLOR_GREEN"%s: "ANSI_COLOR_RESET, Player_Name);
  char Player_Dialogue_23 [100] =  "You're crazy! Freak!\n";


      for (int i = 0; i < strlen(Player_Dialogue_23); i++) {
        printf("%c", Player_Dialogue_23[i]);
        Sleep(15);
    }

  char Narration_27 [320] =  "\n\n\n\nYou bolt away from them, making a bee-line to the room you found. You hear footsteps running after you.\n\nHastily, you shut the door behind you, scrambling to make a barricade.\n\nA bench, a cabinet, a chair- anything to block the door.\n\nYou hurry to the exit, the lock glinting.\n\nYour only chance of escape.\n";


        for (int i = 0; i < strlen(Narration_27); i++) {
          printf("%c", Narration_27[i]);
          Sleep(15);
      }
  press_enter();


  flicker();


  //PASSWORD
  password_game();


}