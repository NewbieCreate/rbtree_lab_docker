#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  //트리 구조체 동적생성
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  if (p == NULL) return NULL;

  // 공통 Nill 생성
  // p ->nil = malloc(sizeof(node_t));
  p ->nil = calloc(1, sizeof(node_t));
  p ->nil->color = RBTREE_BLACK;
  // p ->nil ->left = p ->nil ->right = p ->nil ->parent = NULL;
  
  // 루트초기화 (트리가 비어 있음을 나타냄)
  p->root = p->nil;
  return p;
}

void delete_rbtree(rbtree *t) {

  free(t);
}

void left_rotate(rbtree *t, node_t *x) { // x를 기준으로 왼쪽으로 회전
  // x의 오른쪽 자식 y를 가져온다 (y는 회전 후 x의 부모가 될 노드)
  node_t *y = x ->right;
  // y의 왼쪽 서브트리를 x의 오른쪽 자식으로 옮긴다
  x->right = y ->left;
  // 만약에 y의 왼쪽 자식이 nill이 아니라면
  // 그 왼쪽 자식의 부모를 x로 갱신한다.(부모 포인터 재설정)
  if(y ->left != t->nil){
    y->left->parent = x;
  }
  // y패런츠가 x의 패런츠가 된다
  y->parent = x->parent;

  // 만약 x가 루트였다면, y를 새로운 루트로 설정한다.
  if (x->parent == t->nil) {
    t->root = y;
  }
  // x가 부모의 왼쪽자식이였다면 y를 그자리에 연결
  else if(x == x->parent ->left){
    x->parent->left = y;
  }
  //x가 부모의 오른쪽 자식이였다면, y를 그자리에 연결
  else {
    x->parent->right = y;
  }
  // y의 왼쪽 자식으로 x를 연결하고
  y ->left = x;
  //x의 부모를 y로 재설정한다.
  x ->parent = y;
}

void right_rotate(rbtree *t, node_t *x){ // x를 기준으로 오른쪽으로 회전한다.
  //x의 왼쪽 자식 y를 가져온다
  node_t *y = x ->left;
  //y의 오른쪽 서브트리를 x의 왼쪽 자식으로 옮긴다.
  x->left = y -> right;

  //y의 오른쪽 자식이 nill이 아니라면
  //그 자식의 부모를 x로 재설정한다.
  if(y->right != t->nil){
    y->right->parent = x;
  }
  //y의 부모를 x의 부모로 설정
  y->parent = x->parent;
  
  //x가 루트였다면 y를 새로운 루트로 설정한다.
  if (x->parent == t->nil){
    t->root = y;
  }
  // x가 부모의 오른쪽 자식이였다면 y를 그 위치에 연결
  else if(x == x->parent -> right){
    x -> parent -> right = y;
  }
  // x가 부모의 왼쪽 자식이었다면 y를 그 위치에 연결
  else {
    x -> parent ->left = y;
  }

  //y의 오른쪽 자식으로 x를 연결하고 
  y -> right = x;
  //x의 부모를 y로 재설정한다.
  x -> parent = y;
}

void insert_fixup(rbtree *t, node_t *z) {
  // 새로운노드의 부모노드 색이 RED일때 까지 반복
  while (z->parent->color == RBTREE_RED) {
    
    // 새로운노드의 부모님이 새로운노드의 조상의 왼쪽과 같을때
    if (z->parent == z->parent->parent->left) {
      // 삼촌 노드 y는 새로운 노드의 조상의 오른쪽노드가 된다
      node_t *y = z->parent->parent->right; // 삼촌 노드

      // CASE 1.
      // 만약에 새로운노드의 부모가 레드이고 삼촌의 컬러가 레드일때
      if (y->color == RBTREE_RED) {
        // 새로운노드의 부모는 블랙이 되고
        z->parent->color = RBTREE_BLACK;
        // 삼촌노드도 블랙이 된다.
        y->color = RBTREE_BLACK;
        // 새로운노드의 조상의 컬러는 레드가 된다
        z->parent->parent->color = RBTREE_RED;
        // 새로운 노드의 조상을 다시 가리킨다.
        z = z->parent->parent;

      } else {
        // CASE 2
        // if 할아버지/부모/자신이 꺾인상태 
        if (z == z->parent->right) {
          // 새로운 노드를 부모로 지정
          // → 회전 이후 z가 그 위에 올라올 것이므로 미리 재지정
          z = z->parent;
          // z(부모를 기준으로 로테이션)
          // → 왼-왼 구조로 변경하여 Case 3 처리 가능하게 함
          left_rotate(t, z);
        }

        // CASE 3 상태 
        // 조상(G), 부모(P), 자신(z)이 일직선(왼-왼) 구조일 때
        // 부모(P)를 BLACK, 조상(G)을 RED로 바꿔서 색상 위반 해소
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        // 조상(G)을 기준으로 회전해서 트리 균형 회복
        right_rotate(t, z->parent->parent);
      }

    } else {
      // 부모가 조상의 오른쪽 자식일 때 (대칭 상황)
      node_t *y = z->parent->parent->left;

      // CASE 1. 부모와 삼촌이 모두 RED일 때
      if (y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;

      } else {
        // CASE 2: 꺾인 상태 (오른-왼)
        if (z == z->parent->left) {
          z = z->parent;
          right_rotate(t, z);
        }

        // CASE 3: 일직선 상태 (오른-오른)
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }

  // 종료 전: 루트를 BLACK으로 설정
  t->root->color = RBTREE_BLACK;
} 
    
node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *newnode = malloc(sizeof(node_t)); // 새로운 노드를 메모리에 할당
  if (newnode == NULL) return NULL;

  newnode->color = RBTREE_RED; // 삽입규칙에 따라 색은 무조건 RED
  newnode->key = key;          // 값 저장
  newnode->left = newnode->right = newnode->parent = t->nil; // 공통 nil로 초기화

  node_t *cur = t->root;
  node_t *parent = t->nil;

  // 삽입할 위치 탐색 (BST 규칙에 따라)
  while (cur != t->nil) {
    parent = cur;
    if (key < cur->key) {
      cur = cur->left;
    } else {
      cur = cur->right;
    }
  }
  
  // 부모 노드 지정
  newnode->parent = parent;

  // 트리가 비어있다면 (즉, 루트가 nil이면) 새로운 노드를 루트로 설정
  if (parent == t->nil) {
    t->root = newnode;
  } else if (key < parent->key) {
    parent->left = newnode;
  } else {
    parent->right = newnode;
  }

  // Red-Black Tree의 균형을 맞추는 함수 호출
  insert_fixup(t, newnode);

  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {  
  // TODO: implement find
  return t->root;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
