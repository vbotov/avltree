# avltree
AVL tree implementation in C

## Code example
```c
struct t_key {
   int data;
};

struct t_my_struct {
   struct t_key key;
   char value;
   ...
   t_avl_node node;
};

int compare_my_struct(const void *a, const void *b) {
   return ((struct t_key *)a)->data - ((struct t_key *)b)->data;
}

void print_my_struct(void *n) {
   const struct t_my_struct *v = (const struct t_my_struct *)n;
   printf("%d=%c,", v->key.data, v->value);
}

...
t_avl_tree tree = avl_tree_new(offsetof(struct t_my_struct, node), offsetof(struct t_my_struct, key), compare_my_struct);

struct t_my_struct nodeA = {.key = {.data = 1}, .value = 'A'};
avl_ins(&tree, &nodeA); //should return NULL, tree: [1]=A

struct t_my_struct nodeB = {.key = {.data = 2}, .value = 'B'};
avl_ins(&tree, &nodeB); //should return NULL, tree: [1]=A,[2]=B

struct t_my_struct nodeC = {.key = {.data = 3}, .value = 'C'};
avl_ins(&tree, &nodeC); //should return NULL, tree: [1]=A,[2]=B,[3]=C

struct t_my_struct nodeD = {.key = {.data = 2}, .value = 'D'};
struct t_my_struct *old = avl_ins(&tree, &nodeD); //should return a pointer to [2]=B, and replace nodeB with nodeD in tree: [1]=A,[2]=D,[3]=C

struct t_key key1 = {.data = 1};
struct t_my_struct *find = avl_find(&tree, &key1); //should return a pointer to [1]=A

struct t_key key3 = {.data = 3};
struct t_my_struct *deleted = avl_del(&tree, &key3); //should delete [3]=C and return a pointer to it, tree: [1]=A,[2]=D

avl_apply(&tree, print_my_struct); //print all elements in the tree: "1=A,2=D,"
printf("\n");
```
