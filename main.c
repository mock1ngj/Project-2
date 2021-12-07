/*
Designed on Glade
compiled on GCC
*/

#include <gtk/gtk.h>
#include <cairo.h>
#include "Sched.h"

GtkBuilder *builder;
GUI gui;
Container container;
GtkWidget **entry, **label, *test, *test_box;
GtkTextBuffer *job_view, *burst_view, *arrival_view, *priority_view;
GtkTextIter end, start;
data sched;
chart gantt;

int main(int argc, char *argv[])
{
    sched.job_ctr = 0;
    sched.arrival_ctr = 0;
    sched.priority_ctr = 0;
    gantt.ctr = 0;

    gui.draw = false;

    //array declaration for the gtk widgets
    entry = g_new(GtkWidget, 3);
    label = g_new(GtkWidget, 4);

    
    //starts the gtk loop
    gtk_init(&argc, &argv);

    /*builder object that gets the layout from a .glade file
    which is really just an XML file that stores all the
    info of all the objects
    */
    builder = gtk_builder_new_from_file("Project#2.glade");
    //creates the window
    container.window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    test = GTK_WIDGET(gtk_builder_get_object(builder, "warning"));

    //to be able to exit the window
    g_signal_connect(container.window,"destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    //connects all the signals to their respective objects
    gtk_builder_connect_signals(builder, NULL);

    //creates the container
    container.fixed_container = GTK_WIDGET(gtk_builder_get_object(builder, "fixed_container"));
    test_box = GTK_WIDGET(gtk_builder_get_object(builder, "warning_box"));

    //creates the entry
    entry[0] = GTK_WIDGET(gtk_builder_get_object(builder, "burst_entry"));
    entry[1] = GTK_WIDGET(gtk_builder_get_object(builder, "arrival_entry"));
    entry[2] = GTK_WIDGET(gtk_builder_get_object(builder, "priority_entry"));

    gui.clear = GTK_WIDGET(gtk_builder_get_object(builder, "clear_button"));
    gui.start_button = GTK_WIDGET(gtk_builder_get_object(builder, "start_button"));

    //creates the switch gui
    gui.switch_gui = GTK_WIDGET(gtk_builder_get_object(builder, "switch"));

    label[0] = GTK_WIDGET(gtk_builder_get_object(builder, "switch_label"));
    
    //text area for the different entries
    gui.job_view = GTK_WIDGET(gtk_builder_get_object(builder, "job_view"));
    gui.burst_view = GTK_WIDGET(gtk_builder_get_object(builder, "burst_view"));
    gui.arrival_view = GTK_WIDGET(gtk_builder_get_object(builder, "arrival_view"));
    gui.priority_view = GTK_WIDGET(gtk_builder_get_object(builder, "priority_view"));

    gui.gantt_chart = GTK_WIDGET(gtk_builder_get_object(builder, "gantt_chart"));

    //turns off the priority entry
    gtk_widget_set_sensitive(entry[2], FALSE);

    /*
    this points the text buffers to their respective text area in order
    to show the user input in the text area
    */
    job_view = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui.job_view));
    burst_view = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui.burst_view));
    arrival_view = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui.arrival_view));
    priority_view = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui.priority_view));

    //to be able to see the different widgets/objects
    gtk_widget_show_all(container.window);

    g_object_unref(builder);

    //main loop for the GTK
    gtk_main();
    return EXIT_SUCCESS;
}


void state_set(GtkSwitch *toggle)
{
    sched.state =  gtk_switch_get_active(toggle);
    if(sched.state)
    {
        gtk_label_set_text(GTK_LABEL(label[0]), (const gchar*) "Premptive Priority");
        gtk_widget_set_sensitive(entry[2], TRUE);
    }
    else
    {
        gtk_label_set_text(GTK_LABEL(label[0]), (const gchar*) "First Come First Served");
        gtk_widget_set_sensitive(entry[2], FALSE);
    }
}

void burst_entry_activate(GtkEntry *burst_entry)
{
    char input[10], job_no[10];

    sprintf(job_no, "Job%d\n\n", sched.job_ctr+1);
    sprintf(input, "%s\n\n", gtk_entry_get_text(burst_entry));

    sched.job[sched.job_ctr] = sched.job_ctr+1;
    sched.burst[sched.job_ctr] = atoi(gtk_entry_get_text(burst_entry));

    //prints the job no. on the job text buffer which points to the job text area
    gtk_text_buffer_get_end_iter(job_view, &end);
    gtk_text_buffer_insert(job_view, &end, (const gchar*) job_no, (gint*) -1);

    //prints the burst on the burst text buffer which points to the burst text area
    gtk_text_buffer_get_end_iter(burst_view, &end);
    gtk_text_buffer_insert(burst_view, &end, (const gchar*) input, (gint*) -1);

    sched.job_ctr++;

    if (sched.job_ctr == 8)
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

    gtk_text_buffer_get_end_iter(arrival_view, &end);
    gtk_text_buffer_insert(arrival_view, &end, (const gchar*) input, (gint*) -1);

    sched.arrival_ctr++;

    if (sched.arrival_ctr == sched.job_ctr)
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

    gtk_text_buffer_get_end_iter(priority_view, &end);
    gtk_text_buffer_insert(priority_view, &end, (const gchar*) input, (gint*) -1);

    sched.priority_ctr++;

    if (sched.priority_ctr == sched.job_ctr)
    {
        gtk_widget_set_sensitive(priority_entry, FALSE);
    }

    gtk_entry_set_text(priority_entry, "");
}

