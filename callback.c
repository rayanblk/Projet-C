//
// Created by mathi on 15/01/2018.
//
#include "callback.h"


void destroyWindow(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

void openNotebookTab(GtkWidget *widget, gpointer *data) {
    GtkWidget *mainNotebook;
    GtkWidget *mainNotebookContent = NULL;
    GtkWidget *box;
    GtkWidget *buttonClose;
    GtkWidget *image;
    GtkBuilder *tabBuilder;
    GError *errorMessage = NULL;
    void (*pointer)(GtkWidget *, CallbackParam *);

/*
 * Get the structure, pass on param
 */
    CallbackParam *functionCallbackParam = (CallbackParam *) data;


    pointer = functionCallbackParam->function;


/*
 * With the builder in the structure, find the main Notebook main
 */
    mainNotebook = (GtkWidget *) gtk_builder_get_object(functionCallbackParam->builder,
                                                        (char *) functionCallbackParam->parentName);

/*
 * If the tab, already exist, don't create, but open it
 */
    if ((mainNotebookContent = findChild(mainNotebook, (char *) functionCallbackParam->objectName)) == NULL) {

        /*
         * Load the glade file
         * If error, exit, don't create the notebook
         * Else, create it
         */
        errorMessage = loadGladeFile(&tabBuilder, (char *) functionCallbackParam->fileName);

        if (errorMessage) {
            printf("%s \n", errorMessage->message);
            g_error_free(errorMessage);

            return;
        } else {

            /*
             * Create a new box, to put label and leave button
             * Put first, the label, with the name in the structure received
             */
            box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
            gtk_box_pack_start(GTK_BOX(box), gtk_label_new((char *) functionCallbackParam->objectLabel), TRUE, TRUE, 0);

            /*
             * Create the button from nothing
             * Load, the image, via pixbuf function, to set the size and ratio
             * Put this image on the button
             * Add the button to the box
             */
            buttonClose = gtk_button_new();
            image = gtk_image_new_from_pixbuf(gdk_pixbuf_new_from_file_at_scale("image/cross.png", 8, 8, TRUE, NULL));
            gtk_button_set_image(GTK_BUTTON(buttonClose), image);

            g_signal_connect(G_OBJECT(buttonClose), "clicked", G_CALLBACK(closeNotebookTab), (gpointer) data);

            gtk_box_pack_start(GTK_BOX(box), buttonClose, TRUE, TRUE, 0);


            /*
             * On the new glade file, get the main container, thanks to his name
             * This name come from the structure receive
             * Set name widget, with the name above
             *
             */
            mainNotebookContent = (GtkWidget *) gtk_builder_get_object(tabBuilder,
                                                                       (char *) functionCallbackParam->objectName);

            gtk_widget_set_name(mainNotebookContent, (char *) functionCallbackParam->objectName);

            /*
             * Add the new tab
             * The content is in mainNotebookContent
             * The label box is in box
             * Set latest tab, as current page
             */
            gtk_notebook_append_page(GTK_NOTEBOOK(mainNotebook), mainNotebookContent, box);

            gtk_notebook_set_current_page(GTK_NOTEBOOK(mainNotebook), -1);

            //g_signal_connect(gtk_builder_get_object(tabBuilder, "nameEntry"), "activate", G_CALLBACK(test), NULL);


            if (pointer != NULL) {
                CallbackParam *test = (CallbackParam *) malloc(sizeof(CallbackParam));
                test->mainParam = functionCallbackParam->mainParam;
                test->function = functionCallbackParam->function;
                strcpy((char *) test->objectName, (char *) functionCallbackParam->objectName);
                strcpy((char *) test->parentName, (char *) functionCallbackParam->parentName);
                strcpy((char *) test->objectLabel, (char *) functionCallbackParam->objectLabel);
                strcpy((char *) test->fileName, (char *) functionCallbackParam->fileName);
                test->builder = tabBuilder;
                (*pointer)(mainNotebookContent, test);


            }


            /*
             * Display all the widget contained in the box label
             */
            gtk_widget_show_all(box);
        }

    } else {
        /*
         * If the tab already exists, get the page number of the existing tab
         * Go to this tab
         */
        gtk_notebook_set_current_page(GTK_NOTEBOOK(mainNotebook),
                                      gtk_notebook_page_num(GTK_NOTEBOOK(mainNotebook), mainNotebookContent));
    }

}

void closeNotebookTab(GtkWidget *widget, gpointer *data) {
    GtkWidget *mainNotebook;
    GtkWidget *mainNotebookContent;
    CallbackParam *functionCallbackParam = (CallbackParam *) data;

/*
 * Get the notebook widget, via the builder
 */
    mainNotebook = (GtkWidget *) gtk_builder_get_object(functionCallbackParam->builder,
                                                        (char *) functionCallbackParam->parentName);

    mainNotebookContent = findChild(mainNotebook, (char *) functionCallbackParam->objectName);

    if (mainNotebookContent != NULL) {
        gtk_notebook_remove_page(GTK_NOTEBOOK(mainNotebook),
                                 gtk_notebook_page_num(GTK_NOTEBOOK(mainNotebook), mainNotebookContent));
    }

}

void tabSearch(GtkWidget *widget, gpointer *data) {
//Receive a structure array
    TabSearch *tabMainParam = (TabSearch *) data;
    TabSearchParam **allSearchTabParam;

    PrepareStatement *query = NULL;
    QueryStatement *queryResult = NULL;
    char ***finalData = NULL;

    GList *allCondition = NULL;
    GList *allValue = NULL;
    GList *l;

    GtkListStore *listStore = NULL;
    GtkTreeIter iter;

    GtkWidget *tempWidget = NULL;

    GString *finalStatement;
    GString *valueString;
    GString *conditionString;

    int i;

    if (tabMainParam != NULL) {
        /*
         * Get all search param receive
         */
        allSearchTabParam = tabMainParam->allSearchParam;

        /*
         * Travel all the GTK entry to check if a value is enter
         */
        for (i = 0; i < tabMainParam->numberOfParam; ++i) {
            /*
             * Get the entry via the builder
             * Initialize the string with entry value
             */
            tempWidget = (GtkWidget *) gtk_builder_get_object(tabMainParam->builder, allSearchTabParam[i]->gtkEntryId);
            valueString = g_string_new(gtk_entry_get_text(GTK_ENTRY(tempWidget)));


            if (strlen(valueString->str) > 0) {
                /*
                 * Initialize the condition string
                 * Add this condition to the condition list
                 */
                conditionString = g_string_new(allSearchTabParam[i]->condition);
                g_string_append_printf(conditionString, "%d", g_list_length(allCondition) + 1);
                allCondition = g_list_prepend(allCondition, (gpointer) conditionString->str);

                /*
                 * If want to do a like
                 * type condition == 1
                 * Add the % after and before the param
                 * type condition == 2
                 * Add the % after
                 * type condition == 3
                 * Add the % before
                 */
                if (allSearchTabParam[i]->typeCondition == 1) {
                    valueString = g_string_prepend(valueString, "%");
                    valueString = g_string_append(valueString, "%");
                } else if (allSearchTabParam[i]->typeCondition == 2) {
                    valueString = g_string_append(valueString, "%");
                } else if (allSearchTabParam[i]->typeCondition == 3) {
                    valueString = g_string_prepend(valueString, "%");
                }

                /*
                 * Add the value to value list
                 */
                allValue = g_list_prepend(allValue, (gpointer) valueString->str);

            } else {
                /*
                 * If no text is enter, free the string
                 */
                g_string_free(valueString, TRUE);
            }
        }
        /*
         * Put the receive statement on a GString to easily manipulate it
         */
        finalStatement = g_string_new(tabMainParam->statement);

        /*
         * if the condition list is not empty, add the WHERE key word
         */
        if (g_list_length(allCondition) > 0 && g_list_length(allValue) > 0) {
            finalStatement = g_string_append(finalStatement, " WHERE ");
        }

        i = 0;

        /*
         * Browse all the condition list
         * Add to the statement condition
         * Add or no the END keyword
         */

        for (l = allCondition; l != NULL; l = l->next, i++) {
            if (i != 0)
                finalStatement = g_string_append(finalStatement, " AND ");

            finalStatement = g_string_append(finalStatement, (gchar *) l->data);
        }


        if(tabMainParam->idTypeCondition == 1){
            if(g_list_length(allCondition) > 0 && g_list_length(allValue) > 0){
                finalStatement = g_string_append(finalStatement, " AND ");
            }else{
                finalStatement = g_string_append(finalStatement, " WHERE ");
            }

            finalStatement = g_string_append(finalStatement, tabMainParam->idCondition);
            finalStatement = g_string_append(finalStatement, " = $");
            g_string_append_printf(finalStatement, "%d", g_list_length(allCondition) + 1);
        }

        /*
         * Prepare the query with the statement prepare previously
         */
        query = prepareQuery(tabMainParam->mainParam->databaseInfo, finalStatement->str);

        i = 0;

        /*
         * Bind all the param to the statement
         */
        for (l = allValue; l != NULL; l = l->next, i++) {
            bindParam(query, (gchar *) l->data, i);
        }

        if(tabMainParam->idTypeCondition == 1){
            bindParam(query, tabMainParam->id, g_list_length(allCondition) + 1);
        }

        /*
         * Execute the statement
         * Fetch all the result
         */
        queryResult = executePrepareStatement(query);

        fetchAllResult(queryResult, &finalData);

        /*
         * Get the list store
         * Clear the list
         * Add the new result to the list
         */
        listStore = (GtkListStore *) gtk_builder_get_object(tabMainParam->builder, tabMainParam->listStoreId);

        if (listStore != NULL) {
            gtk_list_store_clear(listStore);

            for (int k = 0; k < queryResult->numberOfrow; ++k) {
                gtk_list_store_append(listStore, &iter);
                for (int m = 0; m < queryResult->numberOfcolumn; ++m) {
                    gtk_list_store_set(listStore, &iter, m, finalData[k][m], -1);
                }
            }
        }

        /*
         * Free all the list use
         * Free all the string use
         * Free the statement memory
         */
        g_list_free(allCondition);
        g_list_free(allValue);

        g_string_free(finalStatement, TRUE);

        closePrepareStatement(query, queryResult, finalData);

    }

}


void openAddNewLeagueForm(GtkWidget *widget, gpointer *data) {

    AllTabParam *mainParam = (AllTabParam *) data;
    GtkWidget *window;
    GtkBuilder *builder;
    GtkWidget *button;
    GError *error = NULL;


    error = loadGladeFile(&builder, "formWidget/newLeagueForm.glade");

    if (error != NULL)
        printf("%s \n", error->message);

    mainParam->builder = builder;

    window = (GtkWidget *) gtk_builder_get_object(builder, "leagueAddForm");

    button = (GtkWidget *) gtk_builder_get_object(builder, "leagueAddFormCloseButton");

    if (button != NULL)
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(closeDialogBox), window);


    button = (GtkWidget *) gtk_builder_get_object(builder, "leagueAddFormCreateButton");


    if (button != NULL)
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(createNewLeague), (gpointer *) mainParam);


    gtk_widget_show_all(window);

}

