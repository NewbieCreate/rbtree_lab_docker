#include "rbtree.h"
#include <stdlib.h>

// 새로운 레드블랙 트리를 생성하는 함수
rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree)); // 트리 구조체 동적 할당
  if (p == NULL) return NULL;

  p->nil = (node_t *)calloc(1, sizeof(node_t)); // NIL 노드 생성 및 초기화
  p->nil->color = RBTREE_BLACK;                  // NIL 노드는 항상 BLACK
  p->nil->left = p->nil;                         // NIL의 왼쪽 자식은 자기 자신
  p->nil->right = p->nil;                        // NIL의 오른쪽 자식도 자기 자신
  p->nil->parent = p->nil;                       // NIL의 부모도 자기 자신

  p->root = p->nil; // 루트 노드는 처음에 NIL로 초기화
  return p;
}

// 트리의 노드를 후위 순회하며 메모리 해제
void delete_node(node_t *cur, node_t *nil){
  if (cur == nil) return;              // 기저 조건: NIL에 도달하면 재귀 종료
  delete_node(cur->left, nil);         // 왼쪽 서브트리 노드 해제 재귀 호출
  delete_node(cur->right, nil);        // 오른쪽 서브트리 노드 해제 재귀 호출
  free(cur);                          // 현재 노드 메모리 해제
}

// 전체 트리 해제 함수
void delete_rbtree(rbtree *t) {
  delete_node(t->root, t->nil);       // 루트부터 모든 노드 삭제
  free(t->nil);                      // NIL 노드 메모리 해제
  free(t);                          // 트리 구조체 메모리 해제
}

// 왼쪽 회전 연산 (x 기준)
void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right;              // y는 x의 오른쪽 자식
  x->right = y->left;                // y의 왼쪽 자식을 x의 오른쪽 자식으로 변경
  if (y->left != t->nil) {           
    y->left->parent = x;             // y의 왼쪽 자식의 부모를 x로 변경
  }
  y->parent = x->parent;             // y의 부모를 x의 부모로 변경
  if (x->parent == t->nil) {
    t->root = y;                    // x가 루트면 y를 루트로 변경
  } else if (x == x->parent->left) {
    x->parent->left = y;            // x가 왼쪽 자식이면 y로 변경
  } else {
    x->parent->right = y;           // x가 오른쪽 자식이면 y로 변경
  }
  y->left = x;                      // x를 y의 왼쪽 자식으로 변경
  x->parent = y;                    // x의 부모를 y로 변경
}

// 오른쪽 회전 연산 (x 기준)
void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x->left;              // y는 x의 왼쪽 자식
  x->left = y->right;               // y의 오른쪽 자식을 x의 왼쪽 자식으로 변경
  if (y->right != t->nil) {
    y->right->parent = x;           // y의 오른쪽 자식의 부모를 x로 변경
  }
  y->parent = x->parent;            // y의 부모를 x의 부모로 변경
  if (x->parent == t->nil) {
    t->root = y;                   // x가 루트면 y를 루트로 변경
  } else if (x == x->parent->right) {
    x->parent->right = y;          // x가 오른쪽 자식이면 y로 변경
  } else {
    x->parent->left = y;           // x가 왼쪽 자식이면 y로 변경
  }
  y->right = x;                    // x를 y의 오른쪽 자식으로 변경
  x->parent = y;                   // x의 부모를 y로 변경
}

