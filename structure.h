//
// Created by mathi on 17/01/2018.
//

#ifndef PROJET_C_PROTOTYPE_H
#define PROJET_C_PROTOTYPE_H

#include <gtk/gtk.h>
#include <libpq-fe.h>
#include "database.h"

typedef struct MainParam {
    DatabaseConnector * databaseInfo;
} MainParam;

typedef struct CallbackParam {
    GtkBuilder * builder;
    char * parentName[250];
    char * objectName[250];
    char * objectLabel[250];
    char * fileName[250];
    void * function;
    MainParam * mainParam;
} CallbackParam;



#endif //PROJET_C_PROTOTYPE_H
