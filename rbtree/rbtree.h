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

#ifndef _RBTREE_H_
#define _RBTREE_H_

enum rbtree_node_color { RED, BLACK };

typedef struct rbtree_node_s {
	void  *key;
	void  *value;
	struct rbtree_node_s  *left;
	struct rbtree_node_s  *right;
	struct rbtree_node_s  *parent;
	enum rbtree_node_color  color;
} *rbtree_node;

typedef struct rbtree_s {
	rbtree_node  root;
} *rbtree;

typedef int (*comparator)(void *, void *);

extern rbtree rbtree_create(void);
extern void *rbtree_lookup(rbtree t, void *key, comparator cmp);
extern void rbtree_traverse(rbtree t, void (*func)(void *, void *));
extern void rbtree_insert(rbtree t, void *key, void *value, comparator cmp);
extern void rbtree_delete(rbtree t, void *key, comparator cmp);

#endif  /* _RBTREE_H_ */

