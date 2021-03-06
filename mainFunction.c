//
// Created by mathi on 16/01/2018.
//

#include "mainFunction.h"

GError *loadGladeFile(GtkBuilder **builder, char *fileName) {
    GError *error = NULL;

    *builder = gtk_builder_new();

    gtk_builder_add_from_file(*builder, fileName, &error);

    return error;
}

GtkWidget *findChild(GtkWidget *parent, const gchar *name) {

    if (strcmp(gtk_widget_get_name(parent), (gchar *) name) == 0) {
        return parent;
    }

    if (GTK_IS_BIN(parent)) {
        GtkWidget *child = gtk_bin_get_child(GTK_BIN(parent));
        return findChild(child, name);
    }

    if (GTK_IS_CONTAINER(parent)) {
        GList *children = gtk_container_get_children(GTK_CONTAINER(parent));
        while ((children = g_list_next(children)) != NULL) {
            GtkWidget *widget = findChild(children->data, name);
            if (widget != NULL) {
                return widget;
            }
        }
    }

    return NULL;
}

/**
 *
 * @param builder The builder of
 * @param parentName
 * @param objectName
 * @param objectLabel
 * @param fileName
 * @param list
 * @return
 */
CallbackParam *
initAddNotebookTabButton(GtkBuilder *builder, char *parentName, char *objectName, char *objectLabel, char *fileName,
                         void *function, GList **list, MainParam *mainParam) {
    CallbackParam *tempCallBackParam = NULL;

    tempCallBackParam = (CallbackParam *) malloc(sizeof(CallbackParam));

    if (tempCallBackParam != NULL) {
        tempCallBackParam->builder = builder;
        tempCallBackParam->mainParam = mainParam;
        tempCallBackParam->function = function;
        strcpy((char *) tempCallBackParam->parentName, parentName);
        strcpy((char *) tempCallBackParam->objectName, objectName);
        strcpy((char *) tempCallBackParam->objectLabel, objectLabel);
        strcpy((char *) tempCallBackParam->fileName, fileName);


        *list = g_list_prepend(*list, tempCallBackParam);


        return tempCallBackParam;
    }

    return NULL;

}

void initLeagueTreeView(GtkWidget *parentBox, CallbackParam *data) {
    /*
     * Add the storage list
     * Make the database request
     * Create the tree view
     * Add database values to this tree view
     */
    GtkTreeView *tempView = NULL;
    GtkListStore *listStore = NULL;
    GtkWidget *button = NULL;
    GtkTreeIter tempIter;
    QueryStatement *queryResult = NULL;
    char ***finalData = NULL;
    TabSearchParam **tabParam = (TabSearchParam **) malloc(2 * sizeof(TabSearchParam *));
    TabSearch *mainParam = (TabSearch *) malloc(sizeof(TabSearch));
    AllTabParam *completeTabParam = (AllTabParam *) malloc(sizeof(AllTabParam));

    /*
     * Get the list store
     */
    listStore = (GtkListStore *) gtk_builder_get_object(data->builder, "leagueListStore");


    /*
     * If the database connector is not set
     */
    if (data->mainParam->databaseInfo->error != 1) {

        /*
         * Select all entries on League tab
         * Fetch All the result to array
         * Append this result to the list store
         * Close the query
         */
        queryResult = query(data->mainParam->databaseInfo,
                            "SELECT id, name, to_char(\"dateUpdate\", 'YYYY-MM-DD') AS \"dateUpdate\" FROM \"League\"");

        fetchAllResult(queryResult, &finalData);

        for (int i = 0; i < queryResult->numberOfrow; ++i) {

            gtk_list_store_append(listStore, &tempIter);
            gtk_list_store_set(listStore, &tempIter, 0, finalData[i][0], 1, finalData[i][1], 2, finalData[i][2], -1);
        }

        closeQuery(queryResult, finalData);
    }

    /*
     * Get the search button
     */
    button = (GtkWidget *) gtk_builder_get_object(data->builder, "searchLeagueButton");

    /*
     * Prepare the search param
     * Add all the condition
     * Add all the GTK entry ID
     * Put all this param on an array
     * Set the builder
     * Set the main param
     * Set the SQL request
     * Set the list store ID
     */
    if (tabParam != NULL && completeTabParam != NULL) {

        tabParam[0] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if (tabParam[0] != NULL) {

            strcpy(tabParam[0]->condition, "id = $");
            tabParam[0]->typeCondition = 0;
            strcpy(tabParam[0]->gtkEntryId, "leagueTabIdEntry");
        }

        tabParam[1] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if (tabParam[1] != NULL) {
            strcpy(tabParam[1]->condition, "name LIKE $");
            tabParam[1]->typeCondition = 1;
            strcpy(tabParam[1]->gtkEntryId, "leagueTabNameEntry");
        }

        mainParam->allSearchParam = tabParam;
        mainParam->builder = data->builder;
        mainParam->mainParam = data->mainParam;
        strcpy(mainParam->statement,
               "SELECT id, name, to_char(\"dateUpdate\", 'YYYY-MM-DD') AS \"dateUpdate\" FROM \"League\"");
        mainParam->numberOfParam = 2;
        strcpy(mainParam->listStoreId, "leagueListStore");

        /*
         * Connect the button to the search button
         * Send all the param added before
         */
        if (button != NULL)
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(tabSearch), mainParam);

        button = (GtkWidget *) gtk_builder_get_object(data->builder, "leagueTabAddButton");

        completeTabParam->centralParam = data->mainParam;
        completeTabParam->builder = NULL;
        completeTabParam->searchParam = mainParam;

        /*
         * Connect the new league button to the dialog box function
         * Send all the param added before
         */
        if (button != NULL)
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(openAddNewLeagueForm),
                             (gpointer) completeTabParam);


        tempView = (GtkTreeView *) gtk_builder_get_object(data->builder, "leagueTreeView");

        if (tempView != NULL)
            g_signal_connect(G_OBJECT(tempView), "row-activated", G_CALLBACK(displayLeagueDetail), (gpointer) data);

    }


}

