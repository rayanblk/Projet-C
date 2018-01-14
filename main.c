#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "sqlite3.h"

void OnDestroy(GtkWidget *pWidget, gpointer pData);

int main(int argc,char **argv)
{
    /* SQL Widget */
    sqlite3 * db;
    char * errorMessage = NULL;
    int returnCode;
    sqlite3_stmt * query;

    /* Déclaration du widget */
    GtkWidget *pWindow;
    GtkBuilder * pBuidler = NULL;
    GError *pError = NULL;
    gtk_init(&argc,&argv);

    /* Création de la fenêtre */
    /*
    pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    /* Connexion du signal "destroy" */

    pBuidler = gtk_builder_new();

    if(pBuidler != NULL){
        gtk_builder_add_from_file(pBuidler, "mainWindow.glade", & pError);

        if(pError == NULL){
            pWindow = (GtkWidget * ) gtk_builder_get_object(pBuidler, "window1");

            g_signal_connect(G_OBJECT(pWindow), "destroy", G_CALLBACK(OnDestroy), NULL);
            /* Affichage de la fenêtre */
            gtk_widget_show_all(pWindow);

        }else{
            return 0;
        }

        gtk_main();
    }





    return 0;
}

void OnDestroy(GtkWidget *pWidget, gpointer pData)
{
    /* Arret de la boucle evenementielle */
    gtk_main_quit();
}