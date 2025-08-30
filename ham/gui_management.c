/**
 * file gui_management.c
 * brief GTK+ GUI implementation for the Hotel Booking System
 * 
 * This file contains the complete GUI implementation including widget creation,
 * screen navigation, and callback functions for user interactions.
 */

 #include "gui_management.h"
 #include "file_operations.h"
 #include "validation.h"
 #include "date_utilities.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 
 /**
  * brief Structure to hold references to all GUI widgets
  * 
  * This structure keeps track of all widgets that need to be accessed
  * from callback functions, avoiding the need for global variables.
  */
 typedef struct {
     // Login widgets
     GtkWidget *username_entry;          ///< Username entry field
     GtkWidget *phone_entry;             ///< Phone number entry field
     GtkWidget *password_entry;          ///< Password entry field
     
     // Room browsing widgets
     GtkWidget *min_price_entry;         ///< Minimum price filter entry
     GtkWidget *max_price_entry;         ///< Maximum price filter entry
     GtkWidget *type_combo;              ///< Room type filter combo box
     GtkWidget *facilities_entry;        ///< Facilities filter entry
     GtkWidget *room_tree;               ///< Room list tree view
     GtkWidget *checkin_entry;           ///< Check-in date entry
     GtkWidget *checkout_entry;          ///< Check-out date entry
     
     // Booking management widgets
     GtkWidget *booking_tree;            ///< Booking list tree view
     
     // Admin widgets
     GtkWidget *new_password_entry;      ///< New password entry field
     GtkWidget *confirm_password_entry;  ///< Confirm password entry field
     GtkWidget *date_entry;              ///< Date entry for revenue reports
     GtkWidget *booking_id_entry;        ///< Booking ID filter entry
     GtkWidget *admin_username_entry;    ///< Username filter entry
     GtkWidget *start_date_entry;        ///< Start date filter entry
     GtkWidget *end_date_entry;          ///< End date filter entry
     GtkWidget *admin_bookings_tree;     ///< Admin bookings tree view
     
     // Receipt widgets
     GtkWidget *receipt_view;            ///< Receipt display text view
     
     // Profile widgets
     GtkWidget *full_name_entry;         ///< Full name entry field
     GtkWidget *id_entry;                ///< ID number entry field
     GtkWidget *email_entry;             ///< Email address entry field
     GtkWidget *address_entry;           ///< Address entry field
     GtkWidget *phone_entry_profile;     ///< Phone number entry field (profile)
     
     // Signup widgets
     GtkWidget *signup_username_entry;   ///< Username entry field (signup)
     GtkWidget *signup_phone_entry;      ///< Phone number entry field (signup)
     GtkWidget *signup_confirm_phone_entry; ///< Confirm phone entry field (signup)
 } AppWidgets;
 
 static AppWidgets widgets;  ///< Instance of widget storage structure
 
 // Helper function declarations
 static GtkListStore* create_room_list_store(void);
 static GtkListStore* create_booking_list_store(void);
 static GtkListStore* create_admin_bookings_list_store(void);
 static void load_rooms_into_store(GtkListStore *store, RoomFilter *filter);
 static void load_user_bookings_into_store(GtkListStore *store, const char *username);
 static void load_all_bookings_into_store(GtkListStore *store, BookingFilter *filter);
 static void display_receipt_in_buffer(Booking *booking, GtkTextBuffer *buffer);
 static void load_receipt_history_for_user(const char *username, GtkTextBuffer *buffer);
 static void update_user_profile_dialog(void);
 
 /**
  * brief Creates a list store for room tree view
  * 
  * Initializes a GTK list store with the appropriate column types
  * for displaying room information.
  * 
  * return Newly created GtkListStore
  */
 static GtkListStore* create_room_list_store(void) {
     return gtk_list_store_new(5, G_TYPE_INT, G_TYPE_STRING, G_TYPE_FLOAT, G_TYPE_STRING, G_TYPE_STRING);
 }
 
 /**
  * brief Creates a list store for booking tree view
  * 
  * Initializes a GTK list store with the appropriate column types
  * for displaying booking information.
  * 
  * return Newly created GtkListStore
  */
 static GtkListStore* create_booking_list_store(void) {
     return gtk_list_store_new(6, G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_FLOAT, G_TYPE_STRING);
 }
 
 /**
  * brief Creates a list store for admin bookings tree view
  * 
  * Initializes a GTK list store with the appropriate column types
  * for displaying booking information in the admin view.
  * 
  * return Newly created GtkListStore
  */
 static GtkListStore* create_admin_bookings_list_store(void) {
     return gtk_list_store_new(8, G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, 
                              G_TYPE_STRING, G_TYPE_STRING, G_TYPE_FLOAT, G_TYPE_STRING);
 }
 
 /**
  * brief Loads rooms into a list store with optional filtering
  * 
  * Reads rooms from the room data file, applies filters if provided,
  * sorts them by price, and adds them to the specified list store.
  * 
  * param store The list store to populate with room data
  * param filter Optional filter criteria for rooms
  */
 static void load_rooms_into_store(GtkListStore *store, RoomFilter *filter) {
     FILE *file = fopen(ROOM_FILE, "r");
     if (file == NULL) return;
 
     Room rooms[MAX_ROOMS];
     int room_count = 0;
     char line[MAX_LEN];
     
     // Read all rooms from file
     while (fgets(line, sizeof(line), file) && room_count < MAX_ROOMS) {
         Room room;
         // Parse room data from line
         sscanf(line, "%d:%[^:]:%f:%[^:]:%[^\n]", 
                &room.room_number, room.type, &room.price, room.status, room.facilities);
         
         // Apply filters if provided
         if (filter != NULL) {
             if (filter->min_price > 0 && room.price < filter->min_price) continue;
             if (filter->max_price > 0 && room.price > filter->max_price) continue;
             if (strlen(filter->type) > 0 && strcmp(room.type, filter->type) != 0) continue;
             if (strlen(filter->facilities) > 0 && strstr(room.facilities, filter->facilities) == NULL) continue;
         }
         
         // Add room to array
         rooms[room_count] = room;
         room_count++;
     }
     fclose(file);
 
     // Sort rooms by price (ascending order)
     qsort(rooms, room_count, sizeof(Room), compare_rooms_by_price);
 
     // Add sorted rooms to the list store
     for (int i = 0; i < room_count; i++) {
         GtkTreeIter iter;
         gtk_list_store_append(store, &iter);
         gtk_list_store_set(store, &iter,
                            0, rooms[i].room_number,
                            1, rooms[i].type,
                            2, rooms[i].price,
                            3, rooms[i].status,
                            4, rooms[i].facilities,
                            -1);
     }
 }
 
 /**
  * brief Loads user bookings into a list store
  * 
  * Reads bookings from the booking data file for a specific user
  * and adds them to the specified list store.
  * 
  * param store The list store to populate with booking data
  * param username The user whose bookings to load
  */
 static void load_user_bookings_into_store(GtkListStore *store, const char *username) {
     FILE *file = fopen(BOOKING_FILE, "r");
     if (file == NULL) return;
 
     char line[MAX_LEN * 3];
     char file_username[MAX_LEN], status[20], booking_date[20], check_in_date[20], check_out_date[20];
     int room_number, booking_id;
     float total_price;
 
     // Read all bookings from file
     while (fgets(line, sizeof(line), file)) {
         // Parse booking data from line
         sscanf(line, "%[^:]:%d:%[^:]:%[^:]:%[^:]:%f:%[^:]:%d",
                file_username, &room_number, booking_date,
                check_in_date, check_out_date, &total_price,
                status, &booking_id);
         
         // Only add bookings for the specified user
         if (strcmp(file_username, username) == 0) {
             GtkTreeIter iter;
             gtk_list_store_append(store, &iter);
             gtk_list_store_set(store, &iter,
                                0, booking_id,
                                1, room_number,
                                2, check_in_date,
                                3, check_out_date,
                                4, total_price,
                                5, status,
                                -1);
         }
     }
     fclose(file);
 }
 
 /**
  * brief Loads all bookings into a list store with optional filtering
  * 
  * Reads all bookings from the booking data file, applies filters if provided,
  * and adds them to the specified list store.
  * 
  * param store The list store to populate with booking data
  * param filter Optional filter criteria for bookings
  */
 static void load_all_bookings_into_store(GtkListStore *store, BookingFilter *filter) {
     FILE *file = fopen(BOOKING_FILE, "r");
     if (file == NULL) return;
 
     char line[MAX_LEN * 3];
     char username[MAX_LEN], status[20], booking_date[20], check_in_date[20], check_out_date[20];
     int room_number, booking_id;
     float total_price;
 
     // Read all bookings from file
     while (fgets(line, sizeof(line), file)) {
         // Parse booking data from line
         sscanf(line, "%[^:]:%d:%[^:]:%[^:]:%[^:]:%f:%[^:]:%d",
                username, &room_number, booking_date,
                check_in_date, check_out_date, &total_price,
                status, &booking_id);
 
         // Apply filters if provided
         if (filter != NULL) {
             if (filter->booking_id != 0 && booking_id != filter->booking_id) continue;
             if (strlen(filter->username) > 0 && strstr(username, filter->username) == NULL) continue;
             
             // Date filtering based on booking date
             if (strlen(filter->start_date) > 0 || strlen(filter->end_date) > 0) {
                 char booking_date_only[11];
                 strncpy(booking_date_only, booking_date, 10);
                 booking_date_only[10] = '\0';
                 
                 if (strlen(filter->start_date) > 0 && strcmp(booking_date_only, filter->start_date) < 0) continue;
                 if (strlen(filter->end_date) > 0 && strcmp(booking_date_only, filter->end_date) > 0) continue;
             }
         }
 
         // Add booking to list store
         GtkTreeIter iter;
         gtk_list_store_append(store, &iter);
         gtk_list_store_set(store, &iter,
                            0, booking_id,
                            1, room_number,
                            2, username,
                            3, booking_date,
                            4, check_in_date,
                            5, check_out_date,
                            6, total_price,
                            7, status,
                            -1);
     }
     fclose(file);
 }
 
 /**
  * brief Displays a booking receipt in a text buffer
  * 
  * Formats a booking's details as a receipt and inserts it into
  * the specified text buffer.
  * 
  * param booking The booking to display as a receipt
  * param buffer The text buffer to insert the receipt into
  */
 static void display_receipt_in_buffer(Booking *booking, GtkTextBuffer *buffer) {
     GtkTextIter iter;
     gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);
     
     // Insert receipt header
     gtk_text_buffer_insert(buffer, &iter, "========== BOOKING RECEIPT ==========\n", -1);
     
     // Format and insert booking details
     char line[200];
     sprintf(line, "Booking ID: %d\n", booking->booking_id);
     gtk_text_buffer_insert(buffer, &iter, line, -1);
     
     sprintf(line, "Customer: %s\n", booking->username);
     gtk_text_buffer_insert(buffer, &iter, line, -1);
     
     sprintf(line, "Room Number: %d\n", booking->room_number);
     gtk_text_buffer_insert(buffer, &iter, line, -1);
     
     sprintf(line, "Booking Date: %s\n", booking->booking_date);
     gtk_text_buffer_insert(buffer, &iter, line, -1);
     
     sprintf(line, "Check-in: %s\n", booking->check_in_date);
     gtk_text_buffer_insert(buffer, &iter, line, -1);
     
     sprintf(line, "Check-out: %s\n", booking->check_out_date);
     gtk_text_buffer_insert(buffer, &iter, line, -1);
     
     sprintf(line, "Total Price: $%.2f\n", booking->total_price);
     gtk_text_buffer_insert(buffer, &iter, line, -1);
     
     sprintf(line, "Status: %s\n", booking->status);
     gtk_text_buffer_insert(buffer, &iter, line, -1);
     
     // Insert receipt footer
     gtk_text_buffer_insert(buffer, &iter, "=====================================\n", -1);
 }
 
 /**
  * brief Loads receipt history for a user into a text buffer
  * 
  * Finds all bookings for a specific user and displays them as
  * receipts in the specified text buffer.
  * 
  * param username The user whose receipt history to load
  * param buffer The text buffer to insert the receipts into
  */
 static void load_receipt_history_for_user(const char *username, GtkTextBuffer *buffer) {
     FILE *file = fopen(BOOKING_FILE, "r");
     if (file == NULL) return;
 
     Booking booking;
     char line[MAX_LEN * 3];
     int count = 0;
     
     // Set initial text in buffer
     gtk_text_buffer_set_text(buffer, "========== RECEIPT HISTORY ==========\n\n", -1);
     
     // Read all bookings from file
     while (fgets(line, sizeof(line), file)) {
         // Parse booking data from line
         sscanf(line, "%[^:]:%d:%[^:]:%[^:]:%[^:]:%f:%[^:]:%d",
                booking.username, &booking.room_number, booking.booking_date,
                booking.check_in_date, booking.check_out_date, &booking.total_price,
                booking.status, &booking.booking_id);
         
         // Only display receipts for the specified user
         if (strcmp(booking.username, username) == 0) {
             display_receipt_in_buffer(&booking, buffer);
             count++;
         }
     }
     fclose(file);
     
     // Display message if no receipts found
     if (count == 0) {
         GtkTextIter iter;
         gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);
         gtk_text_buffer_insert(buffer, &iter, "No receipts found.\n", -1);
     }
 }
 
 /**
  * brief Shows a dialog for updating user profile
  * 
  * Creates and displays a dialog window allowing the user to update
  * their profile information.
  */
 static void update_user_profile_dialog(void) {
     char full_name[MAX_LEN], id_number[20], email[MAX_LEN], address[MAX_LEN], phone[15];
     // Get current profile information
     get_user_profile(current_user, full_name, id_number, email, address, phone);
     
     // Create dialog window
     GtkWidget *dialog = gtk_dialog_new_with_buttons("Update Profile", 
                                                    GTK_WINDOW(window),
                                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    "Cancel", GTK_RESPONSE_CANCEL,
                                                    "Save", GTK_RESPONSE_ACCEPT,
                                                    NULL);
     
     GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
     gtk_container_set_border_width(GTK_CONTAINER(content_area), 10);
     
     // Create grid layout for form fields
     GtkWidget *grid = gtk_grid_new();
     gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
     gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
     gtk_container_add(GTK_CONTAINER(content_area), grid);
     
     // Create form fields with current values
     GtkWidget *full_name_label = gtk_label_new("Full Name:");
     widgets.full_name_entry = gtk_entry_new();
     gtk_entry_set_text(GTK_ENTRY(widgets.full_name_entry), full_name);
     
     GtkWidget *id_label = gtk_label_new("ID Number:");
     widgets.id_entry = gtk_entry_new();
     gtk_entry_set_text(GTK_ENTRY(widgets.id_entry), id_number);
     
     GtkWidget *email_label = gtk_label_new("Email:");
     widgets.email_entry = gtk_entry_new();
     gtk_entry_set_text(GTK_ENTRY(widgets.email_entry), email);
     
     GtkWidget *address_label = gtk_label_new("Address:");
     widgets.address_entry = gtk_entry_new();
     gtk_entry_set_text(GTK_ENTRY(widgets.address_entry), address);
     
     GtkWidget *phone_label = gtk_label_new("Phone:");
     widgets.phone_entry_profile = gtk_entry_new();
     gtk_entry_set_text(GTK_ENTRY(widgets.phone_entry_profile), phone);
     
     // Add fields to grid
     gtk_grid_attach(GTK_GRID(grid), full_name_label, 0, 0, 1, 1);
     gtk_grid_attach(GTK_GRID(grid), widgets.full_name_entry, 1, 0, 1, 1);
     gtk_grid_attach(GTK_GRID(grid), id_label, 0, 1, 1, 1);
     gtk_grid_attach(GTK_GRID(grid), widgets.id_entry, 1, 1, 1, 1);
     gtk_grid_attach(GTK_GRID(grid), email_label, 0, 2, 1, 1);
     gtk_grid_attach(GTK_GRID(grid), widgets.email_entry, 1, 2, 1, 1);
     gtk_grid_attach(GTK_GRID(grid), address_label, 0, 3, 1, 1);
     gtk_grid_attach(GTK_GRID(grid), widgets.address_entry, 1, 3, 1, 1);
     gtk_grid_attach(GTK_GRID(grid), phone_label, 0, 4, 1, 1);
     gtk_grid_attach(GTK_GRID(grid), widgets.phone_entry_profile, 1, 4, 1, 1);
     
     // Show the dialog
     gtk_widget_show_all(dialog);
     
     // Run the dialog and wait for response
     gint result = gtk_dialog_run(GTK_DIALOG(dialog));
     
     // Handle Save button click
     if (result == GTK_RESPONSE_ACCEPT) {
         // Save the updated profile
         save_user_profile(current_user,
                          gtk_entry_get_text(GTK_ENTRY(widgets.full_name_entry)),
                          gtk_entry_get_text(GTK_ENTRY(widgets.id_entry)),
                          gtk_entry_get_text(GTK_ENTRY(widgets.email_entry)),
                          gtk_entry_get_text(GTK_ENTRY(widgets.address_entry)),
                          gtk_entry_get_text(GTK_ENTRY(widgets.phone_entry_profile)));
         update_status("Profile updated successfully!");
     }
     
     // Clean up
     gtk_widget_destroy(dialog);
 }
 /**
 *brief Callback for customer login button click
 * 
 * Navigates to the customer login screen when the customer login button
 * is clicked on the main menu.
 */
