/*
 * Two-way linked list.
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

#include "list.h"

static list_node new_node(void *, list_node, list_node);


list list_create(void)
{
	list  l;

	l = malloc(sizeof(struct list_s));
	if (l == NULL) {
		return NULL;
	}

	l->head = l->tail = NULL;

	return l;
}

list_node new_node(void *value, list_node next, list_node prev)
{
	list_node  n;

	n = malloc(sizeof(struct list_node_s));
	if (n == NULL) {
		return NULL;
	}

	n->value = value;
	n->next = next;
	n->prev = prev;

	return n;
}

int list_insert(list l, void *value)
{
	list_node  n;

	n = new_node(value, l->head, NULL);
	if (n == NULL) {
		return -1;
	}

	if (l->tail == NULL) {
		l->tail = n;
	}
	if (l->head != NULL) {
		l->head->prev = n;
	}
	l->head = n;

	return 0;
}

int list_append(list l, void *value)
{
	list_node  n;

	n = new_node(value, NULL, l->tail);
	if (n == NULL) {
		return -1;
	}

	if (l->head == NULL) {
		l->head = n;
	}
	if (l->tail != NULL) {
		l->tail->next = n;
	}
	l->tail = n;

	return 0;
}

inline list_node list_first_node(list l)
{
	return l->head;
}

inline list_node list_next_node(list_node n)
{
	return n->next;
}

inline void *list_node_value(list_node n)
{
	return n->value;
}

void list_remove(list l, list_node n)
{
	if (n->prev != NULL) {
		n->prev->next = n->next;
	} else {
		l->head = n->next;
	}
	if (n->next != NULL) {
		n->next->prev = n->prev;
	} else {
		l->tail = n->prev;
	}

	free(n);
}

void *list_pop(list l)
{
	void  *result = l->tail->value;

	list_remove(l, l->tail);

	return result;
}

void list_cleanup(list l)
{
	list_node  n, tmp;

	for (n = list_first_node(l); n != NULL; ) {
		tmp = list_next_node(n);
		free(n);
		n = tmp;
	}

	free(l);
}

