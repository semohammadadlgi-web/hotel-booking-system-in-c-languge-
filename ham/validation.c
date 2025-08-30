/**
 * file validation.c
 * brief Implementation of input validation functions
 * 
 * Contains functions for validating user inputs, credentials,
 * and business rules to ensure data integrity.
 */

 #include "validation.h"
 #include "file_operations.h"
 #include <string.h>
 #include <stdio.h>
 #include <ctype.h>
 
 /**
  * brief Checks if a room is available for given dates
  * 
  * Examines all existing bookings to determine if the specified room
  * has any overlapping bookings for the requested dates.
  */
 bool is_room_available(int room_number, const char *check_in, const char *check_out) {
     FILE *file = fopen(BOOKING_FILE, "r");
     if (!file) return true; // If no bookings file, all rooms are available
 
     char line[MAX_LEN * 3];
     char username[MAX_LEN], status[20], booking_date[20], check_in_date[20], check_out_date[20];
     int booking_room_number, booking_id;
     float total_price;
 
     // Read each booking from file
     while (fgets(line, sizeof(line), file)) {
         // Parse booking data
         sscanf(line, "%[^:]:%d:%[^:]:%[^:]:%[^:]:%f:%[^:]:%d",
                username, &booking_room_number, booking_date,
                check_in_date, check_out_date, &total_price, 
                status, &booking_id);
 
         // Check if this booking is for the same room and is active
         if (booking_room_number == room_number && strcmp(status, "active") == 0) {
             // Check for date overlap
             // No overlap if: check_out <= booking_check_in OR check_in >= booking_check_out
             // Overlap if NOT (no overlap condition)
             if (!(strcmp(check_out, check_in_date) <= 0 || strcmp(check_in, check_out_date) >= 0)) {
                 fclose(file);
                 return false; // Room is not available
             }
         }
     }
     fclose(file);
     return true; // Room is available
 }
 
 /**
  * brief Validates administrator password
  * 
  * Checks if the provided password matches the stored admin password.
  * Creates a default admin password file if one doesn't exist.
  */
 bool validate_admin_password(const char *password) {
     FILE *file = fopen(ADMIN_PASS_FILE, "r");
     if (file == NULL) {
         // Create default admin password file if it doesn't exist
         file = fopen(ADMIN_PASS_FILE, "w");
         fprintf(file, "admin123\n");
         fclose(file);
         return strcmp(password, "admin123") == 0;
     }
 
     char stored_password[MAX_LEN];
     fgets(stored_password, sizeof(stored_password), file);
     fclose(file);
     
     // Remove newline character if present
     stored_password[strcspn(stored_password, "\n")] = 0;
     
     return strcmp(password, stored_password) == 0;
 }
 
 /**
  * brief Checks if a user profile exists
  * 
  * Searches the user profiles file for the given username
  * to determine if a profile already exists.
  */
 bool profile_exists(const char *username) {
     FILE *file = fopen(USER_PROFILE_FILE, "r");
     if (file == NULL) {
         return false; // No profiles file means no profiles exist
     }
 
     char line[MAX_LEN * 4];
     // Read each profile from file
     while (fgets(line, sizeof(line), file)) {
         char saved_username[MAX_LEN];
         // Extract username from profile line
         sscanf(line, "%[^:]:", saved_username);
         if (strcmp(username, saved_username) == 0) {
             fclose(file);
             return true; // Profile found
         }
     }
     fclose(file);
     return false; // Profile not found
 }
 
 /**
  * brief Validates username format
  * 
  * Ensures username meets requirements:
  * - 3-20 characters long
  * - Starts with a letter
  * - Contains only letters, numbers, and underscores
  */
 bool is_valid_username(const char *username) {
     if (username == NULL || strlen(username) < 3 || strlen(username) > 20) {
         return false;
     }
     
     // Check if first character is alphabetic
     if (!isalpha(username[0])) {
         return false;
     }
     
     // Check remaining characters (alphanumeric and underscore only)
     for (int i = 0; username[i] != '\0'; i++) {
         if (!isalnum(username[i]) && username[i] != '_') {
             return false;
         }
     }
     return true;
 }
 
 /**
  * brief Validates phone number format
  * 
  * Ensures phone number meets requirements:
  * - 10-15 digits long
  * - Contains only digits (no other characters)
  */
 bool is_valid_phone(const char *phone) {
     if (phone == NULL || strlen(phone) < 10 || strlen(phone) > 15) {
         return false;
     }
     
     // Check if all characters are digits
     for (int i = 0; phone[i] != '\0'; i++) {
         if (!isdigit(phone[i])) {
             return false;
         }
     }
     return true;
 }