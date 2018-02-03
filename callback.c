//
// Created by mathi on 15/01/2018.
//
#include "callback.h"


void destroyWindow(GtkWidget * widget, gpointer data){
    gtk_main_quit();
}

void openNotebookTab(GtkWidget * widget, gpointer * data){
    GtkWidget * mainNotebook;
    GtkWidget * mainNotebookContent = NULL;
    GtkWidget * box;
    GtkWidget * buttonClose;
    GtkWidget * image;
    GtkBuilder * tabBuilder;
    GError * errorMessage = NULL;
    void (*pointer) (GtkWidget *, CallbackParam *);

    /*
     * Get the structure, pass on param
     */
    CallbackParam * functionCallbackParam = (CallbackParam *) data;


    pointer = functionCallbackParam->function;


    /*
     * With the builder in the structure, find the main Notebook main
     */
    mainNotebook = (GtkWidget *) gtk_builder_get_object(functionCallbackParam->builder, (char *) functionCallbackParam->parentName);

    /*
     * If the tab, already exist, don't create, but open it
     */
    if((mainNotebookContent = findChild(mainNotebook, (char *) functionCallbackParam->objectName)) == NULL){

        /*
         * Load the glade file
         * If error, exit, don't create the notebook
         * Else, create it
         */
        errorMessage = loadGladeFile(&tabBuilder, (char *) functionCallbackParam->fileName);

        if(errorMessage){
            printf("%s \n", errorMessage->message);
            g_error_free(errorMessage);

            return;
        }else{

            /*
             * Create a new box, to put label and leave button
             * Put first, the label, with the name in the structure received
             */
            box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
            gtk_box_pack_start(GTK_BOX(box), gtk_label_new((char  *) functionCallbackParam->objectLabel), TRUE, TRUE, 0);

            /*
             * Create the button from nothing
             * Load, the image, via pixbuf function, to set the size and ratio
             * Put this image on the button
             * Add the button to the box
             */
            buttonClose = gtk_button_new();
            image = gtk_image_new_from_pixbuf(gdk_pixbuf_new_from_file_at_scale("image/cross.png"
                    ,8
                    ,8
                    ,TRUE
                    ,NULL));
            gtk_button_set_image(GTK_BUTTON(buttonClose), image);

            g_signal_connect(G_OBJECT(buttonClose), "clicked", G_CALLBACK(closeNotebookTab), (gpointer) data);

            gtk_box_pack_start(GTK_BOX(box), buttonClose, TRUE, TRUE, 0);


            /*
             * On the new glade file, get the main container, thanks to his name
             * This name come from the structure receive
             * Set name widget, with the name above
             *
             */
            mainNotebookContent = (GtkWidget *) gtk_builder_get_object(tabBuilder, (char *) functionCallbackParam->objectName);

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


            if(pointer != NULL){
                CallbackParam * test = (CallbackParam *) malloc(sizeof(CallbackParam));
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

    }else{
        /*
         * If the tab already exists, get the page number of the existing tab
         * Go to this tab
         */
        gtk_notebook_set_current_page(GTK_NOTEBOOK(mainNotebook),gtk_notebook_page_num(GTK_NOTEBOOK(mainNotebook), mainNotebookContent));
    }

}

void closeNotebookTab(GtkWidget * widget, gpointer * data){
    GtkWidget * mainNotebook;
    GtkWidget * mainNotebookContent;
    CallbackParam * functionCallbackParam = (CallbackParam *) data;

    /*
     * Get the notebook widget, via the builder
     */
    mainNotebook = (GtkWidget *) gtk_builder_get_object(functionCallbackParam->builder, (char *) functionCallbackParam->parentName);

    mainNotebookContent = findChild(mainNotebook, (char *) functionCallbackParam->objectName);

    if(mainNotebookContent != NULL){
        gtk_notebook_remove_page(GTK_NOTEBOOK(mainNotebook), gtk_notebook_page_num(GTK_NOTEBOOK(mainNotebook), mainNotebookContent));
    }

}

void tabSearch(GtkWidget * widget, gpointer * data){
    //Receive a structure array
    TabSearch * tabMainParam = (TabSearch *) data;
    TabSearchParam ** allSearchTabParam;

    PrepareStatement * query = NULL;
    QueryStatement * queryResult = NULL;
    char *** finalData = NULL;

    GList * allCondition = NULL;
    GList * allValue = NULL;
    GList * l;

    GtkListStore * listStore = NULL;
    GtkTreeIter iter;

    GtkWidget * tempWidget = NULL;

    GString * finalStatement;
    GString * valueString;
    GString * conditionString;

    int i;

    if(tabMainParam != NULL){
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


            if(strlen(valueString->str) > 0){
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
                if(allSearchTabParam[i]->typeCondition == 1) {
                    valueString = g_string_prepend(valueString, "%");
                    valueString = g_string_append(valueString, "%");
                }else if(allSearchTabParam[i]->typeCondition == 2){
                    valueString = g_string_append(valueString, "%");
                }else if(allSearchTabParam[i]->typeCondition == 3){
                    valueString = g_string_prepend(valueString, "%");
                }

                /*
                 * Add the value to value list
                 */
                allValue = g_list_prepend(allValue, (gpointer) valueString->str);

            }else{
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
        if(g_list_length(allCondition) > 0 && g_list_length(allValue) > 0){
            finalStatement = g_string_append(finalStatement, " WHERE ");
        }


        i = 0;

        /*
         * Browse all the condition list
         * Add to the statement condition
         * Add or no the END keyword
         */
        for (l = allCondition; l != NULL; l = l->next, i++){
            if(i != 0)
                finalStatement = g_string_append(finalStatement, " AND ");

            finalStatement = g_string_append(finalStatement, (gchar *) l->data);
        }

        /*
         * Prepare the query with the statement prepare previously
         */
        query = prepareQuery(tabMainParam->mainParam->databaseInfo, finalStatement->str);

        i = 0;

        /*
         * Bind all the param to the statement
         */
        for (l = allValue; l != NULL; l = l->next, i++){
            bindParam(query, (gchar *) l->data, i);
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

        if(listStore != NULL){

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


void openAddNewLeagueForm(GtkWidget * widget, gpointer * data){

    AllTabParam * mainParam = (AllTabParam *) data;
    CallbackParam * newParam = (CallbackParam *) malloc(sizeof(CallbackParam));
    GtkWidget * window;
    GtkBuilder * builder;
    GtkWidget * button;
    GError * error = NULL;

    newParam->mainParam = mainParam->mainParam->mainParam;
    strcpy(newParam->fileName,mainParam->mainParam->fileName);
    newParam->function  = mainParam->mainParam->function;
    strcpy(newParam->objectLabel,mainParam->mainParam->objectLabel);
    strcpy(newParam->parentName,mainParam->mainParam->parentName);
    strcpy(newParam->objectName,mainParam->mainParam->objectName);

    error = loadGladeFile(&builder, "formWidget/newLeagueForm.glade");

    if(error != NULL)
        printf("%s \n", error->message);

    newParam->builder = builder;

    newParam->builder = builder;

    window = (GtkWidget *) gtk_builder_get_object(builder, "leagueAddForm");

    button = (GtkWidget *) gtk_builder_get_object(builder, "leagueAddFormCloseButton");

    if(button != NULL)
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(closeDialogBox), window);


    button = (GtkWidget *) gtk_builder_get_object(builder, "leagueAddFormCreateButton");

    mainParam->mainParam = newParam;

    if(button != NULL)
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(createNewLeague), (gpointer * ) mainParam);


    gtk_widget_show_all(window);

}

void openAddNewTeamForm(GtkWidget * widget, gpointer * data){

    AllTabParam * mainParam = (AllTabParam *) data;
    CallbackParam * newParam = (CallbackParam *) malloc(sizeof(CallbackParam));
    GtkWidget * window;
    GtkBuilder * builder;
    GtkWidget * button;
    GtkListStore * listStore;
    QueryStatement * exec;
    char *** finalData = NULL;
    GtkTreeIter tempIter;


    newParam->mainParam = mainParam->mainParam->mainParam;
    strcpy(newParam->fileName,mainParam->mainParam->fileName);
    newParam->function  = mainParam->mainParam->function;
    strcpy(newParam->objectLabel,mainParam->mainParam->objectLabel);
    strcpy(newParam->parentName,mainParam->mainParam->parentName);
    strcpy(newParam->objectName,mainParam->mainParam->objectName);

    loadGladeFile(&builder, "formWidget/newTeamForm.glade");

    newParam->builder = builder;

    window = (GtkWidget *) gtk_builder_get_object(builder, "teamAddForm");

    button = (GtkWidget *) gtk_builder_get_object(builder, "teamAddFormCloseButton");

    if(button != NULL)
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(closeDialogBox), window);


    button = (GtkWidget *) gtk_builder_get_object(builder, "teamAddFormCreateButton");

    mainParam->mainParam = newParam;

    if(button != NULL)
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(createNewTeam), mainParam);


    exec = query(mainParam->mainParam->mainParam->databaseInfo, "SELECT\n"
                                                                        "id,\n"
                                                                        "      name\n"
                                                                        "FROM \"League\";");

    fetchAllResult(exec,&finalData);

    listStore = (GtkListStore * ) gtk_builder_get_object(builder,"leagueStore");

    for (int i = 0; i < exec->numberOfrow; ++i) {

        gtk_list_store_append(listStore, &tempIter);
        gtk_list_store_set(listStore, &tempIter
                , 0, finalData[i][0]
                , 1, finalData[i][1]
                , -1);
    }



    gtk_widget_show_all(window);
}

void openAddNewPlayerForm(GtkWidget * widget, gpointer * data){

    AllTabParam * mainParam = (AllTabParam *) data;
    CallbackParam * newParam = (CallbackParam *) malloc(sizeof(CallbackParam));
    GtkWidget * window;
    GtkBuilder * builder;
    GtkWidget * button;

    newParam->mainParam = mainParam->mainParam->mainParam;
    strcpy(newParam->fileName,mainParam->mainParam->fileName);
    newParam->function  = mainParam->mainParam->function;
    strcpy(newParam->objectLabel,mainParam->mainParam->objectLabel);
    strcpy(newParam->parentName,mainParam->mainParam->parentName);
    strcpy(newParam->objectName,mainParam->mainParam->objectName);

    loadGladeFile(&builder, "formWidget/newPlayerForm.glade");

    newParam->builder = builder;

    window = (GtkWidget *) gtk_builder_get_object(builder, "playerAddForm");

    button = (GtkWidget *) gtk_builder_get_object(builder, "playerAddFormCloseButton");

    if(button != NULL)
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(closeDialogBox), window);


    button = (GtkWidget *) gtk_builder_get_object(builder, "playerAddFormCreateButton");

    /*if(button != NULL)
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(createNewTeam), newParam);*/


    gtk_widget_show_all(window);
}



void closeDialogBox (GtkWidget * widget, gpointer * data) {
    GtkWidget * window = (GtkWidget *) data;

    gtk_widget_destroy(window);


}

void createNewLeague (GtkWidget * widget, gpointer * data) {
    AllTabParam *allParam = (AllTabParam *) data;
    GtkWidget *allEntry;
    GtkWidget *currentBox;
    char *param = 0;
    GtkWidget *label;
    char *statement;
    PrepareStatement *query;
    QueryStatement *queryResult;


    allEntry = (GtkWidget *) gtk_builder_get_object(allParam->mainParam->builder, "leagueAddFormNameEntry");

    currentBox = (GtkWidget *) gtk_builder_get_object(allParam->mainParam->builder, "leagueAddFormBox");


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
        query = prepareQuery(allParam->mainParam->mainParam->databaseInfo, statement);
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
            gtk_widget_destroy((GtkWidget *) gtk_builder_get_object(allParam->mainParam->builder, "leagueAddForm"));

            tabSearch(widget, (gpointer *) allParam->searchParam);

        }
        closePrepareStatement(query, queryResult, NULL);

        /*
         * Refresh list store
         */
    }
}

