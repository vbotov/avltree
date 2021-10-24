/* (c) V.Botov */
#include <stdlib.h>
#include "avltree.h"

static int avl_get_h(const t_avl_node *n)
{
   return n == NULL ? 0 : n->h;
}

static void avl_fix_h(t_avl_node *n)
{
   int hl = avl_get_h(n->l);
   int hr = avl_get_h(n->r);
   n->h = (hl > hr ? hl : hr) + 1;
}

static int avl_get_b(const t_avl_node *n)
{
   return avl_get_h(n->r) - avl_get_h(n->l);
}

static t_avl_node *avl_rot_r(t_avl_node *r)
{
   t_avl_node *l = r->l;
   r->l = l->r;
   if (l->r != NULL)
      l->r->p = r;
   l->r = r;
   l->p = r->p;
   r->p = l;
   avl_fix_h(r);
   avl_fix_h(l);
   return l;
}


static t_avl_node *avl_rot_l(t_avl_node *l){
   t_avl_node *r = l->r;
   l->r = r->l;
   if (r->l != NULL)
      r->l->p = l;
   r->l = l;
   r->p = l->p;
   l->p = r;
   avl_fix_h(l);
   avl_fix_h(r);
   return r;
}


static t_avl_node *avl_fix(t_avl_node *node)
{
   avl_fix_h(node);
   int bal = avl_get_b(node);
   if (bal > 1)
   {
      if (avl_get_b(node->r) < 0)
         node->r = avl_rot_r(node->r);
      return avl_rot_l(node);
   }
   else if (bal < -1)
   {
      if (avl_get_b(node->l) > 0)
         node->l = avl_rot_l(node->l);
      return avl_rot_r(node);
   }
   return node;
}


static t_avl_node *avl_node_ins(t_avl_tree *t, t_avl_node *h, void *d)
{
   if (h == NULL)
   {
      t_avl_node *x = t->alloc(sizeof(t_avl_node));
      x->l = x->r = x->p = NULL;
      x->d = (void *)d;
      return x;
   }
   int res = t->cmp(d, h->d);
   if (res < 0)
   {
      h->l = avl_node_ins(t, h->l, d);
      h->l->p = h;
   }
   else if (res > 0)
   {
      h->r = avl_node_ins(t, h->r, d);
      h->r->p = h;
   }
   else
      h->d = d;

   return avl_fix(h);
}


void avl_ins(t_avl_tree *tree, void *data)
{
   tree->root = avl_node_ins(tree, tree->root, data);
}


static t_avl_node *avl_node_find(t_avl_tree *t, t_avl_node *h, const void *d)
{
   while (h != NULL)
   {
      int res = t->cmp(d, h->d);
      if (res == 0)
         break;
      h = res > 0 ? h->r : h->l;
   }
   return h;
}

void *avl_find(t_avl_tree *tree, const void *data)
{
   t_avl_node *node = avl_node_find(tree, tree->root, data);
   return node == NULL ? NULL : node->d;
}

t_avl_node *avl_first(t_avl_node *h)
{
   while (h->l != NULL)
      h = h->l;
   return h;
}

t_avl_node *avl_next(t_avl_node *h)
{
   if (h == NULL)
      return NULL;

   if (h->r != NULL)
   {
      h = h->r;
      while (h->l != NULL)
         h = h->l;
      return h;
   }

   while (h->p != NULL && h == h->p->r)
      h = h->p;
   return h->p;
}

static t_avl_node *avl_first_del(t_avl_node *h)
{
   if (h->l == NULL)
      return h->r;
   h->l = avl_first_del(h->l);
   if (h->l != NULL)
      h->l->p = h;
   return avl_fix(h);
}

static t_avl_node *avl_node_del(t_avl_tree *t, t_avl_node *head, const void *data)
{
   if (head == NULL)
      return NULL;
   int res = t->cmp(data, head->d);
   if (res < 0)
   {
      head->l = avl_node_del(t, head->l, data);
      if (head->l != NULL)
         head->l->p = head;
   }
   else if (res > 0)
   {
      head->r = avl_node_del(t, head->r, data);
      if (head->r != NULL)
         head->r->p = head;
   }
   else
   {
      t_avl_node *l = head->l;
      t_avl_node *r = head->r;
      free(head);

      if (r == NULL)
         return l;
      t_avl_node *f = avl_first(r);
      f->r = avl_first_del(r);
      if (f->r != NULL)
         f->r->p = f;
      f->l = l;
      if (f->l != NULL)
         f->l->p = f;
      return avl_fix(f);
   }
   return avl_fix(head);
}

void avl_del(t_avl_tree *tree, const void *data)
{
   if ((tree->root = avl_node_del(tree, tree->root, data)) != NULL)
      tree->root->p = NULL;
}

t_avl_tree avl_tree_new(int (* cmp)(const void *, const void *))
{
   t_avl_tree tree = {
      .root = NULL,
      .cmp = cmp,
      .free = free,
      .alloc = malloc,
   };
   return tree;
}
