/*
  Nume program   : baby-gronk.c
  Nume concurent : Rares Hanganu
  E-mail         : rares.hanganur@gmail.com
*/
#include <stdio.h>
#include <ctype.h>
#include <sys/time.h>

#define MAXN 50
#define MAXM 50
#define MAX_DEPTH 30
#define MAXCH 128
#define NCUL 5
#define NDIR 4
#define INFINIT 2000000000
#define MAX_TIME 990000 // 0.99 secunde
#define QSIZE 2048

// jucatorul care este la mutare (ramane constant in tot programul)
// | 'J' | 'S' |
// |  0  |  1  |
int juc;

int n, m; // dimensiunile tablei de joc
// tabla de joc, bordata cu valori de 0 (culorile incep de la 1)
char board[MAXN + 2][MAXM + 2];
char temp_board[MAXN + 2][MAXM + 2];
char ocupat[MAXN + 2][MAXM + 2];
char vis[MAXN + 2][MAXM + 2];

struct timeval tv;
int gata; // va fi setat la 1 cand nu mai avem timp
long long start_time;
int max_depth;

int dlin[NDIR] = {-1, 0, 1, 0};
int dcol[NDIR] = {0, 1, 0, -1};

// codificarea culorilor:
// | @ | # | + | . | * |
// | 1 | 2 | 3 | 4 | 5 |
char culori[NCUL] = {'@', '#', '+', '.', '*'};
char car2cod[MAXCH];
char cod2car[MAXCH];

int killer_move[MAX_DEPTH];

struct Frontiera {
  int first, last;
  char frontl[QSIZE], frontc[QSIZE];
} front[2];

// timpul curent in microsecunde
static inline long long getTime() {
  gettimeofday(&tv, NULL);
  return ((long long)tv.tv_sec) * 1000000 + tv.tv_usec;
}

// efectueaza o mutare pe tabla
void moveOnBoard(int l, int c, int from, int to) {
  int dir;

  board[l][c] = to;
  for(dir = 0; dir < NDIR; dir++)
    if(board[l + dlin[dir]][c + dcol[dir]] == from)
      moveOnBoard(l + dlin[dir], c + dcol[dir], from, to);
}

// da fill incepand cu coordonata (l, c) celulelor cu culoarea cul,
// marcandu-le in matricea vis cu variabila mark
void fill(int l, int c, int cul, int mark) {
  int dir;

  vis[l][c] = mark;
  for(dir = 0; dir < NDIR; dir++)
    if(vis[l + dlin[dir]][c + dcol[dir]] == 0 &&
       board[l + dlin[dir]][c + dcol[dir]] == cul)
      fill(l + dlin[dir], c + dcol[dir], cul, mark);
}

void fillInglob(int l, int c, int cul) {
  int dir;

  board[l][c] = cul;
  for(dir = 0; dir < NDIR; dir++)
    if(board[l + dlin[dir]][c + dcol[dir]] > 0 && 
       board[l + dlin[dir]][c + dcol[dir]] != cul)
      fillInglob(l + dlin[dir], c + dcol[dir], cul);
}

// afla daca regiunea care incepe in (l, c) este inglobata
int inglobat(int l, int c, int *vecin) {
  int rez, dir;

  rez = 1;
  vis[l][c] = -1;
  for(dir = 0; dir < NDIR; dir++)
    if(vis[l + dlin[dir]][c + dcol[dir]] == 0) {
      if(!inglobat(l + dlin[dir], c + dcol[dir], vecin) || rez == 0)
        rez = 0;
    } else if(vis[l + dlin[dir]][c + dcol[dir]] > -1) {
      if((*vecin) == -1)
        (*vecin) = vis[l + dlin[dir]][c + dcol[dir]];
      else if(vis[l + dlin[dir]][c + dcol[dir]] != (*vecin))
        rez = 0;
    }
  return rez;
}

static inline int evalStatic(int material[2], int distance[2], 
                             int front_lin[2], int front_col[2]) {
  return (material[0] - material[1]) * MAXN * MAXN * MAXN + 
    (distance[1] * distance[1] - distance[0] * distance[0]) + 
    (front_lin[0] * front_col[0] - front_lin[1] * front_col[1]);
}

