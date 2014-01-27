/*
 * Queue.
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

#include "queue.h"


queue queue_create(size_t size)
{
	queue  q;

	q = malloc(sizeof(struct queue_s));
	if (q == NULL) {
		return NULL;
	}

	q->start = malloc(size * sizeof(void *));
	if (q->start == NULL) {
		free(q);

		return NULL;
	}

	q->end = q->start + size;
	q->first = q->last = q->start;
	q->is_empty = 1;

	return q;
}

void queue_free(queue q)
{
	assert(q != NULL);
	assert(q->start != NULL);

	free(q->start);
	free(q);
}

inline short queue_is_empty(queue q)
{
	return q->is_empty;
}

inline short queue_is_full(queue q)
{
	return q->last == q->first && !q->is_empty;
}

int queue_enqueue(queue q, void *value)
{
	if (queue_is_full(q)) {
		return -1;
	}

	*q->last++ = value;

	if (q->last == q->end) {
		q->last = q->start;
	}

	q->is_empty = 0;

	return 0;
}

void *queue_dequeue(queue q)
{
	void  *value;

	assert(!q->is_empty);

	value = *q->first++;

	if (q->first == q->end) {
		q->first = q->start;
	}

	if (q->first == q->last) {
		q->is_empty = 1;
	}

	return value;
}

