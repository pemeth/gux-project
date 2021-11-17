#ifndef DEBUG
#include <gtk/gtk.h>
#endif
#include <stdlib.h>
#include <stdio.h>

#include "bezier.h"

BezierCurveNode* add_node(
    BezierCurveList *list,
    Point start,
    Point c1,
    Point c2,
    Point end
)
{
    if (list == NULL) {
        return NULL;
    }

    BezierCurveNode *new_node = (BezierCurveNode *) malloc(sizeof(BezierCurveNode));
    new_node->start = start;
    new_node->c1 = c1;
    new_node->c2 = c2;
    new_node->end = end;
    new_node->next = NULL;

    if (list->root == NULL) {
        list->root = new_node;
        list->last = new_node;
    } else {
        list->last->next = new_node;
        list->last = new_node;
    }

    return new_node;
}

void delete_list(BezierCurveList *list)
{
    if (list == NULL) {
        return;
    }

    BezierCurveNode *current = list->root;
    BezierCurveNode *next;
    while (current) {
        next = current->next;

        current->next = NULL;
        free(current);

        current = next;
    }

    list->root = NULL;
    list->last = NULL;
}

void init_list(BezierCurveList *list)
{
    list->root = NULL;
    list->last = NULL;
}

#ifdef DEBUG
int main(int argc, char *argv[])
{
    BezierCurveList list;
    list.root = NULL;
    list.last = NULL;

    add_node(&list, (struct Point) {0,0}, (struct Point) {0,0}, (struct Point) {0,0}, (struct Point) {0,0});
    add_node(&list, (struct Point) {1,1}, (struct Point) {1,1}, (struct Point) {1,1}, (struct Point) {1,1});
    add_node(&list, (struct Point) {2,2}, (struct Point) {2,2}, (struct Point) {2,2}, (struct Point) {2,2});
    add_node(&list, (struct Point) {3,3}, (struct Point) {3,3}, (struct Point) {3,3}, (struct Point) {3,3});

    delete_list(&list);

    return 0;
}
#endif