/*
Designed on Glade
compiled on GCC
*/

#include <gtk/gtk.h>
#include <cairo.h>
#include<stdbool.h>
#include "Sched.h"

#define DONE 100;

GtkBuilder *builder;
GUI gui;
Container container;
GtkWidget **entry, *switch_label, *warning_popup, *warning_container, *total_label;
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
    
    //starts the gtk loop
    gtk_init(&argc, &argv);

    /*builder object that gets the layout from a .glade file
    which is really just an XML file that stores all the
    info of all the objects
    */
    builder = gtk_builder_new_from_file("Project#2.glade");
    //creates the window
    container.window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    warning_popup = GTK_WIDGET(gtk_builder_get_object(builder, "warning"));

    //to be able to exit the window
    g_signal_connect(container.window,"destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    //connects all the signals to their respective objects
    gtk_builder_connect_signals(builder, NULL);

    //creates the container
    container.fixed_container = GTK_WIDGET(gtk_builder_get_object(builder, "fixed_container"));
    warning_container = GTK_WIDGET(gtk_builder_get_object(builder, "warning_box"));

    //creates the entry
    entry[0] = GTK_WIDGET(gtk_builder_get_object(builder, "burst_entry"));
    entry[1] = GTK_WIDGET(gtk_builder_get_object(builder, "arrival_entry"));
    entry[2] = GTK_WIDGET(gtk_builder_get_object(builder, "priority_entry"));

    gui.clear = GTK_WIDGET(gtk_builder_get_object(builder, "clear_button"));
    gui.start_button = GTK_WIDGET(gtk_builder_get_object(builder, "start_button"));

    //creates the switch gui
    gui.switch_gui = GTK_WIDGET(gtk_builder_get_object(builder, "switch"));

    switch_label = GTK_WIDGET(gtk_builder_get_object(builder, "switch_label"));
    total_label = GTK_WIDGET(gtk_builder_get_object(builder, "total_time"));
    
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


//sets the state of the program if its PP or FCFS
void state_set(GtkSwitch *toggle)
{
    sched.state =  gtk_switch_get_active(toggle);
    if(sched.state)
    {
        gtk_label_set_text(GTK_LABEL(switch_label), (const gchar*) "Premptive Priority");
        gtk_widget_set_sensitive(entry[2], TRUE);
    }
    else
    {
        gtk_label_set_text(GTK_LABEL(switch_label), (const gchar*) "First Come First Served");
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

    gtk_label_set_text(GTK_LABEL(total_label), (const gchar*) "Total Time:");

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

//returns the higher priority
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
    char total[50];
    bool element_matched = TRUE;
    gantt.draw_state = TRUE;

    //checks if the number of elements matched the arrival and priority
    if (sched.job_ctr != sched.arrival_ctr || sched.job_ctr != sched.priority_ctr && sched.state)
    {
        //executes the popup window
        gtk_dialog_run(GTK_DIALOG(warning_popup));
        gtk_widget_hide(warning_popup);
        element_matched = FALSE;
    }
    else if (sched.job_ctr != sched.arrival_ctr && sched.state == false)
    {
        gtk_dialog_run(GTK_DIALOG(warning_popup));
        gtk_widget_hide(warning_popup);
        element_matched = FALSE;
    }

    if(sched.state && element_matched)
    {
        sort();

        int i = highPriority(current_time);

        current_time = sched.arrival[i] + 1;
        sched.burst[i]--;
        gantt.ctr++;
        gantt.data[0] = sched.job[i];

        if (sched.burst[i] == 0)
        {
            sched.priority[i] = DONE;
            job_no--;
        }

        //no logic here just pure bruteforce
        /*
        constantly checks if the current index isnt equal to the new index
        which is returned if the current_time is less than or equal to
        sched.arrival[i] KEKW
        */
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
                sched.priority[i] = DONE;
                job_no--;
            }
        }  

        //prints the total time
        sprintf(total,"Total Time:%d", current_time);
        gtk_label_set_text(GTK_LABEL(total_label), (const gchar*) total);
    }
    else if (sched.state == FALSE && element_matched)
    {
        
        sort();
        for (int i = 0; i < sched.job_ctr; i++)
        {
            gantt.data[i] = sched.job[i];
            current_time += sched.burst[i];
            gantt.ctr++;
        }
        //prints the total time
        sprintf(total,"Total Time:%d", current_time);
        gtk_label_set_text(GTK_LABEL(total_label), (const gchar*) total);   
    }

    //checks if the elements matched and turns off the button
    if(element_matched)
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
            //draws the box
            cairo_set_source_rgb(cr, 1, 1, 1);
            cairo_rectangle(cr, box_spacing, 30, 40, 30);
            cairo_set_line_width(cr, 1);
            cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
            cairo_stroke(cr);

            //draws the process_no inside the box
            cairo_set_source_rgb(cr, 1, 1, 1);
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