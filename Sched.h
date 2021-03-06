#include <gtk/gtk.h>
#include <stdbool.h>
/*declares all the necessary objects and stores 
them in a struct for readability and easy access
on main
*/
typedef struct
{
    GtkWidget *switch_gui;
    GtkWidget *job_view;
    GtkWidget *burst_view;
    GtkWidget *arrival_view;
    GtkWidget *priority_view;
    GtkWidget *clear;
    GtkWidget *start_button;
    GtkWidget *gantt_chart;
    bool draw;
}GUI;

typedef struct
{
    GtkWidget *window;
    GtkWidget *best_fit_container;
    GtkWidget *fixed_container;
    GtkWidget *fifo_container;
    GtkWidget *stack;
}Container;

typedef struct
{
    int data[50];
    int ctr;
    bool draw_state;
}chart;

typedef struct
{
    int job[8];
    int burst[8];
    int arrival[8];
    int priority[8];
    int job_ctr;
    int arrival_ctr;
    int priority_ctr;
    gboolean state;
}data;