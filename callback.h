//
// Created by mathi on 15/01/2018.
//
#include <gtk/gtk.h>
#include "structure.h"
#include "mainFunction.h"

#ifndef PROJET_C_CALLBACK_H
#define PROJET_C_CALLBACK_H

void destroyWindow(GtkWidget * widget, gpointer data);

void openNotebookTab(GtkWidget * widget, gpointer * data);

void closeNotebookTab(GtkWidget * widget, gpointer * data);

void test(GtkWidget * widget, gpointer * data);

#endif //PROJET_C_CALLBACK_H
