#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

char *duplicate_string(char *str)
{
    assert(str);
    char *result;
    result = (char *)malloc(strlen(str) + 1);
    strcpy(result, str);
    return result;
}

#define STACK_SIZE 1024

btk_stack_t *create_stack(unsigned item_length)
{
    assert(item_length != 0);

    btk_stack_t *s = (btk_stack_t *)malloc(sizeof(btk_stack_t));

    assert(s);

    s->top = 0;
    s->buffer = malloc(STACK_SIZE * item_length);

    assert(s->buffer != NULL);

    s->item_length = item_length;

    return s;
}

void destroy_stack(btk_stack_t *s)
{
    assert(s);
    assert(s->buffer);

    free(s->buffer);
    memset(s, 0, sizeof(btk_stack_t));
    free(s);
}

void stack_push(btk_stack_t *s, void *item)
{
    assert(s);
    assert(s->buffer);
    assert(item);
    assert(s->top < 1024);

    unsigned char *ptr = (unsigned char *)s->buffer;
    ptr += s->item_length * s->top;
    memcpy(ptr, item, s->item_length);
    s->top++;
}

const void *stack_pop(btk_stack_t *s)
{
    assert(s);
    assert(s->buffer);
    assert(s->top > 0);

    unsigned char *ptr = (unsigned char *)s->buffer;
    s->top--;
    ptr += s->item_length * s->top;
    return ptr;
}

int stack_get_count(btk_stack_t *s)
{
    assert(s);

    return s->top;
}

list_t *create_list(void)
{
    list_t *list;

    list = (list_t *)malloc(sizeof(list_t));

    list->item_count = 0;
    list->head = NULL;

    return list;
}

void destroy_list(list_t *list)
{
    listitem_t *item;
    listitem_t *nitem;

    item = list->head;
    while (item != NULL)
    {
        nitem = item->next;
        free(item);
        list->item_count--;
        item = nitem;
    }
    list->item_count = 0;
    free(list);
}

void list_insert(list_t *list, void *data)
{
    listitem_t *item;
    listitem_t *new_item;

    new_item = (listitem_t *)malloc(sizeof(listitem_t));
    new_item->next = NULL;
    new_item->prev = NULL;
    new_item->data = data;

    if (list->item_count == 0)
    {
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

static void _remove_item(list_t *list, listitem_t *item)
{
    if (item == list->head)
    {
        list->head = item->next;
    }
    if (item->prev != NULL)
    {
        item->prev->next = item->next;
    }
    if (item->next != NULL)
    {
        item->next->prev = item->prev;
    }
    free(item);
    list->item_count--;
}

void list_remove_by_index(list_t *list, int item_index)
{
    int i = 0;
    listitem_t *item;

    item = list->head;
    while (item != NULL)
    {
        if (i == item_index)
        {
            _remove_item(list, item);
            return;
        }
        item = item->next;
        i++;
    }
    fprintf(stderr, "list_remove_by_index, cant find item in the list");
    exit(1);
}

void list_remove_by_data(list_t *list, void *data)
{
    listitem_t *item;

    item = list->head;
    while (item != NULL)
    {
        if (item->data == data)
        {
            _remove_item(list, item);
            return;
        }
        item = item->next;
    }
    fprintf(stderr, "list_remove_by_data, cant find item in the list");
    exit(1);
}

void *list_get_item(list_t *list, int item_index)
{
    int i = 0;
    listitem_t *item;

    item = list->head;
    while (item != NULL)
    {
        if (i == item_index)
        {
            return item->data;
        }
        i++;
        item = item->next;
    }
    fprintf(stderr, "list_get_item, can not find item in the list");
    return NULL;
}

void list_set_item(list_t *list, int item_index, void *data)
{
    int i = 0;
    listitem_t *item;

    item = list->head;
    while (item != NULL)
    {
        if (i == item_index)
        {
            item->data = data;
        }
        i++;
        item = item->next;
    }
}

int list_get_item_count(list_t *list)
{
    return list->item_count;
}
