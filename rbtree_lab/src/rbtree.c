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

void delete_node(node_t *cur, node_t *nil){
  if (cur == nil) return; // node가 nill이랑 같으면 리턴
  delete_node(cur->left, nil); // cur 왼쪽부터 재귀 
  delete_node(cur->right, nil); // 왼쪽 끝나면 오른쪽 재귀 
  free(cur); //동적할당 해제
}

void delete_rbtree(rbtree *t) {
  delete_node(t->root, t->nil); //루트부터 nill까지 재귀 돌려 free 한다
  free(t->nil); // nil노드에 동적할당 해제
  free(t); // t 동적할당 해제
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
  // y부모가 x의 부모가 된다
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
    if (key == cur->key) return t->root;
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

void transplant(rbtree *t, node_t *u, node_t *v) {
 if (u->parent == t->nil){
  t->root = v;
 }
  else if(u == u->parent->left) {
    u->parent->left = v;
  }else{
    u->parent->right = v;
  }
  v->parent = u->parent;
}

void erase_fixup(rbtree *t, node_t *p) {
  node_t *tmp = t->nil; // 색상 교환에 사용할 임시 포인터

 // p가 루트가 아니고 BLACK일 때만 반복
  while (p != t->root && p->color == RBTREE_BLACK) {

    //  왼쪽 자식일 때
    if (p == p->parent->left) {
       // case 1: 형제가 RED인 경우 → 부모와 색 교환, 왼쪽 로테이트 후 case 2~4로 넘어감
      if (p->parent->right->color == RBTREE_RED) {
        tmp = p->parent->color;
        p->parent->color = p->parent->right->color;
        p->parent->right->color = tmp;
        left_rotate(t, p->parent);
        p= p->parent->right;
        // 새 형제로 갱신
        // 이후 case 2~4를 위함
      }

      // case 2: 형제 및 형제의 자식들이 모두 BLACK
      if (p->parent->right->color == RBTREE_BLACK &&
          p->parent->right->left->color == RBTREE_BLACK &&
          p->parent->right->right->color == RBTREE_BLACK) {
        p->parent->right->color = RBTREE_RED; // 형제를 RED로 바꾸고
        p = p->parent;  // 부모를 대상으로 위로 올라감
      }
       // case 3: 형제는 BLACK이고, 형제의 왼쪽 자식이 RED, 오른쪽 자식은 BLACK
      else if (p->parent->right->color == RBTREE_BLACK &&
               p->parent->right->right->color == RBTREE_BLACK &&
               p->parent->right->left->color == RBTREE_RED) {
        p->parent->right->color = RBTREE_RED;
        p->parent->right->left->color = RBTREE_BLACK; 
        right_rotate(t, p->parent->right); // 형제를 기준으로 오른쪽 로테이트해서 case 4 상황으로 만듦
        p = p->parent->right;    // case 4에서 처리할 형제 갱신
      }
      // case 4: 형제는 BLACK이고, 형제의 오른쪽 자식이 RED
      else {
        tmp = p->parent->color;
        p->parent->color = p->parent->right->color;
        p->parent->right->color = tmp;
        p->parent->right->right->color = RBTREE_BLACK;
        left_rotate(t, p->parent); // 부모 기준으로 왼쪽 로테이트
        p = t->root;               // 루트까지 블랙 복원 종료
      }
    }

    // 💡 오른쪽 자식일 때
    else {
      // case 1: 형제가 RED인 경우 → 부모와 색 교환, 우측로테이트 후 case 2~4로 넘어감
      if (p->parent->left->color == RBTREE_RED) {
        tmp = p->parent->color;
        p->parent->color = p->parent->left->color;
        p->parent->left->color = tmp;
        right_rotate(t, p->parent); // 부모 기준으로 우측로테이트
        p = p->parent->left;        // 형제 갱신
        // 이후 case 2~4를 위함
      }

      // case 2: 형제 및 형제의 자식들이 모두 BLACK
      if (p->parent->left->color == RBTREE_BLACK &&
          p->parent->left->left->color == RBTREE_BLACK &&
          p->parent->left->right->color == RBTREE_BLACK) {
        p->parent->left->color = RBTREE_RED;
        p = p->parent;
      }
      // case 3: 형제는 BLACK이고, 형제의 오른쪽 자식은 BLACK, 왼쪽 자식이 RED
      else if (p->parent->left->color == RBTREE_BLACK &&
               p->parent->left->left->color == RBTREE_RED &&
               p->parent->left->right->color == RBTREE_BLACK) {
        p->parent->left->color = RBTREE_RED;
        p->parent->left->left->color = RBTREE_BLACK;
        left_rotate(t, p->parent->left);  // 형제를 기준으로 좌측 로테이트
        p = p->parent->left;              // case 4 위해 형제 갱신
      }
       // case 4: 형제는 BLACK이고, 형제의 왼쪽 자식이 RED
      else {
        tmp = p->parent->color;
        p->parent->color = p->parent->left->color;
        p->parent->left->color = tmp;
        p->parent->left->left->color = RBTREE_BLACK; 
        right_rotate(t, p->parent);                  // 부모 기준으로 우측 로테이트
        p = t->root;                                 // 루트로 올라가면서 종료
      } 
    }
  }
  // 마지막 블랙 복원
  p->color = RBTREE_BLACK;
}
  
node_t *successor(rbtree *t, node_t *p){
    node_t *cur = t->nil;
    cur = p -> right;
    while (cur->left != t->nil) {
    cur = cur -> left;
    }
    return cur;
}

int rbtree_erase(rbtree *t, node_t *p) {
  node_t *cur = t->root;
  node_t *ep;

  while (cur != t->nil) {
    // p를 찾았을 때만 삭제 진행
    if (p == cur) {

      // case 1: 자식이 둘 다 nil인 리프 노드
      if (cur->left == t->nil && cur->right == t->nil) {
        if (cur->color == RBTREE_RED) {
          transplant(t, cur, t->nil);
          free(cur);
        } else {
          ep = cur;
          transplant(t, ep, t->nil);
          erase_fixup(t, ep);
          free(cur);
        }
      }

      // case 2: 자식이 한 명만 있는 경우
      else if (cur->left == t->nil || cur->right == t->nil) {
        node_t *child = (cur->left != t->nil) ? cur->left : cur->right;
        transplant(t, cur, child);
        ep = child;

        if (cur->color == RBTREE_BLACK) {
          if (child->color == RBTREE_RED) {
            child->color = RBTREE_BLACK;
          } else {
            erase_fixup(t, ep);
          }
        }

        free(cur);
      }

      // case 3: 자식이 두 명 있는 경우
      else {
        ep = successor(t, cur);  // 오른쪽 서브트리에서 가장 작은 노드

        // 후계자의 오른쪽 자식이 존재하는 경우
        if (ep->right != t->nil) {
          transplant(t, ep, ep->right);
        } else {
          transplant(t, ep, t->nil);
        }

        transplant(t, cur, ep);

        ep->left = cur->left;
        ep->right = cur->right;

        if (ep->left != t->nil) ep->left->parent = ep;
        if (ep->right != t->nil) ep->right->parent = ep;

        int deleted_color = ep->color;
        ep->color = cur->color;

        if (deleted_color == RBTREE_BLACK) {
          erase_fixup(t, ep->right);
        }

        free(cur);
      }

      break;  // 삭제 완료 시 루프 종료
    }

    // 탐색 계속
    if (p->key < cur->key) {
      cur = cur->left;
    } else {
      cur = cur->right;
    }
  }

  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