void initTeamTreeView(GtkWidget *parentBox, CallbackParam *data) {
    GtkListStore *listStore = NULL;
    GtkWidget *button = NULL;
    GtkTreeIter tempIter;
    QueryStatement *queryResult = NULL;
    char ***finalData = NULL;
    TabSearchParam **tabParam = (TabSearchParam **) malloc(4 * sizeof(TabSearchParam *));
    TabSearch *mainParam = (TabSearch *) malloc(sizeof(TabSearch));
    AllTabParam *completeTabParam = (AllTabParam *) malloc(sizeof(AllTabParam));
    GtkTreeView *tempView = NULL;

    listStore = (GtkListStore *) gtk_builder_get_object(data->builder, "teamListStore");

    if (data->mainParam->databaseInfo->error != 1) {

        queryResult = query(data->mainParam->databaseInfo,
                            "SELECT"
                                    "\"Team\".id,"
                                    "\"Team\".name,"
                                    "\"Team\".city,"
                                    "\"League\".Name AS \"leagueName\","
                                    "\"Team\".stadium,"
                                    "to_char(\"Team\".\"dateUpdate\", 'YYYY-MM-DD') AS \"dateUpdate\""
                                    "FROM \"Team\"\n"
                                    "JOIN \"League\" ON \"Team\".\"idLeague\" = \"League\".id ORDER BY name");

        fetchAllResult(queryResult, &finalData);

        for (int i = 0; i < queryResult->numberOfrow; ++i) {

            gtk_list_store_append(listStore, &tempIter);
            gtk_list_store_set(listStore, &tempIter, 0, finalData[i][0], 1, finalData[i][1], 2, finalData[i][2], 3,
                               finalData[i][3], 4, finalData[i][4], 5, finalData[i][5], -1);
        }

        closeQuery(queryResult, finalData);
    }

    if (tabParam != NULL && completeTabParam != NULL) {

        tabParam[0] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if (tabParam[0] != NULL) {

            strcpy(tabParam[0]->condition, "\"Team\".id = $");
            tabParam[0]->typeCondition = 0;
            strcpy(tabParam[0]->gtkEntryId, "idTeamEntry");
        }

        tabParam[1] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if (tabParam[1] != NULL) {
            strcpy(tabParam[1]->condition, "\"Team\".name ILIKE $");
            tabParam[1]->typeCondition = 1;
            strcpy(tabParam[1]->gtkEntryId, "nameTeamEntry");
        }

        tabParam[2] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if (tabParam[2] != NULL) {
            strcpy(tabParam[2]->condition, "\"Team\".city ILIKE $");
            tabParam[2]->typeCondition = 2;
            strcpy(tabParam[2]->gtkEntryId, "townTeamEntry");
        }

        tabParam[3] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if (tabParam[3] != NULL) {
            strcpy(tabParam[3]->condition, "\"League\".name ILIKE $");
            tabParam[3]->typeCondition = 2;
            strcpy(tabParam[3]->gtkEntryId, "leagueTeamEntry");
        }

        mainParam->allSearchParam = tabParam;
        mainParam->builder = data->builder;
        mainParam->mainParam = data->mainParam;
        strcpy(mainParam->statement,
               "SELECT"
                       "    \"Team\".id,"
                       "    \"Team\".name,"
                       "    \"Team\".city,"
                       "    \"League\".Name AS \"leagueName\","
                       "    \"Team\".stadium,"
                       "    to_char(\"Team\".\"dateUpdate\", 'YYYY-MM-DD') AS \"dateUpdate\""
                       "FROM \"Team\""
                       "JOIN \"League\" ON \"Team\".\"idLeague\" = \"League\".id");
        mainParam->numberOfParam = 4;
        strcpy(mainParam->listStoreId, "teamListStore");

        button = (GtkWidget *) gtk_builder_get_object(data->builder, "teamSearchButton");

        if (button != NULL)
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(tabSearch), mainParam);

        button = (GtkWidget *) gtk_builder_get_object(data->builder, "teamTabNewButton");

        completeTabParam->centralParam = data->mainParam;
        completeTabParam->builder = NULL;
        completeTabParam->searchParam = mainParam;

        if (button != NULL)
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(openAddNewTeamForm), (gpointer) completeTabParam);


        tempView = (GtkTreeView *) gtk_builder_get_object(data->builder, "teamTreeView");

        if (tempView != NULL)
            g_signal_connect(G_OBJECT(tempView), "row-activated", G_CALLBACK(displayTeamDetail), (gpointer) data);

    }
}

