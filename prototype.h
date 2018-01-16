//
// Created by mathi on 17/01/2018.
//

#ifndef PROJET_C_PROTOTYPE_H
#define PROJET_C_PROTOTYPE_H

#include <gtk/gtk.h>

typedef struct CallbackParam {
    GtkBuilder * builder;
    char * parentName[250];
    char * objectName[250];
    char * objectLabel[250];
    char * fileName[250];
} CallbackParam;

#endif //PROJET_C_PROTOTYPE_H