void on_customer_login_clicked(GtkWidget *widget, gpointer data) {
    show_customer_login();
}

/**
 * brief Callback for admin login button click
 * 
 * Navigates to the admin login screen when the admin login button
 * is clicked on the main menu.
 */
void on_admin_login_clicked(GtkWidget *widget, gpointer data) {
    show_admin_login();
}

/**
 * brief Callback for back to main menu button click
 * 
 * Returns to the main menu from any screen when the back button is clicked.
 * Also clears the current user and admin login status.
 */
void on_back_to_main_clicked(GtkWidget *widget, gpointer data) {
    // Reset login status
    strcpy(current_user, "");
    is_admin_logged_in = false;
    show_main_menu();
}

/**
 * brief Callback for customer login form submission
 * 
 * Validates customer credentials and logs the user in if they are correct.
 * Shows appropriate error messages for invalid credentials.
 */
void on_customer_login_submit_clicked(GtkWidget *widget, gpointer data) {
    const char *username = gtk_entry_get_text(GTK_ENTRY(widgets.username_entry));
    const char *phone = gtk_entry_get_text(GTK_ENTRY(widgets.phone_entry));
    
    // Validate login credentials
    if (validate_user_credentials(username, phone)) {
        strcpy(current_user, username);
        if (!profile_exists(current_user)) {
            update_status("Please complete your profile.");
        }
        show_customer_menu();
    } else {
        update_status("Invalid username or phone number.");
    }
}

