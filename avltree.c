/* 
 Copyright (c) 2021 Vladimir Botov

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */



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



static void *avl_node2str(const t_avl_tree *t, t_avl_node *n)
{
   return n == NULL ? NULL : (void *)(((char *)n) - t->node_offset);
}



static void *avl_node2key(const t_avl_tree *t, t_avl_node *n)
{
   return n == NULL ? NULL : (void *)(((char *)n) - t->node_offset + t->key_offset);
}



static t_avl_node *avl_str2node(const t_avl_tree *t, void *d)
{
   return d == NULL ? NULL : (t_avl_node *)(((char *)d) + t->node_offset);
}



static t_avl_node *avl_str2key(const t_avl_tree *t, void *d)
{
   return d == NULL ? NULL : (t_avl_node *)(((char *)d) + t->key_offset);
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



static void avl_node_replace(t_avl_node *n, t_avl_node *o)
{
   if (n == o)
      return;
   *n = *o;
   if (n->l != NULL)
      n->l->p = n;
   if (n->r != NULL)
      n->r->p = n;
   if (o->p != NULL)
   {
      if (o->p->l == o)
         o->p->l = n;
      else
         o->p->r = n;
   }
   o->p = o->l = o->r = NULL;
   o->h = 1;
}



static t_avl_node *avl_node_ins(t_avl_tree *t, t_avl_node *h, void *d, t_avl_node **old)
{
   if (h == NULL)
   {
      t_avl_node *x = avl_str2node(t, d);
      x->l = x->r = x->p = NULL;
      x->h = 1;
      *old = NULL;
      return x;
   }
   int res = t->cmp(avl_str2key(t, d), avl_node2key(t, h));
   if (res < 0)
   {
      h->l = avl_node_ins(t, h->l, d, old);
      h->l->p = h;
   }
   else if (res > 0)
   {
      h->r = avl_node_ins(t, h->r, d, old);
      h->r->p = h;
   }
   else
   {
      *old = h;
      h = avl_str2node(t, d);
      avl_node_replace(h, *old);
   }

   return avl_fix(h);
}


/*!
 * Inserts an element to a tree
 *
 * \param tree a pointer to a tree
 * \param data a pointer to a structure
 * \return a pointer to a replaced structure, or NULL when no collision
 */
void *avl_ins(t_avl_tree *tree, void *data)
{
   t_avl_node *old = NULL;
   tree->root = avl_node_ins(tree, tree->root, data, &old);
   return avl_node2str(tree, old);
}



/*!
 * Look up for an element in a tree
 *
 * \param t a pointer to a tree
 * \param d a pointer to a key
 * \return a pointer to a found structure, or NULL when not found
 */
void *avl_find(const t_avl_tree *tree, const void *key)
{
   t_avl_node *h = tree->root;

   while (h != NULL)
   {
      int res = tree->cmp(key, avl_node2key(tree, h));
      if (res == 0)
         break;
      h = res > 0 ? h->r : h->l;
   }
   return avl_node2str(tree, h);
}



t_avl_node *avl_leftmost(t_avl_node *h)
{
   if (h != NULL)
      for (;h->l != NULL; h = h->l)
         ;
   return h;
}



t_avl_node *avl_node_next(t_avl_node *h)
{
   if (h == NULL)
      return NULL;

   if (h->r != NULL)
      return avl_leftmost(h->r);

   while (h->p != NULL && h == h->p->r)
      h = h->p;

   return h->p;
}



/*!
 * Look up for an element in a tree. In case of non-direct matching, returns a first
 * node, which greater than a key
 *
 * \param t a pointer to a tree
 * \param d a pointer to a key
 * \return a pointer to a found structure, or NULL when not found
 */
void *avl_find_right(const t_avl_tree *tree, const void *key)
{
   int res;
   t_avl_node *h = tree->root;
   t_avl_node *p = h;

   while (h != NULL)
   {
      res = tree->cmp(key, avl_node2key(tree, h));
      if (res == 0)
         break;
      p = h;
      h = res > 0 ? h->r : h->l;
   }
   if (h == NULL)
      h = res < 0 ? p : avl_node_next(p);

   return avl_node2str(tree, h);
}



/*!
 * Retrieve a first element in a tree
 *
 * \param tree AVL tree
 * \return a pointer to a first structure, or NULL when not found
 */
void *avl_first(const t_avl_tree *tree)
{
   return avl_node2str(tree, avl_leftmost(tree->root));
}



/*!
 * Retrieve the next element in a tree
 *
 * \param tree AVL tree
 * \param prev previous element
 * \return a pointer to the next structure, or NULL element is the last
 */
void *avl_next(const t_avl_tree *tree, void *prev)
{
   return avl_node2str(tree, avl_node_next(avl_str2node(tree, prev)));
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



static t_avl_node *avl_node_del(t_avl_tree *t, t_avl_node *head, const void *key, t_avl_node **del)
{
   if (head == NULL)
   {
      *del = NULL;
      return NULL;
   }

   int res = t->cmp(key, avl_node2key(t, head));
   if (res < 0)
   {
      head->l = avl_node_del(t, head->l, key, del);
      if (head->l != NULL)
         head->l->p = head;
   }
   else if (res > 0)
   {
      head->r = avl_node_del(t, head->r, key, del);
      if (head->r != NULL)
         head->r->p = head;
   }
   else
   {
      t_avl_node *l = head->l;
      t_avl_node *r = head->r;

      *del = head;

      if (r == NULL)
         return l;
      t_avl_node *f = avl_leftmost(r);
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



/*!
 * Delete an element from a tree
 *
 * \param tree a tree to alter
 * \param data a pointer to key
 * \return a pointer to a deleted structure, or NULL when not found
 */
void *avl_del(t_avl_tree *tree, const void *key)
{
   t_avl_node *del = NULL;
   if ((tree->root = avl_node_del(tree, tree->root, key, &del)) != NULL)
      tree->root->p = NULL;
   return avl_node2str(tree, del);
}



/*!
 * Create an empty tree
 *
 * \param o t_avl_node offset in a parent structure
 * \param c parent structure compare function
 * \return an empty initialized tree
 */
t_avl_tree avl_tree_new(size_t node_offset, size_t key_offset, t_compare c)
{
   t_avl_tree tree = {
      .root = NULL,
      .node_offset = node_offset,
      .key_offset = key_offset,
      .cmp = c,
   };
   return tree;
}



void avl_node_apply(const t_avl_tree *t, const t_avl_node *h, void (* apply)(void *))
{
   if (h == NULL)
      return;
   avl_node_apply(t, h->l, apply);
   t_avl_node *r = h->r; /* to avoid a situation when 'apply' deallocates h */
   apply((void *)avl_node2str(t, (t_avl_node *)h));
   avl_node_apply(t, r, apply);
}


/*!
 * Executes apply function for all tree elements in ascending order
 *
 * \param tree a pointer to a tree
 * \param apply a pointer to a function receiving a pointer to a parent structure
 */
void avl_apply(const t_avl_tree *tree, void (* apply)(void *))
{
   avl_node_apply(tree, tree->root, apply);
}