#ifndef E_AVLTREE_H
#define E_AVLTREE_H

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
   t_compare cmp;
} t_avl_tree;

void *avl_ins(t_avl_tree *tree, void *data);
void *avl_find(t_avl_tree *t, const void *d);
void *avl_first(t_avl_tree *t);
void *avl_next(t_avl_tree *t, void *p);
void *avl_del(t_avl_tree *tree, const void *data);
t_avl_tree avl_tree_new(size_t o, t_compare c);
void avl_apply(const t_avl_tree *t, void (* apply)(void *));
#endif //E_AVLTREE_H