/**
 * brief Callback for admin login form submission
 * 
 * Validates admin password and logs the admin in if correct.
 * Shows appropriate error messages for invalid passwords.
 */
void on_admin_login_submit_clicked(GtkWidget *widget, gpointer data) {
    const char *password = gtk_entry_get_text(GTK_ENTRY(widgets.password_entry));
    
    if (validate_admin_password(password)) {
        is_admin_logged_in = true;
        update_status("Admin login successful!");
        show_admin_menu();
    } else {
        update_status("Invalid admin password.");
    }
}

/**
 * brief Callback for browse rooms button click
 * 
 * Navigates to the room browsing screen when the browse rooms button is clicked.
 */
void on_browse_rooms_clicked(GtkWidget *widget, gpointer data) {
    show_room_browse();
}

/**
 * brief Callback for view bookings button click
 * 
 * Navigates to the booking management screen when the view bookings button is clicked.
 */
void on_view_bookings_clicked(GtkWidget *widget, gpointer data) {
    show_booking_manage();
}

/**
 * brief Callback for view receipt history button click
 * 
 * Navigates to the receipt history screen when the view receipt history button is clicked.
 */
void on_view_receipt_history_clicked(GtkWidget *widget, gpointer data) {
    show_receipt_history();
}
// new screen for admin change password
void show_admin_change_password_screen() {
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "admin_change_password");
}

/**
 * brief Callback for room filter application
 * 
 * Applies the specified filters to the room list and updates the display
 * with only the rooms that match the filter criteria.
 */
void on_filter_rooms_clicked(GtkWidget *widget, gpointer data) {
    const char *min_price_str = gtk_entry_get_text(GTK_ENTRY(widgets.min_price_entry));
    const char *max_price_str = gtk_entry_get_text(GTK_ENTRY(widgets.max_price_entry));
    const char *facilities = gtk_entry_get_text(GTK_ENTRY(widgets.facilities_entry));
    
    // Get selected room type from combo box
    GtkTreeIter iter;
    gtk_combo_box_get_active_iter(GTK_COMBO_BOX(widgets.type_combo), &iter);
    GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(widgets.type_combo));
    gchar *type;
    gtk_tree_model_get(model, &iter, 0, &type, -1);
    
    // Create filter from user input
    RoomFilter filter;
    filter.min_price = atof(min_price_str);
    filter.max_price = atof(max_price_str);
    strcpy(filter.type, type);
    strcpy(filter.facilities, facilities);
    
    g_free(type);
    
    // Clear current room list and apply filters
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(widgets.room_tree)));
    gtk_list_store_clear(store);
    load_rooms_into_store(store, &filter);
}

/**
 * brief Callback for room booking
 * 
 * Handles the room booking process, including date validation, availability checking,
 * price calculation, and saving the booking to file.
 */
