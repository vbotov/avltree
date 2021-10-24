#ifndef E_AVLTREE_H
#define E_AVLTREE_H

typedef struct t_avl_node {
   struct t_avl_node *l;
   struct t_avl_node *r;
   struct t_avl_node *p;
   int h;
   void *d;
} t_avl_node;

typedef struct {
   t_avl_node *root;
   int (* cmp)(const void *, const void *);
   void (* free)(void *);
   void *(* alloc)(size_t);
} t_avl_tree;

void avl_ins(t_avl_tree *tree, void *data);
void *avl_find(t_avl_tree *tree, const void *data);
t_avl_node *avl_first(t_avl_node *h);
t_avl_node *avl_next(t_avl_node *h);
void avl_del(t_avl_tree *tree, const void *data);
t_avl_tree avl_tree_new(int (* cmp)(const void *, const void *));

#endif //E_AVLTREE_H
