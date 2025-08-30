/**
 * file main.c
 * brief Main entry point for the Hotel Booking System
 * 
 * This file contains the main function and global variable definitions
 * for the hotel booking system application. It initializes the system
 * and starts the GTK main loop.
 */

 #include <gtk/gtk.h>
 #include "gui_management.h"
 #include "file_operations.h"
 
 // Global variable definitions (declared in gui_management.h)
 GtkWidget *window = NULL;              ///< Main application window
 GtkWidget *stack = NULL;               ///< Stack container for screen management
 GtkWidget *status_label = NULL;        ///< Status bar label for messages
 char current_user[MAX_LEN] = "";       ///< Currently logged in user
 bool is_admin_logged_in = false;       ///< Admin login status flag
 
 /**
  * brief Main function - application entry point
  * 
  * Initializes the data files and starts the GUI application.
  * 
  * param argc Number of command line arguments
  * param argv Array of command line arguments
  * return Application exit code (0 for normal exit)
  */
 int main(int argc, char *argv[]) {
     // Initialize data files (create if they don't exist)
     initialize_files();
     
     // Start the GUI application
     initialize_gui(argc, argv);
     
     // Run the GTK main loop (blocks until application exits)
     gtk_main();
     
     return 0;
 }