void on_book_room_clicked(GtkWidget *widget, gpointer data) {
    if (strlen(current_user) == 0) {
        update_status("Please login first to book a room.");
        return;
    }
    
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widgets.room_tree));
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int room_number;
        gtk_tree_model_get(model, &iter, 0, &room_number, -1);
        
        const char *checkin = gtk_entry_get_text(GTK_ENTRY(widgets.checkin_entry));
        const char *checkout = gtk_entry_get_text(GTK_ENTRY(widgets.checkout_entry));
        
        // Validate and parse dates
        char formatted_in[20], formatted_out[20];
        parse_date(checkin, formatted_in);
        parse_date(checkout, formatted_out);

        if (strcmp(formatted_in, "invalid") == 0 || strcmp(formatted_out, "invalid") == 0) {
            update_status("Invalid date format. Use YYYY-MM-DD or DD/MM/YYYY.");
            return;
        }

        if (strcmp(formatted_in, formatted_out) >= 0) {
            update_status("Check-out date must be after check-in.");
            return;
        }

        if (!is_date_in_future(formatted_in)) {
            update_status("Check-in date must be today or in the future.");
            return;
        }

        if (!is_room_available(room_number, formatted_in, formatted_out)) {
            update_status("Room is already booked for those dates.");
            return;
        }

        // Check if user profile is complete
        char full_name[MAX_LEN], id_number[20];
        get_user_profile(current_user, full_name, id_number, NULL, NULL, NULL);
        if (strlen(full_name) == 0 || strlen(id_number) == 0) {
            update_status("Please complete your profile before booking.");
            return;
        }

        // Calculate price based on room rate and duration
        int nights = calculate_duration(formatted_in, formatted_out);
        float price = 0;
        
        // Find room price
        FILE *file = fopen(ROOM_FILE, "r");
        if (file != NULL) {
            Room room;
            char line[MAX_LEN];
            while (fgets(line, sizeof(line), file)) {
                sscanf(line, "%d:%[^:]:%f:%[^:]:%[^\n]", 
                       &room.room_number, room.type, &room.price, room.status, room.facilities);
                if (room.room_number == room_number) {
                    price = room.price * nights;
                    break;
                }
            }
            fclose(file);
        }

        // Generate unique booking ID
        int booking_id = time(NULL) % 1000000;
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char booking_date[20];
        strftime(booking_date, sizeof(booking_date), "%Y-%m-%d %H:%M:%S", t);

        // Save booking to file
        file = fopen(BOOKING_FILE, "a");
        if (file == NULL) {
            update_status("Error recording booking.");
            return;
        }
        fprintf(file, "%s:%d:%s:%s:%s:%.2f:%s:%d\n", 
                current_user, room_number, booking_date,
                formatted_in, formatted_out, price, "active", booking_id);
        fclose(file);

        // Update room status to booked
        file = fopen(ROOM_FILE, "r");
        if (file == NULL) {
            update_status("Error updating room status.");
            return;
        }

        Room rooms[MAX_ROOMS];
        int room_count = 0;
        char line[MAX_LEN];
        while (fgets(line, sizeof(line), file)) {
            sscanf(line, "%d:%[^:]:%f:%[^:]:%[^\n]", 
                   &rooms[room_count].room_number, rooms[room_count].type, 
                   &rooms[room_count].price, rooms[room_count].status, rooms[room_count].facilities);
            if (rooms[room_count].room_number == room_number) {
                strcpy(rooms[room_count].status, "Booked");
            }
            room_count++;
        }
        fclose(file);

        file = fopen(ROOM_FILE, "w");
        if (file == NULL) {
            update_status("Error updating room status.");
            return;
        }
        for (int i = 0; i < room_count; i++) {
            fprintf(file, "%d:%s:%.2f:%s:%s\n", 
                    rooms[i].room_number, 
                    rooms[i].type, 
                    rooms[i].price, 
                    rooms[i].status,
                    rooms[i].facilities);
        }
        fclose(file);

        // Show success message
        char status_msg[100];
        sprintf(status_msg, "Booking confirmed! ID: %d, Total: $%.2f for %d nights.", booking_id, price, nights);
        update_status(status_msg);
    } else {
        update_status("Please select a room to book.");
    }
}

/**
 * brief Callback for booking cancellation
 * 
 * Handles the cancellation of a booking, updating both the booking status
 * and the room availability status.
 */
void on_cancel_booking_clicked(GtkWidget *widget, gpointer data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widgets.booking_tree));
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int booking_id;
        gtk_tree_model_get(model, &iter, 0, &booking_id, -1);
        
        // Update booking status to canceled
        FILE *file = fopen(BOOKING_FILE, "r");
        if (file == NULL) {
            update_status("Error opening booking file.");
            return;
        }

        Booking bookings[MAX_BOOKINGS];
        int count = 0;
        char line[MAX_LEN * 3];
        while (fgets(line, sizeof(line), file)) {
            sscanf(line, "%[^:]:%d:%[^:]:%[^:]:%[^:]:%f:%[^:]:%d",
                   bookings[count].username, &bookings[count].room_number, bookings[count].booking_date,
                   bookings[count].check_in_date, bookings[count].check_out_date, &bookings[count].total_price,
                   bookings[count].status, &bookings[count].booking_id);
            
            if (bookings[count].booking_id == booking_id) {
                strcpy(bookings[count].status, "canceled");
            }
            count++;
        }
        fclose(file);

        // Write updated bookings back to file
        file = fopen(BOOKING_FILE, "w");
        if (file == NULL) {
            update_status("Error updating booking file.");
            return;
        }
        for (int i = 0; i < count; i++) {
            fprintf(file, "%s:%d:%s:%s:%s:%.2f:%s:%d\n",
                    bookings[i].username,
                    bookings[i].room_number,
                    bookings[i].booking_date,
                    bookings[i].check_in_date,
                    bookings[i].check_out_date,
                    bookings[i].total_price,
                    bookings[i].status,
                    bookings[i].booking_id);
        }
        fclose(file);

        // Update room status to available
        file = fopen(ROOM_FILE, "r");
        if (file == NULL) {
            update_status("Error opening room file.");
            return;
        }

        Room rooms[MAX_ROOMS];
        int room_count = 0;
        while (fgets(line, sizeof(line), file)) {
            sscanf(line, "%d:%[^:]:%f:%[^:]:%[^\n]",
                   &rooms[room_count].room_number, rooms[room_count].type,
                   &rooms[room_count].price, rooms[room_count].status, rooms[room_count].facilities);
            room_count++;
        }
        fclose(file);

        // Find the room associated with the canceled booking
        int room_number = -1;
        for (int i = 0; i < count; i++) {
            if (bookings[i].booking_id == booking_id) {
                room_number = bookings[i].room_number;
                break;
            }
        }

        if (room_number != -1) {
            // Update room status
            file = fopen(ROOM_FILE, "w");
            if (file == NULL) {
                update_status("Error updating room file.");
                return;
            }
            for (int i = 0; i < room_count; i++) {
                if (rooms[i].room_number == room_number) {
                    strcpy(rooms[i].status, "Available");
                }
                fprintf(file, "%d:%s:%.2f:%s:%s\n",
                        rooms[i].room_number,
                        rooms[i].type,
                        rooms[i].price,
                        rooms[i].status,
                        rooms[i].facilities);
            }
            fclose(file);
        }

        // Show success message and refresh booking list
        char status_msg[100];
        sprintf(status_msg, "Booking %d canceled successfully.", booking_id);
        update_status(status_msg);
        
        load_user_bookings_into_store(GTK_LIST_STORE(model), current_user);
    } else {
        update_status("Please select a booking to cancel.");
    }
}

/**
 * brief Callback for admin password change
 * 
 * Handles the admin password change process, including validation
 * of the new password and confirmation.
 */
void on_admin_change_password_clicked(GtkWidget *widget, gpointer data) {
    const char *new_password = gtk_entry_get_text(GTK_ENTRY(widgets.new_password_entry));
    const char *confirm_password = gtk_entry_get_text(GTK_ENTRY(widgets.confirm_password_entry));
    
    if (strlen(new_password) < 6) {
        update_status("Password must be at least 6 characters long.");
        return;
    }
    
    if (strcmp(new_password, confirm_password) != 0) {
        update_status("Passwords do not match.");
        return;
    }
    
    change_admin_password(new_password);
    update_status("Admin password changed successfully.");
}

/**
 * brief Callback for revenue calculation
 * 
 * Calculates and displays daily and weekly revenue for the specified date.
 */
