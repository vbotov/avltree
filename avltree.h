#ifndef E_AVLTREE_H
#define E_AVLTREE_H

#include <stddef.h>

typedef int (* t_compare)(const void *, const void *);

typedef struct t_avl_node {
   struct t_avl_node *l;
   struct t_avl_node *r;
   struct t_avl_node *p;
   int h;
} t_avl_node;

typedef struct {
   t_avl_node *root;
   size_t node_offset;
   size_t key_offset;
   t_compare cmp;
} t_avl_tree;

void *avl_ins(t_avl_tree *tree, void *data);
void *avl_find(const t_avl_tree *tree, const void *key);
void *avl_find_right(const t_avl_tree *tree, const void *key);
void *avl_first(const t_avl_tree *t);
void *avl_next(const t_avl_tree *tree, void *prev);
void *avl_del(t_avl_tree *tree, const void *key);
t_avl_tree avl_tree_new(size_t node_offset, size_t key_offset, t_compare c);
void avl_apply(const t_avl_tree *tree, void (* apply)(void *));

#endif //E_AVLTREE_H
