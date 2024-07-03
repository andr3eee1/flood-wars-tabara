#include <stdio.h>
#include <ctype.h>

#define MAXN 50
#define NJUC 2

char viz[NJUC][MAXN + 1][MAXN + 1];

int readInt(FILE *fin) {
  int nr, ch;
  while(!isdigit(ch = fgetc(fin)));
  nr = 0;
  do {
    nr = nr * 10 + ch - '0';
  } while(isdigit(ch = fgetc(fin)));
  return nr;
}

int main() {
  FILE *fin;
  int ch, l, c, juc, op, failed;
  
  fin = fopen("rez.out", "r");
  failed = 0;
  while(isalpha(ch = fgetc(fin)) && failed == 0) {
    op = 0;
    if(ch == 'A') {
      op = 2;
    }
    while(isalpha(ch = fgetc(fin))); // citim cuvantul si spatiul de dupa
    if(op == 0) {
      op = (ch == 'D' ? 1 : 0);
    }
    if(op > 0) {
      l = readInt(fin); // citim coordonatele 
      c = readInt(fin);
      juc = readInt(fin); // am citit si \n-ul de dupa
    }
  
    if(op == 0) { // reset
      for(juc = 0; juc < NJUC; juc++) {
        for(l = 1; l <= MAXN; l++) {
          for(c = 1; c <= MAXN; c++) {
            viz[juc][l][c] = 0;
          }
        }
      }
    } else if(op == 1) { // remove
      if(viz[juc][l][c] == 0) {
        printf("FAILED: remove %d %d %d\n", l, c, juc);
        failed = 1;
      }
      viz[juc][l][c] = 0;
    } else { // add
      if(viz[juc][l][c] > 0) {
        printf("FAILED: add %d %d %d\n", l, c, juc);
        failed = 1;
      }
      viz[juc][l][c] = 1;
    }
  }
  fclose(fin);

  if(failed == 0) {
    printf("All good :)\n");
  }

  return 0;
}
