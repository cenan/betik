#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runtime.h"

scope_t *create_scope(runtime_t *rt)
{
    scope_t *scope = (scope_t *)malloc(sizeof(scope_t));
    scope->variables = create_list();
    scope->reference_count = 1;
    return scope;
}

void destroy_scope(scope_t *s)
{
    destroy_list(s->variables);
    free(s);
}

variable_t *get_variable(runtime_t *rt, char *variable_name)
{
    int i;
    for (i = 0; i < list_get_item_count(rt->current_scope->variables); i++)
    {
        variable_t *var = list_get_item(rt->current_scope->variables, i);
        if (strcmp(variable_name, var->name) == 0)
        {
            return var;
        }
    }
    for (i = 0; i < list_get_item_count(rt->global_scope->variables); i++)
    {
        variable_t *var = list_get_item(rt->global_scope->variables, i);
        if (strcmp(variable_name, var->name) == 0)
        {
            return var;
        }
    }
    return 0;
}

variable_t *create_variable(runtime_t *rt, char *variable_name)
{
    variable_t *var = (variable_t *)malloc(sizeof(variable_t));
    var->name = variable_name;
    var->obj = 0;
    list_insert(rt->current_scope->variables, var);

    return var;
}

object_t *create_object(runtime_t *rt, object_type_t obj_type)
{
    object_t *obj = (object_t *)malloc(sizeof(object_t));

    obj->type = obj_type;
    obj->reference_count = 0;
    obj->data = 0;
    obj->scope = 0;
    obj->properties = create_list();

    return obj;
}

variable_t *get_property(runtime_t *rt, variable_t *var, char *property_name)
{
    for (int i = 0; i < list_get_item_count(var->obj->properties); i++)
    {
        variable_t *p = (variable_t *)list_get_item(var->obj->properties, i);
        if (strcmp(p->name, property_name) == 0)
        {
            return p;
        }
    }
    variable_t *new_prop = (variable_t *)malloc(sizeof(variable_t));
    new_prop->name = duplicate_string(property_name);
    new_prop->obj = 0;
    list_insert(var->obj->properties, new_prop);
    return new_prop;
}

void set_property(runtime_t *rt, object_t *base, char *key, variable_t *value)
{
    variable_t *new_prop = (variable_t *)malloc(sizeof(variable_t));
    new_prop->name = duplicate_string(key);
    new_prop->obj = value->obj;
    list_insert(base->properties, new_prop);
}

variable_t *call_variable_op(runtime_t *rt, variable_t *var1, variable_t *var2, token_type_t tok)
{
    variable_t *var = create_variable(rt, "#");
    var->obj = create_object(rt, OBJ_NUMBER);
    var->obj->reference_count += 1;
    if (TT_OP_ADD == tok)
    {
        if (var1->obj->type == OBJ_NUMBER)
        {
            var->obj->data = (void *)((int)var1->obj->data + (int)var2->obj->data);
        }
        else if (var1->obj->type == OBJ_STRING)
        {
            var->obj->type = OBJ_STRING;
            if (var2->obj->type == OBJ_STRING)
            {
                char *tmp = (char *)malloc(strlen(var1->obj->data) + strlen(var2->obj->data) + 1);
                strcpy(tmp, var1->obj->data);
                strcat(tmp, var2->obj->data);
                var->obj->data = tmp;
            }
            else if (var2->obj->type == OBJ_NUMBER)
            {
                char *tmp = (char *)malloc(strlen(var1->obj->data) + 16);
                strcpy(tmp, var1->obj->data);
                char tmp2[16];
                sprintf(tmp2, "%d", (int)var2->obj->data);
                strcat(tmp, tmp2);
                var->obj->data = tmp;
            }
        }
    }
    else if (TT_OP_SUB == tok)
    {
        var->obj->data = (void *)((int)var1->obj->data - (int)var2->obj->data);
    }
    else if (TT_OP_MUL == tok)
    {
        var->obj->data = (void *)((int)var1->obj->data * (int)var2->obj->data);
    }
    else if (TT_OP_DIV == tok)
    {
        if (0 == (int)var2->obj->data)
        {
            fprintf(stderr, "Division by zero\n");
            exit(EXIT_FAILURE);
        }
        var->obj->data = (void *)((int)var1->obj->data / (int)var2->obj->data);
    }
    else if (TT_OP_GT == tok)
    {
        var->obj->data = (void *)((int)var1->obj->data > (int)var2->obj->data);
    }
    else if (TT_OP_GTE == tok)
    {
        var->obj->data = (void *)((int)var1->obj->data >= (int)var2->obj->data);
    }
    else if (TT_OP_LT == tok)
    {
        var->obj->data = (void *)((int)var1->obj->data < (int)var2->obj->data);
    }
    else if (TT_OP_LTE == tok)
    {
        var->obj->data = (void *)((int)var1->obj->data <= (int)var2->obj->data);
    }
    else if (TT_OP_EQUAL == tok)
    {
        if (var1->obj->type == OBJ_NUMBER)
        {
            var->obj->data = (void *)((int)var1->obj->data == (int)var2->obj->data);
        }
        else
        {
            var->obj->data = (void *)(strcmp((char *)var1->obj->data, (char *)var2->obj->data) == 0);
        }
    }
    else if (TT_OP_NOTEQUAL == tok)
    {
        if (var1->obj->type == OBJ_NUMBER)
        {
            var->obj->data = (void *)((int)var1->obj->data != (int)var2->obj->data);
        }
        else
        {
            var->obj->data = (void *)(strcmp((char *)var1->obj->data, (char *)var2->obj->data) != 0);
        }
    }
    else if (TT_OP_ASSIGN == tok)
    {
        var1->obj = var2->obj;
        var2->obj->reference_count += 1;
    }
    return var;
}
