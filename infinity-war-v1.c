/*
   Nume program   : infinity-war.c
   Nume concurent : Pana Andrei, Danciu Traian, Teodorescu Cosmin, Hanganu Darius
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
//#include "hash.h"

#define MAXN 50
#define MAXM 50
#define INFINIT 2147483647
#define MAXDEPTH 30
#define MAXTIME 980000 // 0.98 sec
#define NCOADA 65536
#define NDIR 4

// ponderii (pentru scor)
#define PONDER_DISTANTA 1
#define PONDER_FRONTIERA 1
#define PONDER_MATERIAL (PONDER_DISTANTA * 100 + PONDER_FRONTIERA * 2500 + 1) 

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
int n,m;
char juc,viz[MAXN + 2][MAXN + 2];

char mut[]={'@','#','+','*','.'};

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
};

struct Scor {
  int arie, dist, frontl, frontc;
  struct Coada frontiera;
} scor[MAXDEPTH + 1][2];

int dlin[NDIR] = {-1, 0, 1, 0}, dcol[NDIR] = {0, 1, 0, -1};

// facem mutare in color pentru jucatorul juc
void makeMove(int color, int juc, int deoth) {
  int ramase, i, l, c, dir, lnou, cnou;

  ramase = 0; // numaram numarul de celule noi neprocesate
  for(i = scor[depth][(int)juc].frontiera.prim; i != scor[depth][(int)juc].frontiera.ultim; i = (i + 1) % NCOADA) {
    if(mat[(int)scor[depth][(int)juc].frontiera.coadal[i]][(int)scor[depth][(int)juc].frontiera.coadac[i]] == color) {
      ramase++;
    }
  }

  while(ramase > 0) { // cat timp mai avem celule noi neprocesate
    // luam prima pozitie din coada
    l = scor[depth][juc].frontiera.coadal[scor[depth][juc].frontiera.prim];
    c = scor[depth][juc].frontiera.coadac[scor[depth][juc].frontiera.prim];
    scor[depth][juc].frontiera.prim = (scor[depth][juc].frontiera.prim + 1) % NCOADA; // avansam in coada

    // printf("%d %d: %c %c %d\n",l,c,mut[color], mut[(int)mat[l][c]], ramase);
    // printf("%d %d\n",scor[depth][juc].frontiera.prim,scor[depth][juc].frontiera.ultim);
    if(mat[l][c] == color) {
      ramase--; // am mai procesat o culoare
      //printf("REMOVED %d %d %d\n", l, c, juc);

      // actualizam scorurile
      scor[depth][juc].arie++;
      if(juc==0){
        scor[depth][0].dist = min(scor[depth][0].dist, l - 1 + m - c);
        scor[depth][0].frontl = max(scor[depth][0].frontl, n + 1 - l);
        scor[depth][0].frontc = max(scor[depth][0].frontc, c);
      }else{
        scor[depth][1].dist = min(scor[depth][1].dist, n - l + c - 1);
        scor[depth][1].frontl = max(scor[depth][1].frontl, l);
        scor[depth][1].frontc = max(scor[depth][1].frontc, m + 1 - c);
      }

      for(dir = 0; dir < NDIR; dir++) {
        lnou = l + dlin[dir];
        cnou = c + dcol[dir];
        if(viz[lnou][cnou] == 0) {
          viz[lnou][cnou] = 1;

          // adaugam noua pozitie in coada
          scor[depth][juc].frontiera.coadal[scor[depth][juc].frontiera.ultim] = lnou;
          scor[depth][juc].frontiera.coadac[scor[depth][juc].frontiera.ultim] = cnou;
          scor[depth][juc].frontiera.ultim = (scor[depth][juc].frontiera.ultim + 1) % NCOADA;

          if(mat[lnou][cnou] == color) {
            ramase++; // avem o noua pozitie de procesat
            //printf("lol\n");
            //printf("ADDED %d %d %d\n", lnou, cnou, juc);
          }
        }
      }
    } else { // nu este buna asa ca o adaugam inapoi in coada
      scor[depth][juc].frontiera.coadal[scor[depth][juc].frontiera.ultim] = l;
      scor[depth][juc].frontiera.coadac[scor[depth][juc].frontiera.ultim] = c;
      scor[depth][juc].frontiera.ultim = (scor[depth][juc].frontiera.ultim + 1) % NCOADA;
    }
  }
  // printf("\n");
}

// evaluarea statica a tablei
static inline int evalStatic(int depth) {
  return PONDER_MATERIAL * (scor[depth][0].arie - scor[depth][1].arie) + // scorul material
         PONDER_DISTANTA * (scor[depth][0].dist - scor[depth][1].dist) + // distanta pana la adversar
         PONDER_FRONTIERA * (scor[depth][0].frontl * scor[depth][0].frontc -
                             scor[depth][1].frontl * scor[depth][1].frontc); // si frontiera de incadrare
}

int negamax(int depth,int alpha,int beta){
  int icolor,score, startKiller, l, c;
  char oldviz[MAXN + 2][MAXN + 2];

  // printf("%d\n", depth);

  if(maxdepth-depth==5){
    cont=((checktime()-tbase)<MAXTIME);
  }

  if(cont&&depth==maxdepth){
    return (1 - 2 * ((depth % 2) ^ juc)) * evalStatic(depth);
  }

  scor[depth + 1][0] = scor[depth][0];
  scor[depth + 1][1] = scor[depth][1];
  for(l = 0; l <= n + 1; l++) {
    for(c = 0; c <= m + 1; c++) {
      viz[depth + 1][l][c] = viz[depth][l][c];
    }
  }

  startKiller = killer[depth];
  if(cont&&killer[depth]>=0){
    icolor=killer[depth];
    if(icolor!=mat[n][1]&&icolor!=mat[1][m]){
      oldscor = scor[depth][(int)juc];
      for(l = 0; l <= n + 1; l++) {
        for(c = 0; c <= m + 1; c++) {
          oldviz[l][c] = viz[l][c];
        }
      }

      makeMove(icolor, juc ^ (depth % 2)); // juc pentru par si juc^1 pentru impar
      
      score=-negamax(depth+1,-beta,-alpha);

      if(score>alpha){
        alpha=score;
      }

      scor[depth][(int)juc] = oldscor;
      for(l = 0; l <= n + 1; l++) {
        for(c = 0; c <= m + 1; c++) {
          viz[l][c] = oldviz[l][c];
        }
      }
    }
  }

  icolor=0;
  while(cont&&alpha<beta&&icolor<5){
    if(icolor!=startKiller&&icolor!=mat[n][1]&&icolor!=mat[1][m]){
      oldscor = scor[depth][(int)juc];
      for(l = 0; l <= n + 1; l++) {
        for(c = 0; c <= m + 1; c++) {
          oldviz[l][c] = viz[l][c];
        }
      }
      makeMove(icolor, juc ^ (depth % 2)); // juc pentru par si juc^1 pentru impar

      score=-negamax(depth+1,-beta,-alpha);

      if(score>alpha){
        alpha=score;
        killer[depth]=icolor;
      }

      scor[depth][(int)juc] = oldscor;
      for(l = 0; l <= n + 1; l++) {
        for(c = 0; c <= m + 1; c++) {
          viz[l][c] = oldviz[l][c];
        }
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

char regiune[MAXN + 2][MAXN + 2];

// marcheaza toate pozitiile care au culoarea color cu val
// ajuta la crearea frontierei initiale
void fillFrontiera(int l, int c, int color, int val) {
  int dir;

  regiune[l][c] = val;
  for(dir = 0; dir < NDIR; dir++) {
    if(regiune[l + dlin[dir]][c + dcol[dir]] == 0 &&
       mat[l + dlin[dir]][c + dcol[dir]] == color) {
      fillFrontiera(l + dlin[dir], c + dcol[dir], color, val);
    }
  }
}

char indRegiune[MAXN + 2][MAXN + 2];
int cnt;

void fillRegiuni(int l, int c) {
  int dir;

  indRegiune[l][c] = cnt;
  for(dir = 0; dir < NDIR; dir++) {
    
  }
}

int main(){
  int l,c,maxicolor;
  char ch;

  tbase = checktime();

  for(l=0;l<5;l++){
    char2int[(int)mut[l]]=l;
    int2char[l]=mut[l];
  }

  juc = (fgetc(stdin) == 'J' ? 0 : 1);
  fgetc(stdin);//'\n'

  // citire matrice
  n=1;
  ch=fgetc(stdin);
  while(!feof(stdin)){
    m=1;
    while(ch!='\n'){
      mat[n][m++]=char2int[(int)ch];
      ch=fgetc(stdin);
    }
    ch=fgetc(stdin);
    n++;
  }
  n--;
  m--;

  // bordare regiune
  for(l = 0; l <= n + 1; l++) {
    regiune[l][0] = regiune[l][m + 1] = -1;
  }
  for(c = 0; c <= m + 1; c++) {
    regiune[0][c] = regiune[n + 1][c] = -1;
  }

  // gasim pozitiile la fiecare 
  fillFrontiera(n, 1, mat[n][1], 1);
  fillFrontiera(1, m, mat[1][m], 2);

  // gasim pozitiile inglobate
  for(l = 1; l <= n; l++) {
    for(c = 1; c <= m; c++) {
      // mergem prin regiune
      if(viz[l][c] == 0) {
        fillRegiune(l, c);
      }
    }
  }

  // resetare killermove
  for(l=0;l<=MAXDEPTH;l++){
    killer[l]=-1;
  }

  maxicolor=100;//ceva fictional, pentru debug
  cont=maxdepth=1;
  while((checktime()-tbase)<MAXTIME&&maxdepth<=MAXDEPTH){
    // initializari frontiera si scoruri
    scor[0][0].dist = scor[0][1].dist = n + m - 2;
    scor[0][0].frontl = scor[0][0].frontc = scor[0][1].frontl = scor[0][1].frontc = 1;
    scor[0][0].frontiera.prim = scor[0][1].frontiera.prim = 0;
    scor[0][0].frontiera.ultim = scor[0][1].frontiera.ultim = 1;
    scor[0][0].frontiera.coadal[0] = n;
    scor[0][0].frontiera.coadac[0] = 1;
    scor[0][1].frontiera.coadal[0] = 1;
    scor[0][1].frontiera.coadac[0] = m;
    viz[n][1] = viz[1][m] = 1;
    makeMove(mat[n][1], 0);
    makeMove(mat[1][m], 1);
    for(l = 1; l <= n; l++) {
      for(c = 1; c <= m; c++) {
        viz[l][c] = 0;
      }
    }
    negamax(0,-INFINIT,INFINIT);
    maxdepth++;
    if(cont){
      maxicolor=killer[0];
    }
  }

  // printf("%d\n", maxdepth);

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
      fputc(int2char[(int)mat[l][c]],stdout);
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
