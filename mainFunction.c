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
CallbackParam * initAddNotebookTabButton(GtkBuilder * builder, char * parentName, char * objectName, char * objectLabel, char * fileName, GList ** list, MainParam * mainParam){
    CallbackParam * tempCallBackParam = NULL;

    tempCallBackParam = (CallbackParam *) malloc(sizeof(CallbackParam ));

    if(tempCallBackParam != NULL){
        tempCallBackParam->builder = builder;
        tempCallBackParam->mainParam = mainParam;
        strcpy((char *) tempCallBackParam->parentName, parentName);
        strcpy((char *) tempCallBackParam->objectName, objectName);
        strcpy((char *) tempCallBackParam->objectLabel, objectLabel);
        strcpy((char *) tempCallBackParam->fileName, fileName);

        *list = g_list_append(*list, tempCallBackParam);

        return tempCallBackParam;
    }

    return NULL;

}

void initTreeView(GtkWidget * parentBox, CallbackParam * data) {
    /*
     * Add the storage list
     * Make the database request
     * Create the tree view
     * Add database values to this tree view
     */
    GtkListStore *listStore = NULL;
    GtkWidget *treeView = NULL;
    GtkCellRenderer *cell = NULL;
    GtkTreeViewColumn *column = NULL;
    GtkTreeIter tempIter;
    QueryStatement * queryResult = NULL;
    const char * param[1];
    char *** finalData = NULL;

    listStore = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    param[0] = "5";

    if(data->mainParam->databaseInfo->error != 1){

        queryResult = query(data->mainParam->databaseInfo, "SELECT id, name, to_char(\"dateUpdate\", 'YYYY-MM-DD') AS date_update FROM \"League\"");

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


    treeView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(listStore));

    cell = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Id", cell, "text", 0, NULL);
    gtk_tree_view_column_set_expand(GTK_TREE_VIEW_COLUMN(column), TRUE);
    gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(column), 0.5);

    gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), column);

    cell = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Name", cell, "text", 1, NULL);
    gtk_tree_view_column_set_expand(GTK_TREE_VIEW_COLUMN(column), TRUE);
    gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(column), 0.5);

    gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), column);

    cell = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Date update", cell, "text", 2, NULL);
    gtk_tree_view_column_set_expand(GTK_TREE_VIEW_COLUMN(column), TRUE);
    gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(column), 0.5);

    gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), column);

    gtk_box_pack_start(GTK_BOX(parentBox), treeView, TRUE, TRUE, 6);

    gtk_widget_show_all(parentBox);
}


