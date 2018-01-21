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

    /*
     * Get the structure, pass on param
     */
    CallbackParam * functionCallbackParam = (CallbackParam *) data;

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

            g_signal_connect(gtk_builder_get_object(tabBuilder, "nameEntry"), "activate", G_CALLBACK(test), NULL);

            initTreeView(mainNotebookContent, functionCallbackParam);

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


void test(GtkWidget * widget, gpointer * data){
    printf("%s \n", gtk_entry_get_text(GTK_ENTRY(widget)));
}

