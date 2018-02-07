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
#include <curl/curl.h>

GError * loadGladeFile(GtkBuilder ** builder, char *fileName);

GtkWidget* findChild(GtkWidget * parent, const gchar * name);

CallbackParam * initAddNotebookTabButton(GtkBuilder * builder, char * parentName, char * objectName, char * objectLabel, char * fileName, void * function, GList ** list, MainParam * MainParam);

void initLeagueTreeView(GtkWidget * parentBox, CallbackParam * data);

void initTeamTreeView(GtkWidget * parentBox, CallbackParam * data);

void initPlayerTreeView(GtkWidget * parentBox, CallbackParam * data);

void initMatchTreeView(GtkWidget * parentBox, CallbackParam * data);

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);

void getHtml (char * file  ,char * url );

void getURL (char * sourcefile , char * hreffile);

int browser (char * sourcefile , char * research , char * url, long * cursorPosition, GList ** existHref);

void getArticle (char * sourcefile);

int roundRobinAlgorithm(int numberOfTeam, int **** returnArray);

void freeRoundRobinArray(int numberOfTeam, int **** arrayToFree);

int insertMatch(int *** allMatch, char *** data, int nmb, int nRound, char * leagueId, GDate * startDateFirstPart, GDate * startDateSecondPart, PrepareStatement * exec);


#endif //PROJET_C_MAINFUNCTION_H
