/*

gcc branch.c -o branch && ./branch <code> [argument 1] [argument 2] ...

*/

#include "limits.h"
#include "signal.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define make_node(pos, parnode, init)                    \
  if (mmax-- == 0) {                                     \
    fputs("You have reached the memory limit!", stderr); \
    return 0;                                           \
  }                                                      \
  pos = malloc(sizeof(struct Node));                     \
  pos->parent = parnode;                                 \
  pos->left = NULL;                                      \
  pos->right = NULL;                                     \
  pos->value = init;
#define savepreg if (regindex < 13) pregisters[regindex++] =
#define create(side)                       \
  if (pointer->side == NULL) {             \
    make_node(pointer->side, pointer, 0); \
    savepreg pointer->side;                \
  }
#define create_parent(side)          \
  if (pointer->parent == NULL) {     \
    create(parent);                  \
    pointer->parent->parent = NULL;  \
    pointer->parent->side = pointer; \
  }
#define binop(fn)                                   \
  if (pointer->left == NULL) {                      \
    make_node(pointer->left, pointer, readint());  \
    savepreg pointer->left;                         \
  }                                                 \
  if (pointer->right == NULL) {                     \
    make_node(pointer->right, pointer, readint()); \
    savepreg pointer->right;                        \
  }                                                 \
  pointer->value = fn(pointer->left->value, pointer->right->value);
#define lli long long int

int mmax = INT_MAX;
int outlim = INT_MAX;

struct Node {
  struct Node* parent;
  struct Node* left;
  struct Node* right;
  lli value;
};

lli readint() {
  lli x;
  if (feof(stdin)) return 0;
  scanf("%lld", &x);
  return x;
}

void _free_mem(struct Node* item, struct Node* last) {
  if (item->parent != NULL && item->parent != last) {
    _free_mem(item->parent, item);
  }
  if (item->left != NULL && item->left != last) {
    _free_mem(item->left, item);
  }
  if (item->right != NULL && item->right != last) {
    _free_mem(item->right, item);
  }
  free(item);
}

void free_mem(struct Node* item) {
  _free_mem(item, item);
}

void _prettyprint(struct Node* root, struct Node* node, int indent) {
  if (root == NULL) {
    for (int i = 0; i < indent; i++) printf("|-- ");
    puts("(null)");
    return;
  }
  if (root->left != NULL || root->right != NULL) {
    _prettyprint(root->left, node, indent + 1);
  }
  for (int i = 0; i < indent; i++) printf("|-- ");
  printf("%lld", root->value);
  if (root == node) printf(" *");
  puts("");
  if (root->left != NULL || root->right != NULL) {
    _prettyprint(root->right, node, indent + 1);
  }
}

void prettyprint(struct Node* node) {
  struct Node* orig = node;
  while (node->parent) node = node->parent;
  _prettyprint(node, orig, 0);
}

lli add(lli x, lli y) {
  return x + y;
}
lli sub(lli x, lli y) {
  return x - y;
}
lli mul(lli x, lli y) {
  return x * y;
}
lli dvd(lli x, lli y) {
  if (y == 0) return 0;
  return x / y;
}
lli mod(lli x, lli y) {
  if (y == 0) return 0;
  return (x % y + y) % y;
}
lli lt(lli x, lli y) {
  return x < y ? 1 : 0;
}
lli gt(lli x, lli y) {
  return x > y ? 1 : 0;
}
lli and(lli x, lli y) {
  return x & y;
}
lli or(lli x, lli y) {
  return x | y;
}
lli expt(lli x, lli y) {
  if (y < 0) return 0;
  lli v = 1;
  while (y--) v *= x;
  return v;
}

struct StackNode {
  int value;
  struct StackNode* next;
};