void initPlayerTreeView(GtkWidget *parentBox, CallbackParam *data) {
    GtkListStore *listStore = NULL;
    GtkWidget *button = NULL;
    GtkTreeIter tempIter;
    QueryStatement *queryResult = NULL;
    char ***finalData = NULL;
    TabSearchParam **tabParam = (TabSearchParam **) malloc(4 * sizeof(TabSearchParam *));
    TabSearch *mainParam = (TabSearch *) malloc(sizeof(TabSearch));
    AllTabParam *completeTabParam = (AllTabParam *) malloc(sizeof(AllTabParam));
    GtkTreeView *tempView = NULL;

    listStore = (GtkListStore *) gtk_builder_get_object(data->builder, "playerListStore");


    if (data->mainParam->databaseInfo->error != 1) {

        queryResult = query(data->mainParam->databaseInfo,
                            "SELECT\n"
                                    "\"Player\".id,\n"
                                    "\"Player\".firstname || ' ' || \"Player\".lastname AS Name,\n"
                                    "\"Team\".name as TeamName,\n"
                                    "\"Position\".acronym as PositionName,\n"
                                    "to_char(\"Player\".\"dateUpdate\", 'YYYY-MM-DD') AS \"dateUpdate\"\n"
                                    "FROM \"Player\"\n"
                                    "JOIN \"Position\" ON \"Player\".\"idPosition\" = \"Position\".id\n"
                                    "JOIN \"Team\" ON \"Player\".\"idTeam\" = \"Team\".id");

        fetchAllResult(queryResult, &finalData);


        for (int i = 0; i < queryResult->numberOfrow; ++i) {

            gtk_list_store_append(listStore, &tempIter);
            gtk_list_store_set(listStore, &tempIter, 0, finalData[i][0], 1, finalData[i][1], 2, finalData[i][2], 3,
                               finalData[i][3], 4, finalData[i][4], -1);
        }

        closeQuery(queryResult, finalData);
    }


    if (tabParam != NULL && completeTabParam != NULL) {

        tabParam[0] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if (tabParam[0] != NULL) {

            strcpy(tabParam[0]->condition, "\"Player\".id = $");
            tabParam[0]->typeCondition = 0;
            strcpy(tabParam[0]->gtkEntryId, "idPlayerEntry");
        }

        tabParam[1] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if (tabParam[1] != NULL) {
            strcpy(tabParam[1]->condition, "\"Player\".firstname || \"Player\".lastname  ILIKE $");
            tabParam[1]->typeCondition = 1;
            strcpy(tabParam[1]->gtkEntryId, "namePlayerEntry");
        }

        tabParam[2] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if (tabParam[2] != NULL) {
            strcpy(tabParam[2]->condition, "\"Team\".name ILIKE $");
            tabParam[2]->typeCondition = 1;
            strcpy(tabParam[2]->gtkEntryId, "teamPlayerEntry");
        }

        tabParam[3] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if (tabParam[3] != NULL) {
            strcpy(tabParam[3]->condition, "\"Position\".acronym ILIKE $");
            tabParam[3]->typeCondition = 1;
            strcpy(tabParam[3]->gtkEntryId, "positionPlayerEntry");
        }


        mainParam->allSearchParam = tabParam;
        mainParam->builder = data->builder;
        mainParam->mainParam = data->mainParam;

        strcpy(mainParam->statement,
               "SELECT\n"
                       "\"Player\".id,\n"
                       "\"Player\".firstname || ' ' || \"Player\".lastname AS Name,\n"
                       "\"Team\".name as TeamName,\n"
                       "\"Position\".acronym as PositionName,\n"
                       "to_char(\"Player\".\"dateUpdate\", 'YYYY-MM-DD') AS \"dateUpdate\"\n"
                       "FROM \"Player\"\n"
                       "JOIN \"Position\" ON \"Player\".\"idPosition\" = \"Position\".id\n"
                       "JOIN \"Team\" ON \"Player\".\"idTeam\" = \"Team\".id");

        mainParam->numberOfParam = 4;
        strcpy(mainParam->listStoreId, "playerListStore");

        button = (GtkWidget *) gtk_builder_get_object(data->builder, "playerSearchButton");

        if (button != NULL)
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(tabSearch), mainParam);

        button = (GtkWidget *) gtk_builder_get_object(data->builder, "playerTabNewButton");

        completeTabParam->centralParam = data->mainParam;
        completeTabParam->builder = NULL;
        completeTabParam->searchParam = mainParam;

        if (button != NULL) {
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(openAddNewPlayerForm),
                             (gpointer) completeTabParam);

        }

        tempView = (GtkTreeView *) gtk_builder_get_object(data->builder, "playerTreeView");

        if (tempView != NULL)
            g_signal_connect(G_OBJECT(tempView), "row-activated", G_CALLBACK(displayPlayerDetail), (gpointer) data);

    }

}

