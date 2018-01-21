#include <stdio.h>
#include <gtk/gtk.h>
#include "database.h"
#include "callback.h"

int main(int argc,char **argv) {
    GtkWidget *mainWindow;
    GtkWidget *button;
    GtkBuilder *mainWindowBuilder;
    GError * errorMessage = NULL;
    GList * allNotebookParam = NULL;
    MainParam * mainParam = (MainParam *) malloc(sizeof(MainParam));
    const char * param[2];
    char * connInfo = "host=localhost port=5432 dbname=LeagueManager user=mathieu password=toshiba26";
    DatabaseConnector * mainDatabaseConnector = NULL;
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

        GtkCssProvider *cssProvider = gtk_css_provider_new();
        gtk_css_provider_load_from_path(cssProvider, "main.css", NULL);
        gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                       GTK_STYLE_PROVIDER(cssProvider),
                                       GTK_STYLE_PROVIDER_PRIORITY_USER);

        /*
         * Initialize database connection
         * Put on the main param structure the database instance
         * Put too, the database connection param
         */
        mainDatabaseConnector = connectToDatabase(connInfo);

        mainParam->databaseInfo = mainDatabaseConnector;

        /*
         * Init with initAddNotebookTabButton, all param needed by the button signal and save on GList
         * Get the League details button, via glade file
         * Connect the clicked action, to the add tab to notebook function, with the init structure
         */
        initAddNotebookTabButton(mainWindowBuilder, "mainNotebook", "leagueTab", "League", "mainTabWidget/leagueTabContent.glade", &allNotebookParam, mainParam);

        button = (GtkWidget *) gtk_builder_get_object(mainWindowBuilder, "homeTabLinkGridManageLeagueButton");

        if(button != NULL)
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(openNotebookTab), (gpointer) g_list_last(allNotebookParam)->data);

        /*
         * Init with initAddNotebookTabButton, all param needed by the button signal and save on GList
         * Get the team details button, via glade file
         * Connect the clicked action, to the add tab to notebook function, with the init structure
         */
        initAddNotebookTabButton(mainWindowBuilder, "mainNotebook", "teamTab", "Team", "mainTabWidget/teamTabContent.glade", &allNotebookParam, mainParam);

        button = (GtkWidget *) gtk_builder_get_object(mainWindowBuilder, "homeTabLinkGridManageTeamButton");

        if(button != NULL)
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(openNotebookTab), (gpointer) g_list_last(allNotebookParam)->data);

        /*
         * Init with initAddNotebookTabButton, all param needed by the button signal and save on GList
         * Get the player details button, via glade file
         * Connect the clicked action, to the add tab to notebook function, with the init structure
         */
        initAddNotebookTabButton(mainWindowBuilder, "mainNotebook", "playerTab", "Player", "mainTabWidget/playerTabContent.glade", &allNotebookParam, mainParam);

        button = (GtkWidget *) gtk_builder_get_object(mainWindowBuilder, "homeTabLinkGridManagePlayerButton");

        if(button != NULL)
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

        endConnectionToDatabase(mainDatabaseConnector);

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