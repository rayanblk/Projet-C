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
    GtkListStore *listStore = NULL;
    GtkWidget * button = NULL;
    GtkTreeIter tempIter;
    QueryStatement * queryResult = NULL;
    char *** finalData = NULL;


    listStore = (GtkListStore *) gtk_builder_get_object(data->builder, "leagueListStore");

    if(data->mainParam->databaseInfo->error != 1){

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

    button = (GtkWidget *) gtk_builder_get_object(data->builder, "searchLeagueButton");

    TabSearchParam tabParam[2];
    TabSearch mainParam;

    tabParam[0].condition = "id = $";
    tabParam[0].typeCondition = 0;
    strcpy(tabParam[0].gtkEntryId, "leagueTabIdEntry");

    tabParam[1].condition = "name LIKE $";
    tabParam[1].typeCondition = 1;
    strcpy(tabParam[1].gtkEntryId, "leagueTabNameEntry");

    mainParam.allSearchParam = tabParam;


    if(button != NULL)
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(tabSearch), data);

    button = (GtkWidget *) gtk_builder_get_object(data->builder, "leagueTabAddButton");

    if(button != NULL)
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(openAddNewLeagueForm), data);




}

void initTeamTreeView(GtkWidget * parentBox, CallbackParam * data){
    printf("team view");
}

