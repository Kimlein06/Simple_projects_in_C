#include <gtk/gtk.h>

GtkTextBuffer *buffer;
GList *undo_stack = NULL;
GList *redo_stack = NULL;

void save_current_text_to_stack(GList **stack) {
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    *stack = g_list_prepend(*stack, text);
}

void on_undo_clicked(GtkWidget *widget, gpointer data) {
    if (undo_stack) {
        save_current_text_to_stack(&redo_stack);
        gchar *last = (gchar*) undo_stack->data;
        gtk_text_buffer_set_text(buffer, last, -1);
        undo_stack = g_list_delete_link(undo_stack, undo_stack);
    }
}

void on_redo_clicked(GtkWidget *widget, gpointer data) {
    if (redo_stack) {
        save_current_text_to_stack(&undo_stack);
        gchar *last = (gchar*) redo_stack->data;
        gtk_text_buffer_set_text(buffer, last, -1);
        redo_stack = g_list_delete_link(redo_stack, redo_stack);
    }
}

void on_commit_clicked(GtkWidget *widget, gpointer data) {
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    undo_stack = g_list_prepend(undo_stack, g_strdup(text));
    g_free(text);

    g_list_free_full(redo_stack, g_free);  // clear redo stack
    redo_stack = NULL;
}

void on_save_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Save File",
                                         GTK_WINDOW(data),
                                         action,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Save", GTK_RESPONSE_ACCEPT,
                                         NULL);
    chooser = GTK_FILE_CHOOSER(dialog);
    gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename(chooser);

        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(buffer, &start);
        gtk_text_buffer_get_end_iter(buffer, &end);
        gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

        FILE *f = fopen(filename, "w");
        if (f != NULL) {
            fputs(text, f);
            fclose(f);
            g_print("File saved: %s\\n", filename);
        } else {
            g_print("Failed to save file.\\n");
        }

        g_free(text);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void on_text_changed(GtkTextBuffer *buf, gpointer user_data) {
    save_current_text_to_stack(&undo_stack);
    g_list_free_full(redo_stack, g_free);
    redo_stack = NULL;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "C Text Editor (Undo/Redo)");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *undo_btn = gtk_button_new_with_label("Undo");
    GtkWidget *redo_btn = gtk_button_new_with_label("Redo");
    GtkWidget *commit_btn = gtk_button_new_with_label("Commit");
    GtkWidget *save_btn = gtk_button_new_with_label("Save to file");
    GtkWidget *textview = gtk_text_view_new();

    gtk_box_pack_start(GTK_BOX(toolbar), undo_btn, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(toolbar), redo_btn, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(toolbar), commit_btn, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(toolbar), save_btn, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), textview, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    //g_signal_connect(buffer, "changed", G_CALLBACK(on_text_changed), NULL);
    g_signal_connect(undo_btn, "clicked", G_CALLBACK(on_undo_clicked), NULL);
    g_signal_connect(redo_btn, "clicked", G_CALLBACK(on_redo_clicked), NULL);
    g_signal_connect(commit_btn, "clicked", G_CALLBACK(on_commit_clicked), NULL);
    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_save_clicked), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
