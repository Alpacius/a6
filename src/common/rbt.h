// The implementation comes from ngx.

#pragma once

#include    <stddef.h>
#include    <stdint.h>
#include    <common/miscutils.h>

#define     RBT_RED      1
#define     RBT_BLACK    0

struct rbtree;

struct rbtree_node {
    void *key_ref;
    struct rbtree_node *left, *right, *parent;
    struct rbtree *meta;
    uint8_t color;
};

struct rbtree {
    struct rbtree_node *root, *sentinel, sentinel_;
    int (*key_compare)(const void *, const void *);
};

void rbt_insert(struct rbtree *tree, struct rbtree_node *node);
void rbt_init(struct rbtree *tree, int (*key_compare)(const void *, const void *));
void rbt_delete(struct rbtree *tree, struct rbtree_node *node);
void rbt_detach(struct rbtree_node *node);
struct rbtree_node *rbtree_min(struct rbtree *tree);
struct rbtree_node *rbtree_max(struct rbtree *tree);
struct rbtree_node *rbt_find(struct rbtree *tree, const void *key);

#define     map_indexed                         struct rbtree_node mapctl
#define     mhandle_of(o_, op)                  (&((o_) op mapctl))
#define     mapindex_from_imm(o_)               mhandle_of((o_), .)
#define     mapindex_from_ptr(o_)               mhandle_of((o_), ->)
#define     map_indexed_val(m_, t_)             container_of((m_), t_, mapctl)

void rbt_insert(struct rbtree *tree, struct rbtree_node *node);
void rbt_init(struct rbtree *tree, int (*key_compare)(const void *, const void *));
void rbt_delete(struct rbtree *tree, struct rbtree_node *node);
void rbt_detach(struct rbtree_node *node);
struct rbtree_node *rbtree_min(struct rbtree *tree);
struct rbtree_node *rbtree_max(struct rbtree *tree);
struct rbtree_node *rbt_find(struct rbtree *tree, const void *key);
