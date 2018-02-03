//
// Created by mathi on 16/01/2018.
//

#include "mainFunction.h"

GError * loadGladeFile(GtkBuilder ** builder, char *fileName){
    GError * error = NULL;

    *builder = gtk_builder_new();

    gtk_builder_add_from_file(*builder, fileName, &error);

    return error;
}

GtkWidget* findChild(GtkWidget * parent, const gchar * name) {

    if (strcmp(gtk_widget_get_name(parent), (gchar*)name) == 0) {
        return parent;
    }

    if (GTK_IS_BIN(parent)) {
        GtkWidget *child = gtk_bin_get_child(GTK_BIN(parent));
        return findChild(child, name);
    }

    if (GTK_IS_CONTAINER(parent)) {
        GList *children = gtk_container_get_children(GTK_CONTAINER(parent));
        while ((children = g_list_next(children)) != NULL) {
            GtkWidget* widget = findChild(children->data, name);
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
CallbackParam * initAddNotebookTabButton(GtkBuilder * builder, char * parentName, char * objectName, char * objectLabel, char * fileName,void * function, GList ** list, MainParam * mainParam){
    CallbackParam * tempCallBackParam = NULL;

    tempCallBackParam = (CallbackParam *) malloc(sizeof(CallbackParam ));

    if(tempCallBackParam != NULL){
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

void initLeagueTreeView(GtkWidget * parentBox, CallbackParam * data) {
    /*
     * Add the storage list
     * Make the database request
     * Create the tree view
     * Add database values to this tree view
     */
    GtkTreeView * tempView = NULL;
    GtkListStore *listStore = NULL;
    GtkWidget * button = NULL;
    GtkTreeIter tempIter;
    QueryStatement * queryResult = NULL;
    char *** finalData = NULL;
    TabSearchParam ** tabParam = (TabSearchParam **) malloc(2 * sizeof(TabSearchParam *));
    TabSearch * mainParam = (TabSearch *) malloc(sizeof(TabSearch));
    AllTabParam * completeTabParam = (AllTabParam *) malloc(sizeof(AllTabParam));

    /*
     * Get the list store
     */
    listStore = (GtkListStore *) gtk_builder_get_object(data->builder, "leagueListStore");


    /*
     * If the database connector is not set
     */
    if(data->mainParam->databaseInfo->error != 1){

        /*
         * Select all entries on League tab
         * Fetch All the result to array
         * Append this result to the list store
         * Close the query
         */
        queryResult = query(data->mainParam->databaseInfo, "SELECT id, name, to_char(\"dateUpdate\", 'YYYY-MM-DD') AS \"dateUpdate\" FROM \"League\"");

        fetchAllResult(queryResult, &finalData);

        for (int i = 0; i < queryResult->numberOfrow; ++i) {

            gtk_list_store_append(listStore, &tempIter);
            gtk_list_store_set(listStore, &tempIter
                    , 0, finalData[i][0]
                    , 1, finalData[i][1]
                    , 2, finalData[i][2]
                    , -1);
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
    if(tabParam != NULL && completeTabParam != NULL){

        tabParam[0] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if(tabParam[0] != NULL){

            strcpy(tabParam[0]->condition, "id = $");
            tabParam[0]->typeCondition = 0;
            strcpy(tabParam[0]->gtkEntryId, "leagueTabIdEntry");
        }

        tabParam[1] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if(tabParam[1] != NULL){
            strcpy(tabParam[1]->condition, "name LIKE $");
            tabParam[1]->typeCondition = 1;
            strcpy(tabParam[1]->gtkEntryId, "leagueTabNameEntry");
        }

        mainParam->allSearchParam = tabParam;
        mainParam->builder = data->builder;
        mainParam->mainParam = data->mainParam;
        strcpy(mainParam->statement, "SELECT id, name, to_char(\"dateUpdate\", 'YYYY-MM-DD') AS \"dateUpdate\" FROM \"League\"");
        mainParam->numberOfParam = 2;
        strcpy(mainParam->listStoreId, "leagueListStore");

        /*
         * Connect the button to the search button
         * Send all the param added before
         */
        if(button != NULL)
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(tabSearch), mainParam);

        button = (GtkWidget *) gtk_builder_get_object(data->builder, "leagueTabAddButton");

        completeTabParam->mainParam = data;
        completeTabParam->searchParam = mainParam;

        /*
         * Connect the new league button to the dialog box function
         * Send all the param added before
         */
        if(button != NULL)
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(openAddNewLeagueForm), (gpointer) completeTabParam);


        tempView = (GtkTreeView *) gtk_builder_get_object(data->builder, "leagueTreeView");

        if(tempView != NULL)
            g_signal_connect(G_OBJECT(tempView), "row-activated", G_CALLBACK(displayLeagueDetail), (gpointer) data);

    }


}

void initTeamTreeView(GtkWidget * parentBox, CallbackParam * data){
    GtkListStore *listStore = NULL;
    GtkWidget * button = NULL;
    GtkTreeIter tempIter;
    QueryStatement * queryResult = NULL;
    char *** finalData = NULL;
    TabSearchParam ** tabParam = (TabSearchParam **) malloc(4 * sizeof(TabSearchParam *));
    TabSearch * mainParam = (TabSearch *) malloc(sizeof(TabSearch));
    AllTabParam * completeTabParam = (AllTabParam *) malloc(sizeof(AllTabParam));
    GtkTreeView * tempView = NULL;

    listStore = (GtkListStore *) gtk_builder_get_object(data->builder, "teamListStore");

    if(data->mainParam->databaseInfo->error != 1){

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
            gtk_list_store_set(listStore, &tempIter
                    , 0, finalData[i][0]
                    , 1, finalData[i][1]
                    , 2, finalData[i][2]
                    , 3, finalData[i][3]
                    , 4, finalData[i][4]
                    , 5, finalData[i][5]
                    , -1);
        }

        closeQuery(queryResult, finalData);
    }

    if(tabParam != NULL && completeTabParam != NULL){

        tabParam[0] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if(tabParam[0] != NULL){

            strcpy(tabParam[0]->condition, "\"Team\".id = $");
            tabParam[0]->typeCondition = 0;
            strcpy(tabParam[0]->gtkEntryId, "idTeamEntry");
        }

        tabParam[1] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if(tabParam[1] != NULL){
            strcpy(tabParam[1]->condition, "\"Team\".name ILIKE $");
            tabParam[1]->typeCondition = 1;
            strcpy(tabParam[1]->gtkEntryId, "nameTeamEntry");
        }

        tabParam[2] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if(tabParam[2] != NULL){
            strcpy(tabParam[2]->condition, "\"Team\".city ILIKE $");
            tabParam[2]->typeCondition = 2;
            strcpy(tabParam[2]->gtkEntryId, "townTeamEntry");
        }

        tabParam[3] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if(tabParam[3] != NULL){
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

        if(button != NULL)
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(tabSearch), mainParam);

        button = (GtkWidget *) gtk_builder_get_object(data->builder, "teamTabNewButton");

        completeTabParam->mainParam = data;
        completeTabParam->searchParam = mainParam;

        if(button != NULL)
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(openAddNewTeamForm), (gpointer) completeTabParam);


        tempView = (GtkTreeView *) gtk_builder_get_object(data->builder, "teamTreeView");

        if(tempView != NULL)
            g_signal_connect(G_OBJECT(tempView), "row-activated", G_CALLBACK(displayTeamDetail), (gpointer) data);

    }
}


void initPlayerTreeView(GtkWidget * parentBox, CallbackParam * data){
    GtkListStore *listStore = NULL;
    GtkWidget * button = NULL;
    GtkTreeIter tempIter;
    QueryStatement * queryResult = NULL;
    char *** finalData = NULL;
    TabSearchParam ** tabParam = (TabSearchParam **) malloc(4 * sizeof(TabSearchParam *));
    TabSearch * mainParam = (TabSearch *) malloc(sizeof(TabSearch));
    AllTabParam * completeTabParam = (AllTabParam *) malloc(sizeof(AllTabParam));
    GtkTreeView * tempView = NULL;

    listStore = (GtkListStore *) gtk_builder_get_object(data->builder, "playerListStore");


    if(data->mainParam->databaseInfo->error != 1) {

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
            gtk_list_store_set(listStore, &tempIter
                    , 0, finalData[i][0]
                    , 1, finalData[i][1]
                    , 2, finalData[i][2]
                    , 3, finalData[i][3]
                    , 4, finalData[i][4]
                    , -1);
        }

        closeQuery(queryResult, finalData);
    }


    if(tabParam != NULL && completeTabParam != NULL){

        tabParam[0] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if(tabParam[0] != NULL){

            strcpy(tabParam[0]->condition, "\"Player\".id = $");
            tabParam[0]->typeCondition = 0;
            strcpy(tabParam[0]->gtkEntryId, "idPlayerEntry");
        }

        tabParam[1] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if(tabParam[1] != NULL){
            strcpy(tabParam[1]->condition, "\"Player\".firstname || \"Player\".lastname  ILIKE $");
            tabParam[1]->typeCondition = 1;
            strcpy(tabParam[1]->gtkEntryId, "namePlayerEntry");
        }

        tabParam[2] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if(tabParam[2] != NULL){
            strcpy(tabParam[2]->condition, "\"Team\".name ILIKE $");
            tabParam[2]->typeCondition = 1;
            strcpy(tabParam[2]->gtkEntryId, "teamPlayerEntry");
        }

        tabParam[3] = (TabSearchParam *) malloc(sizeof(TabSearchParam));
        if(tabParam[3] != NULL){
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

        if(button != NULL)
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(tabSearch), mainParam);

        button = (GtkWidget *) gtk_builder_get_object(data->builder, "playerTabNewButton");

        completeTabParam->mainParam = data;
        completeTabParam->searchParam = mainParam;

        if(button != NULL) {
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(openAddNewPlayerForm), (gpointer) completeTabParam);


        }

        tempView = (GtkTreeView *) gtk_builder_get_object(data->builder, "playerTreeView");

        if(tempView != NULL)
            g_signal_connect(G_OBJECT(tempView), "row-activated", G_CALLBACK(displayPlayerDetail), (gpointer) data);

       }

}


void initMatchTreeView(GtkWidget * parentBox, CallbackParam * data){
    GtkListStore *listStore = NULL;
    GtkWidget * button = NULL;
    GtkTreeIter tempIter;
    QueryStatement * queryResult = NULL;
    char *** finalData = NULL;
    TabSearchParam ** tabParam = (TabSearchParam **) malloc(2 * sizeof(TabSearchParam *));
    TabSearch * mainParam = (TabSearch *) malloc(sizeof(TabSearch));
    AllTabParam * completeTabParam = (AllTabParam *) malloc(sizeof(AllTabParam));
    GtkTreeView * tempView = NULL;


    listStore = (GtkListStore *) gtk_builder_get_object(data->builder, "matchListStore");

    if(data->mainParam->databaseInfo->error != 1) {

        queryResult = query(data->mainParam->databaseInfo,
                            "SELECT \"Match\".id,\n"
                                    "\"HomeTeam\".\"name\" || ' vs '|| \"OutsideTeam\".\"name\" AS name,\n"
                                    "\"Match\".\"numberOfGoalHomeTeam\"|| '-' || \"Match\".\"numberOfGoalOutsideTeam\" AS goals,\n"
                                    " \"Match\".\"stadium\",\n"
                                    " \"Match\".\"dateUpdate\"\n"
                                    "FROM \"Match\"\n"
                                    "JOIN \"Team\" as \"HomeTeam\" on \"Match\".\"homeTeam\" = \"HomeTeam\".id\n"
                                    "JOIN \"Team\" as \"OutsideTeam\" on \"Match\".\"outsideTeam\" = \"OutsideTeam\".id");

        fetchAllResult(queryResult, &finalData);




        for (int i = 0; i < queryResult->numberOfrow; ++i) {

            gtk_list_store_append(listStore, &tempIter);
            gtk_list_store_set(listStore, &tempIter
                    , 0, finalData[i][0]
                    , 1, finalData[i][1]
                    , 2, finalData[i][2]
                    , 3, finalData[i][3]
                    , 4, finalData[i][4]
                    , -1);
        }

        closeQuery(queryResult, finalData);
    }


    if(tabParam != NULL && completeTabParam != NULL) {

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

        /*
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
         */


        mainParam->allSearchParam = tabParam;
        mainParam->builder = data->builder;
        mainParam->mainParam = data->mainParam;

        strcpy(mainParam->statement,
               "SELECT \"Match\".id,\n"
                       "\"HomeTeam\".\"name\" || ' vs '|| \"OutsideTeam\".\"name\" AS name,\n"
                       "\"Match\".\"numberOfGoalHomeTeam\"|| '-' || \"Match\".\"numberOfGoalOutsideTeam\" AS goals,\n"
                       " \"Match\".\"stadium\",\n"
                       " \"Match\".\"dateUpdate\"\n"
                       "FROM \"Match\"\n"
                       "JOIN \"Team\" as \"HomeTeam\" on \"Match\".\"homeTeam\" = \"HomeTeam\".id\n"
                       "JOIN \"Team\" as \"OutsideTeam\" on \"Match\".\"outsideTeam\" = \"OutsideTeam\".id");

        mainParam->numberOfParam = 2;

        strcpy(mainParam->listStoreId, "matchListStore");

        button = (GtkWidget *) gtk_builder_get_object(data->builder, "matchSearchButton");

        if (button != NULL){
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(tabSearch), mainParam);
        }



        completeTabParam->mainParam = data;
        completeTabParam->searchParam = mainParam;



        }

        tempView = (GtkTreeView *) gtk_builder_get_object(data->builder, "matchTreeView");

        if (tempView != NULL) {
            g_signal_connect(G_OBJECT(tempView), "row-activated", G_CALLBACK(displayPlayerDetail), (gpointer) data);
        }

    }