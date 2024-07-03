/*
   Nume program   : infinity-matrix.c
   Nume concurent : Pana Andrei, Danciu Traian, Teodorescu Cosmin, Hanganu Darius
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "hash.h"

#define MAXN 50
#define MAXM 50
#define INFINIT 2147483647
#define MAXDEPTH 30
#define MAXTIME 980000 // 0.98 sec
#define NCOADA 256
#define NDIR 4

// ponderii (pentru scor)
#define PONDER_MATERIAL 2500 
#define PONDER_DISTANTA 1
#define PONDER_FRONTIERA 1

struct timeval tv;
int cont;
long long tbase;
int maxdepth;

int killer[MAXDEPTH+1]; // killer moves

// timpul curent in microsecunde (milionime de secunda)
static inline long long checktime() {
  gettimeofday(&tv, NULL);
  return ((long long)tv.tv_sec) * 1000000 + tv.tv_usec;
}

char mat[MAXN+2][MAXM+2];
int n,m,scorej,scores;
char juc,mutch;
char viz[MAXN + 2][MAXN + 2];

char mut[]={'@','#','+','*','.'};

char atj,ats;

char int2char[5];
int char2int[127];

static inline int max(int a, int b){
  return a>b?a:b;
}

static inline int min(int a, int b){
  return a<b?a:b;
}

static inline int abs_(int x){
  return (x<0?-x:x);
}

struct Coada {
  int prim, ultim;
  char coadal[NCOADA], coadac[NCOADA];
} frontiera[2];
int dist[2], arie[2], frontl[2], frontc[2];

int dlin[NDIR] = {-1, 0, 1, 0}, dcol[NDIR] = {0, 1, 0, -1};

// facem mutare in color pentru jucatorul juc
void makeMove(int color, int juc) {
  int oldultim, dir, lnou, cnou, l, c;

  // copy paste, urat dar ajuta pentru a evita un test
  oldultim = frontiera[juc].ultim;
  if(juc == 0) { // jucatorul de jos
    while(frontiera[juc].prim != oldultim) {
      // luam prima pozitie din coada
      l = frontiera[juc].coadal[frontiera[juc].prim];
      c = frontiera[juc].coadac[frontiera[juc].prim];
      frontiera[juc].prim = (frontiera[juc].prim + 1) % NCOADA;
    
      for(dir = 0; dir < NDIR; dir++) {
        // coordonatele noi
        lnou = l + dlin[dir];
        cnou = c + dcol[dir];
        
        if(viz[lnou][cnou] == 0 && mat[lnou][cnou] == color) { // daca nu e luata si e culoarea buna
          viz[lnou][cnou] = 1;

          // adaugam noua pozitie in coada
          frontiera[juc].coadal[frontiera[juc].ultim] = lnou;
          frontiera[juc].coadac[frontiera[juc].ultim] = cnou;
          frontiera[juc].ultim = (frontiera[juc].ultim + 1) % NCOADA;

          // actualizam scorurile
          arie[juc]++;
          dist[juc] = min(dist[juc], lnou - 1 + m - cnou);
          frontl[juc] = min(frontl[juc], lnou);
          frontc[juc] = max(frontc[juc], cnou);
        }
      }
    }
  } else { // jucatorul de sus
    while(frontiera[juc].prim != oldultim) {
      // luam prima pozitie din coada
      l = frontiera[juc].coadal[frontiera[juc].prim];
      c = frontiera[juc].coadac[frontiera[juc].prim];
      frontiera[juc].prim = (frontiera[juc].prim + 1) % NCOADA;

      for(dir = 0; dir < NDIR; dir++) {
        // coordonatele noi
        lnou = l + dlin[dir];
        cnou = c + dcol[dir];
        
        if(viz[lnou][cnou] == 0 && mat[lnou][cnou] == color) { // daaca nu e luata si e culoarea buna        
          viz[lnou][cnou] = 1;

          // adaugam noua pozitie in coada
          frontiera[juc].coadal[frontiera[juc].ultim] = lnou;
          frontiera[juc].coadac[frontiera[juc].ultim] = cnou;
          frontiera[juc].ultim = (frontiera[juc].ultim + 1) % NCOADA;

          // actualizam scorurile
          arie[juc]++;
          dist[juc] = min(dist[juc], n - lnou + cnou - 1);
          frontl[juc] = max(frontl[juc], lnou);
          frontc[juc] = min(frontc[juc], cnou);
        }
      }
    }
  }
}

// evaluarea statica a tablei
int evalStatic(int depth) {
  return (1 - depth % 2 * 2) * (PONDER_MATERIAL * (arie[0] - arie[1]) + // scorul material
                                PONDER_DISTANTA * (dist[0] - dist[1]) + // distanta pana la adversar
                                PONDER_FRONTIERA * ((n - frontl[0] + 1) * frontc[0] -
                                                    frontl[1] * (m - frontc[0] + 1))); // si frontiera de incadrare
}

int negamax(int depth,int alpha,int beta){
  int icolor,score;

  if(maxdepth-depth==5){
    cont=((checktime()-tbase)<MAXTIME);
  }

  if(cont&&depth==maxdepth){
    return ((depth+1-juc)%2*2-1)*evalStatic(depth);
  }

  if(cont&&killer[depth]>=0){
    icolor=killer[depth];
    if(icolor!=mat[n][1]&&icolor!=mat[1][m]){
      makeMove(icolor, juc ^ (depth % 2)); // juc pentru par si juc^1 pentru impar
      
      score=-negamax(depth+1,-beta,-alpha);

      if(score>alpha){
        alpha=score;
      }
    }
  }

  icolor=0;
  while(cont&&alpha<beta&&icolor<5){
    if(icolor!=killer[depth]&&icolor!=mat[n][1]&&icolor!=mat[1][m]){
      makeMove(icolor, juc ^ (depth % 2)); // juc pentru par si juc^1 pentru impar

      score=-negamax(depth+1,-beta,-alpha);

      if(score>alpha){
        alpha=score;
        killer[depth]=icolor;
      }
    }
    icolor++;
  }

  return alpha<beta?alpha:beta;
}

// fill care schimba toate pozitiile cu culoarea vechi in nou
void fillMutare(int l, int c, int vechi, int nou) {
  int dir;

  mat[l][c] = nou;
  for(dir = 0; dir < NDIR; dir++) {
    if(mat[l + dlin[dir]][c + dcol[dir]] == vechi) { // daca e de schimbat 
      fillMutare(l + dlin[dir], c + dcol[dir], vechi, nou);
    }
  }
}

int main(){
  int l,c,maxicolor;
  char ch;

  // initializari frontiera si scoruri
  frontiera[0].prim = frontiera[1].prim = 0;
  frontiera[0].ultim = frontiera[1].ultim = 1;
  frontiera[0].coadal[0] = frontl[0] = n;
  frontiera[0].coadac[0] = frontc[0] = 1;
  frontiera[1].coadal[1] = frontl[1] = 1;
  frontiera[1].coadac[1] = frontc[1] = m;
  dist[0] = dist[1] = n + m - 2;
  arie[0] = arie[1] = 1;

  for(l=0;l<5;l++){
    char2int[mut[l]]=l;
    int2char[l]=mut[l];
  }

  juc = (fgetc(stdin) == 'J' ? 0 : 1);
  fgetc(stdin);//'\n'

  n=1;
  ch=fgetc(stdin);
  while(!feof(stdin)){
    m=1;
    while(ch!='\n'){
      mat[n][m++]=char2int[ch];
      ch=fgetc(stdin);
    }
    ch=fgetc(stdin);
    n++;
  }
  n--;
  m--;

  for(l=0;l<=n+1;l++){
    mat[l][0]=mat[l][m+1]=-1;
  }
  for(c=0;c<=m+1;c++){
    mat[0][c]=mat[n+1][c]=-1;
  }

  // resetare killermove
  for(l=0;l<MAXDEPTH;l++){
    killer[l]=-1;
  }

  maxicolor=100;//ceva fictional, pentru debug
  cont=maxdepth=1;
  while((checktime()-tbase)<MAXTIME&&maxdepth<=MAXDEPTH){
    negamax(0,-INFINIT,INFINIT);
    maxdepth++;
    if(cont){
      maxicolor=killer[0];
    }
  }

  // mutarea finala
  if(juc == 0) {
    fillMutare(n, 1, mat[n][1], maxicolor);
  } else {
    fillMutare(1, m, mat[1][m], maxicolor);
  }

  // afisare tabla
  fputc(juc == 0 ? 'S' : 'J', stdout);
  fputc('\n',stdout);
  for(l=1;l<=n;l++){
    for(c=1;c<=m;c++){
      fputc(int2char[mat[l][c]],stdout);
    }
    fputc('\n',stdout);
  }

  return 0;
}
/*
J
.#+#.*.@.@
@.+*@.+*#+
**#++@**#@
#@#@.@@+@#
++@++@#.@.
**##*@*#++
#@@*.+*.*+
*/