void openAddNewTeamForm(GtkWidget *widget, gpointer *data) {

    AllTabParam *mainParam = (AllTabParam *) data;
    GtkWidget *window;
    GtkBuilder *builder;
    GtkWidget *button;
    GtkListStore *listStore;
    QueryStatement *exec;
    char ***finalData = NULL;
    GtkTreeIter tempIter;

    loadGladeFile(&builder, "formWidget/newTeamForm.glade");

    mainParam->builder = builder;

    window = (GtkWidget *) gtk_builder_get_object(builder, "teamAddForm");

    button = (GtkWidget *) gtk_builder_get_object(builder, "teamAddFormCloseButton");

    if (button != NULL)
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(closeDialogBox), window);


    button = (GtkWidget *) gtk_builder_get_object(builder, "teamAddFormCreateButton");


    if (button != NULL)
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(createNewTeam), mainParam);


    exec = query(mainParam->centralParam->databaseInfo, "SELECT\n"
            "id,\n"
            "      name\n"
            "FROM \"League\";");


    fetchAllResult(exec, &finalData);

    listStore = (GtkListStore *) gtk_builder_get_object(builder, "leagueStore");

    for (int i = 0; i < exec->numberOfrow; ++i) {

        gtk_list_store_append(listStore, &tempIter);
        gtk_list_store_set(listStore, &tempIter, 0, finalData[i][0], 1, finalData[i][1], -1);
    }


    gtk_widget_show_all(window);
}

void openAddNewPlayerForm(GtkWidget *widget, gpointer *data) {

    AllTabParam *mainParam = (AllTabParam *) data;
    GtkWidget *window;
    GtkBuilder *builder;
    GtkWidget *button;
    GtkListStore *listStorePosition;
    GtkListStore *listStoreTeam;
    QueryStatement *exec;
    char ***finalDataPosition = NULL;
    char ***finalDataTeam = NULL;
    GtkTreeIter tempIter;

    loadGladeFile(&builder, "formWidget/newPlayerForm.glade");

    mainParam->builder = builder;

    window = (GtkWidget *) gtk_builder_get_object(builder, "playerAddForm");

    button = (GtkWidget *) gtk_builder_get_object(builder, "playerAddFormCloseButton");

    if (button != NULL)
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(closeDialogBox), window);


    button = (GtkWidget *) gtk_builder_get_object(builder, "playerAddFormCreateButton");

    if (button != NULL)
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(createNewPlayer), mainParam);


    exec = query(mainParam->centralParam->databaseInfo, "SELECT\n"

            "id,\n"
            "      name\n"
            "FROM \"Position\";");

    fetchAllResult(exec, &finalDataPosition);

    listStorePosition = (GtkListStore *) gtk_builder_get_object(builder, "positionStore");


    for (int i = 0; i < exec->numberOfrow; ++i) {

        gtk_list_store_append(listStorePosition, &tempIter);
        gtk_list_store_set(listStorePosition, &tempIter, 0, finalDataPosition[i][0], 1, finalDataPosition[i][1], -1);
    }

    exec = query(mainParam->centralParam->databaseInfo, "SELECT\n"
            "id,\n"
            "      name\n"
            "FROM \"Team\";");

    fetchAllResult(exec, &finalDataTeam);

    listStoreTeam = (GtkListStore *) gtk_builder_get_object(builder, "teamStore");

    for (int i = 0; i < exec->numberOfrow; ++i) {

        gtk_list_store_append(listStoreTeam, &tempIter);
        gtk_list_store_set(listStoreTeam, &tempIter, 0, finalDataTeam[i][0], 1, finalDataTeam[i][1], -1);
    }


    gtk_widget_show_all(window);
}

void openAddNewStrikerForm(GtkWidget *widget, gpointer *data) {

    AllTabParam *mainParam = (AllTabParam *) data;
    GtkWidget *window;
    GtkBuilder *builder;
    GtkWidget *button;
    GtkListStore *listStoreStriker;
    GtkListStore *listStoreTeam;
    QueryStatement *exec;
    PrepareStatement *query;
    char ***finalDataStriker = NULL;
    char ***finalDataTeam = NULL;
    GtkTreeIter tempIter;

    loadGladeFile(&builder, "formWidget/newStriker.glade");

    mainParam->builder = builder;

    window = (GtkWidget *) gtk_builder_get_object(builder, "strikerAddForm");

    button = (GtkWidget *) gtk_builder_get_object(builder, "strikerAddFormCloseButton");

    if (button != NULL)
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(closeDialogBox), window);


       button = (GtkWidget *) gtk_builder_get_object(builder, "strikerAddFormCreateButton");

       if (button != NULL)
           g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(createNewStriker), mainParam);


    query = prepareQuery(mainParam->centralParam->databaseInfo, "SELECT\n"
            "  \"Player\".id,\n"
            "  \"Player\".firstname || ' ' || \"Player\".lastname AS \"fullName\"\n"
            "FROM \"Match\", \"Player\"\n"
            "WHERE \"Match\".\"id\" = $1\n"
            "AND (\"Player\".\"idTeam\" = \"Match\".\"outsideTeam\" OR \"Player\".\"idTeam\" = \"Match\".\"homeTeam\")");

    bindParam(query, mainParam->searchParam->id, 0);
    exec = executePrepareStatement(query);

    fetchAllResult(exec, &finalDataStriker);

    listStoreStriker = (GtkListStore *) gtk_builder_get_object(builder, "strikerStore");


    for (int i = 0; i < exec->numberOfrow; ++i) {

        gtk_list_store_append(listStoreStriker, &tempIter);
        gtk_list_store_set(listStoreStriker, &tempIter, 0, finalDataStriker[i][0], 1, finalDataStriker[i][1], -1);
    }

    closePrepareStatement(query, exec, finalDataStriker);

    query = prepareQuery(mainParam->centralParam->databaseInfo, "SELECT\n"
            "  \"Team\".id,\n"
            "  \"Team\".name\n"
            "FROM \"Match\", \"Team\"\n"
            "WHERE \"Match\".id = $1\n"
            "AND (\"Team\".id = \"Match\".\"outsideTeam\" OR \"Team\".id = \"Match\".\"homeTeam\")");

    bindParam(query, mainParam->searchParam->id, 0);
    exec = executePrepareStatement(query);

    fetchAllResult(exec, &finalDataTeam);

    listStoreTeam = (GtkListStore *) gtk_builder_get_object(builder, "teamStore");

    for (int i = 0; i < exec->numberOfrow; ++i) {

        gtk_list_store_append(listStoreTeam, &tempIter);
        gtk_list_store_set(listStoreTeam, &tempIter, 0, finalDataTeam[i][0], 1, finalDataTeam[i][1], -1);
    }


    gtk_widget_show_all(window);
}


void closeDialogBox(GtkWidget *widget, gpointer *data) {
    GtkWidget *window = (GtkWidget *) data;

    gtk_widget_destroy(window);


}

