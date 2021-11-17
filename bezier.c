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
        // First to be appended
        list->root = new_node;
    } else {
        // Find last and append
        BezierCurveNode *last = list->root;
        while (last->next) {
            last = last->next;
        }
        last->next = new_node;
    }

    list->n_nodes++;

    return new_node;
}

void remove_nth(BezierCurveList *list, size_t n)
{
    if (list == NULL) {
        return;
    }

    BezierCurveNode *previous = NULL;
    BezierCurveNode *current = list->root;
    for (size_t i = 0; i < n && current != NULL; i++) {
        previous = current;
        current = current->next;
    }

    // `n` too large
    if (current == NULL) {
        return;
    }

    if (previous == NULL) {
        // First in list
        list->root = current->next;
    } else {
        // Not first in list
        previous->next = current->next;
    }

    current->next = NULL;
    free(current);
    list->n_nodes--;
}

void remove_last(BezierCurveList *list)
{
    remove_nth(list, list->n_nodes - 1);
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
        list->n_nodes--;

        current = next;
    }

    list->root = NULL;
}

void init_list(BezierCurveList *list)
{
    list->root = NULL;
    list->n_nodes = 0;
}

void print_node(BezierCurveNode *node)
{
    printf("start:\t%f, %f\n", node->start.x,node->start.x);
    printf("c1:\t%f, %f\n", node->c1.x,node->c1.x);
    printf("c2:\t%f, %f\n", node->c2.x,node->c2.x);
    printf("end:\t%f, %f\n", node->end.x,node->end.x);
}

void print_list(BezierCurveList *list)
{
    if (list == NULL) {
        return;
    }

    BezierCurveNode *current = list->root;
    while (current) {
        print_node(current);
        printf("----------------------\n");
        current = current->next;
    }

    printf("Number of nodes: %lu\n", list->n_nodes);
}

#ifdef DEBUG
int main(int argc, char *argv[])
{
    BezierCurveList list;
    init_list(&list);

    add_node(&list, (struct Point) {0,0}, (struct Point) {0,0}, (struct Point) {0,0}, (struct Point) {0,0});
    add_node(&list, (struct Point) {1,1}, (struct Point) {1,1}, (struct Point) {1,1}, (struct Point) {1,1});
    add_node(&list, (struct Point) {2,2}, (struct Point) {2,2}, (struct Point) {2,2}, (struct Point) {2,2});
    add_node(&list, (struct Point) {3,3}, (struct Point) {3,3}, (struct Point) {3,3}, (struct Point) {3,3});

    delete_list(&list);

    return 0;
}
#endif