int main(int argc, char** argv) {
  int codi = 1;
  if (argc >= 2 && strcmp(argv[1], "-m") == 0) {
    if (argc >= 4) {
      mmax = atoi(argv[2]);
      outlim = atoi(argv[3]);
      codi = 4;
    } else {
      fputs("You have entered the memory flag without enough arguments!", stderr);
      return 0;
    }
  }
  if (argc <= codi) {
    fputs("You have not entered any code! The code "
      "should go in the a command line argument.", stderr);
    return 0;
  }

  char* code = argv[codi];

  int count = 0;
  int bal = 0;
  int index = 0;
  while (code[index]) {
    if (code[index] == '[') {
      count++;
      bal++;
    } else if (code[index] == ']') {
      bal--;
      if (bal < 0) {
        fputs("Your brackets are not balanced (missing open bracket)!", stderr);
        return 0;
      }
    }
    index++;
  }
  if (bal > 0) {
    fputs("Your brackets are not balanced (missing close bracket)!", stderr);
    return 0;
  }
  int len = index;
  int otc[len];
  int cto[len];
  int stack[count];
  int stackindex = 0;
  index = 0;
  while (code[index]) {
    if (code[index] == '[') {
      stack[stackindex++] = index;
    } else if (code[index] == ']') {
      otc[stack[--stackindex]] = index;
      cto[index] = stack[stackindex];
    }
    index++;
  }
  index = 0;
  int vregisters[13] = { 0 };
  bool sregisters[13] = { false };
  struct Node* pregisters[13] = { NULL };
  int regindex = 1;
  struct Node* pointer;
  make_node(pointer, NULL, 0);
  if (argc >= codi + 2) {
    vregisters[0] = pointer->value = atoi(argv[codi + 1]);
    sregisters[0] = true;
  }
  pregisters[0] = pointer;
  for (int argi = codi + 2; argi < argc; argi++) {
    make_node(pointer->left, pointer, atoi(argv[argi]));
    pointer = pointer->left;
    if (argi < codi + 14) {
      vregisters[argi - 1 - codi] = pointer->value;
      sregisters[argi - 1 - codi] = true;
    }
  }
  while (pointer->parent != NULL) pointer = pointer->parent;
  struct StackNode* ipstack = NULL;
  while (code[index]) {
    if (code[index] >= '0' && code[index] <= '9') {
      lli num = code[index] - '0';
      while (code[++index] >= '0' && code[index] <= '9') {
        num = num * 10 + code[index] - '0';
      }
      index--;
      pointer->value = num;
    } else if (code[index] == '.') {
      printf("%c", (char) pointer->value);
      if (--outlim <= 0) {
        fputs("The output was too long and was truncated!\n\n", stderr);
        return 0;
      }
    } else if (code[index] == '#') {
      printf("%lld", pointer->value);
      lli v = pointer->value;
      if (v < 0) {
        outlim--;
        v = -v;
      }
      if (v == 0) {
        outlim--;
      } else {
        while (v > 0) {
          outlim--;
          v /= 10;
          if (v == 0) outlim--;
        }
      }
      if (outlim <= 0) {
        fputs("The output was too long and was truncated!\n\n", stderr);
        return 0;
      }
    } else if (code[index] == ',') {
      pointer->value = (long long int) getchar();
      if (feof(stdin)) {
        pointer->value = 0;
      }
    } else if (code[index] == '$') {
      pointer->value = readint();
    } else if (code[index] == '^') {
      create_parent(left);
      pointer = pointer->parent;
    } else if (code[index] == '/') {
      create(left);
      pointer = pointer->left;
    } else if (code[index] == '\\') {
      create(right);
      pointer = pointer->right;
    } else if (code[index] == ';') {
      create_parent(left);
      pointer->value = pointer->parent->value;
    } else if (code[index] == '+') {
      binop(add);
    } else if (code[index] == '-') {
      binop(sub);
    } else if (code[index] == '*') {
      binop(mul);
    } else if (code[index] == ':') {
      binop(dvd);
    } else if (code[index] == '%') {
      binop(mod);
    } else if (code[index] == '<') {
      binop(lt);
    } else if (code[index] == '>') {
      binop(gt);
    } else if (code[index] == '&') {
      binop(and);
    } else if (code[index] == '|') {
      binop(or);
    } else if (code[index] == '\'') {
      binop(expt);
    } else if (code[index] == '~') {
      if (ipstack != NULL) {
        struct StackNode* temp = ipstack->next;
        index = ipstack->value;
        free(ipstack);
        ipstack = temp;
      }
    } else if (code[index] == '!') {
      pointer->value = pointer->value == 0 ? 1 : 0;
    } else if (code[index] == '_') {
      pointer->value = -pointer->value;
    } else if (code[index] == '@') {
      struct StackNode* csn = malloc(sizeof(struct StackNode));
      csn->value = index;
      csn->next = ipstack;
      ipstack = csn;
      index = 0;
      continue;
    } else if (code[index] == '?') {
      if (pointer->value == 0) {
        create(left);
        pointer = pointer->left;
      } else {
        create(right);
        pointer = pointer->right;
      }
    } else if (code[index] == '[') {
      if (pointer->value == 0) {
        index = otc[index];
      }
    } else if (code[index] == ']') {
      if (pointer->value != 0) {
        index = cto[index];
      }
    } else if (code[index] >= 'A' && code[index] <= 'M') {
      pregisters[code[index] - 'A'] = pointer;
      regindex = 13;
    } else if (code[index] >= 'a' && code[index] <= 'm') {
      if (pregisters[code[index] - 'a'] != NULL) {
        pointer = pregisters[code[index] - 'a'];
      }
    } else if (code[index] >= 'N' && code[index] <= 'Z') {
      vregisters[code[index] - 'N'] = pointer->value;
      sregisters[code[index] - 'N'] = true;
    } else if (code[index] >= 'n' && code[index] <= 'z') {
      if (sregisters[code[index] - 'n']) {
        pointer->value = vregisters[code[index] - 'n'];
      }
    } else if (code[index] == '(') {
      create_parent(right);
      if (pointer == pointer->parent->right) {
        struct Node* a = pointer->parent->left;
        struct Node* b = pointer->left;
        struct Node* c = pointer->right;
        lli tempval = pointer->parent->value;
        pointer->parent->value = pointer->value;
        pointer = pointer->parent;
        pointer->left = pointer->right;
        pointer->left->value = tempval;
        pointer->left->left = a;
        pointer->left->right = b;
        pointer->right = c;
      } else {
        create(right);
        create(left);
        int t = pointer->value;
        pointer->value = pointer->right->value;
        pointer->right->value = pointer->left->value;
        pointer->left->value = t;
        pointer = pointer->left;
      }
    } else if (code[index] == ')') {
      create_parent(left);
      if (pointer == pointer->parent->left) {
        struct Node* a = pointer->left;
        struct Node* b = pointer->right;
        struct Node* c = pointer->parent->right;
        lli tempval = pointer->parent->value;
        pointer->parent->value = pointer->value;
        pointer = pointer->parent;
        pointer->right = pointer->left;
        pointer->left->value = tempval;
        pointer->right->left = b;
        pointer->right->right = c;
        pointer->left = a;
      } else {
        create(left);
        create(right);
        lli t = pointer->value;
        pointer->value = pointer->left->value;
        pointer->left->value = pointer->right->value;
        pointer->right->value = t;
        pointer = pointer->right;
      }
    } else if (code[index] == '{') {
      pointer->value--;
    } else if (code[index] == '}') {
      pointer->value++;
    } else if (code[index] == '\"') {
      create_parent(left);
      pointer->parent->value = pointer->value;
    } else if (code[index] == '`') {
      prettyprint(pointer);
    } else if (code[index] == '\n') {
      free_mem(pointer);
      make_node(pointer, NULL, 0);
      pregisters[0] = pointer;
      for (int i = 1; i < 13; i++) pregisters[i] = NULL;
      regindex = 1;
    }
    index++;
  }
  free_mem(pointer);
}
