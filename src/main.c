#include "widgets.h"
#include "client.h"
#include "client_menu.h"

int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("com.example.GtkMenu", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;
}