void initMatchTreeView(GtkWidget *parentBox, CallbackParam *data) {

    GtkListStore *listStore = NULL;
    GtkWidget *button = NULL;
    GtkTreeIter tempIter;
    QueryStatement *queryResult = NULL;
    char ***finalData = NULL;
    TabSearchParam **tabParam = (TabSearchParam **) malloc(2 * sizeof(TabSearchParam *));
    TabSearch *mainParam = (TabSearch *) malloc(sizeof(TabSearch));
    GtkTreeView *tempView = NULL;
    WindowCalendarParam *calendarParam = (WindowCalendarParam *) malloc(sizeof(WindowCalendarParam));


    listStore = (GtkListStore *) gtk_builder_get_object(data->builder, "matchListStore");

    if (data->mainParam->databaseInfo->error != 1) {

        queryResult = query(data->mainParam->databaseInfo,
                            "SELECT \"Match\".id,\n"
                                    "\"HomeTeam\".\"name\" || ' vs '|| \"OutsideTeam\".\"name\" AS name,\n"
                                    "\"Match\".\"numberOfGoalHomeTeam\"|| '-' || \"Match\".\"numberOfGoalOutsideTeam\" AS goals,\n"
                                    " \"Match\".\"stadium\",\n"
                                    "to_char(\"Match\".\"date\", 'YYYY-MM-DD')\n"
                                    "FROM \"Match\"\n"
                                    "JOIN \"Team\" as \"HomeTeam\" on \"Match\".\"homeTeam\" = \"HomeTeam\".id\n"
                                    "JOIN \"Team\" as \"OutsideTeam\" on \"Match\".\"outsideTeam\" = \"OutsideTeam\".id\n"
                                    "ORDER BY \"Match\".date ASC, \"Match\".id ASC");

        fetchAllResult(queryResult, &finalData);


        for (int i = 0; i < queryResult->numberOfrow; ++i) {

            gtk_list_store_append(listStore, &tempIter);
            gtk_list_store_set(listStore, &tempIter, 0, finalData[i][0], 1, finalData[i][1], 2, finalData[i][2], 3,
                               finalData[i][3], 4, finalData[i][4], -1);
        }

        closeQuery(queryResult, finalData);
    }


    if (tabParam != NULL) {

        tabParam[0] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if (tabParam[0] != NULL) {

            strcpy(tabParam[0]->condition, "\"Match\".id = $");
            tabParam[0]->typeCondition = 0;
            strcpy(tabParam[0]->gtkEntryId, "idMatchEntry");
        }

        tabParam[1] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if (tabParam[1] != NULL) {
            strcpy(tabParam[1]->condition, "\"HomeTeam\".name || \"OutsideTeam\".name  ILIKE $");
            tabParam[1]->typeCondition = 1;
            strcpy(tabParam[1]->gtkEntryId, "nameNameEntry");
        }

        tabParam[2] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if (tabParam[2] != NULL) {
            strcpy(tabParam[2]->condition, "to_char(\"Match\".\"date\", 'YYYY-MM-DD') ILIKE $");
            tabParam[2]->typeCondition = 1;
            strcpy(tabParam[2]->gtkEntryId, "dateMatchEntry");
        }


        mainParam->allSearchParam = tabParam;
        mainParam->builder = data->builder;
        mainParam->mainParam = data->mainParam;

        strcpy(mainParam->statement,
               "SELECT \"Match\".id,\n"
                       "\"HomeTeam\".\"name\" || ' vs '|| \"OutsideTeam\".\"name\" AS name,\n"
                       "\"Match\".\"numberOfGoalHomeTeam\"|| '-' || \"Match\".\"numberOfGoalOutsideTeam\" AS goals,\n"
                       " \"Match\".\"stadium\",\n"
                       "to_char(\"Match\".\"date\", 'YYYY-MM-DD')\n"
                       "FROM \"Match\"\n"
                       "JOIN \"Team\" as \"HomeTeam\" on \"Match\".\"homeTeam\" = \"HomeTeam\".id\n"
                       "JOIN \"Team\" as \"OutsideTeam\" on \"Match\".\"outsideTeam\" = \"OutsideTeam\".id"
                       );

        mainParam->numberOfParam = 3;

        strcpy(mainParam->listStoreId, "matchListStore");

        button = (GtkWidget *) gtk_builder_get_object(data->builder, "matchSearchButton");

        if (button != NULL) {
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(tabSearch), mainParam);
        }

        button = (GtkWidget *) gtk_builder_get_object(data->builder, "dateMatchEntry");


        if (calendarParam != NULL) {

            calendarParam->destinationWidget = button;
            calendarParam->calendarWindow = NULL;
            calendarParam->month = -1;
            calendarParam->day = -1;
            calendarParam->year = -1;
        }


        if (button != NULL)
            g_signal_connect(G_OBJECT(button), "icon-press", G_CALLBACK(openCalendar), (gpointer *) calendarParam);

    }

    tempView = (GtkTreeView *) gtk_builder_get_object(data->builder, "matchTreeView");

    if (tempView != NULL) {
        g_signal_connect(G_OBJECT(tempView), "row-activated", G_CALLBACK(displayMatchDetail), (gpointer) data);
    }
}