void createNewTeam (GtkWidget * widget, gpointer * data) {
    AllTabParam * allParam = (AllTabParam *) data;
    GtkWidget * allEntry [4];
    GtkWidget * currentBox;
    char * param [4] = {0};
    GtkWidget * label;
    char * statement;
    PrepareStatement * query;
    QueryStatement * queryResult;
    char errorMessage[1000] = {0};

    allEntry [0] = (GtkWidget *) gtk_builder_get_object(allParam->mainParam->builder, "teamAddFormNameEntry");
    allEntry [1] = (GtkWidget *) gtk_builder_get_object(allParam->mainParam->builder, "teamAddFormCityEntry");
    allEntry [2] = (GtkWidget *) gtk_builder_get_object(allParam->mainParam->builder, "teamAddFormStadiumEntry");
    allEntry [3] = (GtkWidget *) gtk_builder_get_object(allParam->mainParam->builder, "teamAddFormLeagueComboBox");



    currentBox = (GtkWidget * )gtk_builder_get_object(allParam->mainParam->builder,"teamAddFormBox");

    if (strlen(param[0] = (char *) gtk_entry_get_text(GTK_ENTRY(allEntry[0]))) <= 0)
        strcat(errorMessage,"Verify team's name \n");

    if (strlen(param[1] = (char *) gtk_entry_get_text(GTK_ENTRY(allEntry[1]))) <= 0)
        strcat(errorMessage,"Verify team's city \n");

    if (strlen(param[2] = (char *) gtk_entry_get_text(GTK_ENTRY(allEntry[2]))) <= 0)
        strcat(errorMessage,"Verify team's stadium \n");


    if ((param[3] = (char *) gtk_combo_box_get_active_id(GTK_COMBO_BOX(allEntry[3]))) == NULL || strlen(param[3]) <= 0)
        strcat(errorMessage,"Verify team's league \n");




    if(strlen(errorMessage) > 0) {


        if((label = findChild(currentBox,"errorLabelName")) == NULL) {
            label = gtk_label_new(errorMessage);
            gtk_widget_set_name(label,"errorLabelName");
            gtk_box_pack_start(GTK_BOX(currentBox), label,TRUE,TRUE,2);
        } else{
            gtk_label_set_label(GTK_LABEL(label),errorMessage);
        }

        gtk_widget_show_all(currentBox);

    }else {
        if ((label = findChild(currentBox, "errorLabelName")) != NULL) {
            gtk_widget_destroy(label);
        }


        statement = "INSERT INTO  \"Team\" (name,city,stadium,\"idLeague\") VALUES ($1,$2,$3,$4) ";
        query = prepareQuery(allParam->mainParam->mainParam->databaseInfo, statement);
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
            gtk_widget_destroy((GtkWidget *) gtk_builder_get_object(allParam->mainParam->builder, "teamAddForm"));

            tabSearch(widget, (gpointer *) allParam->searchParam);

        }
        closePrepareStatement(query, queryResult, NULL);


        /*
         * Refresh list store
         */
    }
}