// 삽입 후 색 균형을 맞추는 함수 (CASE 1~3 처리)
void insert_fixup(rbtree *t, node_t *z) {
  while (z->parent->color == RBTREE_RED) {    // 부모가 RED인 동안 반복
    if (z->parent == z->parent->parent->left) {
      node_t *y = z->parent->parent->right;   // 삼촌 노드 설정
      if (y->color == RBTREE_RED) {            // CASE 1: 부모와 삼촌이 RED인 경우
        z->parent->color = RBTREE_BLACK;       // 부모 BLACK으로 변경
        y->color = RBTREE_BLACK;                // 삼촌 BLACK으로 변경
        z->parent->parent->color = RBTREE_RED;  // 조상 RED로 변경
        z = z->parent->parent;                  // 조상 노드로 이동
      } else {
        if (z == z->parent->right) {            // CASE 2: 삼촌 BLACK, 꺾인 경우
          z = z->parent;
          left_rotate(t, z);                     // 회전으로 일직선 형태로 만듦
        }
        // CASE 3: 삼촌 BLACK, 일직선인 경우
        z->parent->color = RBTREE_BLACK;        
        z->parent->parent->color = RBTREE_RED;  
        right_rotate(t, z->parent->parent);     // 조상 기준 오른쪽 회전
      }
    } else {
      node_t *y = z->parent->parent->left;     // 삼촌 노드 (반대쪽)
      if (y->color == RBTREE_RED) {            // CASE 1 (대칭)
        z->parent->color = RBTREE_BLACK;       
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->left) {             // CASE 2 (대칭)
          z = z->parent;
          right_rotate(t, z);
        }
        // CASE 3 (대칭)
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;               // 루트는 항상 BLACK으로 유지
}

// 새 노드를 삽입하는 함수
node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *newnode = (node_t *)malloc(sizeof(node_t));
  if (newnode == NULL) return NULL;

  newnode->key = key;
  newnode->color = RBTREE_RED;
  newnode->left = newnode->right = newnode->parent = t->nil;

  node_t *cur = t->root;
  node_t *parent = t->nil;

  // 중복 키도 허용하도록 <= 조건으로 탐색
  while (cur != t->nil) {
    parent = cur;
    if (key <= cur->key) {   // 중복 키도 왼쪽 서브트리에 삽입
      cur = cur->left;
    } else {
      cur = cur->right;
    }
  }

  newnode->parent = parent;
  if (parent == t->nil) {
    t->root = newnode; // 트리가 비어있을 때
  } else if (key <= parent->key) {
    parent->left = newnode;
  } else {
    parent->right = newnode;
  }

  insert_fixup(t, newnode); // 균형 복구
  return newnode;
}

// 특정 키를 가진 노드를 찾는 함수
node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *cur = t->root;
  while (cur != t->nil) {
    if (key == cur->key) {
      return cur;  // 가장 먼저 발견한 노드 반환
    } else if (key < cur->key) {
      cur = cur->left;
    } else {
      cur = cur->right;
    }
  }
  return NULL;
}

// 최소값 노드를 반환
node_t *rbtree_min(const rbtree *t) {
  node_t *cur = t->root;                              // 루트부터 시작
  while (cur->left != t->nil) {
    cur = cur->left;                                  // 왼쪽 자식으로 계속 이동
  }
  return cur;                                         // 최소값 노드 반환
}

// 최대값 노드를 반환
node_t *rbtree_max(const rbtree *t) {
  node_t *cur = t->root;                              // 루트부터 시작
  while (cur->right != t->nil) {
    cur = cur->right;                                 // 오른쪽 자식으로 계속 이동
  }
  return cur;                                         // 최대값 노드 반환
}

// u 노드를 v로 대체하는 함수
void transplant(rbtree *t, node_t *u, node_t *v) {
  if (u->parent == t->nil) {
    t->root = v;                                      // u가 루트면 v를 루트로
  } else if (u == u->parent->left) {
    u->parent->left = v;                              // u가 왼쪽 자식이면 v로 대체
  } else {
    u->parent->right = v;                             // u가 오른쪽 자식이면 v로 대체
  }
  v->parent = u->parent;                              // 항상 부모 연결 처리
}

// 후계자(오른쪽 서브트리의 최소 노드) 반환
node_t *successor(rbtree *t, node_t *x) {
  x = x->right;                                       // 오른쪽 서브트리로 이동
  while (x->left != t->nil) {
    x = x->left;                                      // 왼쪽 자식으로 계속 이동
  }
  return x;                                           // 후계자 노드 반환
}

