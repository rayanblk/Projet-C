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
    gchar condition[250];
    int typeCondition;
} TabSearchParam;

typedef struct TabSearch{
    char statement[450];
    char listStoreId[250];
    GtkBuilder * builder;
    int numberOfParam;
    char id[20];
    TabSearchParam ** allSearchParam;
    MainParam * mainParam;
} TabSearch;

typedef struct AllTabParam{
    TabSearch * searchParam;
    GtkBuilder * builder;
    MainParam * centralParam;
} AllTabParam;

typedef struct CallBackParamWithId{
    CallbackParam * allCalbackParam;
    char id[10];
} CallBackParamWithId;

typedef struct WindowCalendarParam{
    GtkWidget * destinationWidget;
    GtkWidget * calendarWindow;
    gint day;
    gint month;
    gint year;
} WindowCalendarParam;

typedef struct AllLeagueMatchParam{
    CallBackParamWithId * mainParam;
    WindowCalendarParam ** allCalendarParam;
    GtkBuilder * builder;
}AllLeagueMatchParam;




#endif //PROJET_C_PROTOTYPE_H