void displayLeagueDetail(GtkTreeView  * treeView, GtkTreePath * path, GtkTreeViewColumn * column, gpointer * data){
    GtkTreeIter iter;
    GtkTreeModel * model;
    char * test;
    CallbackParam * allParam = (CallbackParam *) data;
    CallBackParamWithId * allParamWithId = (CallBackParamWithId *) malloc(sizeof(CallBackParamWithId));
    GError * error = NULL;
    GtkWidget * window = NULL;
    GtkWidget * tempWidget = NULL;
    PrepareStatement * query = NULL;
    QueryStatement * resultQuery = NULL;
    char ** finalData = NULL;

    model = gtk_tree_view_get_model(treeView);
    gtk_tree_model_get_iter(model, &iter, path);


    /*
     * Get the dialog box
     * Display the value
     */
    if((error = loadGladeFile(&allParam->builder, "detailWidget/leagueDetail.glade")) == NULL){

        window = (GtkWidget *) gtk_builder_get_object(allParam->builder, "leagueDetailWindow");

        if(window != NULL){
            gtk_widget_show_all(window);

            tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "leagueDetailIdValue");
            gtk_tree_model_get(model, &iter, 0, &test, -1);
            gtk_label_set_label(GTK_LABEL(tempWidget), test);

            query = prepareQuery(allParam->mainParam->databaseInfo, "SELECT id, name, to_char(\"dateUpdate\", 'YYYY-MM-DD') AS \"dateCreate\", to_char(\"dateUpdate\", 'YYYY-MM-DD') AS \"dateUpdate\" FROM \"League\" WHERE id = $1");

            bindParam(query, test, 0);

            resultQuery = executePrepareStatement(query);

            if(resultQuery->error != 1){

                fetchResult(resultQuery, &finalData);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "leagueDetailNameValue");

                if(tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[1]);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "leagueDetailDateCreateValue");

                if(tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[2]);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "leagueDetailDateUpdateValue");

                if(tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[3]);
            }

            closePrepareStatement(query, resultQuery, (char ***) finalData);

            tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "leagueDetailCloseButton");

            if(tempWidget != NULL)
                g_signal_connect(G_OBJECT(tempWidget), "clicked", G_CALLBACK(closeDialogBox), (gpointer) window);

            tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "leagueDetailDetailButton");

            allParamWithId->allCalbackParam = allParam;
            strcpy(allParamWithId->id, test);

            if(tempWidget != NULL)
                g_signal_connect(G_OBJECT(tempWidget), "clicked", G_CALLBACK(openMoreDetailLeague), (gpointer) allParamWithId);

        }

    }else{
        printf("%s \n", error->message);
        g_error_free(error);
    }



}

