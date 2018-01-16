//
// Created by mathi on 16/01/2018.
//

#ifndef PROJET_C_MAINFUNCTION_H
#define PROJET_C_MAINFUNCTION_H

#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include "prototype.h"
#include "callback.h"

GError * loadGladeFile(GtkBuilder ** builder, char *fileName);

GtkWidget* findChild(GtkWidget * parent, const gchar * name);

CallbackParam * initAddNotebookTabButton(GtkBuilder * builder, char * parentName, char * objectName, char * objectLabel, char * fileName, GList ** list);

#endif //PROJET_C_MAINFUNCTION_H
