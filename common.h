/**
 * Copyright 2010-2011 Cenan Özen <cenan.ozen@gmail.com>
 * This file is part of Betik.
 *
 * Betik is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Betik is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Betik. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef common_h
#define common_h

char* duplicate_string(char* str);

typedef struct {
	int      top;
	void*    buffer;
	unsigned item_length;
} stack_t;

stack_t* create_stack(unsigned item_length);
void destroy_stack(stack_t* s);
void stack_push(stack_t* s, void* item);
const void* stack_pop(stack_t* s);
int stack_get_count(stack_t* s);

typedef struct _listitem_t {
	void* data;
	struct _listitem_t* prev;
	struct _listitem_t* next;
} listitem_t;

typedef struct {
	int item_count;
	listitem_t* head;
} list_t;

list_t* create_list();
void destroy_list(list_t* list);
void list_insert(list_t* list, void* data);
void list_remove_by_index(list_t* list, int item_index);
void list_remove_by_data(list_t* list, void* data);
void* list_get_item(list_t* list, int item_index);
void list_set_item(list_t* list, int item_index, void* data);
int list_get_item_count(list_t* list);


#endif // common_h

