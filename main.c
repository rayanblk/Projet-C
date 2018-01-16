#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include "sqlite3.h"
#include "callback.h"

int main(int argc,char **argv) {
    GtkWidget *mainWindow;
    GtkWidget *button;
    GtkBuilder *mainWindowBuilder;
    GError * errorMessage = NULL;
    GList * allNotebookParam = NULL;

    gtk_init(&argc, &argv);

    mainWindowBuilder = gtk_builder_new();

    gtk_builder_add_from_file(mainWindowBuilder, "mainWindow.glade", &errorMessage);

    if(errorMessage == NULL){
        /*
         * Get the main window, via the glade file
         */
        mainWindow = (GtkWidget *) gtk_builder_get_object(mainWindowBuilder, "mainWindow");
        gtk_window_maximize(GTK_WINDOW(mainWindow));

        g_signal_connect(mainWindow, "destroy", G_CALLBACK(destroyWindow), NULL);

        /*
         * Init with initAddNotebookTabButton, all param needed by the button signal and save on GList
         * Get the League details button, via glade file
         * Connect the clicked action, to the add tab to notebook function, with the init structure
         */
        initAddNotebookTabButton(mainWindowBuilder, "mainNotebook", "leagueTab", "League", "mainTabWidget/leagueTabContent.glade", &allNotebookParam);

        button = (GtkWidget *) gtk_builder_get_object(mainWindowBuilder, "homeTabLinkGridManageLeagueButton");

        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(openNotebookTab), (gpointer) g_list_last(allNotebookParam)->data);

        /*
         * Init with initAddNotebookTabButton, all param needed by the button signal and save on GList
         * Get the team details button, via glade file
         * Connect the clicked action, to the add tab to notebook function, with the init structure
         */
        initAddNotebookTabButton(mainWindowBuilder, "mainNotebook", "teamTab", "Team", "mainTabWidget/teamTabContent.glade", &allNotebookParam);

        button = (GtkWidget *) gtk_builder_get_object(mainWindowBuilder, "homeTabLinkGridManageTeamButton");

        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(openNotebookTab), (gpointer) g_list_last(allNotebookParam)->data);

        /*
         * Init with initAddNotebookTabButton, all param needed by the button signal and save on GList
         * Get the player details button, via glade file
         * Connect the clicked action, to the add tab to notebook function, with the init structure
         */
        initAddNotebookTabButton(mainWindowBuilder, "mainNotebook", "playerTab", "Player", "mainTabWidget/playerTabContent.glade", &allNotebookParam);

        button = (GtkWidget *) gtk_builder_get_object(mainWindowBuilder, "homeTabLinkGridManagePlayerButton");

        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(openNotebookTab), (gpointer) g_list_last(allNotebookParam)->data);

        /*
         * Show all child widget of main Window
         */
        gtk_widget_show_all(mainWindow);

        /*
         * Enter in gtk main loop
         */
        gtk_main();

        /*
         * Free the GList of button
         */
        g_list_free(allNotebookParam);

        return 0;
    }else{
        /*
         * If builder add from file, failed,
         * Free the error variable
         * Stop the code execution
         */
        g_error_free(errorMessage);

        return 0;
    }
}