void createNewLeague(GtkWidget *widget, gpointer *data) {
    AllTabParam *allParam = (AllTabParam *) data;
    GtkWidget *allEntry;
    GtkWidget *currentBox;
    char *param = 0;
    GtkWidget *label;
    char *statement;
    PrepareStatement *query;
    QueryStatement *queryResult;


    allEntry = (GtkWidget *) gtk_builder_get_object(allParam->builder, "leagueAddFormNameEntry");

    currentBox = (GtkWidget *) gtk_builder_get_object(allParam->builder, "leagueAddFormBox");


    if (strlen(param = (char *) gtk_entry_get_text(GTK_ENTRY(allEntry))) <= 0) {


        if ((label = findChild(currentBox, "errorLabelName")) == NULL) {
            label = gtk_label_new("Verify league's name");
            gtk_widget_set_name(label, "errorLabelName");
            gtk_box_pack_start(GTK_BOX(currentBox), label, TRUE, TRUE, 2);
        } else {
            gtk_label_set_label(GTK_LABEL(label), "Verify league's name");
        }

        gtk_widget_show_all(currentBox);

    } else {
        if ((label = findChild(currentBox, "errorLabelName")) != NULL) {
            gtk_widget_destroy(label);
        }


        statement = "INSERT INTO  \"League\" (name) VALUES ($1) ";
        query = prepareQuery(allParam->centralParam->databaseInfo, statement);
        bindParam(query, param, 0);
        queryResult = executePrepareStatement(query);


        if (queryResult->error == 1) {
            if ((label = findChild(currentBox, "errorLabelName")) == NULL) {
                label = gtk_label_new("Cannot insert in database");
                gtk_widget_set_name(label, "errorLabelName");

                gtk_box_pack_start(GTK_BOX(currentBox), label, TRUE, TRUE, 2);
            } else {
                gtk_label_set_label(GTK_LABEL(label), "Cannot insert in database");
            }

            gtk_widget_show_all(currentBox);

        } else {
            gtk_widget_destroy((GtkWidget *) gtk_builder_get_object(allParam->builder, "leagueAddForm"));

            tabSearch(widget, (gpointer *) allParam->searchParam);

        }
        closePrepareStatement(query, queryResult, NULL);
    }
}

void createNewTeam(GtkWidget *widget, gpointer *data) {
    AllTabParam *allParam = (AllTabParam *) data;
    GtkWidget *allEntry[4];
    GtkWidget *currentBox;
    char *param[4] = {0};
    GtkWidget *label;
    char *statement;
    PrepareStatement *query;
    QueryStatement *queryResult;
    char errorMessage[1000] = {0};


    allEntry[0] = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamAddFormNameEntry");
    allEntry[1] = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamAddFormCityEntry");
    allEntry[2] = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamAddFormStadiumEntry");
    allEntry[3] = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamAddFormLeagueComboBox");


    currentBox = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamAddFormBox");


    if (strlen(param[0] = (char *) gtk_entry_get_text(GTK_ENTRY(allEntry[0]))) <= 0)
        strcat(errorMessage, "Verify team's name \n");

    if (strlen(param[1] = (char *) gtk_entry_get_text(GTK_ENTRY(allEntry[1]))) <= 0)
        strcat(errorMessage, "Verify team's city \n");

    if (strlen(param[2] = (char *) gtk_entry_get_text(GTK_ENTRY(allEntry[2]))) <= 0)
        strcat(errorMessage, "Verify team's stadium \n");


    if ((param[3] = (char *) gtk_combo_box_get_active_id(GTK_COMBO_BOX(allEntry[3]))) == NULL || strlen(param[3]) <= 0)
        strcat(errorMessage, "Verify team's league \n");


    if (strlen(errorMessage) > 0) {


        if ((label = findChild(currentBox, "errorLabelName")) == NULL) {
            label = gtk_label_new(errorMessage);
            gtk_widget_set_name(label, "errorLabelName");
            gtk_box_pack_start(GTK_BOX(currentBox), label, TRUE, TRUE, 2);
        } else {
            gtk_label_set_label(GTK_LABEL(label), errorMessage);
        }

        gtk_widget_show_all(currentBox);

    } else {
        if ((label = findChild(currentBox, "errorLabelName")) != NULL) {
            gtk_widget_destroy(label);
        }


        statement = "INSERT INTO  \"Team\" (name,city,stadium,\"idLeague\") VALUES ($1,$2,$3,$4) ";
        query = prepareQuery(allParam->centralParam->databaseInfo, statement);
        bindParam(query, param[0], 0);
        bindParam(query, param[1], 1);
        bindParam(query, param[2], 2);
        bindParam(query, param[3], 3);
        queryResult = executePrepareStatement(query);


        if (queryResult->error == 1) {
            if ((label = findChild(currentBox, "errorLabelName")) == NULL) {
                label = gtk_label_new("Cannot insert in database");
                gtk_widget_set_name(label, "errorLabelName");

                gtk_box_pack_start(GTK_BOX(currentBox), label, TRUE, TRUE, 2);
            } else {
                gtk_label_set_label(GTK_LABEL(label), "Cannot insert in database");
            }

            gtk_widget_show_all(currentBox);

        } else {
            gtk_widget_destroy((GtkWidget *) gtk_builder_get_object(allParam->builder, "teamAddForm"));

            tabSearch(widget, (gpointer *) allParam->searchParam);

        }
        closePrepareStatement(query, queryResult, NULL);
    }
}

void createNewPlayer(GtkWidget *widget, gpointer *data) {
    AllTabParam *allParam = (AllTabParam *) data;
    GtkWidget *allEntry[4];
    GtkWidget *currentBox;
    char *param[4] = {0};
    GtkWidget *label;
    char *statement;
    PrepareStatement *query;
    QueryStatement *queryResult;
    char errorMessage[1000] = {0};


    allEntry[0] = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerAddFormFirstnameEntry");
    allEntry[1] = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerAddFormLastnameEntry");
    allEntry[2] = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerAddFormTeamComboBox");
    allEntry[3] = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerAddFormPositionComboBox");

    currentBox = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerAddFormBox");


    if (strlen(param[0] = (char *) gtk_entry_get_text(GTK_ENTRY(allEntry[0]))) <= 0)
        strcat(errorMessage, "Verify player's firstname \n");

    if (strlen(param[1] = (char *) gtk_entry_get_text(GTK_ENTRY(allEntry[1]))) <= 0)
        strcat(errorMessage, "Verify player's lastname \n");

    if ((param[2] = (char *) gtk_combo_box_get_active_id(GTK_COMBO_BOX(allEntry[2]))) == NULL || strlen(param[2]) <= 0)
        strcat(errorMessage, "Verify player's team \n");


    if ((param[3] = (char *) gtk_combo_box_get_active_id(GTK_COMBO_BOX(allEntry[3]))) == NULL || strlen(param[3]) <= 0)
        strcat(errorMessage, "Verify player's position \n");


    if (strlen(errorMessage) > 0) {


        if ((label = findChild(currentBox, "errorLabelName")) == NULL) {
            label = gtk_label_new(errorMessage);
            gtk_widget_set_name(label, "errorLabelName");
            gtk_box_pack_start(GTK_BOX(currentBox), label, TRUE, TRUE, 2);
        } else {
            gtk_label_set_label(GTK_LABEL(label), errorMessage);
        }

        gtk_widget_show_all(currentBox);

    } else {
        if ((label = findChild(currentBox, "errorLabelName")) != NULL) {
            gtk_widget_destroy(label);
        }


        statement = "INSERT INTO  \"Player\" (firstname,lastname,\"idTeam\",\"idPosition\") VALUES ($1,$2,$3,$4) ";
        query = prepareQuery(allParam->centralParam->databaseInfo, statement);
        bindParam(query, param[0], 0);
        bindParam(query, param[1], 1);
        bindParam(query, param[2], 2);
        bindParam(query, param[3], 3);
        queryResult = executePrepareStatement(query);


        if (queryResult->error == 1) {
            if ((label = findChild(currentBox, "errorLabelName")) == NULL) {
                label = gtk_label_new("Cannot insert in database");
                gtk_widget_set_name(label, "errorLabelName");

                gtk_box_pack_start(GTK_BOX(currentBox), label, TRUE, TRUE, 2);
            } else {
                gtk_label_set_label(GTK_LABEL(label), "Cannot insert in database");
            }

            gtk_widget_show_all(currentBox);

        } else {
            gtk_widget_destroy((GtkWidget *) gtk_builder_get_object(allParam->builder, "playerAddForm"));

            tabSearch(widget, (gpointer *) allParam->searchParam);

        }
        closePrepareStatement(query, queryResult, NULL);
    }
}