void updateFront(int juc, int mut, struct Frontiera *front, 
                 char ocupat[MAXN + 2][MAXM + 2], int material[2], 
                 int distance[2], int front_lin[2], int front_col[2]) {
  int i, cnt_cul, dir, l, c, nl, nc;

  cnt_cul = 0;
  for(i = front->first; i != front->last; i = (i + 1) % QSIZE)
    cnt_cul += board[(int)front->frontl[i]][(int)front->frontc[i]] == mut;

  while(cnt_cul > 0) {
    l = front->frontl[front->first];
    c = front->frontc[front->first];
    front->first = (front->first + 1) % QSIZE;
    if(board[l][c] == mut) { // mutarea absoarbe celula (l, c)?
      if(juc == 0) {
        // avem o noua distanta minima?
        if(l - 1 + m - c < distance[0])
          distance[0] = l - 1 + m - c;

        if(n - l + 1 > front_lin[0])
          front_lin[0] = n - l + 1;
        if(c > front_col[0])
          front_col[0] = c;
      } else {
        // avem o noua distanta minima?
        if(n - l + c - 1 < distance[1])
          distance[1] = n - l + c - 1;

        if(l > front_lin[1])
          front_lin[1] = l;
        if(m - c + 1 > front_col[1])
          front_col[1] = m - c + 1;
      }
      material[juc]++; // s-a mai alaturat o celula ariei noastre

      // adaugam noile celula in frontiera
      for(dir = 0; dir < NDIR; dir++) {
        nl = l + dlin[dir];
        nc = c + dcol[dir];
        if(!ocupat[nl][nc]) {
          front->frontl[front->last] = nl;
          front->frontc[front->last] = nc;
          ocupat[nl][nc] = 1;
          if(board[nl][nc] == mut)
            cnt_cul++;
          front->last = (front->last + 1) % QSIZE;
        }
      }

      cnt_cul--;
    } else { // celula ramane in frontiera, nu ii facem nimic
      front->frontl[front->last] = l;
      front->frontc[front->last] = c;
      front->last = (front->last + 1) % QSIZE;
    }
  }
}

int negamax(int depth, int alpha, int beta, struct Frontiera front[2], 
            char ocupat[MAXN + 2][MAXM + 2], int material[2], int distance[2],
            int front_lin[2], int front_col[2], int last_move[2]) {
  int cul, l, c, new_material[2], new_distance[2], new_move[2], 
    new_fl[2], new_fc[2], eval, i, km;
  struct Frontiera new_front[2];
  char new_ocupat[MAXN + 2][MAXM + 2];

  // la nivel cu 5 deasupra nivel maxim ne intrebam daca s-a terminat timpul
  if(max_depth - depth == 5)
    gata = (getTime() - start_time) > MAX_TIME;

  if(!gata && (depth == max_depth || material[0] + material[1] == n * m))
    return (1 - (((depth + juc) & 1) << 1)) * 
      evalStatic(material, distance, front_lin, front_col);

  // avem un killer move la adancimea curenta?
  if(!gata && killer_move[depth] >= 1) {
    cul = killer_move[depth];
    // mutarea este valida?
    if(cul != last_move[0] && cul != last_move[1]) {
      // efectuam mutarea

      // mai intai trebuie sa facem o copie fiecarei variabile pentru care
      // nu putem sa dam undo
      for(i = 0; i < 2; i++) {
        new_material[i] = material[i];
        new_distance[i] = distance[i];
        new_front[i] = front[i];
        new_fl[i] = front_lin[i];
        new_fc[i] = front_col[i];
      }
      for(l = 0; l <= n + 1; l++)
        for(c = 0; c <= m + 1; c++)
          new_ocupat[l][c] = ocupat[l][c];

      updateFront((juc + depth) & 1, cul, &(new_front[(juc + depth) & 1]), 
        new_ocupat, new_material, new_distance, new_fl, new_fc);
      new_move[0] = last_move[0];
      new_move[1] = last_move[1];
      new_move[(juc + depth) & 1] = cul;
      eval = -negamax(depth + 1, -beta, -alpha, new_front, new_ocupat, 
        new_material, new_distance, new_fl, new_fc, new_move);
      if(eval > alpha)
        alpha = eval;
    }
  }

  // salvam killer move-ul deoarece in cazul in care se schimba exista o sansa 
  // sa reevaluam aceeasi mutare
  km = killer_move[depth];
  cul = 1;
  while(!gata && alpha < beta && cul <= NCUL) {
    // daca mutarea este valida si nu este killer move
    if(cul != last_move[0] && cul != last_move[1] && cul != km) {
      // efectuam mutarea

      // mai intai trebuie sa facem o copie fiecarei variabile pentru care
      // nu putem sa dam undo
      for(i = 0; i < 2; i++) {
        new_material[i] = material[i];
        new_distance[i] = distance[i];
        new_front[i] = front[i];
        new_fl[i] = front_lin[i];
        new_fc[i] = front_col[i];
      }
      for(l = 0; l <= n + 1; l++)
        for(c = 0; c <= m + 1; c++)
          new_ocupat[l][c] = ocupat[l][c];

      updateFront((juc + depth) & 1, cul, &(new_front[(juc + depth) & 1]), 
        new_ocupat, new_material, new_distance, new_fl, new_fc);
      new_move[0] = last_move[0];
      new_move[1] = last_move[1];
      new_move[(juc + depth) & 1] = cul;
      eval = -negamax(depth + 1, -beta, -alpha, new_front, new_ocupat, 
        new_material, new_distance, new_fl, new_fc, new_move);
      if(eval > alpha) {
        alpha = eval;
        killer_move[depth] = cul;
      }
    }
    cul++;
  }

  return alpha < beta ? alpha : beta;
}

