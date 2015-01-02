/**
 * Copyright 2010-2015 Cenan Ozen <betik@cenanozen.com>
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

char* duplicate_string(char* str)
{
	assert(str);
	char* result;
	result = (char*)malloc(strlen(str) + 1);
	strcpy(result, str);
	return result;
}

#define STACK_SIZE 1024

btk_stack_t* create_stack(unsigned item_length)
{
	assert(item_length != 0);

	btk_stack_t* s = (btk_stack_t*)malloc(sizeof(btk_stack_t));

	assert(s);

	s->top = 0;
	s->buffer = malloc(STACK_SIZE * item_length);

	assert(s->buffer != NULL);

	s->item_length = item_length;

	return s;
}

void destroy_stack(btk_stack_t* s)
{
	assert(s);
	assert(s->buffer);

	free(s->buffer);
	memset(s, 0, sizeof(btk_stack_t));
	free(s);
}

void stack_push(btk_stack_t* s, void* item)
{
	assert(s);
	assert(s->buffer);
	assert(item);
	assert(s->top < 1024);

	unsigned char* ptr = (unsigned char*)s->buffer;
	ptr += s->item_length * s->top;
	memcpy(ptr, item, s->item_length);
	s->top++;
}

const void* stack_pop(btk_stack_t* s)
{
	assert(s);
	assert(s->buffer);
	assert(s->top > 0);

	unsigned char* ptr = (unsigned char*)s->buffer;
	s->top--;
	ptr += s->item_length * s->top;
	return ptr;
}

int stack_get_count(btk_stack_t* s)
{
	assert(s);

	return s->top;
}


list_t* create_list(void)
{
	list_t* list;

	list = (list_t*)malloc(sizeof(list_t));

	list->item_count = 0;
	list->head = NULL;

	return list;
}

void destroy_list(list_t* list)
{
	listitem_t* item;
	listitem_t* nitem;

	item = list->head;
	while (item != NULL) {
		nitem = item->next;
		free(item);
		list->item_count--;
		item = nitem;
	}
	list->item_count = 0;
	free(list);
}

void list_insert(list_t* list, void* data)
{
	listitem_t* item;
	listitem_t* new_item;

	new_item = (listitem_t*)malloc(sizeof(listitem_t));
	new_item->next = NULL;
	new_item->prev = NULL;
	new_item->data = data;

	if (list->item_count == 0) {
		list->head = new_item;
		list->item_count = 1;
		return;
	}
	// find last item
	for (item = list->head; item->next != NULL; item = item->next)
		; // nothing
	item->next = new_item;
	new_item->prev = item;
	list->item_count++;
}

static void _remove_item(list_t* list, listitem_t* item)
{
	if (item == list->head) {
		list->head = item->next;
	}
	if (item->prev != NULL) {
		item->prev->next = item->next;
	}
	if (item->next != NULL) {
		item->next->prev = item->prev;
	}
	free(item);
	list->item_count--;
}

void list_remove_by_index(list_t* list, int item_index)
{
	int i = 0;
	listitem_t* item;

	item = list->head;
	while (item != NULL) {
		if (i == item_index) {
			_remove_item(list, item);
			return;
		}
		item = item->next;
		i++;
	}
	fprintf(stderr, "list_remove_by_index, cant find item in the list");
	exit(1);
}

void list_remove_by_data(list_t* list, void* data)
{
	listitem_t* item;

	item = list->head;
	while (item != NULL) {
		if (item->data == data) {
			_remove_item(list, item);
			return;
		}
		item = item->next;
	}
	fprintf(stderr, "list_remove_by_data, cant find item in the list");
	exit(1);
}

void* list_get_item(list_t* list, int item_index)
{
	int i = 0;
	listitem_t* item;

	item = list->head;
	while (item != NULL) {
		if (i == item_index) {
			return item->data;
		}
		i++;
		item = item->next;
	}
	fprintf(stderr, "list_get_item, can not find item in the list");
	return NULL;
}

void list_set_item(list_t* list, int item_index, void* data)
{
	int i = 0;
	listitem_t* item;

	item = list->head;
	while (item != NULL) {
		if (i == item_index) {
			item->data = data;
		}
		i++;
		item = item->next;
	}
}

int list_get_item_count(list_t* list)
{
	return list->item_count;
}

// unit tests {{{
#ifdef UNITTESTS
#include "seatest.h"

static void btk_stack_test()
{
	int a = 10, b = 20, c = 30;
	btk_stack_t* s = create_stack(sizeof(int));
	stack_push(s, &a);
	stack_push(s, &b);
	stack_push(s, &c);
	assert_int_equal(30, *(int*)stack_pop(s));
	assert_int_equal(20, *(int*)stack_pop(s));
	assert_int_equal(10, *(int*)stack_pop(s));
	destroy_stack(s);
}


list_t* create_list();
void destroy_list(list_t* list);
void list_insert(list_t* list, void* data);
void list_remove_by_index(list_t* list, int item_index);
void list_remove_by_data(list_t* list, void* data);
void* list_get_item(list_t* list, int item_index);
int list_get_item_count(list_t* list);

static void list_test()
{
	list_t* l = create_list();
	list_insert(l, (void*)10);
	list_insert(l, (void*)20);
	list_insert(l, (void*)30);
	list_insert(l, (void*)40);
	assert_int_equal(10, (int)list_get_item(l, 0));
	assert_int_equal(20, (int)list_get_item(l, 1));
	assert_int_equal(30, (int)list_get_item(l, 2));
	assert_int_equal(40, (int)list_get_item(l, 3));
	assert_int_equal(4, list_get_item_count(l));
	list_remove_by_index(l, 1);
	assert_int_equal(3, list_get_item_count(l));
	assert_int_equal(30, (int)list_get_item(l, 1));
	list_remove_by_data(l, (void*)10);
	assert_int_equal(30, (int)list_get_item(l, 0));
	assert_int_equal(2, list_get_item_count(l));

	int i;
	for (i = 0; i < 10000; i++) {
		list_insert(l, (void*)i);
	}
	assert_int_equal(10002, list_get_item_count(l));
	for (i = 0; i < 10000; i++) {
		list_remove_by_data(l, (void*)i);
	}
	assert_int_equal(2, list_get_item_count(l));
	destroy_list(l);
}

void common_lib_test_fixture(void)
{
	test_fixture_start();
	run_test(btk_stack_test);
	run_test(list_test);
	test_fixture_end();
}

#endif // UNITTESTS
// }}}