void displayTeamDetail(GtkTreeView  * treeView, GtkTreePath * path, GtkTreeViewColumn * column, gpointer * data){
    GtkTreeIter iter;
    GtkTreeModel * model;
    char * test;
    CallbackParam * allParam = (CallbackParam *) data;
    GError * error = NULL;
    GtkWidget * window = NULL;
    GtkWidget * tempWidget = NULL;
    PrepareStatement * query = NULL;
    QueryStatement * resultQuery = NULL;
    char ** finalData = NULL;

    model = gtk_tree_view_get_model(treeView);
    gtk_tree_model_get_iter(model, &iter, path);


    /*
     * Get the dialog box
     * Display the value
     */
    if((error = loadGladeFile(&allParam->builder, "detailWidget/teamDetail.glade")) == NULL){

        window = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamDetailWindow");

        if(window != NULL){
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

            if(resultQuery->error != 1) {

                fetchResult(resultQuery, &finalData);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamDetailNameValue");

                if(tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[1]);


                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamDetailCityValue");

                if(tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[2]);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamDetailLeagueValue");

                if(tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[3]);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamDetailStadiumValue");

                if(tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[4]);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamDetailDateCreateValue");

                if(tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[5]);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamDetailDateUpdateValue");

                if(tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[6]);

                closePrepareStatement(query, resultQuery, (char ***) finalData);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "teamDetailCloseButton");

                if(tempWidget != NULL)
                    g_signal_connect(G_OBJECT(tempWidget), "clicked", G_CALLBACK(closeDialogBox), (gpointer) window);

            }
        }

    }else{
        printf("%s \n", error->message);
        g_error_free(error);
    }

}