// calculeaza dimensiunile tablei si citeste tabla de joc
void readBoard() {
  int c, ch;

  // vom citi prima linie separat pentru a afla numarul de coloane al tablei
  m = 0;
  while((ch = fgetc(stdin)) != '\n') // cat timp nu s-a terminat prima linie
    board[1][++m] = car2cod[ch]; // coloanele sunt numerotate de la 1
  n = 1; // am citit deja o linie
  // cat timp exista primul caracter din linie
  while((ch = fgetc(stdin)) != EOF) {
    board[n + 1][1] = car2cod[ch];
    for(c = 2; c <= m; c++)
      board[n + 1][c] = car2cod[fgetc(stdin)];
    fgetc(stdin);
    n++; // am mai citit o linie
  }
}

void writeBoard() {
  int l, c;

  for(l = 1; l <= n; l++) {
    for(c = 1; c <= m; c++)
      fputc(cod2car[(int)board[l][c]], stdout);
    fputc('\n', stdout);
  }
}

void calcOptim() {
  int best_move, material[2], distance[2], l, c, last_move[2], 
    front_lin[2], front_col[2], vecin;

  // vom calcula in scorurile materiale si regiunile inglobate
  for(l = 1; l <= n; l++)
    for(c = 1; c <= m; c++)
      vis[l][c] = 0;
  for(l = 0; l <= n + 1; l++)
    vis[l][0] = vis[l][m + 1] = -1;
  for(c = 0; c <= m + 1; c++)
    vis[0][c] = vis[n + 1][c] = -1;
  // fill jucatorului 1 cu valoarea 1
  fill(n, 1, board[n][1], 1);
  // fill jucatorului 2 cu valoarea 2
  fill(1, m, board[1][m], 2);

  for(l = 1; l <= n; l++)
    for(c = 1; c <= m; c++)
      temp_board[l][c] = board[l][c];
  for(l = 1; l <= n; l++)
    for(c = 1; c <= m; c++) {
      vecin = -1;
      if(vis[l][c] == 0 && inglobat(l, c, &vecin))
        fillInglob(l, c, vecin == 1 ? board[n][1] : board[1][m]);
    }

  // initializam frontierele
  front[0] = (struct Frontiera){.first = 0, .last = 1, 
    .frontl = {n}, .frontc = {1}};
  front[1] = (struct Frontiera){.first = 0, .last = 1, 
    .frontl = {1}, .frontc = {m}};

  ocupat[n][1] = ocupat[1][m] = 1;
  for(l = 0; l <= n + 1; l++)
    ocupat[l][0] = ocupat[l][m + 1] = 1;
  for(c = 0; c <= m + 1; c++)
    ocupat[0][c] = ocupat[n + 1][c] = 1;

  material[0] = material[1] = 0;
  distance[0] = distance[1] = n + m - 2;
  front_lin[0] = front_lin[1] = front_col[0] = front_col[1] = 1;
  updateFront(0, board[n][1], &front[0], ocupat, material, distance, 
    front_lin, front_col);
  updateFront(1, board[1][m], &front[1], ocupat, material, distance, 
    front_lin, front_col);

  for(l = 1; l <= n; l++)
    for(c = 1; c <= m; c++)
      board[l][c] = temp_board[l][c];

  last_move[0] = board[n][1];
  last_move[1] = board[1][m];

  gata = 0;
  max_depth = best_move = 1;
  // cat timp nu s-a terminat timpul si nu am ajuns la adancimea
  // maxima (la care oricum nu putem ajunge)
  while(getTime() - start_time <= MAX_TIME && max_depth <= MAX_DEPTH) {
    negamax(0, -INFINIT, INFINIT, front, ocupat, material, distance, 
      front_lin, front_col, last_move);
    max_depth++; // trecem la urmatoarea adancime
    if(!gata)                     // daca cautarea s-a executat pana la capat
      best_move = killer_move[0]; // retinem mutarea optima aflata
  }

  // efectuam mutarea gasita
  if(juc == 0)
    moveOnBoard(n, 1, board[n][1], best_move);
  else
    moveOnBoard(1, m, board[1][m], best_move);
}

int main() {
  int cod;

  // initializam vectorii car2cod si cod2car
  for(cod = 1; cod <= NCUL; cod++) {
    // culori[cod - 1] are asociat codul cod
    car2cod[(int)culori[cod - 1]] = cod;
    // codul cod are asociat culori[cod - 1]
    cod2car[cod] = culori[cod - 1];
  }

  start_time = getTime();

  juc = (fgetc(stdin) == 'S');
  fgetc(stdin);
  readBoard();
  calcOptim();
  fputc(juc == 0 ? 'S' : 'J', stdout);
  fputc('\n', stdout);
  writeBoard();

  return 0;
}
