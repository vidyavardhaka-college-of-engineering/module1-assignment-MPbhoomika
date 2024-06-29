#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
 //Sample input 3 0 1 3 4 2 5 7 8 6 
//Sample input 3 8 1 3 4 0 2 7 6 5 
int ** goal;
int N;

void allocate_mem(int ** * arr) {
  * arr = (int ** ) malloc(N * sizeof(int * ));
  int i;
  for (i = 0; i < N; i++)
    ( * arr)[i] = (int * ) malloc(N * sizeof(int));
}

void deallocate_mem(int ** * arr) {
  int i, N;
  for (i = 0; i < N; i++)
    free(( * arr)[i]);
  free( * arr);
}

void createTiles(int ** tiles, int ** arr) {
  int i, j;
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
      tiles[i][j] = arr[i][j];
}

void createGoal(void) {
  int i, j;
  allocate_mem( & goal);
  int filler = 1;
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      if (i == N - 1 && j == N - 1)
        goal[i][j] = 0;
      else
        goal[i][j] = filler++;
    }
  }
}

void display(int ** t) {
  int i, j;
  printf("\n%d\n", N);
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      if (t[i][j] == 0) printf("  ");
      else
        printf("%d ", t[i][j]);
    }
    printf("\n");
  }
}

int hamming(int ** tiles) {
  int count = 0, j, i;
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++) {
      if (tiles[i][j] == 0) continue;
      if (tiles[i][j] != goal[i][j]) count++;
    }
  return count;
}

bool isGoal(int ** t) {
  int i, j;
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
      if (t[i][j] != goal[i][j]) return false;
  return true;
}

bool equals(int ** p, int ** q) {
  int i, j;
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
      if (p[i][j] != q[i][j]) return false;
  return true;
}

void swap(int ** surface, int x1, int y1, int x2, int y2) {
  int temp = surface[x1][y1];
  surface[x1][y1] = surface[x2][y2];
  surface[x2][y2] = temp;
}

void copy(int ** toRet, int ** origin) {
  int i, j;
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
      toRet[i][j] = origin[i][j];
}

//finds twin of board by exchanging any two blocks
void boardTwin(int ** toRet) {
  int i, j;
  for (i = 0; i < N; i++) {
    for (j = 0; j < N - 1; j++) {
      if (toRet[i][j] != 0 && toRet[i][j + 1] != 0) {
        swap(toRet, i, j, i, j + 1);
      }
    }
  }
}

typedef struct board_ {
  int ** data;
  // Lower values indicate higher priority 
  int cost;
  int level;
  struct board_ * parent;
}
board;

board * newBoard(int ** arr, board * parent, int level) {
  board * temp = (board * ) malloc(sizeof(board));
  allocate_mem( & (temp -> data));
  createTiles(temp -> data, arr);
  temp -> cost = hamming(arr);
  temp -> level = level;
  temp -> parent = parent;
  return temp;
}

typedef struct node {
  board * b;
  
  int priority;
  struct node * next;
}
Node;

Node * newNode(board * brd) {
  Node * temp = (Node * ) malloc(sizeof(Node));
  temp -> b = brd;
  temp -> priority = brd -> cost + brd -> level;
  temp -> next = NULL;
  return temp;
}


board * peek(Node ** head) {
  return ( * head) -> b;
}


void pop(Node ** head) {
  Node * temp = * head;
  ( * head) = ( * head) -> next;
  free(temp);
}


void push(Node ** head, board * d) {
  Node * start = ( * head);
  
  Node * temp = newNode(d);
  int p = d -> cost + d -> level;
  if (( * head) -> priority > p) {
    
    temp -> next = * head;
    ( * head) = temp;
  } else {

    
    while (start -> next != NULL && start -> next -> priority < p) {
      start = start -> next;
    }

     
    temp -> next = start -> next;
    start -> next = temp;
  }
}


int isEmpty(Node ** head) {
  return ( * head) == NULL;
}
void pushNeighbors(board * brd, Node * pq) {
  int i, j, stop = 0;
  int ** temp, ** t;
  allocate_mem( & temp);
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++)
      if (brd -> data[i][j] == 0) {
        stop = 1;
        break;
      }
    if (stop == 1) break;
  }
  if (i + 1 < N) {
    copy(temp, brd -> data);
    swap(temp, i + 1, j, i, j);
    board * dChild = newBoard(temp, brd, brd -> level + 1);
    if (pq == NULL) {
      pq = newNode(dChild);
    } else
      push( & pq, dChild);
  }
  if (j - 1 >= 0) {
    copy(temp, brd -> data);
    swap(temp, i, j - 1, i, j);
    board * lChild = newBoard(temp, brd, brd -> level + 1);
    if (pq == NULL) {
      pq = newNode(lChild);
    } else
      push( & pq, lChild);
  }
  if (i - 1 >= 0) {
    copy(temp, brd -> data);
    swap(temp, i - 1, j, i, j);
    board * uChild = newBoard(temp, brd, brd -> level + 1);
    if (pq == NULL) {
      pq = newNode(uChild);
    } else
      push( & pq, uChild);
  }

  if (j + 1 < N) {
    copy(temp, brd -> data);
    swap(temp, i, j + 1, i, j);
    board * rChild = newBoard(temp, brd, brd -> level + 1);
    if (pq == NULL) {
      pq = newNode(rChild);
    } else
      push( & pq, rChild);
  }

}

void printPath(board * root) {
  if (root == NULL) return;
  else printPath(root -> parent);
  display(root -> data);
}

void solve(int ** arr) {
  board * root = newBoard(arr, NULL, 0);
  Node * pq = newNode(root);
  int d = 0;
  while (!isEmpty( & pq)) {
    d++;
    board * peeked = peek( & pq);
    if (isGoal(peeked -> data)) {
      printf("\nPath");
      printPath(peeked);
      return;
    }
  
    if (d >= 2500) {
      printf(". ");
      d = 0;
    }
    pushNeighbors(peeked, pq);
    pop( & pq);
  }
}

int main() {
  
  int i, j, ** arr;
  printf("Enter input:");
  scanf("%d", & N);
  createGoal();
  arr = malloc(N * sizeof(int * )); // N is the number of the rows
  for (i = 0; i < N; i++)
    arr[i] = malloc(N * sizeof(int)); // N is the number of the columns
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++) {
      scanf("%d", & arr[i][j]);
    }
  solve(arr);
  return 0;
}