void displayPlayerDetail(GtkTreeView  * treeView, GtkTreePath * path, GtkTreeViewColumn * column, gpointer * data){
    GtkTreeIter iter;
    GtkTreeModel * model;
    char * test;
    CallbackParam * allParam = (CallbackParam *) data;
    GError * error = NULL;
    GtkWidget * window = NULL;
    GtkWidget * tempWidget = NULL;
    PrepareStatement * query = NULL;
    QueryStatement * resultQuery = NULL;
    char ** finalData = NULL;

    model = gtk_tree_view_get_model(treeView);
    gtk_tree_model_get_iter(model, &iter, path);


    /*
     * Get the dialog box
     * Display the value
     */
    if((error = loadGladeFile(&allParam->builder, "detailWidget/playerDetail.glade")) == NULL){

        window = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerDetailWindow");



        if(window != NULL){

            gtk_widget_show_all(window);
            tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerDetailIdValue");
            gtk_tree_model_get(model, &iter, 0, &test, -1);
            if(tempWidget != NULL)
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

            if(resultQuery->error != 1) {

                fetchResult(resultQuery, &finalData);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerDetailNameValue");

                if(tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[1]);


                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerDetailTeamValue");

                if(tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[2]);


                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerDetailLeagueValue");

                if(tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[3]);


                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerDetailPositionValue");

                if(tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[4]);


                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerDetailDateCreateValue");

                if(tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[5]);


                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerDetailDateUpdateValue");

                if(tempWidget != NULL)
                    gtk_label_set_label(GTK_LABEL(tempWidget), finalData[6]);


                closePrepareStatement(query, resultQuery, (char ***) finalData);

                tempWidget = (GtkWidget *) gtk_builder_get_object(allParam->builder, "playerDetailCloseButton");

                if(tempWidget != NULL)
                    g_signal_connect(G_OBJECT(tempWidget), "clicked", G_CALLBACK(closeDialogBox), (gpointer) window);

            }
        }

    }else{
        printf("%s \n", error->message);
        g_error_free(error);
    }

}


