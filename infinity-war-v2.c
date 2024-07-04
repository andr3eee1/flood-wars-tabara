/*
    Nume program   : infinity-war.c
    Nume concurent : Pana Andrei, Danciu Traian, Teodorescu Cosmin, Hanganu Darius
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> // pentru structura timeval

static inline int min(int a, int b) {
  return a < b ? a : b;
}
static inline int max(int a, int b) {
  return a > b ? a : b;
}

#define MAXTIME 980000 // 0.98 secunde
// cronometrarea timpului
struct timeval tv;
long long start_time;
int cont;
static inline long long checkTime() {
  gettimeofday(&tv, NULL);
  return ((long long)tv.tv_sec) * 1000000 + tv.tv_usec;
}

#define MAXCH 128
char char2int[MAXCH], int2char[] = {0, '@', '#', '+', '*', '.'};
//       valorile incep de la 1 deci matricea e deja bordata cu 0

#define MAXN 50

char mat[MAXN + 2][MAXN + 2];
int n, m, juc;
void citireTabla() {
  int ch, i;

  for(i = 1; i <= 5; i++) {
    char2int[(int)int2char[i]] = i;
  }

  // jucatorul de jos e 0, iar cel de sus e 1
  juc = (fgetc(stdin) == 'J' ? 0 : 1);
  fgetc(stdin); // caracterul '\n'

  n = 0;
  while(char2int[ch = fgetc(stdin)]) {
    m = 0;
    n++;
    do {
      mat[n][++m] = char2int[ch];
    } while(char2int[ch = fgetc(stdin)]);
  }
}
void afisareTabla() {
  int l, c;

  fputc(juc == 0 ? 'S' : 'J', stdout);
  fputc('\n', stdout);

  for(l = 1; l <= n; l++) {
    for(c = 1; c <= m; c++) {
      fputc(int2char[(int)mat[l][c]], stdout);
    }
    fputc('\n', stdout);
  }
}

#define NCOADA 4096
struct Coada {
  int prim, ultim;
  char coadal[NCOADA], coadac[NCOADA];
};

#define MAXDEPTH 30
#define INFINIT 200000000
int max_depth;
char killer[MAXDEPTH + 1]; // e deja 0, nu mai trebuie resetat
int arie[MAXDEPTH + 1][2], dist[MAXDEPTH + 1][2];
int frontl[MAXDEPTH + 1][2], frontc[MAXDEPTH + 1][2];
int player_color[MAXDEPTH + 1][2];
struct Coada frontiera[MAXDEPTH + 1][2];
char viz[MAXDEPTH + 1][MAXN + 2][MAXN + 2];

#define NDIR 4
int dlin[] = {-1, 0, 1, 0}, dcol[] = {0, 1, 0, -1};

#define PONDER_DISTANTA 1
#define MAX_DISTANTA 100
#define PONDER_FRONTIERA 1
#define MAX_FRONTIERA 2500
#define PONDER_MATERIAL (PONDER_DISTANTA * MAX_DISTANTA + PONDER_FRONTIERA * MAX_FRONTIERA + 1)
// evaluarea statica a tablei la adancime depth
int evalStatic(int depth) {
  return PONDER_MATERIAL * (arie[depth][0] - arie[depth][1]) + // scorul material
         PONDER_DISTANTA * (dist[depth][0] - dist[depth][1]) + // distanta pana la adversar
         PONDER_FRONTIERA * (frontl[depth][0] * frontc[depth][0] - // frontiera de incadrare 
                             frontl[depth][1] * frontc[depth][1]);
}

// copiaza datele jucatorilor de la adancimea depth la adancimea depth + 1
void copyPlayerData(int depth) {
  int i, l, c;

  for(i = 0; i < 2; i++) {
    arie[depth + 1][i] = arie[depth][i];
    dist[depth + 1][i] = dist[depth][i];
    frontl[depth + 1][i] = frontl[depth][i];
    frontc[depth + 1][i] = frontc[depth][i];
    frontiera[depth + 1][i] = frontiera[depth][i];
    player_color[depth + 1][i] = player_color[depth][i];
  }

  for(l = 0; l <= n + 1; l++) {
    for(c = 0; c <= m + 1; c++) {
      viz[depth + 1][l][c] = viz[depth][l][c];
    }
  }  
}

// face mutarea color pentru jucatorul juc
void makeMove(int depth, int juc, int color) { // TODO: de scris
  int i, ramase = 0, l, c, dir, lnou, cnou;

  for(i = frontiera[depth][juc].prim; i != frontiera[depth][juc].ultim; i = (i + 1) % NCOADA) {
    ramase += mat[(int)frontiera[depth][juc].coadal[i]][(int)frontiera[depth][juc].coadac[i]] == color;
  }
  while(ramase > 0) {
    l = frontiera[depth][juc].coadal[frontiera[depth][juc].prim];
    c = frontiera[depth][juc].coadac[frontiera[depth][juc].prim];
    frontiera[depth][juc].prim = (frontiera[depth][juc].prim + 1) % NCOADA;
    if(juc == 1) {
//      fprintf(stderr, "%d %d\n", l, c);
    }

    if(mat[l][c] == color) {
      ramase--;
      if(juc == 0) {
        arie[depth][0]++;
        dist[depth][0] = min(dist[depth][0], n - l + c - 1);
        frontl[depth][0] = max(frontl[depth][0], n - l + 1);
        frontc[depth][0] = max(frontc[depth][0], c);
      } else {
        arie[depth][1]++;
        dist[depth][1] = min(dist[depth][1], l - 1 + m - c);
        frontl[depth][1] = max(frontl[depth][1], l);
        frontc[depth][1] = max(frontc[depth][1], m - c + 1);
      }

      for(dir = 0; dir < NDIR; dir++) {
        lnou = l + dlin[dir];
        cnou = c + dcol[dir];
        if((viz[depth][lnou][cnou] & (juc + 1)) == 0) {
          viz[depth][lnou][cnou] |= juc + 1;
          if(mat[lnou][cnou] == color) {
            ramase++;
          }
          frontiera[depth][juc].coadal[frontiera[depth][juc].ultim] = lnou;
          frontiera[depth][juc].coadac[frontiera[depth][juc].ultim] = cnou;
          frontiera[depth][juc].ultim = (frontiera[depth][juc].ultim + 1) % NCOADA;
        }
      }
    } else {
      frontiera[depth][juc].coadal[frontiera[depth][juc].ultim] = l;
      frontiera[depth][juc].coadac[frontiera[depth][juc].ultim] = c;
      frontiera[depth][juc].ultim = (frontiera[depth][juc].ultim + 1) % NCOADA;
    }
  }  
}

// construieste frontiera initiala
void makeFront() { // TODO: de adaugat zonele inglobate (if possible)
  dist[0][0] = dist[0][1] = n + m - 2;
  frontl[0][0] = frontc[0][0] = frontl[0][1] = frontc[0][1] = 1;
  player_color[0][0] = mat[n][1];
  player_color[0][1] = mat[1][m];
  frontiera[0][0].prim = frontiera[0][1].prim = 0;
  frontiera[0][0].ultim = frontiera[0][1].ultim = 1;
  frontiera[0][0].coadal[0] = n;
  frontiera[0][0].coadac[0] = 1;
  frontiera[0][1].coadal[0] = 1;
  frontiera[0][1].coadac[0] = m;
  viz[0][n][1] = 1;
  viz[0][1][m] = 2;
  makeMove(0, 0, mat[n][1]);
  makeMove(0, 1, mat[1][m]);
}

int negamax(int depth, int alpha, int beta) {
  int icolor, start_killer, scor;

  if(max_depth - depth == 5) {
    cont = (checkTime() - start_time < MAXTIME);
  }

  if(cont && depth == max_depth) {
    return (1 - (((depth + juc) & 1) << 1)) * evalStatic(depth);
  }

  start_killer = killer[depth];
  if(cont && killer[depth] > 0) {
    icolor = killer[depth];
    if(icolor != player_color[depth][0] && icolor != player_color[depth][1]) { // daca e mutare valida
      copyPlayerData(depth);
      makeMove(depth + 1, (depth + juc) & 1, icolor);

      scor = -negamax(depth + 1, -beta, -alpha);
      if(scor > alpha) {
        alpha = scor;
      }
    }
  }

  icolor = 1;
  while(cont && alpha < beta && icolor <= 5) {
    // daca n-am facut mutarea deja si daca e mutare valida
    if(icolor != start_killer && icolor != player_color[depth][0] && icolor != player_color[depth][1]) {
      copyPlayerData(depth);
      makeMove(depth + 1, (depth + juc) & 1, icolor);

      scor = -negamax(depth + 1, -beta, -alpha);
      //fprintf(stderr, "%d %d\n", icolor, scor);
      if(scor > alpha) {
        alpha = scor;
        killer[depth] = icolor;
      }
    }

    icolor++;
  }

  return alpha < beta ? alpha : beta;
}

// mutam celulele care au avut culoarea vechi in culoarea nou
void fillMutare(int l, int c, int vechi, int nou) {
  int dir;

  mat[l][c] = nou;
  for(dir = 0; dir < NDIR; dir++) {
    if(mat[l + dlin[dir]][c + dcol[dir]] == vechi) {
      fillMutare(l + dlin[dir], c + dcol[dir], vechi, nou);
    }
  }
}

int main() {
  int final_move, l, c;   

  start_time = checkTime();
  citireTabla();
  makeFront();

  for(l = 0; l <= n + 1; l++) {
    viz[0][l][0] = viz[0][l][m + 1] = 3;
  }
  for(c = 0; c <= m + 1; c++) {
    viz[0][0][c] = viz[0][n + 1][c] = 3;
  }
  
  final_move = 0;
  max_depth = 0;
  cont = 1;
  while((checkTime() - start_time) < MAXTIME && max_depth < MAXDEPTH) {
    max_depth++;
    negamax(0, -INFINIT, INFINIT);
    if(cont) {
      final_move = killer[0];
    }
  }

  //fprintf(stderr, "%d\n", final_move);

  // facem mutarea final_move
  if(juc == 0) {
    fillMutare(n, 1, mat[n][1], final_move);
  } else {
    fillMutare(1, m, mat[1][m], final_move);
  }
  afisareTabla();
  
  return 0;
}
