#include    <common/rbt.h>


static struct rbtree_node sentinel_common = { .color = RBT_BLACK, .key_ref = NULL, .left = NULL, .right = NULL, .parent = NULL };

static 
void rbt_bst_insert(struct rbtree *tree, struct rbtree_node *node) {
    struct rbtree_node **target = NULL, *pos = tree->root;
    do {
        if (*(target = (tree->key_compare(node->key_ref, pos->key_ref) <= 0) ? &(pos->left) : &(pos->right)) != tree->sentinel)
            pos = *target;
    } while (*target != tree->sentinel);
    (*target = node), (node->parent = pos), (node->left = node->right = tree->sentinel), (node->color = RBT_RED);
}

static inline
void rbt_left_rotate(struct rbtree_node **root, struct rbtree_node *node, struct rbtree_node *sentinel) {
    struct rbtree_node *tmp = node->right;
    (node->right = tmp->left), ((tmp->left != sentinel) && (tmp->left->parent = node)), (tmp->parent = node->parent);
    if (node == *root)
        *root = tmp;
    else if (node == node->parent->left)
        node->parent->left = tmp;
    else
        node->parent->right = tmp;
    (tmp->left = node), (node->parent = tmp);
}

static inline
void rbt_right_rotate(struct rbtree_node **root, struct rbtree_node *node, struct rbtree_node *sentinel) {
    struct rbtree_node *tmp = node->left;
    (node->left = tmp->right), ((tmp->right != sentinel) && (tmp->right->parent = node)), (tmp->parent = node->parent);
    if (node == *root)
        *root = tmp;
    else if (node == node->parent->right)
        node->parent->right = tmp;
    else
        node->parent->left = tmp;
    (tmp->right = node), (node->parent = tmp);
}

void rbt_insert(struct rbtree *tree, struct rbtree_node *node) {
    struct rbtree_node **root = &(tree->root), *tmp = NULL;
    if (unlikely(tree->root == tree->sentinel)) {
        (tree->root = node), (tree->root->parent = NULL), (tree->root->left = tree->root->right = tree->sentinel), (tree->root->color = RBT_BLACK);
        return;
    }
    rbt_bst_insert(tree, node);
    while ((node != *root) && (node->parent->color == RBT_RED)) {
        if (node->parent == node->parent->parent->left) {
            tmp = node->parent->parent->right;
            if (tmp->color == RBT_RED) {
                (node->parent->color = RBT_BLACK), (tmp->color = RBT_BLACK), (node->parent->parent->color = RBT_RED);
                node = node->parent->parent;
            } else {
                if (node == node->parent->right) {
                    node = node->parent;
                    rbt_left_rotate(root, node, tree->sentinel);
                }
                (node->parent->color = RBT_BLACK), (node->parent->parent->color = RBT_RED);
                rbt_right_rotate(root, node->parent->parent, tree->sentinel);
            }
        } else {
            tmp = node->parent->parent->left;
            if (tmp->color == RBT_RED) {
                (node->parent->color = RBT_BLACK), (tmp->color = RBT_BLACK), (node->parent->parent->color = RBT_RED);
                node = node->parent->parent;
            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    rbt_right_rotate(root, node, tree->sentinel);
                }
                (node->parent->color = RBT_BLACK), (node->parent->parent->color = RBT_RED);
                rbt_left_rotate(root, node->parent->parent, tree->sentinel);
            }
        }
    }
    (*root)->color = RBT_BLACK;
    node->meta = tree;
}


static inline
struct rbtree_node *rbtree_getmin(struct rbtree_node *node, struct rbtree_node *sentinel) {
    while (node->left != sentinel)
        node = node->left;
    return node;
}

static inline
struct rbtree_node *rbtree_getmax(struct rbtree_node *node, struct rbtree_node *sentinel) {
    while (node->right != sentinel)
        node = node->right;
    return node;
}

struct rbtree_node *rbtree_min(struct rbtree *tree) {
    if (tree->root == tree->sentinel) return tree->root;
    return rbtree_getmin(tree->root, tree->sentinel);
}

struct rbtree_node *rbtree_max(struct rbtree *tree) {
    if (tree->root == tree->sentinel) return tree->root;
    return rbtree_getmax(tree->root, tree->sentinel);
}