void openMoreDetailLeague(GtkWidget * widget, gpointer * data){
    CallBackParamWithId * allParam = (CallBackParamWithId *) data;
    PrepareStatement * query = NULL;
    QueryStatement * queryResult = NULL;
    char *** finalData = NULL;
    GtkBuilder* builder = NULL;
    GtkWidget * window = NULL;
    GtkWidget * temp = NULL;
    WindowCalendarParam ** calendarParam = (WindowCalendarParam **) malloc(2 * sizeof(WindowCalendarParam *));
    AllLeagueMatchParam * allLeagueParam = (AllLeagueMatchParam *) malloc(1 * sizeof(AllLeagueMatchParam));
    GtkTreeIter iter;
    GtkWidget * listStore = NULL;
    GError * error = NULL;

    error = loadGladeFile(&builder, "leagueDetail/main.glade");

    if(error == NULL){

        window = (GtkWidget *) gtk_builder_get_object(builder, "leagueMoreDetail");

        if(allParam->allCalbackParam->mainParam->databaseInfo->error != 1){

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

            if(queryResult->error != 1){
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

            if(temp != NULL)
                g_signal_connect(G_OBJECT(temp), "row-activated", G_CALLBACK(displayTeamDetail), (gpointer) allParam->allCalbackParam);


            /*
             * Match list store update
             */
            query = prepareQuery(allParam->allCalbackParam->mainParam->databaseInfo,
                                 "SELECT\n"
                                 "    \"Match\".Id AS \"idMatch\",\n"
                                 "    T1.name AS \"homeTeamName\",\n"
                                 "    T2.name AS \"outsideTeamName\",\n"
                                 "    \"Match\".date AS \"matchDate\"\n"
                                 "FROM \"Match\"\n"
                                 "JOIN \"Team\" AS T1 ON \"Match\".\"homeTeam\" = T1.id\n"
                                 "JOIN \"Team\" AS T2 ON \"Match\".\"outsideTeam\" = T2.id\n"
                                 "WHERE \"Match\".\"idLeague\" = $1"
                                 "  AND date >= now()");

            bindParam(query, allParam->id, 0);

            finalData = NULL;

            queryResult = executePrepareStatement(query);

            if(queryResult->error != 1){
                fetchAllResult(queryResult, &finalData);

                listStore = (GtkWidget *) gtk_builder_get_object(builder, "matchListStore");

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

            /*
             * League param tab
             */
            temp = (GtkWidget * ) gtk_builder_get_object(builder, "firstDateEntry");

            calendarParam[0] = (WindowCalendarParam *) malloc(1 * sizeof(WindowCalendarParam));

            if(calendarParam[0] != NULL){
                strcpy(calendarParam[0]->fileName, "leagueDetail/main.glade");
                strcpy(calendarParam[0]->calendarWindowId, "calendarWindow");
                strcpy(calendarParam[0]->calendarId, "mainCalendar");
                calendarParam[0]->destinationWidget = temp;
                calendarParam[0]->calendarWindow = NULL;
                calendarParam[0]->month = -1;
                calendarParam[0]->day = -1;
                calendarParam[0]->year = -1;
            }


            if(temp != NULL)
                g_signal_connect(G_OBJECT(temp), "icon-press", G_CALLBACK(openCalendar), (gpointer *) calendarParam[0]);

            temp = (GtkWidget * ) gtk_builder_get_object(builder, "secondDateEntry");

            calendarParam[1] = (WindowCalendarParam *) malloc(1 * sizeof(WindowCalendarParam));

            if(calendarParam[1] != NULL){
                strcpy(calendarParam[1]->fileName, "leagueDetail/main.glade");
                strcpy(calendarParam[1]->calendarWindowId, "calendarWindow");
                strcpy(calendarParam[1]->calendarId, "mainCalendar");
                calendarParam[1]->destinationWidget = temp;
                calendarParam[1]->calendarWindow = NULL;
                calendarParam[1]->month = -1;
                calendarParam[1]->day = -1;
                calendarParam[1]->year = -1;
            }


            if(temp != NULL)
                g_signal_connect(G_OBJECT(temp), "icon-press", G_CALLBACK(openCalendar), (gpointer *) calendarParam[1]);


            temp = (GtkWidget * ) gtk_builder_get_object(builder, "createButton");

            if(allLeagueParam != NULL){
                allLeagueParam->allCalendarParam = calendarParam;
                allLeagueParam->builder = builder;
                allLeagueParam->mainParam = allParam;
            }

            if(temp != NULL)
                g_signal_connect(G_OBJECT(temp), "clicked", G_CALLBACK(newLeagueMatch), (gpointer *) allLeagueParam);
        }


        if(window != NULL)
            gtk_widget_show_all(window);




    }else{
        printf("%s \n", error->message);
        g_error_free(error);
    }

}

void openCalendar (GtkEntry * widget, GtkEntryIconPosition iconPos, GdkEvent *event, gpointer * data){
    GtkBuilder * builder = NULL;
    GError * error = NULL;
    GtkWidget * window = NULL;
    GtkWidget * calendar = NULL;
    WindowCalendarParam * calendarParam = (WindowCalendarParam *) data;

    error = loadGladeFile(&builder, calendarParam->fileName);

    if(error == NULL){
        window = (GtkWidget *) gtk_builder_get_object(builder, calendarParam->calendarWindowId);

        if(window != NULL){

            calendarParam->calendarWindow = window;

            calendar = (GtkWidget *) gtk_builder_get_object(builder, calendarParam->calendarId);

            if(calendar != NULL)
                g_signal_connect(G_OBJECT(calendar), "day-selected-double-click", G_CALLBACK(daySelect), calendarParam);

            gtk_widget_show_all(window);
        }else{
            printf("No window to display");
        }
    }else{
        printf("%s \n", error->message);
        g_error_free(error);
    }
}

void daySelect(GtkCalendar * calendar, gpointer * data){
    WindowCalendarParam * calendarParam = (WindowCalendarParam *) data;
    guint year = 0;
    guint day = 0;
    guint month = 0;
    gchar tempChar[250];

    gtk_calendar_get_date(calendar, &year, &month, &day);

    gtk_widget_destroy(calendarParam->calendarWindow);

    sprintf(tempChar, "%d-%02d-%02d", year, month + 1, day);

    if(calendarParam->destinationWidget != NULL)
        gtk_entry_set_text(GTK_ENTRY(calendarParam->destinationWidget), tempChar);

    calendarParam->day = day;
    calendarParam->month = month;
    calendarParam->year = year;
}


void newLeagueMatch(GtkWidget * widget, gpointer * data) {
    AllLeagueMatchParam *allMatchParam = (AllLeagueMatchParam *) data;
    WindowCalendarParam **allParam = allMatchParam->allCalendarParam;
    GtkWidget *entry[2] = {NULL};
    GDate *date[2] = {NULL};
    GDate *currentDate = NULL;
    GTimeVal *currentTime = NULL;
    GtkBuilder *builder = NULL;
    GtkWidget *errorWidget = NULL;
    GList *allChild = NULL, *tempList = NULL;
    PrepareStatement * query = NULL;
    QueryStatement * queryResult = NULL;
    char ***finalData = NULL;
    char ** temp = NULL;
    char tempChar[20] = {0};
    char * pointerChar = NULL;
    GString * statement = g_string_new("");
    int dateCompare, error = 0, nbSort = 0, nmb = 20, tmp = 0, k = 1, m = 0;
    const GDateMonth monthNames[] = {G_DATE_JANUARY, G_DATE_FEBRUARY, G_DATE_MARCH, G_DATE_APRIL, G_DATE_MAY, G_DATE_JUNE, G_DATE_JULY, G_DATE_AUGUST, G_DATE_SEPTEMBER, G_DATE_OCTOBER, G_DATE_NOVEMBER, G_DATE_DECEMBER};

    currentTime = (GTimeVal *) malloc(sizeof(GTimeVal));
    currentDate = g_date_new();
    g_get_current_time(currentTime);
    g_date_set_time_val(currentDate, currentTime);

    if(allMatchParam->builder != NULL)
        errorWidget = (GtkWidget *) gtk_builder_get_object(allMatchParam->builder, "errorMessageBox");

    entry[0] = allParam[0]->destinationWidget;
    entry[1] = allParam[1]->destinationWidget;

    if(allParam[0]->day > -1 && allParam[0]->month > -1 && allParam[0]->year > -1)
        date[0] = g_date_new_dmy((GDateDay) allParam[0]->day, (GDateMonth) monthNames[allParam[0]->month], (GDateYear) allParam[0]->year);

    if(allParam[1]->day > -1 && allParam[1]->month > -1 && allParam[1]->year > -1)
        date[1] = g_date_new_dmy((GDateDay) allParam[1]->day, (GDateMonth) monthNames[allParam[1]->month], (GDateYear) allParam[1]->year);


    /*
     * Compare the two date
     * If return  =0 error date are equal
     * IF return > 0 error date 1 is superior to date 2
     */
    if(errorWidget != NULL)
        allChild = gtk_container_get_children(GTK_CONTAINER(errorWidget));

    for (tempList = allChild; tempList != NULL; tempList = tempList->next)
        gtk_container_remove(GTK_CONTAINER(errorWidget), GTK_WIDGET(tempList->data));

    g_list_free(allChild);

    if(date[0] != NULL && date[1] != NULL && g_date_valid(date[0]) && g_date_valid(date[1])){
        if((dateCompare = g_date_compare(date[0], date[1])) == 0) {
            gtk_box_pack_start(GTK_BOX(errorWidget), gtk_label_new("Date can't be equal"), FALSE, FALSE, 0);
            error = 1;
        }else if(dateCompare > 0){
            gtk_box_pack_start(GTK_BOX(errorWidget), gtk_label_new("Date of the first season part can't be superior to the second part start date"), FALSE, FALSE, 0);
            error = 1;
        }

        if(error == 0 && g_date_compare(currentDate, date[0]) >= 0){
            gtk_box_pack_start(GTK_BOX(errorWidget), gtk_label_new("Date of the first part, need to be after today"), FALSE, FALSE, 0);
            error = 1;
        }

        if(error == 0 && g_date_days_between(date[0], date[1]) < 19 * 7){
            if(errorWidget != NULL)
                gtk_box_pack_start(GTK_BOX(errorWidget), gtk_label_new("Start date of the second part, is before the end of the first part"), FALSE, FALSE, 0);
            error = 1;
        }

        if(error == 0){
            query = prepareQuery(allMatchParam->mainParam->allCalbackParam->mainParam->databaseInfo,
                    "SELECT\n"
                    "      id,\n"
                    "      stadium\n"
                    "FROM \"Team\"\n"
                    "WHERE \"idLeague\" = $1");

            bindParam(query, allMatchParam->mainParam->id, 0);

            queryResult = executePrepareStatement(query);

            if(queryResult->error != 1){
                fetchAllResult(queryResult, &finalData);

                if(queryResult->numberOfrow != 20){
                    gtk_box_pack_start(GTK_BOX(errorWidget), gtk_label_new("Twenty team are needed to generate the match"), FALSE, FALSE, 0);
                    error = 1;
                }

                if(error == 0){
                    /*for (int i = 0; i < queryResult->numberOfRow; ++i) {
                        nbSort = rand() % queryResult->numberOfRow - 1;
                        temp = finalData[i];
                        finalData[i] = finalData[nbSort];1
                        finalData[nbSort] = temp;
                    }*/

                    statement = g_string_append(statement,
                            "INSERT INTO \"Match\"(\"homeTeam\",\n"
                            "                      \"outsideTeam\",\n"
                            "                       date,\n"
                            "                       \"idLeague\",\n"
                            "                       stadium)\n"
                            "                    VALUES ");


                    query = prepareQuery(allMatchParam->mainParam->allCalbackParam->mainParam->databaseInfo, statement->str);

                    int days = (nmb % 2 == 0) ? nmb -1 : nmb;
                    for (int i=0; i<days; i++) {
                        g_date_strftime(tempChar, 20, "%Y-%m-%d", date[0]);
                        pointerChar = g_strdup(tempChar);

                        for (int j=0; j<nmb/2; j++) {
                            int t1 = (j+i) % nmb;
                            int t2 = (nmb - j -1 +  i) % nmb;
                            
                            if(t1 == 0 || t2 == 0)
                                printf("%d & %d / Team n° %d vs team n° %d \n", i, j,t1, t2);
                            if(tmp == 1)
                                statement = g_string_append(statement, ", ");
                            tmp = 1;

                            statement = g_string_append(statement, "(");
                            for (int l = 0; l < 5; ++l) {
                                if(l != 4) {
                                    g_string_append_printf(statement, "$%d,", k++);
                                }else{
                                    g_string_append_printf(statement, "$%d", k++);
                                }
                            }
                            statement = g_string_append(statement, ")");


                            bindParam(query, finalData[t1][0], m++);
                            bindParam(query, finalData[t2][0], m++);
                            bindParam(query, pointerChar, m++);
                            bindParam(query, allMatchParam->mainParam->id, m++);
                            bindParam(query, finalData[t1][1], m++);
                        }
                        printf("\n \n");
                        g_date_add_days(date[0], 7);
                        if(i == 10)
                            break;
                    }

                    query->query = statement->str;
                    queryResult = executePrepareStatement(query);

                    if(queryResult->error == 1)
                        printf("error");

                    closePrepareStatement(query, queryResult, NULL);
                }

            }else{
                gtk_box_pack_start(GTK_BOX(errorWidget), gtk_label_new("Internal error"), FALSE, FALSE, 0);
                error = 1;
            }

        }


    }else{
        gtk_box_pack_start(GTK_BOX(errorWidget), gtk_label_new("Invalid date, please check your entry"), FALSE, FALSE, 0);
    }

    if(errorWidget != NULL)
        gtk_widget_show_all(errorWidget);

    if(date[0] != NULL)
        g_date_free(date[0]);

    if(date[1] != NULL)
        g_date_free(date[1]);

    g_free(currentTime);
    g_date_free(currentDate);

}