void createNewStriker(GtkWidget *widget, gpointer *data) {
    AllTabParam *allParam = (AllTabParam *) data;
    GtkWidget *allEntry[3];
    GtkWidget *currentBox;
    char *param[3] = {0};
    GtkWidget *label;
    char *statement;
    char **finalData;
    PrepareStatement *query = NULL;
    QueryStatement *queryResult = NULL;
    char errorMessage[1000] = {0};

    allEntry[0] = (GtkWidget *) gtk_builder_get_object(allParam->builder, "strikerNameComboBox1");
    allEntry[1] = (GtkWidget *) gtk_builder_get_object(allParam->builder, "strikerTeamComboBox");
    allEntry[2] = (GtkWidget *) gtk_builder_get_object(allParam->builder, "strikerTimeEntry");

    currentBox = (GtkWidget *) gtk_builder_get_object(allParam->builder, "strikerAddFormBox");

    if ((param[0] = (char *) gtk_combo_box_get_active_id(GTK_COMBO_BOX(allEntry[0]))) == NULL || strlen(param[0]) <= 0)
        strcat(errorMessage, "Verify striker's name \n");

    if ((param[1] = (char *) gtk_combo_box_get_active_id(GTK_COMBO_BOX(allEntry[1]))) == NULL || strlen(param[1]) <= 0)
        strcat(errorMessage, "Verify striker's team \n");

    if (strlen(param[2] = (char *) gtk_entry_get_text(GTK_ENTRY(allEntry[2]))) <= 0)
        strcat(errorMessage, "Verify time of the goal \n");

    if (strlen(errorMessage) > 0) {


        if ((label = findChild(currentBox, "errorLabelName")) == NULL) {
            label = gtk_label_new(errorMessage);
            gtk_widget_set_name(label, "errorLabelName");
            gtk_box_pack_start(GTK_BOX(currentBox), label, TRUE, TRUE, 2);
        } else {
            gtk_label_set_label(GTK_LABEL(label), errorMessage);
        }

        gtk_widget_show_all(currentBox);

    } else {
        if ((label = findChild(currentBox, "errorLabelName")) != NULL) {
            gtk_widget_destroy(label);
        }


        statement = "INSERT INTO \"Striker\"\n"
                "(\"idPlayer\",\"teamScorer\", time,\"idMatch\")\n"
                "VALUES ($1,$2,$3,$4)";
        query = prepareQuery(allParam->centralParam->databaseInfo, statement);
        bindParam(query, param[0], 0);
        bindParam(query, param[1], 1);
        bindParam(query, param[2], 2);
        bindParam(query, allParam->searchParam->id,3);
        queryResult = executePrepareStatement(query);


        if (queryResult->error == 1) {
            if ((label = findChild(currentBox, "errorLabelName")) == NULL) {
                label = gtk_label_new("Cannot insert in database");
                gtk_widget_set_name(label, "errorLabelName");

                gtk_box_pack_start(GTK_BOX(currentBox), label, TRUE, TRUE, 2);
            } else {
                gtk_label_set_label(GTK_LABEL(label), "Cannot insert in database");
            }

            gtk_widget_show_all(currentBox);

        } else {

            closePrepareStatement(query, queryResult, NULL);

            query = NULL;
            queryResult = NULL;

            query = prepareQuery(allParam->centralParam->databaseInfo,
                    "UPDATE \"Match\"\n"
                            "SET \"numberOfGoalHomeTeam\" =\n"
                            "                  CASE $1::INTEGER\n"
                            "                    WHEN \"homeTeam\" THEN \"numberOfGoalHomeTeam\" + 1\n"
                            "                    ELSE \"numberOfGoalHomeTeam\"\n"
                            "                  END\n"
                            ",\"numberOfGoalOutsideTeam\" =\n"
                            "                  CASE $2::INTEGER\n"
                            "                    WHEN \"outsideTeam\" THEN \"Match\".\"numberOfGoalOutsideTeam\" + 1\n"
                            "                    ELSE \"numberOfGoalOutsideTeam\"\n"
                            "                  END\n"
                            "WHERE id = $3");

            bindParam(query, param[1],0);
            bindParam(query, param[1],1);
            bindParam(query, allParam->searchParam->id,2);


            queryResult = executePrepareStatement(query);

            query = prepareQuery(allParam->centralParam->databaseInfo,"SELECT\n"
                    "  \"numberOfGoalHomeTeam\"|| '-' || \"numberOfGoalOutsideTeam\"\n"
                    "FROM \"Match\"\n"
                    "WHERE \"Match\".id = $1");

            bindParam(query,allParam->searchParam->id,0);
            queryResult = executePrepareStatement(query);

            fetchResult(queryResult,&finalData);

            label = (GtkWidget *)gtk_builder_get_object(allParam->searchParam->builder,"scoreMatchValue");

            if (label != NULL){
                if(queryResult->numberOfrow > 0) {
                    gtk_label_set_label(GTK_LABEL(label), finalData[0]);
                } else{
                    gtk_label_set_label(GTK_LABEL(label),"error");
                }
            }
            gtk_widget_destroy((GtkWidget *) gtk_builder_get_object(allParam->builder, "strikerAddForm"));

            tabSearch(widget, (gpointer *) allParam->searchParam);

        }
        closePrepareStatement(query, queryResult, NULL);


        /*
         * Refresh list store
         */
    }
}


void displayLeagueDetail(GtkTreeView *treeView, GtkTreePath *path, GtkTreeViewColumn *column, gpointer *data) {
    GtkTreeIter iter;
    GtkTreeModel *model;
    char *test;
    CallbackParam *allParam = (CallbackParam *) data;
    CallBackParamWithId *allParamWithId = (CallBackParamWithId *) malloc(sizeof(CallBackParamWithId));
    GError *error = NULL;
    GtkWidget *window = NULL;
    GtkWidget *tempWidget = NULL;
    PrepareStatement *query = NULL;
    QueryStatement *resultQuery = NULL;
    char **finalData = NULL;

    model = gtk_tree_view_get_model(treeView);
    gtk_tree_model_get_iter(model, &iter, path);


/*
 * Get the dialog box
 * Display the value
 */
    if ((error = loadGladeFile(&allParam->builder, "detailWidget/leagueDetail.glade")) == NULL) {

        window = (GtkWidget *) gtk_builder_get_object(allParam->builder, "leagueDetailWindow");

        if (window != NULL) {
            gtk_widget_show_all(window);

            tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "leagueDetailIdValue");
            gtk_tree_model_get(model, &iter, 0, &test, -1);
            gtk_label_set_label(GTK_LABEL(tempWidget), test);

            query = prepareQuery(allParam->mainParam->databaseInfo,
                                 "SELECT id, name, to_char(\"dateUpdate\", 'YYYY-MM-DD') AS \"dateCreate\", to_char(\"dateUpdate\", 'YYYY-MM-DD') AS \"dateUpdate\" FROM \"League\" WHERE id = $1");

            bindParam(query, test, 0);

            resultQuery = executePrepareStatement(query);

            if (resultQuery->error != 1) {

                fetchResult(resultQuery, &finalData);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "leagueDetailNameValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[1]);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "leagueDetailDateCreateValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[2]);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "leagueDetailDateUpdateValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[3]);
            }

            closePrepareStatement(query, resultQuery, (char ***) finalData);

            tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "leagueDetailCloseButton");

            if (tempWidget != NULL)
                g_signal_connect(G_OBJECT(tempWidget), "clicked", G_CALLBACK(closeDialogBox), (gpointer) window);

            tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "leagueDetailDetailButton");

            allParamWithId->allCalbackParam = allParam;
            strcpy(allParamWithId->id, test);

            if (tempWidget != NULL)
                g_signal_connect(G_OBJECT(tempWidget), "clicked", G_CALLBACK(openMoreDetailLeague),
                                 (gpointer) allParamWithId);

        }

    } else {
        printf("%s \n", error->message);
        g_error_free(error);
    }


}

void displayTeamDetail(GtkTreeView *treeView, GtkTreePath *path, GtkTreeViewColumn *column, gpointer *data) {
    GtkTreeIter iter;
    GtkTreeModel *model;
    char *test;
    CallbackParam *allParam = (CallbackParam *) data;
    GError *error = NULL;
    GtkWidget *window = NULL;
    GtkWidget *tempWidget = NULL;
    PrepareStatement *query = NULL;
    QueryStatement *resultQuery = NULL;
    char **finalData = NULL;

    model = gtk_tree_view_get_model(treeView);
    gtk_tree_model_get_iter(model, &iter, path);


/*
 * Get the dialog box
 * Display the value
 */
    if ((error = loadGladeFile(&allParam->builder, "detailWidget/teamDetail.glade")) == NULL) {

        window = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamDetailWindow");

        if (window != NULL) {
            gtk_widget_show_all(window);

            tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamDetailIdValue");
            gtk_tree_model_get(model, &iter, 0, &test, -1);
            gtk_label_set_label(GTK_LABEL(tempWidget), test);

            query = prepareQuery(allParam->mainParam->databaseInfo,
                                 "SELECT"
                                         "\"Team\".id,"
                                         "\"Team\".name,"
                                         "\"Team\".city,"
                                         "\"League\".Name AS \"leagueName\","
                                         "\"Team\".stadium,"
                                         "to_char(\"Team\".\"dateAdd\", 'YYYY-MM-DD') AS \"dateCreate\","
                                         "to_char(\"Team\".\"dateUpdate\", 'YYYY-MM-DD') AS \"dateUpdate\""
                                         "FROM \"Team\""
                                         "JOIN \"League\" ON \"Team\".\"idLeague\" = \"League\".id\n"
                                         "WHERE \"Team\".id = $1");

            bindParam(query, test, 0);

            resultQuery = executePrepareStatement(query);

            if (resultQuery->error != 1) {

                fetchResult(resultQuery, &finalData);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamDetailNameValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[1]);


                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamDetailCityValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[2]);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamDetailLeagueValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[3]);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamDetailStadiumValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[4]);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamDetailDateCreateValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[5]);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamDetailDateUpdateValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[6]);

                closePrepareStatement(query, resultQuery, (char ***) finalData);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamDetailCloseButton");

                if (tempWidget != NULL)
                    g_signal_connect(G_OBJECT(tempWidget), "clicked", G_CALLBACK(closeDialogBox), (gpointer) window);

            }
        }

    } else {
        printf("%s \n", error->message);
        g_error_free(error);
    }

}

