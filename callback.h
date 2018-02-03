//
// Created by mathi on 15/01/2018.
//
#include <stdlib.h>
#include <gtk/gtk.h>
#include "structure.h"
#include "mainFunction.h"

#ifndef PROJET_C_CALLBACK_H
#define PROJET_C_CALLBACK_H

void destroyWindow(GtkWidget * widget, gpointer data);


void openNotebookTab(GtkWidget * widget, gpointer * data);

void closeNotebookTab(GtkWidget * widget, gpointer * data);


void tabSearch(GtkWidget * widget, gpointer * data);

void openAddNewLeagueForm(GtkWidget * widget, gpointer * data);

void openAddNewTeamForm(GtkWidget * widget, gpointer * data);


void closeDialogBox(GtkWidget * widget,gpointer * data);


void createNewLeague (GtkWidget * widget, gpointer * data);


void displayLeagueDetail(GtkTreeView  * treeView, GtkTreePath * path, GtkTreeViewColumn * column, gpointer * user_data);

void displayTeamDetail(GtkTreeView  * treeView, GtkTreePath * path, GtkTreeViewColumn * column, gpointer * data);

void displayPlayerDetail(GtkTreeView  * treeView, GtkTreePath * path, GtkTreeViewColumn * column, gpointer * data);

void openAddNewPlayerForm(GtkWidget * widget, gpointer * data);

void createNewTeam (GtkWidget * widget, gpointer * data);

void createNewPlayer (GtkWidget * widget, gpointer * data);

void openMoreDetailLeague(GtkWidget * widget, gpointer * data);


void openCalendar(GtkEntry * widget, GtkEntryIconPosition iconPos, GdkEvent *event, gpointer * user_data);

void daySelect(GtkCalendar * calendar, gpointer * data);

void newLeagueMatch(GtkWidget * widget, gpointer * data);

#endif //PROJET_C_CALLBACK_H
