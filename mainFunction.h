//
// Created by mathi on 16/01/2018.
//

#ifndef PROJET_C_MAINFUNCTION_H
#define PROJET_C_MAINFUNCTION_H

#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <libpq-fe.h>
#include "structure.h"
#include "callback.h"

GError * loadGladeFile(GtkBuilder ** builder, char *fileName);

GtkWidget* findChild(GtkWidget * parent, const gchar * name);

CallbackParam * initAddNotebookTabButton(GtkBuilder * builder, char * parentName, char * objectName, char * objectLabel, char * fileName, void * function, GList ** list, MainParam * MainParam);

void initLeagueTreeView(GtkWidget * parentBox, CallbackParam * data);

void initTeamTreeView(GtkWidget * parentBox, CallbackParam * data);

void initPlayerTreeView(GtkWidget * parentBox, CallbackParam * data);

void initMatchTreeView(GtkWidget * parentBox, CallbackParam * data);
#endif //PROJET_C_MAINFUNCTION_H