void initNewsTreeView(GtkWidget *parentBox, CallbackParam *data){

    GtkWidget *tempWidget = NULL;
    GtkTreeView *tempView = NULL;
    AllTabParam * tabParam = (AllTabParam *) calloc(1, sizeof(AllTabParam));

    tabParam->builder = data->builder;
    tabParam->centralParam = data->mainParam;

    tempWidget = (GtkWidget *) gtk_builder_get_object(data->builder, "newsSearchButton");

    if(tempWidget != NULL)
        g_signal_connect(G_OBJECT(tempWidget), "clicked", G_CALLBACK(searchArticle), (gpointer *) tabParam);


    tempView = (GtkTreeView *) gtk_builder_get_object(data->builder, "newsTreeView");

    if (tempView != NULL)
        g_signal_connect(G_OBJECT(tempView), "row-activated", G_CALLBACK(displayNewsDetail), (gpointer) data);
}

int roundRobinAlgorithm(int numberOfTeam, int ****returnArray) {
    int ***roundRobinArray = *returnArray;
    int nRound = numberOfTeam - 1;
    int i, j, l, tmp;

    roundRobinArray = (int ***) calloc(2, sizeof(int **));
    /*
     * First dimension return
     */
    if (roundRobinArray != NULL) {
        roundRobinArray[0] = (int **) calloc((size_t) (nRound * 2), sizeof(int *));
        roundRobinArray[1] = (int **) calloc((size_t) (nRound * 2), sizeof(int *));
    } else {
        return -1;
    }

    /*
     * Check if the allocation of the second dimension is correct
     */
    if (roundRobinArray[0] == NULL || roundRobinArray[1] == 0)
        return -1;

    /*
     * Allocate the third dimension
     */
    for (i = 0; i < nRound * 2; ++i) {

        roundRobinArray[0][i] = (int *) calloc((size_t) (numberOfTeam / 2), sizeof(int));

        roundRobinArray[1][i] = (int *) calloc((size_t) (numberOfTeam / 2), sizeof(int));
    }

    roundRobinArray[0][0][0] = -1;
    for (i = 1; i < numberOfTeam / 2; ++i) {
        roundRobinArray[0][0][i] = i;
        roundRobinArray[1][0][i] = nRound - i;
        if (i % 2 == 1) {
            tmp = roundRobinArray[0][0][i];
            roundRobinArray[0][0][i] = roundRobinArray[1][0][i];
            roundRobinArray[1][0][i] = tmp;
        }
    }


    for (i = 1; i < nRound; ++i) {
        for (j = 0; j < numberOfTeam / 2; ++j) {
            for (l = 0; l < 2; ++l) {
                if (roundRobinArray[l][i - 1][j] >= 0) {
                    roundRobinArray[l][i][j] = (roundRobinArray[l][i - 1][j] + 1) % nRound;
                } else {
                    roundRobinArray[l][i][j] = -1;
                }
            }
        }
    }

    for (i = 1; i < nRound - 1; i += 2) {
        roundRobinArray[0][i][0] = roundRobinArray[1][i][0];
        roundRobinArray[1][i][0] = -1;
    }

    for (i = 0; i < nRound; ++i) {
        for (j = 0; j < numberOfTeam / 2; ++j) {
            roundRobinArray[0][i][j] = roundRobinArray[0][i][j] + 1;
            roundRobinArray[1][i][j] = roundRobinArray[1][i][j] + 1;
        }
    }

    /*
     * Second part of round robin array
     */
    tmp = roundRobinArray[0][nRound - 2][0];
    roundRobinArray[0][nRound - 2][0] = roundRobinArray[1][nRound - 2][0];
    roundRobinArray[1][nRound - 2][0] = tmp;
    tmp = roundRobinArray[0][nRound - 1][0];
    roundRobinArray[0][nRound - 1][0] = roundRobinArray[1][nRound - 1][0];
    roundRobinArray[1][nRound - 1][0] = tmp;

    for (i = 0; i < nRound; ++i) {
        for (j = 0; j < numberOfTeam / 2; ++j) {
            roundRobinArray[0][i + nRound][j] = roundRobinArray[1][i][j];
            roundRobinArray[1][i + nRound][j] = roundRobinArray[0][i][j];
        }
    }

    /*for (i = 0; i < nRound *2 ; ++i) {
        printf("%d:\n", i);
        for (j = 0; j < numberOfTeam / 2; ++j) {
            printf("  %d:\n", j);
            printf("    %d \n", roundRobinArray[0][i][j]);
            printf("    %d \n", roundRobinArray[1][i][j]);
        }
    }*/

    *returnArray = roundRobinArray;

    return nRound;

}