void rbt_delete(struct rbtree *tree, struct rbtree_node *node) {
    struct rbtree_node **root = &(tree->root), *tmp = NULL, *subst = NULL;
    if (node->left == tree->sentinel)
        (tmp = node->right), (subst = node);
    else if (node->right == tree->sentinel)
        (tmp = node->left), (subst = node);
    else 
        (subst = rbtree_getmin(node->right, tree->sentinel)), (tmp = (subst->left != tree->sentinel) ? subst->left : subst->right);
    if (subst == *root) {
        (*root = tmp), (tmp->color = RBT_BLACK);
        return;
    }
    uint8_t subst_color = subst->color;
    (subst == subst->parent->left) ? (subst->parent->left = tmp) : (subst->parent->right = tmp);
    if (subst == node) {
        tmp->parent = subst->parent;
    } else {
        (subst->parent == node) ? (tmp->parent = subst) : (tmp->parent = subst->parent);
        (subst->left = node->left), (subst->right = node->right), (subst->parent = node->parent), (subst->color = node->color);
        if (node == *root)
            *root = subst;
        else
            (node == node->parent->left) ? (node->parent->left = subst) : (node->parent->right = subst);
        ((subst->left != tree->sentinel) && (subst->left->parent = subst)), ((subst->right != tree->sentinel) && (subst->right->parent = subst));
    }
    if (subst_color == RBT_RED) return;
    node->left = node->right = NULL;
    struct rbtree_node *fix_target = NULL;
    while ((tmp != *root) && (tmp->color == RBT_BLACK)) {
        if (tmp == tmp->parent->left) {
            fix_target = tmp->parent->right;
            if (fix_target->color == RBT_RED) {
                (fix_target->color = RBT_BLACK), (tmp->parent->color = RBT_RED);
                rbt_left_rotate(root, tmp->parent, tree->sentinel);
                fix_target = tmp->parent->right;
            }
            if ((fix_target->left->color == RBT_BLACK) && (fix_target->right->color == RBT_BLACK)) {
                (fix_target->color = RBT_RED), (tmp = tmp->parent);
            } else {
                if (fix_target->right->color == RBT_BLACK) {
                    (fix_target->left->color = RBT_BLACK), (fix_target->color = RBT_RED);
                    rbt_right_rotate(root, fix_target, tree->sentinel);
                    fix_target = tmp->parent->right;
                }
                (fix_target->color = tmp->parent->color), (tmp->parent->color = RBT_BLACK), (fix_target->right->color = RBT_BLACK);
                rbt_left_rotate(root, tmp->parent, tree->sentinel);
                tmp = *root;
            }
        } else {
            fix_target = tmp->parent->left;
            if (fix_target->color == RBT_RED) {
                (fix_target->color = RBT_BLACK), (tmp->parent->color = RBT_RED);
                rbt_right_rotate(root, tmp->parent, tree->sentinel);
                fix_target = tmp->parent->left;
            }
            if ((fix_target->left->color == RBT_BLACK) && (fix_target->right->color == RBT_BLACK)) {
                (fix_target->color = RBT_RED), (tmp = tmp->parent);
            } else {
                if (fix_target->left->color == RBT_BLACK) {
                    (fix_target->right->color = RBT_BLACK), (fix_target->color = RBT_RED);
                    rbt_left_rotate(root, fix_target, tree->sentinel);
                    fix_target = tmp->parent->left;
                }
                (fix_target->color = tmp->parent->color), (tmp->parent->color = RBT_BLACK), (fix_target->left->color = RBT_BLACK);
                rbt_right_rotate(root, tmp->parent, tree->sentinel);
                tmp = *root;
            }
        }
    }
    tmp->color = RBT_BLACK;
}

void rbt_detach(struct rbtree_node *node) {
    if (node->meta)
        rbt_delete(node->meta, node);
}

struct rbtree_node *rbt_find(struct rbtree *tree, const void *key) {
    struct rbtree_node *iter = tree->root;
    while (iter != tree->sentinel) {
        switch (tree->key_compare(key, iter->key_ref)) {
            case 0:
                return iter;
            case -1:
                iter = iter->left;
                break;
            case 1:
                iter = iter->right;
                break;
        }
    }
    return NULL;
}

void rbt_init(struct rbtree *tree, int (*key_compare)(const void *, const void *)) {
    tree->sentinel_.color = RBT_BLACK;
    (tree->sentinel = tree->root = &(tree->sentinel_)), (tree->key_compare = key_compare);
}

