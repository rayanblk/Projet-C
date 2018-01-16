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

CallbackParam * initAddNotebookTabButton(GtkBuilder * builder, char * parentName, char * objectName, char * objectLabel, char * fileName, GList ** list){
    CallbackParam * tempCallBackParam = NULL;

    tempCallBackParam = (CallbackParam *) malloc(sizeof(CallbackParam ));

    if(tempCallBackParam != NULL){
        tempCallBackParam->builder = builder;
        strcpy((char *) tempCallBackParam->parentName, parentName);
        strcpy((char *) tempCallBackParam->objectName, objectName);
        strcpy((char *) tempCallBackParam->objectLabel, objectLabel);
        strcpy((char *) tempCallBackParam->fileName, fileName);

        *list = g_list_append(*list, tempCallBackParam);

        return tempCallBackParam;
    }

    return NULL;

}