void freeRoundRobinArray(int numberOfTeam, int ****arrayToFree) {

    int nRound = numberOfTeam - 1;
    int i, j, l, tmp;
    for (i = 0; i < nRound * 2; ++i) {

        free((*arrayToFree)[0][i]);
        free((*arrayToFree)[1][i]);

    }

    free((*arrayToFree)[0]);
    free((*arrayToFree)[1]);

    free(*arrayToFree);

    *arrayToFree = NULL;
}

int insertMatch(int ***allMatch, char ***data, int nmb, int nRound, char *leagueId, GDate *startDateFirstPart, GDate *startDateSecondPart, PrepareStatement *exec) {
    int i, j, l, k = 1, n, tmp = 0, m = 0, loopInitializer, loopEnd;
    char tempChar[20];
    char *pointerChar;
    GString *statement;
    GDate *tempDate;

    /*
     * Put the query to a gstring, to make easier manipulation
     */
    statement = g_string_new(exec->query);

    for (n = 0; n < 2; ++n) {

        if (n == 0) {
            tempDate = startDateFirstPart;
            loopInitializer = 0;
            loopEnd = nRound;
        } else {
            tempDate = startDateSecondPart;
            loopInitializer = nRound;
            loopEnd = nRound * 2;
        }

        for (i = loopInitializer; i < loopEnd; i++) {
            g_date_strftime(tempChar, 20, "%Y-%m-%d", tempDate);
            pointerChar = g_strdup(tempChar);

            for (j = 0; j < nmb / 2; j++) {

                if (tmp == 1)
                    statement = g_string_append(statement, ", ");
                tmp = 1;

                statement = g_string_append(statement, "(");
                for (l = 0; l < 5; ++l) {
                    if (l != 4) {
                        g_string_append_printf(statement, "$%d,", k++);
                    } else {
                        g_string_append_printf(statement, "$%d", k++);
                    }
                }
                statement = g_string_append(statement, ")");

                bindParam(exec, data[allMatch[0][i][j]][0], m++);
                bindParam(exec, data[allMatch[1][i][j]][0], m++);
                bindParam(exec, pointerChar, m++);
                bindParam(exec, leagueId, m++);
                bindParam(exec, data[allMatch[0][i][j]][1], m++);
            }

            g_date_add_days(tempDate, 7);
        }
    }

    exec->query = statement->str;


    return k;

}


