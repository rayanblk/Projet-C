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

void leagueTabFormSearch(GtkWidget * widget, gpointer * data);

void tabSearch(GtkWidget * widget, gpointer * data);

void openAddNewLeagueForm(GtkWidget * widget, gpointer * data);

void openAddNewTeamForm(GtkWidget * widget, gpointer * data);

void closeDialogBox(GtkWidget * widget,gpointer * data);

void createNewLeague (GtkWidget * widget, gpointer * data);

void displayLeagueDetail(GtkTreeView  * treeView, GtkTreePath * path, GtkTreeViewColumn * column, gpointer * user_data);

void displayTeamDetail(GtkTreeView  * treeView, GtkTreePath * path, GtkTreeViewColumn * column, gpointer * data);

void displayPlayerDetail(GtkTreeView  * treeView, GtkTreePath * path, GtkTreeViewColumn * column, gpointer * data);

#endif //PROJET_C_CALLBACK_H