void clear_button_clicked(GtkButton *clear)
{


    /*
    deletes the content of the text area
    */
    gantt.draw_state = FALSE;
    gtk_text_buffer_get_bounds(job_view, &start, &end);
    gtk_text_buffer_delete(job_view, &start, &end);
    gtk_text_buffer_get_bounds(burst_view, &start, &end);
    gtk_text_buffer_delete(burst_view, &start, &end);
    gtk_text_buffer_get_bounds(arrival_view, &start, &end);
    gtk_text_buffer_delete(arrival_view, &start, &end);
    gtk_text_buffer_get_bounds(priority_view, &start, &end);
    gtk_text_buffer_delete(priority_view, &start, &end);

    for (int i = 0; i < 0; i++)
    {
        sched.burst[i] = NULL;
        sched.arrival[i] = NULL;
        sched.priority[i] = NULL;
    }
    
    for (int i = 0; i < 50; i++)
    {
        gantt.data[i] = NULL;
    }
    
    sched.arrival_ctr = 0;
    sched.job_ctr = 0;
    sched.priority_ctr = 0;
    gantt.ctr = 0;

    gtk_widget_set_sensitive(entry[0], TRUE);
    gtk_widget_set_sensitive(entry[1], TRUE);
    gtk_widget_set_sensitive(gui.start_button, TRUE);

    if(sched.state)
    {
        gtk_widget_set_sensitive(entry[2], TRUE);
    }
    //updates the gantt chart
    gtk_widget_queue_draw(gui.gantt_chart);
}

void sort()
{
    int temp;

    //sorts the elements by arrival time
    for (int i = 0; i < sched.job_ctr; i++)
    {
        for (int j = 1; j < (sched.job_ctr-i); j++)
        {
            if(sched.arrival[j-1] > sched.arrival[j])
            {
                temp = sched.job[j-1];
                sched.job[j-1] = sched.job[j];
                sched.job[j] = temp;

                temp = sched.burst[j-1];
                sched.burst[j-1] = sched.burst[j];
                sched.burst[j] = temp;

                temp = sched.arrival[j-1];
                sched.arrival[j-1] = sched.arrival[j];
                sched.arrival[j] = temp;

                temp = sched.priority[j-1];
                sched.priority[j-1] = sched.priority[j];
                sched.priority[j] = temp;
            }
        }
    }
}

int highPriority(int current_time)
{
    int min_val = sched.priority[0];
    int high_priority = 0;
    for (int i = 0;i < sched.job_ctr && sched.arrival[i] <= current_time; i++)
    {
        if(sched.priority[i]<min_val)
        {
            min_val = sched.priority[i];
            high_priority = i;
        }
    }
    return high_priority;
}

void ok_clicked(GtkButton *ok_button)
{
    int current_time = 0, job_no = sched.job_ctr;
    gantt.draw_state = TRUE;

    if (sched.job_ctr < sched.arrival_ctr || sched.job_ctr < sched.priority_ctr)
    {
        gtk_dialog_run(GTK_DIALOG(test));
        
    }
    

    if(sched.state && sched.job_ctr == sched.arrival_ctr && sched.job_ctr == sched.priority_ctr)
    {
        sort();

        int i = highPriority(current_time);

        current_time = sched.arrival[i] + 1;
        sched.burst[i]--;
        gantt.ctr++;
        gantt.data[0] = sched.job[i];

        if (sched.burst[i] == 0)
        {
            sched.priority[i] = 100;
            job_no--;
        }

        for (int prev_indx, j = 1; job_no > 0;)
        {
            prev_indx = i;
            i = highPriority(current_time);
            if (prev_indx != i)
            {
                gantt.data[j] = sched.job[i];
                j++;
                gantt.ctr++;
            }

            current_time++;
            sched.burst[i]--;

            if (sched.burst[i] == 0)
            {
                sched.priority[i] = 100;
                job_no--;
            }
        }  
    }
    else if (sched.state == FALSE && sched.job_ctr == sched.arrival_ctr)
    {
        
        sort();
        for (int i = 0; i < sched.job_ctr; i++)
        {
            gantt.data[i] = sched.job[i];
            gantt.ctr++;
        }   
    }

    gtk_widget_set_sensitive(gui.start_button, FALSE);
    //updates gantt chart
    gtk_widget_queue_draw(gui.gantt_chart);

}

gboolean draw_gantt_chart (GtkWidget *widget, cairo_t *cr, gpointer *data)
{
    cairo_text_extents_t extents;
    char process_no[5];
    double box_spacing = 50;
    double font_spacing = 60;
    for (int i = 0; i < gantt.ctr; i++)
    {
        if (gantt.draw_state)
        {
            cairo_set_source_rgb(cr, 0, 0.5, 0.95);
            cairo_rectangle(cr, box_spacing, 30, 40, 30);
            cairo_set_line_width(cr, 1);
            cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
            cairo_stroke(cr);
            cairo_set_source_rgb(cr, 0, 0, 0);
            sprintf(process_no, "P%d", gantt.data[i]);
            cairo_text_extents(cr, process_no, &extents);
            cairo_set_font_size(cr, 15);
            cairo_move_to(cr, font_spacing, 50);
            cairo_show_text(cr, process_no);
            box_spacing+=40;
            font_spacing+=40;
        }
    }
}