// 삭제 후 균형 복구 함수 (CASE 1~4)
void erase_fixup(rbtree *t, node_t *x) {
  while (x != t->root && x->color == RBTREE_BLACK) { // x가 루트가 아니고 BLACK인 동안 반복
    if (x == x->parent->left) {                       // x가 왼쪽 자식일 때
      node_t *w = x->parent->right;                   // w는 x의 형제
      if (w->color == RBTREE_RED) {                    // CASE 1: w가 RED인 경우
        w->color = RBTREE_BLACK;                       // w를 BLACK으로
        x->parent->color = RBTREE_RED;                 // 부모를 RED로
        left_rotate(t, x->parent);                      // 부모 기준 왼쪽 회전
        w = x->parent->right;                           // 새로운 형제 갱신
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) { // CASE 2: w와 자식이 모두 BLACK
        w->color = RBTREE_RED;                          // w를 RED로
        x = x->parent;                                  // x를 부모로 올림
      } else {
        if (w->right->color == RBTREE_BLACK) {         // CASE 3: w가 BLACK, 왼쪽 자식 RED, 오른쪽 자식 BLACK
          w->left->color = RBTREE_BLACK;                // w의 왼쪽 자식을 BLACK으로
          w->color = RBTREE_RED;                         // w를 RED로
          right_rotate(t, w);                            // w 기준 오른쪽 회전
          w = x->parent->right;                          // 새로운 형제 갱신
        }
        // CASE 4: w가 BLACK, 오른쪽 자식 RED
        w->color = x->parent->color;                     // w의 색을 부모 색으로 변경
        x->parent->color = RBTREE_BLACK;                 // 부모를 BLACK으로
        w->right->color = RBTREE_BLACK;                  // w의 오른쪽 자식을 BLACK으로
        left_rotate(t, x->parent);                        // 부모 기준 왼쪽 회전
        x = t->root;                                      // 반복 종료를 위해 x를 루트로 설정
      }
    } else {                                            // x가 오른쪽 자식일 때 (대칭 처리)
      node_t *w = x->parent->left;                      // w는 x의 형제
      if (w->color == RBTREE_RED) {                      // CASE 1 (대칭)
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) { // CASE 2 (대칭)
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        if (w->left->color == RBTREE_BLACK) {            // CASE 3 (대칭)
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        // CASE 4 (대칭)
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;                             // x를 BLACK으로 설정
}

// 노드 삭제 함수
int rbtree_erase(rbtree *t, node_t *z) {
  node_t *y = z;                                      // 삭제할 노드 y 초기화
  node_t *x;                                          // 균형 복구 대상 노드 x 선언
  color_t y_original_color = y->color;                // y의 원래 색 저장

  if (z->left == t->nil) {                             // 자식이 왼쪽에 없으면
    x = z->right;                                      // x는 오른쪽 자식
    transplant(t, z, z->right);                        // z 대신 오른쪽 자식 대체
  } else if (z->right == t->nil) {                     // 자식이 오른쪽에 없으면
    x = z->left;                                       // x는 왼쪽 자식
    transplant(t, z, z->left);                         // z 대신 왼쪽 자식 대체
  } else {
    y = successor(t, z);                               // y를 z의 후계자로 설정
    y_original_color = y->color;                       // y의 원래 색 저장
    x = y->right;                                      // x는 y의 오른쪽 자식
    if (y->parent == z) {                              // y의 부모가 z일 경우
      x->parent = y;                                   // x의 부모를 y로 변경
    } else {
      transplant(t, y, y->right);                      // y 대신 y의 오른쪽 자식 대체
      y->right = z->right;                             // z의 오른쪽 자식을 y의 오른쪽 자식으로 연결
      y->right->parent = y;                            // 오른쪽 자식의 부모를 y로 설정
    }
    transplant(t, z, y);                               // z 대신 y 대체
    y->left = z->left;                                 // z의 왼쪽 자식을 y의 왼쪽 자식으로 연결
    y->left->parent = y;                               // 왼쪽 자식의 부모를 y로 설정
    y->color = z->color;                               // y의 색을 z의 색으로 설정
  }

  if (y_original_color == RBTREE_BLACK) {             // 원래 색이 BLACK이면 균형 복구 필요
    erase_fixup(t, x);
  }
  free(z);                                            // 삭제 노드 메모리 해제
  return 0;
}

// 중위 순회로 정렬된 배열에 키 삽입
void inorder(const node_t *cur, const rbtree *t, key_t *arr, size_t n, int *index) {
  if (cur == t->nil) return;                          // NIL 노드에 도달하면 종료
  inorder(cur->left, t, arr, n, index);              // 왼쪽 서브트리 재귀 호출
  if (*index < (int)n) {                              // 배열 범위 체크
    arr[(*index)++] = cur->key;                       // 현재 노드 키 배열에 저장
  }
  inorder(cur->right, t, arr, n, index);             // 오른쪽 서브트리 재귀 호출
}

// 트리의 키들을 배열로 저장
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  int index = 0;                                      // 배열 인덱스 초기화
  inorder(t->root, t, arr, n, &index);                // 중위 순회로 배열 채우기
  return 0;
}