/*
 * Curl function
 */
static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *) stream);
    return written;
}

void getHtml(char *file, char *url) {

    CURL *curl_handle;
    FILE *bodyfile;

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();


    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);


    bodyfile = fopen(file, "wb");

    if (!bodyfile) {
        curl_easy_cleanup(curl_handle);
    }

    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, bodyfile);
    curl_easy_perform(curl_handle);
    fclose(bodyfile);
    curl_easy_cleanup(curl_handle);


}

void getURL(char *sourcefile, char *hreffile) {
    FILE *bodyfile;
    FILE *href;

    bodyfile = fopen(sourcefile, "r");

    char chaine[2000] = "";
    char *res;
    char *result;
    char *temp;


    href = fopen(hreffile, "w+");

    if (bodyfile != NULL) {
        while (fgets(chaine, 2000, bodyfile) != NULL) {
            res = strstr(chaine, "href");

            if (res != NULL) {
                if ((temp = strchr(res, '"')) != NULL)
                    result = temp + 1;

                if ((temp = strchr(result, '"')) != NULL)
                    *temp = 0;

                fprintf(href, "%s\n", result);
            }
        }
        fclose(href);
        fclose(bodyfile);
    }
}

int browser(char *sourcefile, char *research, char *url, long *cursorPosition, GList **existHref) {
    char *res;
    char *http;
    GString *result;
    FILE *searchfile;
    searchfile = fopen(sourcefile, "r");
    char search[2000] = "";
    GList *temp = NULL;
    int condition = 0;

    if (searchfile == NULL) {
        printf("Please verify your source file");
        return -1;
    }

    research = g_utf8_strdown(research, -1);

    fseek(searchfile, *cursorPosition, SEEK_SET);

    while (fgets(search, 2000, searchfile) != NULL) {
        strcpy(search, g_utf8_strdown(search, -1));

        res = strstr(search, research);

        search[strlen(search) - 1] = 0;

        if (res != NULL) {
            http = strstr(search, "http");
            result = g_string_new(search);

            if (http == NULL) {
                condition = 0;
                result = g_string_prepend(result, url);
                for (temp = *existHref; temp != NULL; temp = temp->next) {
                    if (strcmp((char *) temp->data, result->str) == 0) {
                        condition = 1;
                        break;
                    }
                }
                if (condition == 0) {
                    getHtml("article.html", result->str);
                    *cursorPosition = ftell(searchfile);
                    fclose(searchfile);
                    *existHref = g_list_prepend(*existHref, (gpointer *) result->str);
                    return 0;
                }

            }


            g_string_free(result, TRUE);

        }

    }

    fclose(searchfile);

    return -1;

}

