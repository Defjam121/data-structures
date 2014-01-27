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

#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct queue_s {
	void  **start;
	void  **end;
	void  **first;
	void  **last;
	short  is_empty;
} *queue;

extern queue queue_create(size_t size);
extern void queue_free(queue q);
extern short queue_is_empty(queue q);
extern short queue_is_full(queue q);
extern int queue_enqueue(queue q, void *value);
extern void *queue_dequeue(queue q);

#endif  /* _QUEUE_H_ */