void on_admin_view_revenue_clicked(GtkWidget *widget, gpointer data) {
    const char *date = gtk_entry_get_text(GTK_ENTRY(widgets.date_entry));
    
    float daily_revenue = calculate_daily_revenue(date);
    float weekly_revenue = calculate_weekly_revenue(date);
    
    char revenue_msg[200];
    sprintf(revenue_msg, "Revenue for %s: Daily: $%.2f, Weekly: $%.2f", date, daily_revenue, weekly_revenue);
    update_status(revenue_msg);
}

/**
 * brief Callback for admin booking filtering
 * 
 * Applies filters to the admin bookings view based on the specified
 * criteria (booking ID, username, date range).
 */
void on_admin_filter_bookings_clicked(GtkWidget *widget, gpointer data) {
    const char *booking_id_str = gtk_entry_get_text(GTK_ENTRY(widgets.booking_id_entry));
    const char *username = gtk_entry_get_text(GTK_ENTRY(widgets.admin_username_entry));
    const char *start_date = gtk_entry_get_text(GTK_ENTRY(widgets.start_date_entry));
    const char *end_date = gtk_entry_get_text(GTK_ENTRY(widgets.end_date_entry));
    
    BookingFilter filter;
    filter.booking_id = atoi(booking_id_str);
    strcpy(filter.username, username);
    strcpy(filter.start_date, start_date);
    strcpy(filter.end_date, end_date);
    
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(widgets.admin_bookings_tree)));
    gtk_list_store_clear(store);
    load_all_bookings_into_store(store, &filter);
}

/**
 * brief Callback for sign up button click
 * 
 * Navigates to the sign up screen when the create account button is clicked.
 */
void on_sign_up_clicked(GtkWidget *widget, gpointer data) {
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "customer_signup");
}

/**
 * brief Callback for sign up form submission
 * 
 * Handles the user registration process, including input validation
 * and saving the new user to the system.
 */
void on_sign_up_submit_clicked(GtkWidget *widget, gpointer data) {
    const char *username = gtk_entry_get_text(GTK_ENTRY(widgets.signup_username_entry));
    const char *phone = gtk_entry_get_text(GTK_ENTRY(widgets.signup_phone_entry));
    const char *confirm_phone = gtk_entry_get_text(GTK_ENTRY(widgets.signup_confirm_phone_entry));
    
    // Validate inputs
    if (!is_valid_username(username)) {
        update_status("Username must be 3-20 characters (letters, numbers, underscore only)");
        return;
    }
    
    if (!is_valid_phone(phone)) {
        update_status("Phone must be 10-15 digits only");
        return;
    }
    
    if (strcmp(phone, confirm_phone) != 0) {
        update_status("Phone numbers don't match");
        return;
    }
    
    // Register user
    if (register_user(username, phone)) {
        update_status("Registration successful! Please login.");
        show_customer_login();
        
        // Clear signup fields
        gtk_entry_set_text(GTK_ENTRY(widgets.signup_username_entry), "");
        gtk_entry_set_text(GTK_ENTRY(widgets.signup_phone_entry), "");
        gtk_entry_set_text(GTK_ENTRY(widgets.signup_confirm_phone_entry), "");
    } else {
        update_status("Username already taken. Please choose another.");
    }
}

/**
 * brief Callback for back to login button click
 * 
 * Returns to the login screen from the sign up screen.
 */
void on_back_to_login_clicked(GtkWidget *widget, gpointer data) {
    show_customer_login();
}

/**
 * brief Callback for manage profile button click
 * 
 * Opens the profile management dialog when the manage profile button is clicked.
 */
void on_manage_profile_clicked(GtkWidget *widget, gpointer data) {
    update_user_profile_dialog();
}
/**
 * brief Shows the main menu screen
 * 
 * Sets the stack to display the main menu, which is the starting point
 * of the application with options for customer login, admin login, and room browsing.
 */
void show_main_menu() {
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "main_menu");
}

/**
 * brief Shows the customer login screen
 * 
 * Sets the stack to display the customer login form where users can
 * enter their username and phone number to log in.
 */
void show_customer_login() {
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "customer_login");
}

/**
 * brief Shows the customer menu screen
 * 
 * Sets the stack to display the customer menu, which provides options
 * for booking rooms, managing bookings, viewing receipts, and managing profile.
 */
void show_customer_menu() {
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "customer_menu");
}

/**
 * brief Shows the admin login screen
 * 
 * Sets the stack to display the admin login form where administrators
 * can enter their password to log in.
 */
void show_admin_login() {
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "admin_login");
}

/**
 * brief Shows the admin menu screen
 * 
 * Sets the stack to display the admin menu, which provides options
 * for viewing all bookings, revenue reports, and changing admin password.
 */
void show_admin_menu() {
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "admin_menu");
}

/**
 * brief Shows the room browsing screen
 * 
 * Sets the stack to display the room browsing interface, where users
 * can filter, view, and book available rooms.
 * Also loads the available rooms into the room list.
 */
void show_room_browse() {
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "room_browse");
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(widgets.room_tree)));
    gtk_list_store_clear(store);
    load_rooms_into_store(store, NULL);
}

/**
 * brief Shows the booking management screen
 * 
 * Sets the stack to display the booking management interface, where users
 * can view and cancel their existing bookings.
 * Also loads the user's bookings into the booking list.
 */
void show_booking_manage() {
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "booking_manage");
    if (strlen(current_user) > 0) {
        GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(widgets.booking_tree)));
        gtk_list_store_clear(store);
        load_user_bookings_into_store(store, current_user);
    }
}

/**
 * brief Shows the receipt history screen
 * 
 * Sets the stack to display the receipt history interface, where users
 * can view their booking receipts.
 * Also loads the user's receipt history.
 */
void show_receipt_history() {
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "receipt_history");
    if (strlen(current_user) > 0) {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets.receipt_view));
        load_receipt_history_for_user(current_user, buffer);
    }
}

/**
 * brief Shows the admin bookings screen
 * 
 * Sets the stack to display the admin bookings interface, where administrators
 * can view and filter all bookings in the system.
 * Also loads all bookings into the admin bookings list.
 */
void show_admin_bookings() {
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "admin_bookings");
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(widgets.admin_bookings_tree)));
    gtk_list_store_clear(store);
    load_all_bookings_into_store(store, NULL);
}

/**
 * brief Updates the status label with a message
 * 
 * Sets the text of the status label at the bottom of the application
 * to display informative messages to the user.
 * 
 * param message The message to display in the status label
 */
void update_status(const char *message) {
    gtk_label_set_text(GTK_LABEL(status_label), message);
}
/**
 * brief Initializes the GUI application
 * 
 * Sets up the main window, creates all GUI components, and connects
 * signals to callback functions. This is the main initialization function
 * for the entire GUI interface.
 * 
 * param argc Number of command line arguments (passed to gtk_init)
 * param argv Array of command line arguments (passed to gtk_init)
 */
