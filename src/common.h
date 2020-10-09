#ifndef common_h
#define common_h

char *duplicate_string(char *str);

typedef struct
{
    int top;
    void *buffer;
    unsigned item_length;
} btk_stack_t;

btk_stack_t *create_stack(unsigned item_length);
void destroy_stack(btk_stack_t *s);
void stack_push(btk_stack_t *s, void *item);
const void *stack_pop(btk_stack_t *s);
int stack_get_count(btk_stack_t *s);

typedef struct _listitem_t
{
    void *data;
    struct _listitem_t *prev;
    struct _listitem_t *next;
} listitem_t;

typedef struct
{
    int item_count;
    listitem_t *head;
} list_t;

list_t *create_list();
void destroy_list(list_t *list);
void list_insert(list_t *list, void *data);
void list_remove_by_index(list_t *list, int item_index);
void list_remove_by_data(list_t *list, void *data);
void *list_get_item(list_t *list, int item_index);
void list_set_item(list_t *list, int item_index, void *data);
int list_get_item_count(list_t *list);

#define LAST_LIST_ITEM(lst) list_gem_item(lst, list_get_item_count(lst))

#endif
