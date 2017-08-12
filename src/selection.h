#ifndef SELECTION_H
#define SELECTION_H

#include "view.h"

struct selection_info {
    BlockIter si;
    long so;
    long eo;
    bool swapped;
};

void init_selection(View *v, struct selection_info *info);
long prepare_selection(View *v);
char *view_get_selection(View *v, long *size);
int get_nr_selected_lines(struct selection_info *info);
int get_nr_selected_chars(struct selection_info *info);

#endif
