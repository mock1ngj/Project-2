/*
Designed on Glade
compiled on GCC
*/

#include <gtk/gtk.h>
#include <stdbool.h>
#include "Sched.h"

GtkBuilder *builder;
GUI gui;
Container container;
GtkWidget **entry, **label;
GtkTextBuffer *job, *burst, *arrival, *priority;
GtkTextIter end, start;
data sched;


int main(int argc, char *argv[])
{

    entry = g_new(GtkWidget, 3);
    label = g_new(GtkWidget, 4);
    //starts the gtk loop
    gtk_init(&argc, &argv);

    /*builder object that gets the layout from a .glade file
    which is really just an XML file that stores all the
    info of all the objects
    */
    builder = gtk_builder_new_from_file("DPAS.glade");
    //creates the window
    container.window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

    //to be able to exit the window
    g_signal_connect(container.window,"destroy", G_CALLBACK(gtk_main_quit), NULL);

    //connects all the signals to their respective objects
    gtk_builder_connect_signals(builder, NULL);

    //creates the container
    container.fixed_container = GTK_WIDGET(gtk_builder_get_object(builder, "fixed_container"));

    //creates the entry
    entry[0] = GTK_WIDGET(gtk_builder_get_object(builder, "burst_entry"));
    entry[1] = GTK_WIDGET(gtk_builder_get_object(builder, "arrival_entry"));
    entry[2] = GTK_WIDGET(gtk_builder_get_object(builder, "priority_entry"));

    gui.clear = GTK_WIDGET(gtk_builder_get_object(builder, "clear_button"));

    //creates the switch gui
    gui.switch_gui = GTK_WIDGET(gtk_builder_get_object(builder, "switch"));

    label[0] = GTK_WIDGET(gtk_builder_get_object(builder, "switch_label"));
    gui.job_view = GTK_WIDGET(gtk_builder_get_object(builder, "job_view"));
    gui.burst_view = GTK_WIDGET(gtk_builder_get_object(builder, "burst_view"));
    gui.arrival_view = GTK_WIDGET(gtk_builder_get_object(builder, "arrival_view"));
    gui.priority_view = GTK_WIDGET(gtk_builder_get_object(builder, "priority_view"));

    /*
    this points the text buffers to their respective text area in order
    to show the user input in the text area
    */
    job = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui.job_view));
    burst = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui.burst_view));
    arrival = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui.arrival_view));
    priority = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui.priority_view));

    //to be able to see the different widgets/objects
    gtk_widget_show_all(container.window);

    g_object_unref(builder);

    //main loop for the GTK
    gtk_main();
    return EXIT_SUCCESS;
}


void state_set(GtkSwitch *toggle)
{
    gboolean state =  gtk_switch_get_active(toggle);
    if(state)
    {
        gtk_label_set_text(GTK_LABEL(label[0]), (const gchar*) "Premptive Priority");
        gtk_widget_set_sensitive(entry[2], TRUE);
        sched.state = TRUE;
    }
    else
    {
        gtk_label_set_text(GTK_LABEL(label[0]), (const gchar*) "First Come First Served");
        gtk_widget_set_sensitive(entry[2], FALSE);
        sched.state = FALSE;
    }
}

void burst_entry_activate(GtkEntry *burst_entry)
{
    char input[10], job_no[10];

    sprintf(job_no, "Job%d\n\n", sched.burst_ctr+1);
    sprintf(input, "%s\n\n", gtk_entry_get_text(burst_entry));

    sched.burst[sched.burst_ctr] = atoi(gtk_entry_get_text(burst_entry));

    //prints the job no. on the job text buffer which points to the job text area
    gtk_text_buffer_get_end_iter(job, &end);
    gtk_text_buffer_insert(job, &end, (const gchar*) job_no, (gint*) -1);

    //prints the burst on the burst text buffer which points to the burst text area
    gtk_text_buffer_get_end_iter(burst, &end);
    gtk_text_buffer_insert(burst, &end, (const gchar*) input, (gint*) -1);

    sched.burst_ctr++;

    if (sched.burst_ctr == 8)
    {
        gtk_widget_set_sensitive(burst_entry, FALSE);
    }

    gtk_entry_set_text(burst_entry, "");
}

void arrival_entry_activate(GtkEntry *arrival_entry)
{
    char input[10];

    sprintf(input, "%s\n\n", gtk_entry_get_text(arrival_entry));

    sched.arrival[sched.arrival_ctr] = atoi(gtk_entry_get_text(arrival_entry));

    gtk_text_buffer_get_end_iter(arrival, &end);
    gtk_text_buffer_insert(arrival, &end, (const gchar*) input, (gint*) -1);

    sched.arrival_ctr++;

    if (sched.arrival_ctr == sched.burst_ctr)
    {
        gtk_widget_set_sensitive(arrival_entry, FALSE);
    }

    gtk_entry_set_text(arrival_entry, "");
}

void priority_entry_activate(GtkEntry *priority_entry)
{
    char input[10];

    sprintf(input, "%s\n\n", gtk_entry_get_text(priority_entry));

    sched.priority[sched.priority_ctr] = atoi(gtk_entry_get_text(priority_entry));

    gtk_text_buffer_get_end_iter(priority, &end);
    gtk_text_buffer_insert(priority, &end, (const gchar*) input, (gint*) -1);

    sched.priority_ctr++;

    if (sched.priority_ctr == sched.burst_ctr)
    {
        gtk_widget_set_sensitive(priority_entry, FALSE);
    }

    gtk_entry_set_text(priority_entry, "");
}

void clear_button_clicked(GtkButton *clear)
{
    gtk_text_buffer_get_bounds(job, &start, &end);
    gtk_text_buffer_delete(job, &start, &end);
    gtk_text_buffer_get_bounds(burst, &start, &end);
    gtk_text_buffer_delete(burst, &start, &end);
    gtk_text_buffer_get_bounds(arrival, &start, &end);
    gtk_text_buffer_delete(arrival, &start, &end);
    gtk_text_buffer_get_bounds(priority, &start, &end);
    gtk_text_buffer_delete(priority, &start, &end);

    for (int i = 0; i < 0; i++)
    {
        sched.burst[i] = NULL;
        sched.arrival[i] = NULL;
        sched.priority[i] = NULL;
    }
    
    sched.arrival_ctr = 0;
    sched.burst_ctr = 0;
    sched.priority_ctr = 0;

    gtk_widget_set_sensitive(entry[0], TRUE);
    gtk_widget_set_sensitive(entry[1], TRUE);

    if(sched.state)
    {
        gtk_widget_set_sensitive(entry[2], TRUE);
    }
}

void ok_clicked(GtkButton *ok_button)
{

}