void initialize_gui(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    // Create main application window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Hotel Booking System");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Create stack for different screens with slide transition
    stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    
    // Create status label for messages
    status_label = gtk_label_new("Welcome to Hotel Booking System");
    gtk_widget_set_halign(status_label, GTK_ALIGN_START);
    gtk_widget_set_margin_start(status_label, 10);
    gtk_widget_set_margin_end(status_label, 10);
    gtk_widget_set_margin_top(status_label, 10);
    gtk_widget_set_margin_bottom(status_label, 10);
    
    // Create main menu screen
    GtkWidget *main_menu_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(main_menu_box), 10);
    
    GtkWidget *title_label = gtk_label_new("Hotel Booking System");
    PangoAttrList *attrs = pango_attr_list_new();
    pango_attr_list_insert(attrs, pango_attr_scale_new(1.5));
    gtk_label_set_attributes(GTK_LABEL(title_label), attrs);
    
    GtkWidget *customer_login_btn = gtk_button_new_with_label("Customer Login");
    GtkWidget *browse_rooms_btn = gtk_button_new_with_label("Browse Rooms (Without Login)");
    GtkWidget *admin_login_btn = gtk_button_new_with_label("Admin Login");
    GtkWidget *exit_btn = gtk_button_new_with_label("Exit");
    
    // Connect main menu buttons to callbacks
    g_signal_connect(customer_login_btn, "clicked", G_CALLBACK(on_customer_login_clicked), NULL);
    g_signal_connect(admin_login_btn, "clicked", G_CALLBACK(on_admin_login_clicked), NULL);
    g_signal_connect(browse_rooms_btn, "clicked", G_CALLBACK(on_browse_rooms_clicked), NULL);
    g_signal_connect(exit_btn, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    
    // Pack main menu widgets
    gtk_box_pack_start(GTK_BOX(main_menu_box), title_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_menu_box), customer_login_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_menu_box), browse_rooms_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_menu_box), admin_login_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_menu_box), exit_btn, FALSE, FALSE, 0);
    
    gtk_stack_add_titled(GTK_STACK(stack), main_menu_box, "main_menu", "Main Menu");
    
    // Create customer login screen
    GtkWidget *customer_login_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(customer_login_box), 10);
    
    GtkWidget *login_title = gtk_label_new("Customer Login");
    GtkWidget *username_label = gtk_label_new("Username:");
    widgets.username_entry = gtk_entry_new();
    GtkWidget *phone_label = gtk_label_new("Phone Number:");
    widgets.phone_entry = gtk_entry_new();
    GtkWidget *login_btn = gtk_button_new_with_label("Login");
    GtkWidget *signup_btn = gtk_button_new_with_label("Create Account");
    GtkWidget *back_btn = gtk_button_new_with_label("Back to Main Menu");
    
    // Connect customer login buttons to callbacks
    g_signal_connect(back_btn, "clicked", G_CALLBACK(on_back_to_main_clicked), NULL);
    g_signal_connect(login_btn, "clicked", G_CALLBACK(on_customer_login_submit_clicked), NULL);
    g_signal_connect(signup_btn, "clicked", G_CALLBACK(on_sign_up_clicked), NULL);
    
    // Pack customer login widgets
    gtk_box_pack_start(GTK_BOX(customer_login_box), login_title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_login_box), username_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_login_box), widgets.username_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_login_box), phone_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_login_box), widgets.phone_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_login_box), login_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_login_box), signup_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_login_box), back_btn, FALSE, FALSE, 0);
    
    gtk_stack_add_titled(GTK_STACK(stack), customer_login_box, "customer_login", "Customer Login");
    
    // Create customer signup screen
    GtkWidget *customer_signup_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(customer_signup_box), 10);
    
    GtkWidget *signup_title = gtk_label_new("Create Account");
    GtkWidget *signup_username_label = gtk_label_new("Username:");
    widgets.signup_username_entry = gtk_entry_new();
    GtkWidget *signup_phone_label = gtk_label_new("Phone Number:");
    widgets.signup_phone_entry = gtk_entry_new();
    GtkWidget *signup_confirm_phone_label = gtk_label_new("Confirm Phone:");
    widgets.signup_confirm_phone_entry = gtk_entry_new();
    GtkWidget *signup_submit_btn = gtk_button_new_with_label("Sign Up");
    GtkWidget *signup_back_btn = gtk_button_new_with_label("Back to Login");
    
    // Connect signup buttons to callbacks
    g_signal_connect(signup_back_btn, "clicked", G_CALLBACK(on_back_to_login_clicked), NULL);
    g_signal_connect(signup_submit_btn, "clicked", G_CALLBACK(on_sign_up_submit_clicked), NULL);
    
    // Pack signup widgets
    gtk_box_pack_start(GTK_BOX(customer_signup_box), signup_title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_signup_box), signup_username_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_signup_box), widgets.signup_username_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_signup_box), signup_phone_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_signup_box), widgets.signup_phone_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_signup_box), signup_confirm_phone_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_signup_box), widgets.signup_confirm_phone_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_signup_box), signup_submit_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_signup_box), signup_back_btn, FALSE, FALSE, 0);
    
    gtk_stack_add_titled(GTK_STACK(stack), customer_signup_box, "customer_signup", "Sign Up");
    
    // Create customer menu screen
    GtkWidget *customer_menu_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(customer_menu_box), 10);
    
    GtkWidget *customer_title = gtk_label_new("Customer Menu");
    GtkWidget *view_book_btn = gtk_button_new_with_label("View & Book Available Rooms");
    GtkWidget *manage_bookings_btn = gtk_button_new_with_label("Manage My Bookings");
    GtkWidget *view_receipts_btn = gtk_button_new_with_label("View Receipt History");
    GtkWidget *manage_profile_btn = gtk_button_new_with_label("Manage My Profile");
    GtkWidget *customer_logout_btn = gtk_button_new_with_label("Logout");
    
    // Connect customer menu buttons to callbacks
    g_signal_connect(customer_logout_btn, "clicked", G_CALLBACK(on_back_to_main_clicked), NULL);
    g_signal_connect(manage_profile_btn, "clicked", G_CALLBACK(on_manage_profile_clicked), NULL);
    g_signal_connect(view_book_btn, "clicked", G_CALLBACK(on_browse_rooms_clicked), NULL);
    g_signal_connect(manage_bookings_btn, "clicked", G_CALLBACK(on_view_bookings_clicked), NULL);
    g_signal_connect(view_receipts_btn, "clicked", G_CALLBACK(on_view_receipt_history_clicked), NULL);
    
    // Pack customer menu widgets
    gtk_box_pack_start(GTK_BOX(customer_menu_box), customer_title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_menu_box), view_book_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_menu_box), manage_bookings_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_menu_box), view_receipts_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_menu_box), manage_profile_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(customer_menu_box), customer_logout_btn, FALSE, FALSE, 0);
    
    gtk_stack_add_titled(GTK_STACK(stack), customer_menu_box, "customer_menu", "Customer Menu");
    
    // Create admin login screen
    GtkWidget *admin_login_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(admin_login_box), 10);
    
    GtkWidget *admin_title = gtk_label_new("Admin Login");
    GtkWidget *password_label = gtk_label_new("Password:");
    widgets.password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(widgets.password_entry), FALSE);
    GtkWidget *admin_login_submit_btn = gtk_button_new_with_label("Login");
    GtkWidget *admin_back_btn = gtk_button_new_with_label("Back to Main Menu");
    
    // Connect admin login buttons to callbacks
    g_signal_connect(admin_back_btn, "clicked", G_CALLBACK(on_back_to_main_clicked), NULL);
    g_signal_connect(admin_login_submit_btn, "clicked", G_CALLBACK(on_admin_login_submit_clicked), NULL);

    // Pack admin login widgets
    gtk_box_pack_start(GTK_BOX(admin_login_box), admin_title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_login_box), password_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_login_box), widgets.password_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_login_box), admin_login_submit_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_login_box), admin_back_btn, FALSE, FALSE, 0);
    
    gtk_stack_add_titled(GTK_STACK(stack), admin_login_box, "admin_login", "Admin Login");
    
    // Create admin menu screen
    GtkWidget *admin_menu_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(admin_menu_box), 10);

    GtkWidget *admin_menu_title = gtk_label_new("Admin Menu");
    GtkWidget *view_all_bookings_btn = gtk_button_new_with_label("View All Bookings");
    GtkWidget *view_revenue_btn = gtk_button_new_with_label("View Revenue Reports");
    GtkWidget *change_password_btn = gtk_button_new_with_label("Change Admin Password");
    GtkWidget *admin_logout_btn = gtk_button_new_with_label("Logout");
    // Connect admin menu buttons to callbacks
    g_signal_connect(admin_logout_btn, "clicked", G_CALLBACK(on_back_to_main_clicked), NULL);
    g_signal_connect(view_all_bookings_btn, "clicked", G_CALLBACK(show_admin_bookings), NULL);
    g_signal_connect(view_revenue_btn, "clicked", G_CALLBACK(on_admin_view_revenue_clicked), NULL);
    g_signal_connect(change_password_btn, "clicked", G_CALLBACK(show_admin_change_password_screen), NULL);

    // Pack admin menu widgets
    gtk_box_pack_start(GTK_BOX(admin_menu_box), admin_menu_title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_menu_box), view_all_bookings_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_menu_box), view_revenue_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_menu_box), change_password_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_menu_box), admin_logout_btn, FALSE, FALSE, 0);

    gtk_stack_add_titled(GTK_STACK(stack), admin_menu_box, "admin_menu", "Admin Menu");

    // Create room browse screen
    GtkWidget *room_browse_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(room_browse_box), 10);

    GtkWidget *room_browse_title = gtk_label_new("Browse Available Rooms");

    // Filter controls
    GtkWidget *filter_frame = gtk_frame_new("Filter Options");
    GtkWidget *filter_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(filter_grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(filter_grid), 10);

    GtkWidget *min_price_label = gtk_label_new("Min Price:");
    widgets.min_price_entry = gtk_entry_new();
    GtkWidget *max_price_label = gtk_label_new("Max Price:");
    widgets.max_price_entry = gtk_entry_new();
    GtkWidget *type_label = gtk_label_new("Room Type:");

    // Create room type combo box
    GtkListStore *type_store = gtk_list_store_new(1, G_TYPE_STRING);
    GtkTreeIter type_iter;
    gtk_list_store_append(type_store, &type_iter);
    gtk_list_store_set(type_store, &type_iter, 0, "", -1);
    gtk_list_store_append(type_store, &type_iter);
    gtk_list_store_set(type_store, &type_iter, 0, "Single", -1);
    gtk_list_store_append(type_store, &type_iter);
    gtk_list_store_set(type_store, &type_iter, 0, "Double", -1);
    gtk_list_store_append(type_store, &type_iter);
    gtk_list_store_set(type_store, &type_iter, 0, "Suite", -1);

    widgets.type_combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(type_store));
    GtkCellRenderer *type_renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(widgets.type_combo), type_renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(widgets.type_combo), type_renderer, "text", 0, NULL);
    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets.type_combo), 0);

    GtkWidget *facilities_label = gtk_label_new("Facilities:");
    widgets.facilities_entry = gtk_entry_new();
    GtkWidget *filter_btn = gtk_button_new_with_label("Apply Filters");

    // Arrange filter controls in grid
    gtk_grid_attach(GTK_GRID(filter_grid), min_price_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(filter_grid), widgets.min_price_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(filter_grid), max_price_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(filter_grid), widgets.max_price_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(filter_grid), type_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(filter_grid), widgets.type_combo, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(filter_grid), facilities_label, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(filter_grid), widgets.facilities_entry, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(filter_grid), filter_btn, 0, 4, 2, 1);

    gtk_container_add(GTK_CONTAINER(filter_frame), filter_grid);

    // Date selection for booking
    GtkWidget *date_frame = gtk_frame_new("Booking Dates");
    GtkWidget *date_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(date_grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(date_grid), 10);

    GtkWidget *checkin_label = gtk_label_new("Check-in (YYYY-MM-DD):");
    widgets.checkin_entry = gtk_entry_new();
    GtkWidget *checkout_label = gtk_label_new("Check-out (YYYY-MM-DD):");
    widgets.checkout_entry = gtk_entry_new();
    GtkWidget *book_btn = gtk_button_new_with_label("Book Selected Room");

    gtk_grid_attach(GTK_GRID(date_grid), checkin_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(date_grid), widgets.checkin_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(date_grid), checkout_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(date_grid), widgets.checkout_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(date_grid), book_btn, 0, 2, 2, 1);

    gtk_container_add(GTK_CONTAINER(date_frame), date_grid);
    
    // Room list
    GtkWidget *room_list_frame = gtk_frame_new("Available Rooms");
    GtkWidget *room_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(room_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    GtkListStore *room_store = create_room_list_store();
    widgets.room_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(room_store));
    
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    // Create columns for room list
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Room #", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.room_tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Type", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.room_tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Price", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.room_tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Status", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.room_tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Facilities", renderer, "text", 4, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.room_tree), column);
    
    gtk_container_add(GTK_CONTAINER(room_scroll), widgets.room_tree);
    gtk_container_add(GTK_CONTAINER(room_list_frame), room_scroll);
    
    // Back button
    GtkWidget *room_back_btn = gtk_button_new_with_label("Back");
    
    // Connect room browse buttons to callbacks
    g_signal_connect(room_back_btn, "clicked", G_CALLBACK(on_back_to_main_clicked), NULL);
    g_signal_connect(filter_btn, "clicked", G_CALLBACK(on_filter_rooms_clicked), NULL);
    g_signal_connect(book_btn, "clicked", G_CALLBACK(on_book_room_clicked), NULL);
    
    // Pack room browse box
    gtk_box_pack_start(GTK_BOX(room_browse_box), room_browse_title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(room_browse_box), filter_frame, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(room_browse_box), date_frame, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(room_browse_box), room_list_frame, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(room_browse_box), room_back_btn, FALSE, FALSE, 0);
    
    gtk_stack_add_titled(GTK_STACK(stack), room_browse_box, "room_browse", "Browse Rooms");
    
    // Create booking management screen
    GtkWidget *booking_manage_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(booking_manage_box), 10);
    
    GtkWidget *booking_title = gtk_label_new("Manage Bookings");
    
    // Booking list
    GtkWidget *booking_list_frame = gtk_frame_new("Your Bookings");
    GtkWidget *booking_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(booking_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    GtkListStore *booking_store = create_booking_list_store();
    widgets.booking_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(booking_store));

    // Create columns for booking list
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Booking ID", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.booking_tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Room #", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.booking_tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Check-in", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.booking_tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Check-out", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.booking_tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Total", renderer, "text", 4, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.booking_tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Status", renderer, "text", 5, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.booking_tree), column);

    gtk_container_add(GTK_CONTAINER(booking_scroll), widgets.booking_tree);
    gtk_container_add(GTK_CONTAINER(booking_list_frame), booking_scroll);

    // Cancel button
    GtkWidget *cancel_btn = gtk_button_new_with_label("Cancel Selected Booking");

    // Back button
    GtkWidget *booking_back_btn = gtk_button_new_with_label("Back");

    // Connect booking management buttons to callbacks
    g_signal_connect(booking_back_btn, "clicked", G_CALLBACK(show_customer_menu), NULL);
    g_signal_connect(cancel_btn, "clicked", G_CALLBACK(on_cancel_booking_clicked), NULL);

    // Pack booking manage box
    gtk_box_pack_start(GTK_BOX(booking_manage_box), booking_title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(booking_manage_box), booking_list_frame, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(booking_manage_box), cancel_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(booking_manage_box), booking_back_btn, FALSE, FALSE, 0);

    gtk_stack_add_titled(GTK_STACK(stack), booking_manage_box, "booking_manage", "Manage Bookings");
    
    // Create receipt history screen
    GtkWidget *receipt_history_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(receipt_history_box), 10);

    GtkWidget *receipt_title = gtk_label_new("Receipt History");

    // Receipt display
    GtkWidget *receipt_frame = gtk_frame_new("Your Receipts");
    GtkWidget *receipt_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(receipt_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    widgets.receipt_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(widgets.receipt_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(widgets.receipt_view), GTK_WRAP_WORD);

    gtk_container_add(GTK_CONTAINER(receipt_scroll), widgets.receipt_view);
    gtk_container_add(GTK_CONTAINER(receipt_frame), receipt_scroll);

    // Back button
    GtkWidget *receipt_back_btn = gtk_button_new_with_label("Back");

    g_signal_connect(receipt_back_btn, "clicked", G_CALLBACK(show_customer_menu), NULL);

    // Pack receipt history box
    gtk_box_pack_start(GTK_BOX(receipt_history_box), receipt_title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(receipt_history_box), receipt_frame, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(receipt_history_box), receipt_back_btn, FALSE, FALSE, 0);

    gtk_stack_add_titled(GTK_STACK(stack), receipt_history_box, "receipt_history", "Receipt History");

    // Create admin bookings screen
    GtkWidget *admin_bookings_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(admin_bookings_box), 10);

    GtkWidget *admin_bookings_title = gtk_label_new("Admin Bookings Search");
    // Filter controls
    GtkWidget *admin_filter_frame = gtk_frame_new("Search Filters");
    GtkWidget *admin_filter_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(admin_filter_grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(admin_filter_grid), 10);

    GtkWidget *booking_id_label = gtk_label_new("Booking ID:");
    widgets.booking_id_entry = gtk_entry_new();
    GtkWidget *admin_username_label = gtk_label_new("Customer Username:");
    widgets.admin_username_entry = gtk_entry_new();
    GtkWidget *start_date_label = gtk_label_new("Start Date (YYYY-MM-DD):");
    widgets.start_date_entry = gtk_entry_new();
    GtkWidget *end_date_label = gtk_label_new("End Date (YYYY-MM-DD):");
    widgets.end_date_entry = gtk_entry_new();
    GtkWidget *admin_filter_btn = gtk_button_new_with_label("Apply Filters");
    
    gtk_grid_attach(GTK_GRID(admin_filter_grid), booking_id_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(admin_filter_grid), widgets.booking_id_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(admin_filter_grid), admin_username_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(admin_filter_grid), widgets.admin_username_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(admin_filter_grid), start_date_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(admin_filter_grid), widgets.start_date_entry, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(admin_filter_grid), end_date_label, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(admin_filter_grid), widgets.end_date_entry, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(admin_filter_grid), admin_filter_btn, 0, 4, 2, 1);

    gtk_container_add(GTK_CONTAINER(admin_filter_frame), admin_filter_grid);

    // Bookings list for admin view
    GtkWidget *admin_bookings_list_frame = gtk_frame_new("Bookings List");
    GtkWidget *admin_bookings_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(admin_bookings_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    GtkListStore *admin_bookings_store = create_admin_bookings_list_store();
    widgets.admin_bookings_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(admin_bookings_store));

    // Create columns for admin bookings view
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Booking ID", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.admin_bookings_tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Room #", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.admin_bookings_tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Customer", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.admin_bookings_tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Booking Date", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.admin_bookings_tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Check-in", renderer, "text", 4, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.admin_bookings_tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Check-out", renderer, "text", 5, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.admin_bookings_tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Total", renderer, "text", 6, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.admin_bookings_tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Status", renderer, "text", 7, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.admin_bookings_tree), column);

    gtk_container_add(GTK_CONTAINER(admin_bookings_scroll), widgets.admin_bookings_tree);
    gtk_container_add(GTK_CONTAINER(admin_bookings_list_frame), admin_bookings_scroll);

    // Back button
    GtkWidget *admin_bookings_back_btn = gtk_button_new_with_label("Back to Admin Menu");

    // Connect admin bookings buttons to callbacks
    g_signal_connect(admin_bookings_back_btn, "clicked", G_CALLBACK(show_admin_menu), NULL);
    g_signal_connect(admin_filter_btn, "clicked", G_CALLBACK(on_admin_filter_bookings_clicked), NULL);

    // Pack admin bookings box
    gtk_box_pack_start(GTK_BOX(admin_bookings_box), admin_bookings_title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_bookings_box), admin_filter_frame, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_bookings_box), admin_bookings_list_frame, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(admin_bookings_box), admin_bookings_back_btn, FALSE, FALSE, 0);

    gtk_stack_add_titled(GTK_STACK(stack), admin_bookings_box, "admin_bookings", "Admin Bookings");

    // Create admin revenue screen
    GtkWidget *admin_revenue_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(admin_revenue_box), 10);

    GtkWidget *revenue_title = gtk_label_new("Revenue Reports");
    GtkWidget *date_label = gtk_label_new("Date (YYYY-MM-DD):");
    widgets.date_entry = gtk_entry_new();
    GtkWidget *revenue_btn = gtk_button_new_with_label("Calculate Revenue");
    GtkWidget *revenue_back_btn = gtk_button_new_with_label("Back to Admin Menu");

    g_signal_connect(revenue_back_btn, "clicked", G_CALLBACK(show_admin_menu), NULL);
    g_signal_connect(revenue_btn, "clicked", G_CALLBACK(on_admin_view_revenue_clicked), NULL);

    // Pack admin revenue box
    gtk_box_pack_start(GTK_BOX(admin_revenue_box), revenue_title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_revenue_box), date_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_revenue_box), widgets.date_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_revenue_box), revenue_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_revenue_box), revenue_back_btn, FALSE, FALSE, 0);

    gtk_stack_add_titled(GTK_STACK(stack), admin_revenue_box, "admin_revenue", "Revenue Reports");

    // Create admin change password screen
    GtkWidget *admin_change_password_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(admin_change_password_box), 10);

    GtkWidget *password_title = gtk_label_new("Change Admin Password");
    GtkWidget *new_password_label = gtk_label_new("New Password:");
    widgets.new_password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(widgets.new_password_entry), FALSE);
    GtkWidget *confirm_password_label = gtk_label_new("Confirm Password:");
    widgets.confirm_password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(widgets.confirm_password_entry), FALSE);
    GtkWidget *change_pass_submit_btn = gtk_button_new_with_label("Change Password");
    GtkWidget *password_back_btn = gtk_button_new_with_label("Back to Admin Menu");

    g_signal_connect(password_back_btn, "clicked", G_CALLBACK(show_admin_menu), NULL);
    g_signal_connect(change_pass_submit_btn, "clicked", G_CALLBACK(on_admin_change_password_clicked), NULL);

    // Pack admin change password box
    gtk_box_pack_start(GTK_BOX(admin_change_password_box), password_title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_change_password_box), new_password_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_change_password_box), widgets.new_password_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_change_password_box), confirm_password_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_change_password_box), widgets.confirm_password_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_change_password_box), change_pass_submit_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(admin_change_password_box), password_back_btn, FALSE, FALSE, 0);

    gtk_stack_add_titled(GTK_STACK(stack), admin_change_password_box, "admin_change_password", "Change Password");

    // Create main container with stack and status bar
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(main_box), stack, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), status_label, FALSE, FALSE, 0);

    // Add main container to window
    gtk_container_add(GTK_CONTAINER(window), main_box);

    // Show all widgets and start with main menu
    gtk_widget_show_all(window);
    show_main_menu();
}    