void getArticle(char *sourcefile, char ** buffer, char ** title) {
    char *res;
    char *result;
    char *startP = NULL;
    char *endP = NULL;
    char * divP = NULL;
    char *temp = NULL;

    FILE *searchfile;
    searchfile = fopen(sourcefile, "r");
    char * search = (char *) malloc(10000 * sizeof(char));
    char *tempSearch = NULL;
    GString *article = g_string_new("");
    int draw = 0;
    int numberOfDiv = 0;


    if (searchfile == NULL) {
        printf("Please verify your source file");
    }

    if (searchfile != NULL) {


        while (fgets(search, 10000, searchfile) != NULL) {

            res = strstr(search, "<h1 itemprop=\"headline\"");

            //search[strlen(search) - 1] = 0;

            if (res != NULL) {
                if ((temp = strchr(res, '>')) != NULL)
                    result = temp + 1;

                if ((temp = strchr(result, '<')) != NULL)
                    *temp = 0;

                *title = strdup(result);
            }
        }

        fseek(searchfile, 0, SEEK_SET);


        while (((res = strstr(search, "class=\"line article-text\" itemprop=\"articleBody\">")) == NULL) &&
               fgets(search, 2000, searchfile) != NULL);


        if (res != NULL) {

            while (res != NULL || fgets(search, 10000, searchfile) != NULL) {

                //article = g_string_erase(article,0,-1);



                res = NULL;

                do{

                    tempSearch = strdup(search);

                    if ((endP = strstr(search, "</p>")) != NULL) {
                        draw = 0;
                    }

                    // Quand on trouve notre p write = 1
                    if ((startP = strstr(search, "<p>")) != NULL) {
                        draw = 1;
                    }

                    // Quand on trouve notre p fermant write 0


                    if ((temp = strstr(search, "<div>")) != NULL) {
                        numberOfDiv++;
                    }


                    if(endP != NULL && startP!=NULL){

                        if(startP > endP){
                            *endP = 0;
                            article = g_string_append(article, search);
                            article = g_string_append(article, startP + 3);
                            search += strlen(startP) - 1;

                        }else if(endP > startP){
                            *endP = 0;
                            article = g_string_append(article, startP + 3);
                            search = endP + 1;
                        }

                    }else if(endP != NULL && startP == NULL){
                        *endP = 0;
                        article = g_string_append(article, search);
                        search = endP + 1;

                    }else if(startP != NULL && endP == NULL){

                        article = g_string_append(article, startP + 3);
                        search += strlen(search) - 1;

                    }else if (draw == 1) {
                        article = g_string_append(article, search);
                        search += strlen(search) - 1;
                    }

                    if ((divP = strstr(tempSearch, "</div>")) != NULL) {

                        if (numberOfDiv == 0) {

                            *divP = 0;
                            *buffer = article->str;

                            if(startP == NULL && endP == NULL){
                                fclose(searchfile);
                                return;
                            }

                        } else {
                            numberOfDiv--;
                            search = divP + 1;
                        }
                    }
                    g_free(tempSearch);

                }while(startP != NULL || endP != NULL || divP != NULL);

            }

        }

    }
    fclose(searchfile);
}

void clearHTMLData(char ** buffer){
    GString * tempString = g_string_new(*buffer);
    char * tempChar = tempString->str;
    char * test = strdup(tempString->str);
    char * startChar = NULL;
    char * endChar = NULL;

    do{
        startChar = strchr(tempString->str, '<');
        endChar = strchr(tempString->str, '>');

        if(startChar != NULL && endChar != NULL){
            if(endChar < startChar){
                tempString = g_string_erase(tempString, (gssize) (endChar - tempString->str), 1);
            }else{
                tempString = g_string_erase(tempString, (gssize) (startChar - tempString->str), (gssize) (endChar - startChar + 1));
            }
        }

        if(startChar != NULL && endChar == NULL)
            tempString = g_string_erase(tempString, (gssize) (startChar - tempString->str), -1);


    }while(startChar != NULL || endChar != NULL);

    if(*buffer != NULL){
        *buffer = tempString->str;
    }


    g_string_free(tempString, FALSE);


}
