//
// Created by mathi on 17/01/2018.
//

#ifndef PROJET_C_PROTOTYPE_H
#define PROJET_C_PROTOTYPE_H

#include <gtk/gtk.h>
#include "database.h"

typedef struct MainParam {
    DatabaseConnector * databaseInfo;
} MainParam;

typedef struct CallbackParam {
    GtkBuilder * builder;
    char parentName[250];
    char objectName[250];
    char objectLabel[250];
    char fileName[250];
    void * function;
    MainParam * mainParam;
} CallbackParam;

typedef struct TabSearchParam{
    char gtkEntryId[250];
    gchar * condition;
    int typeCondition;
} TabSearchParam;

typedef struct TabSearch{
    char statement[250];
    char listStoreId[250];
    GtkBuilder * builder;
    int numberOfParam;
    TabSearchParam * allSearchParam;
    MainParam * mainParam;
} TabSearch;




#endif //PROJET_C_PROTOTYPE_H