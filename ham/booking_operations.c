/**
 * file booking_operations.c
 * brief Implementation of booking-related functions
 * 
 * Contains functions for managing bookings, calculating revenue,
 * and generating receipts.
 */

 #include "booking_operations.h"
 #include "file_operations.h"
 #include <stdio.h>
 #include <string.h>
 #include <time.h>
 
 /**
  * brief Calculates daily revenue for a specific date
  * 
  * Sums the total price of all active bookings made on the specified date.
  */
 float calculate_daily_revenue(const char *date) {
     FILE *file = fopen(BOOKING_FILE, "r");
     if (file == NULL) return 0.0;
 
     float revenue = 0.0;
     char line[MAX_LEN * 3];
     char username[MAX_LEN], status[20], booking_date[20], check_in_date[20], check_out_date[20];
     int room_number, booking_id;
     float total_price;
 
     // Read each booking from file
     while (fgets(line, sizeof(line), file)) {
         // Parse booking data
         sscanf(line, "%[^:]:%d:%[^:]:%[^:]:%[^:]:%f:%[^:]:%d",
                username, &room_number, booking_date,
                check_in_date, check_out_date, &total_price,
                status, &booking_id);
         
         // Check if booking date matches and status is active
         if (strncmp(booking_date, date, 10) == 0 && strcmp(status, "active") == 0) {
             revenue += total_price;
         }
     }
     fclose(file);
     
     return revenue;
 }
 
 /**
  * brief Calculates weekly revenue for a specific date
  * 
  * Sums the total price of all active bookings made during
  * the week containing the specified date.
  */
 float calculate_weekly_revenue(const char *date) {
     char start_date[11], end_date[11];
     // Get the Monday and Sunday of the week containing the date
     get_week_range(date, start_date, end_date);
 
     FILE *file = fopen(BOOKING_FILE, "r");
     if (file == NULL) return 0.0;
 
     float revenue = 0.0;
     char line[MAX_LEN * 3];
     char username[MAX_LEN], status[20], booking_date[20], check_in_date[20], check_out_date[20];
     int room_number, booking_id;
     float total_price;
     
     // Read each booking from file
     while (fgets(line, sizeof(line), file)) {
         // Parse booking data
         sscanf(line, "%[^:]:%d:%[^:]:%[^:]:%[^:]:%f:%[^:]:%d",
                username, &room_number, booking_date,
                check_in_date, check_out_date, &total_price,
                status, &booking_id);
         
         // Extract date part from booking date (ignore time)
         char booking_date_only[11];
         strncpy(booking_date_only, booking_date, 10);
         booking_date_only[10] = '\0';
 
         // Check if booking date is within the week and status is active
         if (strcmp(booking_date_only, start_date) >= 0 && 
             strcmp(booking_date_only, end_date) <= 0 && 
             strcmp(status, "active") == 0) {
             revenue += total_price;
         }
     }
     fclose(file);
     
     return revenue;
 }
 
 /**
  * brief Displays a booking receipt
  * 
  * Formats and displays the details of a booking in a receipt format.
  * This function will be implemented in the GUI context.
  */
 void display_receipt(Booking *booking) {
     // This will be implemented in GUI context
 }
 
 /**
  * brief Loads receipt history for a user
  * 
  * Retrieves and displays the booking history (receipts) for a specific user.
  * This function will be implemented in the GUI context.
  */
 void load_receipt_history(const char *username) {
     // This will be implemented in GUI context
 }