void displayPlayerDetail(GtkTreeView *treeView, GtkTreePath *path, GtkTreeViewColumn *column, gpointer *data) {
    GtkTreeIter iter;
    GtkTreeModel *model;
    char *test;
    CallbackParam *allParam = (CallbackParam *) data;
    GError *error = NULL;
    GtkWidget *window = NULL;
    GtkWidget *tempWidget = NULL;
    PrepareStatement *query = NULL;
    QueryStatement *resultQuery = NULL;
    char **finalData = NULL;

    model = gtk_tree_view_get_model(treeView);
    gtk_tree_model_get_iter(model, &iter, path);


/*
 * Get the dialog box
 * Display the value
 */
    if ((error = loadGladeFile(&allParam->builder, "detailWidget/playerDetail.glade")) == NULL) {

        window = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerDetailWindow");


        if (window != NULL) {

            gtk_widget_show_all(window);
            tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerDetailIdValue");
            gtk_tree_model_get(model, &iter, 0, &test, -1);
            if (tempWidget != NULL)
                gtk_label_set_label(GTK_LABEL(tempWidget), test);

            query = prepareQuery(allParam->mainParam->databaseInfo,
                                 "SELECT\n"
                                         "\"Player\".id,\n"
                                         "\"Player\".firstname || ' ' || \"Player\".lastname AS Name,\n"
                                         "\"Team\".name as \"teamName\",\n"
                                         "\"League\".name as \"leagueName\",\n"
                                         "\"Position\".name as \"positionName\",\n"
                                         "to_char(\"Player\".\"dateAdd\", 'YYYY-MM-DD') AS \"dateAdd\",\n"
                                         "to_char(\"Player\".\"dateUpdate\", 'YYYY-MM-DD') AS \"dateUpdate\"\n"
                                         "FROM \"Player\"\n"
                                         "JOIN \"Position\" ON \"Player\".\"idPosition\" = \"Position\".id\n"
                                         "JOIN \"Team\" ON \"Player\".\"idTeam\" = \"Team\".id\n"
                                         "JOIN \"League\" ON \"Team\".\"idLeague\" = \"League\".id\n"
                                         "WHERE \"Player\".id = $1");

            bindParam(query, test, 0);

            resultQuery = executePrepareStatement(query);

            if (resultQuery->error != 1) {

                fetchResult(resultQuery, &finalData);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerDetailNameValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[1]);


                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerDetailTeamValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[2]);


                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerDetailLeagueValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[3]);


                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerDetailPositionValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[4]);


                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerDetailDateCreateValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[5]);


                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerDetailDateUpdateValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[6]);


                closePrepareStatement(query, resultQuery, (char ***) finalData);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerDetailCloseButton");

                if (tempWidget != NULL)
                    g_signal_connect(G_OBJECT(tempWidget), "clicked", G_CALLBACK(closeDialogBox), (gpointer) window);

            }
        }

    } else {
        printf("%s \n", error->message);
        g_error_free(error);
    }

}

