/*
 * Red-black tree.
 *
 * Copyright (C) 2014  Andrew A. Usenok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <assert.h>

#include "rbtree.h"


typedef rbtree_node  node;
typedef enum rbtree_node_color  color;

static node grandparent(node n);
static node sibling(node n);
static node uncle(node n);
static color node_color(node n);
static node new_node(void *key, void *value, color node_color, node left, node right);
static node lookup_node(rbtree t, void *key, comparator cmp);
static void traverse(node n, void (*func)(void *, void *));
static void replace_node(rbtree t, node oldn, node newn);
static void rotate_left(rbtree t, node n);
static void rotate_right(rbtree t, node n);
static void augment_insert(rbtree t, node n);
static node maximum_node(node root);
static void augment_delete(rbtree t, node n);
static void verify_properties(rbtree t);
static void verify_property_1(node root);
static void verify_property_2(node root);
static void verify_property_4(node root);
static void verify_property_5(node root);
static void verify_property_5_helper(node n, int black_count, int *black_count_path);


inline node grandparent(node n)
{
	assert(n != NULL);
	assert(n->parent != NULL);  // not the root node
	assert(n->parent->parent != NULL);  // not child of root

	return n->parent->parent;
}

inline node sibling(node n)
{
	assert(n != NULL);
	assert(n->parent != NULL);  // root node has no sibling

	if (n == n->parent->left) {
		return n->parent->right;
	} else {
		return n->parent->left;
	}
}

inline node uncle(node n)
{
	assert(n != NULL);
	assert(n->parent != NULL);  // root node has no uncle
	assert(n->parent->parent != NULL);  // children of root have no uncle

	return sibling(n->parent);
}

inline color node_color(node n)
{
	return n == NULL ? BLACK : n->color;
}

rbtree rbtree_create()
{
	rbtree  t;
	
	t = malloc(sizeof(struct rbtree_s));
	t->root = NULL;

	verify_properties(t);

	return t;
}

node new_node(void *key, void *value, color node_color, node left, node right)
{
	node  new;
	
	new = malloc(sizeof(struct rbtree_node_s));
	new->key = key;
	new->value = value;
	new->color = node_color;
	new->left = left;
	new->right = right;

	if (left != NULL) {
		left->parent = new;
	}
	if (right != NULL) {
		right->parent = new;
	}
	new->parent = NULL;

	return new;
}

node lookup_node(rbtree t, void *key, comparator cmp)
{
	node  n = t->root;
	int  res;

	while (n != NULL) {
		res = cmp(key, n->key);
		if (res == 0) {
			return n;
		} else if (res < 0) {
			n = n->left;
		} else {
			n = n->right;
		}
	}

	return n;
}

void *rbtree_lookup(rbtree t, void *key, comparator cmp)
{
	node  n;
	
	n = lookup_node(t, key, cmp);

	return n == NULL ? NULL : n->value;
}

void traverse(node n, void (*func)(void *, void *))
{
	if (n == NULL) {
		return;
	} else {
		traverse(n->left, func);
		func(n->key, n->value);
		traverse(n->right, func);
	}
}

void rbtree_traverse(rbtree t, void (*func)(void *, void *))
{
	traverse(t->root, func);
}

void replace_node(rbtree t, node old, node new)
{
	if (old->parent == NULL) {  // old node is root
		t->root = new;
	} else {
		if (old == old->parent->left) {
			old->parent->left = new;
		} else {
			old->parent->right = new;
		}
	}

	if (new != NULL) {
		new->parent = old->parent;
	}
}

void rotate_left(rbtree t, node n)
{
	node  r = n->right;

	replace_node(t, n, r);

	n->right = r->left;
	if (r->left != NULL) {
		r->left->parent = n;
	}
	r->left = n;
	n->parent = r;
}

void rotate_right(rbtree t, node n)
{
	node  l = n->left;

	replace_node(t, n, l);

	n->left = l->right;
	if (l->right != NULL) {
		l->right->parent = n;
	}
	l->right = n;
	n->parent = l;
}

void rbtree_insert(rbtree t, void *key, void *value, comparator cmp)
{
	node  new, n;
	int  res;
	
	new = new_node(key, value, RED, NULL, NULL);

	if (t->root == NULL) {
		t->root = new;
	} else {
		n = t->root;
		while (1) {
			res = cmp(key, n->key);
			if (res == 0) {
				n->value = value;

				free(new);

				return;
			} else if (res < 0) {
				if (n->left == NULL) {
					n->left = new;

					break;
				} else {
					n = n->left;
				}
			} else {
				if (n->right == NULL) {
					n->right = new;

					break;
				} else {
					n = n->right;
				}
			}
		}

		new->parent = n;
	}

	augment_insert(t, new);

	verify_properties(t);
}

void augment_insert(rbtree t, node n)
{
	// Case 1.
	if (n->parent == NULL) {
		n->color = BLACK;

		return;
	}

	// Case 2.
	if (node_color(n->parent) == BLACK) {
		return;
	}

	// Case 3.
	if (node_color(uncle(n)) == RED) {
		n->parent->color = BLACK;
		uncle(n)->color = BLACK;
		grandparent(n)->color = RED;

		augment_insert(t, grandparent(n));

		return;
	}

	// Case 4.
	if (n == n->parent->right && n->parent == grandparent(n)->left) {
		rotate_left(t, n->parent);
		n = n->left;
	} else if (n == n->parent->left && n->parent == grandparent(n)->right) {
		rotate_right(t, n->parent);
		n = n->right;
	}

	// Case 5.
	n->parent->color = BLACK;
	grandparent(n)->color = RED;

	if (n == n->parent->left && n->parent == grandparent(n)->left) {
		rotate_right(t, grandparent(n));
	} else {
		assert(n == n->parent->right && n->parent == grandparent(n)->right);

		rotate_left(t, grandparent(n));
	}
}

node maximum_node(node n)
{
	assert(n != NULL);

	while (n->right != NULL) {
		n = n->right;
	}

	return n;
}

void rbtree_delete(rbtree t, void *key, comparator cmp)
{
	node  n, child;

	n = lookup_node(t, key, cmp);
	if (n == NULL) {
		return;  // key not found, do nothing
	}

	if (n->left != NULL && n->right != NULL) {
		node  pred;

		// Copy key/value from predecessor and then delete it instead.
		pred = maximum_node(n->left);
		n->key = pred->key;
		n->value = pred->value;
		n = pred;
	}

	assert(n->left == NULL || n->right == NULL);

	child = n->right == NULL ? n->left : n->right;
	if (node_color(n) == BLACK) {
		n->color = node_color(child);

		augment_delete(t, n);
	}

	replace_node(t, n, child);

	if (n->parent == NULL && child != NULL) {
		child->color = BLACK;
	}

	free(n);

	verify_properties(t);
}

void augment_delete(rbtree t, node n)
{
	// Case 1.
	if (n->parent == NULL) {
		return;
	}
	
	// Case 2.
	if (node_color(sibling(n)) == RED) {
		n->parent->color = RED;
		sibling(n)->color = BLACK;

		if (n == n->parent->left) {
			rotate_left(t, n->parent);
		} else {
			rotate_right(t, n->parent);
		}
	}

	// Case 3.
	if (node_color(n->parent) == BLACK &&
	    node_color(sibling(n)) == BLACK &&
	    node_color(sibling(n)->left) == BLACK &&
	    node_color(sibling(n)->right) == BLACK) {
		sibling(n)->color = RED;

		augment_delete(t, n->parent);

		return;
	}
	
	// Case 4.
	if (node_color(n->parent) == RED &&
	           node_color(sibling(n)) == BLACK &&
	           node_color(sibling(n)->left) == BLACK &&
	           node_color(sibling(n)->right) == BLACK) {
		sibling(n)->color = RED;
		n->parent->color = BLACK;

		return;
	}

	// Case 5.
	if (n == n->parent->left &&
	           node_color(sibling(n)) == BLACK &&
	           node_color(sibling(n)->left) == RED &&
	           node_color(sibling(n)->right) == BLACK) {
		sibling(n)->color = RED;
		sibling(n)->left->color = BLACK;

		rotate_right(t, sibling(n));
	} else if (n == n->parent->right &&
	           node_color(sibling(n)) == BLACK &&
	           node_color(sibling(n)->right) == RED &&
	           node_color(sibling(n)->left) == BLACK) {
		sibling(n)->color = RED;
		sibling(n)->right->color = BLACK;

		rotate_left(t, sibling(n));
	}

	// Case 6.
	sibling(n)->color = node_color(n->parent);
	n->parent->color = BLACK;

	if (n == n->parent->left) {
		assert(node_color(sibling(n)->right) == RED);

		sibling(n)->right->color = BLACK;
		rotate_left(t, n->parent);
	} else {
		assert (node_color(sibling(n)->left) == RED);

		sibling(n)->left->color = BLACK;
		rotate_right(t, n->parent);
	}
}

void verify_properties(rbtree t)
{
#ifdef VERIFY_RBTREE
	verify_property_1(t->root);
	verify_property_2(t->root);
	/* Property 3 is implicit */
	verify_property_4(t->root);
	verify_property_5(t->root);
#endif
}

void verify_property_1(node n)
{
	assert(node_color(n) == RED || node_color(n) == BLACK);

	if (n == NULL) {
		return;
	}

	verify_property_1(n->left);
	verify_property_1(n->right);
}

void verify_property_2(node root)
{
	assert(node_color(root) == BLACK);
}

void verify_property_4(node n)
{
	if (node_color(n) == RED) {
		assert(node_color(n->left) == BLACK);
		assert(node_color(n->right) == BLACK);
		assert(node_color(n->parent) == BLACK);
	}

	if (n == NULL) {
		return;
	}

	verify_property_4(n->left);
	verify_property_4(n->right);
}

void verify_property_5(node root)
{
	int  black_count_path = -1;

	verify_property_5_helper(root, 0, &black_count_path);
}

void verify_property_5_helper(node n, int black_count, int *path_black_count)
{
	if (node_color(n) == BLACK) {
		black_count++;
	}

	if (n == NULL) {
		if (*path_black_count == -1) {
			*path_black_count = black_count;
		} else {
			assert(black_count == *path_black_count);
		}

		return;
	}

	verify_property_5_helper(n->left, black_count, path_black_count);
	verify_property_5_helper(n->right, black_count, path_black_count);
}