void displayMatchDetail(GtkTreeView *treeView, GtkTreePath *path, GtkTreeViewColumn *column, gpointer *data) {
    GtkTreeIter iter;
    GtkTreeModel *model;
    GtkListStore *strikerListStore;
    TabSearch *mainParam = NULL;
    GtkBuilder *builder = NULL;
    AllTabParam *completeTabParam = NULL;
    char *test;
    char ***strikerData = NULL;
    CallbackParam *allParam = (CallbackParam *) data;
    GError *error = NULL;
    GtkWidget *window = NULL;
    GtkWidget *tempWidget = NULL;
    PrepareStatement *query = NULL;
    QueryStatement *resultQuery = NULL;
    char **finalData = NULL;
    char status[20] = {0};

    model = gtk_tree_view_get_model(treeView);
    gtk_tree_model_get_iter(model, &iter, path);

    mainParam = (TabSearch *) malloc(sizeof(TabSearch));
    completeTabParam = (AllTabParam *) malloc(sizeof(AllTabParam));

    if (mainParam != NULL) {
        strcpy(mainParam->statement, "SELECT\n"
                "  \"Striker\".id,\n"
                "  \"Player\".firstname || ' ' || \"Player\".lastname AS \"fullName\",\n"
                "  \"Team\".name,\n"
                "  \"Striker\".time\n"
                "FROM \"Striker\"\n"
                "JOIN \"Team\" ON \"Striker\".\"teamScorer\" = \"Team\".id\n"
                "JOIN \"Player\" ON \"Striker\".\"idPlayer\" = \"Player\".id\n");


        strcpy(mainParam->listStoreId, "matchListStore");
        mainParam->numberOfParam = 0;
        mainParam->allSearchParam = NULL;
        mainParam->mainParam = allParam->mainParam;
        strcpy(mainParam->idCondition,"\"Striker\".\"idMatch\"");
        mainParam->idTypeCondition = 1;
    }


    if (completeTabParam != NULL) {
        completeTabParam->builder = builder;
        completeTabParam->centralParam = allParam->mainParam;
        completeTabParam->searchParam = mainParam;
    }


    /*
     * Get the dialog box
     * Display the value
     */
    if ((error = loadGladeFile(&builder, "detailWidget/matchDetail.glade")) == NULL) {

        window = (GtkWidget *) gtk_builder_get_object(builder, "matchDetailWindow");

        mainParam->builder = builder;
        completeTabParam->builder = builder;

        if (window != NULL) {

            gtk_widget_show_all(window);
            gtk_tree_model_get(model, &iter, 0, &test, -1);

            query = prepareQuery(allParam->mainParam->databaseInfo, "SELECT\n"
                    "  \"HomeTeam\".\"name\",\n"
                    "  \"OutsideTeam\".\"name\",\n"
                    "  \"numberOfGoalHomeTeam\"|| '-' || \"numberOfGoalOutsideTeam\",\n"
                    "  \"status\",\n"
                    "  to_char(\"date\",'DD:MM:YYYY'),\n"
                    "  \"Match\".stadium\n"
                    "FROM \"Match\"\n"
                    "JOIN \"Team\" AS \"HomeTeam\" ON \"HomeTeam\".id = \"Match\".\"homeTeam\"\n"
                    "JOIN \"Team\" AS \"OutsideTeam\" ON \"OutsideTeam\".id = \"Match\".\"outsideTeam\"\n"
                    "WHERE \"Match\".id = $1");

            bindParam(query, test, 0);
            resultQuery = executePrepareStatement(query);

            if (resultQuery->error != 1) {

                fetchResult(resultQuery, &finalData);
                strcpy(status,finalData[3]);

                tempWidget = (GtkWidget *) gtk_builder_get_object(builder, "homeTeamLabel");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[0]);

                tempWidget = (GtkWidget *) gtk_builder_get_object(builder, "outsideTeamLabel");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[1]);

                tempWidget = (GtkWidget *) gtk_builder_get_object(builder, "scoreMatchValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[2]);

                tempWidget = (GtkWidget *) gtk_builder_get_object(builder, "statusValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), getMatchStatus(finalData[3]));

                tempWidget = (GtkWidget *) gtk_builder_get_object(builder, "dateValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[4]);

                tempWidget = (GtkWidget *) gtk_builder_get_object(builder, "stadiumValue");

                if (tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[5]);
            }

            closePrepareStatement(query, resultQuery, &finalData);

            query = prepareQuery(allParam->mainParam->databaseInfo,
                                 "SELECT\n"
                                         "  \"Striker\".id,\n"
                                         "  \"Player\".firstname || ' ' || \"Player\".lastname AS \"fullName\",\n"
                                         "  \"Team\".name,\n"
                                         "  \"Striker\".time\n"
                                         "FROM \"Striker\"\n"
                                         "JOIN \"Team\" ON \"Striker\".\"teamScorer\" = \"Team\".id\n"
                                         "JOIN \"Player\" ON \"Striker\".\"idPlayer\" = \"Player\".id\n"
                                         "WHERE \"Striker\".\"idMatch\" = $1");

            bindParam(query, test, 0);
            resultQuery = executePrepareStatement(query);
            fetchAllResult(resultQuery, &strikerData);

            strikerListStore = (GtkListStore *) gtk_builder_get_object(builder, "matchListStore");
            for (int i = 0; i < resultQuery->numberOfrow; ++i) {

                gtk_list_store_append(strikerListStore, &iter);
                gtk_list_store_set(strikerListStore, &iter, 0, strikerData[i][0],
                                                            1, strikerData[i][1],
                                                            2, strikerData[i][2],
                                                            3, strikerData[i][3], -1);
            }

            tempWidget = (GtkWidget *) gtk_builder_get_object(builder, "newScorerButton");

            strcpy(mainParam->id, test);

            if (tempWidget != NULL)
                g_signal_connect(G_OBJECT(tempWidget), "clicked", G_CALLBACK(openAddNewStrikerForm), (gpointer*) completeTabParam);

            tempWidget = (GtkWidget *) gtk_builder_get_object(builder,"changeStatusButton");

            if(tempWidget != NULL){
                if(strcmp(status,"1") == 0 )
                    gtk_widget_set_sensitive(tempWidget,FALSE);
                g_signal_connect(G_OBJECT(tempWidget), "clicked", G_CALLBACK(updateStatusMatch),(gpointer*) completeTabParam);

        }

        } else {
            printf("%s \n", error->message);
            g_error_free(error);
        }

    }
}

void searchArticle(GtkWidget *widget, gpointer *data){
    AllTabParam * mainParam = (AllTabParam *) data;
    GtkWidget * listStore = NULL;
    GtkWidget * tempWidget = NULL;
    char * research = NULL;
    GtkTreeIter tempIter;
    GString *param = NULL, *statement = NULL;
    PrepareStatement * query = NULL;
    QueryStatement * resultQuery = NULL;
    char * buffer, * title;
    char *** finalData = NULL;
    int temp = 0;
    long cursor = 0;
    int i = 1;
    GList * existHref = NULL;

    listStore = (GtkWidget *) gtk_builder_get_object(mainParam->builder, "newsListStore");

    tempWidget = (GtkWidget *) gtk_builder_get_object(mainParam->builder, "themeNewsEntry");

    param = g_string_new("");
    statement = g_string_new("");

    if(tempWidget != NULL)
        research = (char *) gtk_entry_get_text(GTK_ENTRY(tempWidget));


    query = prepareQuery(mainParam->centralParam->databaseInfo, "");

    statement = g_string_append(statement,
                                "INSERT INTO \"Article\"(url, title, content) \n"
                                "    VALUES ");

    getHtml("testx.html","http://www.footmercato.net");

    getURL("testx.html","hrefs.html");


    while((temp = browser("hrefs.html",research,"http://www.footmercato.net/",&cursor,&existHref)) == 0){
        getArticle("article.html", &buffer, &title);
        clearHTMLData(&buffer);

        if(i != 1)
            statement = g_string_append(statement, ",");

        g_string_append_printf(statement, " ($%d, $%d, $%d) ", i, i + 1, i + 2);


        bindParam(query, (char *) g_list_first(existHref)->data, i - 1);
        bindParam(query, title, i);
        bindParam(query, buffer, i + 1);
        i+= 3;
    }
    if(i > 1){

        query->query = statement->str;
        resultQuery = executePrepareStatement(query);
    }

    closePrepareStatement(query, resultQuery, NULL);
    g_string_free(statement, TRUE);

    query = NULL;
    resultQuery = NULL;

    query = prepareQuery(mainParam->centralParam->databaseInfo,
            "SELECT\n"
            "  id,\n"
            "  url,\n"
            "  title,\n"
            "  content,\n"
            "  TO_CHAR(\"dateAdd\", 'YYYY-MM-DD HH24:MI')\n"
            "FROM \"Article\"\n"
            "WHERE url ILIKE $1");

    param = g_string_new("%");
    param = g_string_append(param, research);
    param = g_string_append(param, "%");

    bindParam(query, param->str, 0);

    resultQuery = executePrepareStatement(query);

    fetchAllResult(resultQuery, &finalData);

    for (i = 0; i < resultQuery->numberOfrow; ++i){
        gtk_list_store_append(GTK_LIST_STORE(listStore), &tempIter);
        gtk_list_store_set(GTK_LIST_STORE(listStore), &tempIter,
                           0, finalData[i][0],
                           1, finalData[i][2],
                           2, finalData[i][3],
                           3, finalData[i][4],
                           -1);
    }
}

void openMoreDetailLeague(GtkWidget *widget, gpointer *data) {

    CallBackParamWithId *allParam = (CallBackParamWithId *) data;
    PrepareStatement *query = NULL;
    QueryStatement *queryResult = NULL;
    char ***finalData = NULL;
    GtkBuilder *builder = NULL;
    GtkWidget *window = NULL;
    GtkWidget *temp = NULL;
    WindowCalendarParam **calendarParam = (WindowCalendarParam **) malloc(2 * sizeof(WindowCalendarParam *));
    AllLeagueMatchParam *allLeagueParam = (AllLeagueMatchParam *) malloc(1 * sizeof(AllLeagueMatchParam));
    GtkTreeIter iter, fatherIter;
    GtkWidget *listStore = NULL;
    GError *error = NULL;

    error = loadGladeFile(&builder, "leagueDetail/main.glade");

    if (error == NULL) {

        window = (GtkWidget *) gtk_builder_get_object(builder, "leagueMoreDetail");

        if (allParam->allCalbackParam->mainParam->databaseInfo->error != 1) {

            /*
             * Team list store update
             */
            query = prepareQuery(allParam->allCalbackParam->mainParam->databaseInfo,
                                 "SELECT"
                                         "\"Team\".id,"
                                         "\"Team\".name,"
                                         "\"Team\".city,"
                                         "\"Team\".stadium\n"
                                         "FROM \"Team\"\n"
                                         "JOIN \"League\" ON \"Team\".\"idLeague\" = \"League\".id\n"
                                         "WHERE \"League\".id = $1");

            bindParam(query, allParam->id, 0);

            queryResult = executePrepareStatement(query);

            if (queryResult->error != 1) {
                fetchAllResult(queryResult, &finalData);

                listStore = (GtkWidget *) gtk_builder_get_object(builder, "teamListStore");

                for (int i = 0; i < queryResult->numberOfrow; ++i) {
                    gtk_list_store_append(GTK_LIST_STORE(listStore), &iter);
                    gtk_list_store_set(GTK_LIST_STORE(listStore), &iter,
                                       0, finalData[i][0],
                                       1, finalData[i][1],
                                       2, finalData[i][2],
                                       3, finalData[i][3],
                                       -1);
                }
            }

            closePrepareStatement(query, queryResult, finalData);

            temp = (GtkWidget *) gtk_builder_get_object(builder, "leagueMoreDetailTeamTreeView");

            if (temp != NULL)
                g_signal_connect(G_OBJECT(temp), "row-activated", G_CALLBACK(displayTeamDetail),
                                 (gpointer) allParam->allCalbackParam);


            /*
             * Match list store update
             */
            query = prepareQuery(allParam->allCalbackParam->mainParam->databaseInfo,
                                 "SELECT\n"
                                         "    \"Match\".id AS \"idMatch\",\n"
                                         "    \"T1\".name AS \"homeTeamName\",\n"
                                         "    \"T2\".name AS \"outsideTeamName\",\n"
                                         "    to_char(\"Match\".date, 'YYYY-mm-dd') AS \"matchDate\"\n"
                                         "FROM \"Match\"\n"
                                         "JOIN \"Team\" AS \"T1\" ON \"Match\".\"homeTeam\" = \"T1\".id\n"
                                         "JOIN \"Team\" AS \"T2\" ON \"Match\".\"outsideTeam\" = \"T2\".id\n"
                                         "WHERE \"Match\".\"idLeague\" = $1"
                                         "  AND date >= now()"
                                         "ORDER BY \"Match\".date ASC, \"Match\".id ASC");

            bindParam(query, allParam->id, 0);

            finalData = NULL;

            queryResult = executePrepareStatement(query);

            if (queryResult->error != 1) {
                fetchAllResult(queryResult, &finalData);

                listStore = (GtkWidget *) gtk_builder_get_object(builder, "matchListStore");
                if (queryResult->numberOfrow > 0) {
                    gtk_tree_store_append(GTK_TREE_STORE(listStore), &fatherIter, NULL);
                    gtk_tree_store_set(GTK_TREE_STORE(listStore), &fatherIter, 3, finalData[0][3], -1);
                }

                for (int i = 0; i < queryResult->numberOfrow; ++i) {
                    gtk_tree_store_append(GTK_TREE_STORE(listStore), &iter, &fatherIter);
                    gtk_tree_store_set(GTK_TREE_STORE(listStore), &iter,
                                       0, finalData[i][0],
                                       1, finalData[i][1],
                                       2, finalData[i][2],
                                       3, finalData[i][3],
                                       -1);

                    if (i + 1 < queryResult->numberOfrow && strcmp(finalData[i][3], finalData[i + 1][3]) != 0) {
                        gtk_tree_store_append(GTK_TREE_STORE(listStore), &fatherIter, NULL);
                        gtk_tree_store_set(GTK_TREE_STORE(listStore), &fatherIter, 3, finalData[i + 1][3], -1);
                    }
                }
            }

            closePrepareStatement(query, queryResult, finalData);


            /*
             * Match list store update
             */
            query = prepareQuery(allParam->allCalbackParam->mainParam->databaseInfo,
                                 "SELECT\n"
                                         "  \"Team\".name,\n"
                                         "  COALESCE(\n"
                                         "      (SELECT SUM(\n"
                                         "        CASE\n"
                                         "          WHEN \"numberOfGoalOutsideTeam\" = \"numberOfGoalHomeTeam\" AND \"Match\".status = 1 THEN 1\n"
                                         "          WHEN \"numberOfGoalOutsideTeam\" > \"numberOfGoalHomeTeam\" AND \"outsideTeam\" = \"Team\".Id AND \"Match\".status = 1 THEN 3\n"
                                         "          WHEN \"numberOfGoalHomeTeam\" > \"numberOfGoalOutsideTeam\" AND \"homeTeam\" = \"Team\".Id AND \"Match\".status = 1 THEN 3\n"
                                         "          ELSE 0\n"
                                         "        END\n"
                                         "      )FROM \"Match\" WHERE (\"outsideTeam\" = \"Team\".id OR  \"homeTeam\" = \"Team\".id)), 0) AS point,\n"
                                         "  COALESCE(\n"
                                         "      (SELECT count(id)\n"
                                         "        FROM \"Match\"\n"
                                         "        WHERE (\"outsideTeam\" = \"Team\".id OR  \"homeTeam\" = \"Team\".id)\n"
                                         "              AND \"Match\".status = 1\n"
                                         "      )\n"
                                         "      , 0\n"
                                         "  ) AS numberOfMatchPlayed,\n"
                                         "  COALESCE(\n"
                                         "      (SELECT count(id)\n"
                                         "       FROM \"Match\"\n"
                                         "       WHERE ((\"outsideTeam\" = \"Team\".id AND \"numberOfGoalOutsideTeam\" > \"numberOfGoalHomeTeam\")\n"
                                         "              OR (\"homeTeam\" = \"Team\".id) AND \"numberOfGoalHomeTeam\" > \"numberOfGoalOutsideTeam\")\n"
                                         "              AND \"Match\".status = 1\n"
                                         "      )\n"
                                         "      , 0\n"
                                         "  ) AS W,\n"
                                         "  COALESCE(\n"
                                         "      (SELECT count(id)\n"
                                         "       FROM \"Match\"\n"
                                         "       WHERE ((\"outsideTeam\" = \"Team\".id AND \"numberOfGoalOutsideTeam\" = \"numberOfGoalHomeTeam\")\n"
                                         "              OR (\"homeTeam\" = \"Team\".id) AND \"numberOfGoalHomeTeam\" = \"numberOfGoalOutsideTeam\")\n"
                                         "             AND \"Match\".status = 1\n"
                                         "      )\n"
                                         "      , 0\n"
                                         "  )AS N,\n"
                                         "  COALESCE(\n"
                                         "      (SELECT count(id)\n"
                                         "       FROM \"Match\"\n"
                                         "       WHERE ((\"outsideTeam\" = \"Team\".id AND \"numberOfGoalOutsideTeam\" < \"numberOfGoalHomeTeam\")\n"
                                         "              OR (\"homeTeam\" = \"Team\".id) AND \"numberOfGoalHomeTeam\" < \"numberOfGoalOutsideTeam\")\n"
                                         "             AND \"Match\".status = 1\n"
                                         "      )\n"
                                         "      ,0\n"
                                         "  ) AS L,\n"
                                         "  COALESCE(\n"
                                         "      (SELECT SUM(CASE\n"
                                         "                  WHEN \"outsideTeam\" = \"Team\".id\n"
                                         "                    THEN \"numberOfGoalOutsideTeam\"\n"
                                         "                  WHEN \"homeTeam\" = \"Team\".id\n"
                                         "                    THEN \"numberOfGoalHomeTeam\"\n"
                                         "                  END)\n"
                                         "       FROM \"Match\"\n"
                                         "       WHERE (\"outsideTeam\" = \"Team\".id OR \"homeTeam\" = \"Team\".id)\n"
                                         "             AND \"Match\".status = 1\n"
                                         "      )\n"
                                         "      , 0\n"
                                         "  )AS put,\n"
                                         "  COALESCE(\n"
                                         "      (SELECT SUM(CASE\n"
                                         "                  WHEN \"outsideTeam\" = \"Team\".id\n"
                                         "                    THEN \"numberOfGoalHomeTeam\"\n"
                                         "                  WHEN \"homeTeam\" = \"Team\".id\n"
                                         "                    THEN \"numberOfGoalOutsideTeam\"\n"
                                         "                  END)\n"
                                         "       FROM \"Match\"\n"
                                         "       WHERE (\"outsideTeam\" = \"Team\".id OR \"homeTeam\" = \"Team\".id)\n"
                                         "             AND \"Match\".status = 1\n"
                                         "      )\n"
                                         "      , 0\n"
                                         "  )AS take,\n"
                                         "  COALESCE(\n"
                                         "      (SELECT SUM(CASE\n"
                                         "                    WHEN \"outsideTeam\" = \"Team\".id THEN \"numberOfGoalOutsideTeam\"\n"
                                         "                    WHEN \"homeTeam\" = \"Team\".id THEN \"numberOfGoalHomeTeam\"\n"
                                         "                  END)\n"
                                         "      FROM \"Match\"\n"
                                         "      WHERE (\"outsideTeam\" = \"Team\".id OR  \"homeTeam\" = \"Team\".id)\n"
                                         "            AND \"Match\".status = 1\n"
                                         "      )\n"
                                         "        -\n"
                                         "      (SELECT SUM(CASE\n"
                                         "                    WHEN \"outsideTeam\" = \"Team\".id THEN \"numberOfGoalHomeTeam\"\n"
                                         "                    WHEN \"homeTeam\" = \"Team\".id THEN \"numberOfGoalOutsideTeam\"\n"
                                         "                  END\n"
                                         "      )FROM \"Match\"\n"
                                         "      WHERE (\"outsideTeam\" = \"Team\".id OR  \"homeTeam\" = \"Team\".id)\n"
                                         "            AND \"Match\".status = 1\n"
                                         "      )\n"
                                         "      ,0\n"
                                         "  ) AS diff\n"
                                         "FROM \"Team\"\n"
                                         "WHERE \"idLeague\" = $1\n"
                                         "ORDER BY point DESC");

            bindParam(query, allParam->id, 0);

            finalData = NULL;

            queryResult = executePrepareStatement(query);

            if (queryResult->error != 1) {
                fetchAllResult(queryResult, &finalData);

                listStore = (GtkWidget *) gtk_builder_get_object(builder, "rankingListStore");

                for (int i = 0; i < queryResult->numberOfrow; ++i) {
                    gtk_list_store_append(GTK_LIST_STORE(listStore), &iter);
                    gtk_list_store_set(GTK_LIST_STORE(listStore), &iter,
                                       0, finalData[i][0],
                                       1, finalData[i][1],
                                       2, finalData[i][2],
                                       3, finalData[i][3],
                                       4, finalData[i][4],
                                       5, finalData[i][5],
                                       6, finalData[i][6],
                                       7, finalData[i][7],
                                       8, finalData[i][8],
                                       -1);
                }
            }

            closePrepareStatement(query, queryResult, finalData);

            /*
             * League param tab
             */
            temp = (GtkWidget *) gtk_builder_get_object(builder, "firstDateEntry");

            calendarParam[0] = (WindowCalendarParam *) malloc(1 * sizeof(WindowCalendarParam));


            if (calendarParam[0] != NULL) {

                calendarParam[0]->destinationWidget = temp;
                calendarParam[0]->calendarWindow = NULL;
                calendarParam[0]->month = -1;
                calendarParam[0]->day = -1;
                calendarParam[0]->year = -1;
            }


            if (temp != NULL)
                g_signal_connect(G_OBJECT(temp), "icon-press", G_CALLBACK(openCalendar),(gpointer *) calendarParam[0]);

            temp = (GtkWidget *) gtk_builder_get_object(builder, "secondDateEntry");

            calendarParam[1] = (WindowCalendarParam *) malloc(1 * sizeof(WindowCalendarParam));

            if (calendarParam[1] != NULL) {

                calendarParam[1]->destinationWidget = temp;
                calendarParam[1]->calendarWindow = NULL;
                calendarParam[1]->month = -1;
                calendarParam[1]->day = -1;
                calendarParam[1]->year = -1;
            }


            if (temp != NULL)
                g_signal_connect(G_OBJECT(temp), "icon-press", G_CALLBACK(openCalendar),
                                 (gpointer *) calendarParam[1]);


            temp = (GtkWidget *) gtk_builder_get_object(builder, "createButton");

            if (allLeagueParam != NULL) {
                allLeagueParam->allCalendarParam = calendarParam;
                allLeagueParam->builder = builder;
                allLeagueParam->mainParam = allParam;
            }

            if (temp != NULL)
                g_signal_connect(G_OBJECT(temp), "clicked", G_CALLBACK(newLeagueMatch),
                                 (gpointer *) allLeagueParam);
        }


        if (window != NULL)
            gtk_widget_show_all(window);


    } else {
        printf("%s \n", error->message);
        g_error_free(error);
    }

}

void openCalendar(GtkEntry *widget, GtkEntryIconPosition iconPos, GdkEvent *event, gpointer *data) {
    GtkBuilder *builder = NULL;
    GError *error = NULL;
    GtkWidget *window = NULL;
    GtkWidget *calendar = NULL;
    WindowCalendarParam *calendarParam = (WindowCalendarParam *) data;

    error = loadGladeFile(&builder, "toolWidget/calendar.glade");

    if (error == NULL) {
        window = (GtkWidget *) gtk_builder_get_object(builder, "calendarWindow");

        if (window != NULL) {

            calendarParam->calendarWindow = window;

            calendar = (GtkWidget *) gtk_builder_get_object(builder, "mainCalendar");

            if (calendarParam->day != -1)
                gtk_calendar_select_day(GTK_CALENDAR(calendar), (guint) calendarParam->day);

            if (calendarParam->month != -1 && calendarParam->year != -1)
                gtk_calendar_select_month(GTK_CALENDAR(calendar), (guint) calendarParam->month,
                                          (guint) calendarParam->year);

            if (calendar != NULL)
                g_signal_connect(G_OBJECT(calendar), "day-selected-double-click", G_CALLBACK(daySelect),
                                 calendarParam);

            gtk_widget_show_all(window);
        } else {
            printf("No window to display");
        }
    } else {
        printf("%s \n", error->message);
        g_error_free(error);
    }
}

void daySelect(GtkCalendar *calendar, gpointer *data) {
    WindowCalendarParam *calendarParam = (WindowCalendarParam *) data;
    guint year = 0;
    guint day = 0;
    guint month = 0;
    gchar tempChar[250];

    gtk_calendar_get_date(calendar, &year, &month, &day);

    gtk_widget_destroy(calendarParam->calendarWindow);

    sprintf(tempChar, "%d-%02d-%02d", year, month + 1, day);

    if (calendarParam->destinationWidget != NULL)
        gtk_entry_set_text(GTK_ENTRY(calendarParam->destinationWidget), tempChar);

    calendarParam->day = day;
    calendarParam->month = month;
    calendarParam->year = year;
}

void newLeagueMatch(GtkWidget *widget, gpointer *data) {

    AllLeagueMatchParam *allMatchParam = (AllLeagueMatchParam *) data;
    WindowCalendarParam **allParam = allMatchParam->allCalendarParam;
    GDate *date[2] = {NULL};
    GDate *currentDate = NULL;
    GTimeVal *currentTime = NULL;
    GtkWidget *errorWidget = NULL;
    GList *allChild = NULL, *tempList = NULL;
    PrepareStatement *exec = NULL;
    QueryStatement *queryResult = NULL;
    char ***finalData = NULL;
    char **configuration = NULL;
    char errorMessage[100] = {0};
    GString *statement = g_string_new("");
    int dateCompare, error = 0, nmb = 20, k, nRound;
    int ***roundRobinArray = NULL;
    const GDateMonth monthNames[] = {G_DATE_JANUARY, G_DATE_FEBRUARY, G_DATE_MARCH, G_DATE_APRIL, G_DATE_MAY,
                                     G_DATE_JUNE, G_DATE_JULY, G_DATE_AUGUST, G_DATE_SEPTEMBER, G_DATE_OCTOBER,
                                     G_DATE_NOVEMBER, G_DATE_DECEMBER};


    /*
     * Create a variable with current date and time
     */
    currentTime = (GTimeVal *) malloc(sizeof(GTimeVal));
    currentDate = g_date_new();
    g_get_current_time(currentTime);
    g_date_set_time_val(currentDate, currentTime);


    /*
     * Get the error message box
     */
    if (allMatchParam->builder != NULL)

        errorWidget = (GtkWidget *) gtk_builder_get_object(allMatchParam->builder, "errorMessageBox");


    /*
     * If day, month and year are different from 1
     * Init twice g date structure, with the two date choose by the user
     */
    if (allParam[0]->day > -1 && allParam[0]->month > -1 && allParam[0]->year > -1)
        date[0] = g_date_new_dmy((GDateDay) allParam[0]->day, (GDateMonth) monthNames[allParam[0]->month],
                                 (GDateYear) allParam[0]->year);


    if (allParam[1]->day > -1 && allParam[1]->month > -1 && allParam[1]->year > -1)
        date[1] = g_date_new_dmy((GDateDay) allParam[1]->day, (GDateMonth) monthNames[allParam[1]->month],
                                 (GDateYear) allParam[1]->year);


    /*
     * Get all the children of the gtk error message box
     * Travel list, and remove all the children of the error message box
     * Free the list
     */
    if (errorWidget != NULL)
        allChild = gtk_container_get_children(GTK_CONTAINER(errorWidget));

    for (tempList = allChild; tempList != NULL; tempList = tempList->next)
        gtk_container_remove(GTK_CONTAINER(errorWidget), GTK_WIDGET(tempList->data));

    g_list_free(allChild);


    /*
     * Check if both date are valid;
     */
    if (date[0] != NULL && date[1] != NULL && g_date_valid(date[0]) && g_date_valid(date[1])) {
        /*
         * Check if date are different
         * Check if the first date are before the second date
         */
        if ((dateCompare = g_date_compare(date[0], date[1])) == 0) {

            gtk_box_pack_start(GTK_BOX(errorWidget), gtk_label_new("Date can't be equal"), FALSE, FALSE, 0);
            error = 1;
        } else if (dateCompare > 0) {
            gtk_box_pack_start(GTK_BOX(errorWidget), gtk_label_new(
                    "Date of the first season part can't be superior to the second part start date"), FALSE, FALSE,
                               0);
            error = 1;
        }

        /*
         * First date need to be after current date
         */
        if (error == 0 && g_date_compare(currentDate, date[0]) >= 0) {
            gtk_box_pack_start(GTK_BOX(errorWidget), gtk_label_new("Date of the first part, need to be after today"),
                               FALSE, FALSE, 0);
            error = 1;
        }

        /*
         * Second date need to be after the end of the first part of the league
         */
        if (error == 0 && g_date_days_between(date[0], date[1]) < 19 * 7) {
            if (errorWidget != NULL)
                gtk_box_pack_start(GTK_BOX(errorWidget),
                                   gtk_label_new("Start date of the second part, is before the end of the first part"),
                                   FALSE, FALSE, 0);
            error = 1;
        }

        /*
         * Check if the league is not already configure
         */
        exec = prepareQuery(allMatchParam->mainParam->allCalbackParam->mainParam->databaseInfo,
                            "SELECT\n"
                                    "  configure\n"
                                    "FROM \"League\"\n"
                                    "WHERE id = $1");


        bindParam(exec, allMatchParam->mainParam->id, 0);

        queryResult = executePrepareStatement(exec);

        fetchResult(queryResult, &configuration);

        if (strcmp(configuration[0], "1") == 0) {
            gtk_box_pack_start(GTK_BOX(errorWidget), gtk_label_new("This league has already be configured"), FALSE,
                               FALSE, 0);
            error = 1;
        }

        closePrepareStatement(exec, queryResult, &configuration);

        if (error != 1) {

            /*
             * Get all the team of the league
             */
            exec = prepareQuery(allMatchParam->mainParam->allCalbackParam->mainParam->databaseInfo,
                                "SELECT\n"
                                        "      id,\n"
                                        "      stadium\n"
                                        "FROM \"Team\"\n"
                                        "WHERE \"idLeague\" = $1");

            bindParam(exec, allMatchParam->mainParam->id, 0);

            queryResult = executePrepareStatement(exec);

            if (queryResult->error != 1) {
                fetchAllResult(queryResult, &finalData);


                /*
                 * Twenty team are needed to generate the match
                 */
                if (queryResult->numberOfrow != 20) {
                    gtk_box_pack_start(GTK_BOX(errorWidget),
                                       gtk_label_new("Twenty team are needed to generate the match"), FALSE, FALSE, 0);
                    error = 1;
                }

                if (error != 1) {

                    statement = g_string_append(statement,
                                                "INSERT INTO \"Match\"(\"homeTeam\",\n"
                                                        "                      \"outsideTeam\",\n"
                                                        "                       date,\n"
                                                        "                       \"idLeague\",\n"
                                                        "                       stadium)\n"
                                                        "                    VALUES ");


                    exec = prepareQuery(allMatchParam->mainParam->allCalbackParam->mainParam->databaseInfo,
                                        statement->str);

                    /*
                     * Get all the match scheduled
                     */
                    nRound = roundRobinAlgorithm(nmb, &roundRobinArray);

                    if (roundRobinArray == NULL || nRound == -1)
                        error = 1;

                    if (error != 1) {
                        /*
                         * Insert all the match prepare statement query
                         */
                        k = insertMatch(roundRobinArray, finalData, nmb, nRound, allMatchParam->mainParam->id, date[0],
                                        date[1], exec);

                        freeRoundRobinArray(nmb, &roundRobinArray);
                        //exec->query = statement->str;
                        queryResult = executePrepareStatement(exec);

                        if (queryResult->error == 1) {
                            gtk_box_pack_start(GTK_BOX(errorWidget),
                                               gtk_label_new("An error has occured when inserting the match"), FALSE,
                                               FALSE, 0);
                            error = 1;
                        }

                        closePrepareStatement(exec, queryResult, NULL);

                        if (error != 1) {
                            sprintf(errorMessage, "%d match have been created\n", k / 5);
                            gtk_box_pack_start(GTK_BOX(errorWidget), gtk_label_new(errorMessage), FALSE, FALSE, 0);

                            exec = prepareQuery(allMatchParam->mainParam->allCalbackParam->mainParam->databaseInfo,
                                                "UPDATE \"League\"\n"
                                                        "SET configure = 1\n"
                                                        "WHERE id = $1");
                            bindParam(exec, allMatchParam->mainParam->id, 0);


                            queryResult = executePrepareStatement(exec);

                            while (queryResult->error != 0)
                                queryResult = executePrepareStatement(exec);

                            closePrepareStatement(exec, queryResult, NULL);
                        }
                    } else {
                        gtk_box_pack_start(GTK_BOX(errorWidget), gtk_label_new("Internal error"), FALSE, FALSE, 0);
                    }
                }

            } else {
                gtk_box_pack_start(GTK_BOX(errorWidget), gtk_label_new("Internal error"), FALSE, FALSE, 0);
                return;
            }
        }

    } else {
        gtk_box_pack_start(GTK_BOX(errorWidget), gtk_label_new("Invalid date, please check your entry"), FALSE, FALSE,
                           0);
        return;
    }

    /*
     * Free all the param use
     */
    if (errorWidget != NULL)

        gtk_widget_show_all(errorWidget);

    if (date[0] != NULL)
        g_date_free(date[0]);

    if (date[1] != NULL)
        g_date_free(date[1]);

    g_free(currentTime);
    g_date_free(currentDate);

}

void updateStatusMatch(GtkWidget *widget, gpointer *data){
    AllTabParam *completeTabParam = (AllTabParam *) data;
    PrepareStatement *query = NULL;
    QueryStatement *resultQuery = NULL;
    GtkWidget *newStatus = NULL;

    query = prepareQuery(completeTabParam->centralParam->databaseInfo,
                         "UPDATE \"Match\"\n"
                                 "SET status = 1\n"
                                 "WHERE id = $1");

    bindParam(query,completeTabParam->searchParam->id,0);
    resultQuery = executePrepareStatement(query);

    if(resultQuery->error == 0){
        gtk_widget_set_sensitive(widget,FALSE);
    }

    newStatus = (GtkWidget *)gtk_builder_get_object(completeTabParam->builder,"statusValue");

    if (newStatus != NULL)
        gtk_label_set_label(GTK_LABEL(newStatus), getMatchStatus("1"));

    closePrepareStatement(query,resultQuery,NULL);


}