/**
 * file file_operations.c
 * brief Implementation of file I/O operations
 * 
 * Contains functions for handling all file operations including
 * user data, room data, bookings, and system configuration.
 */

 #include "file_operations.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <sys/stat.h>  // For directory creation
 
 /**
  * brief Initializes all data files required by the system
  * 
  * Creates the data directory and all necessary data files with
  * default content if they don't already exist.
  */
 void initialize_files() {
     // Create data directory if it doesn't exist
     #ifdef _WIN32
         system("mkdir data 2>nul");  // Windows command
     #else
         mkdir("data", 0755);         // Linux/Mac command
     #endif
     
     FILE *file;
     
     // Create room file with default rooms if not exists
     file = fopen(ROOM_FILE, "r");
     if (file == NULL) {
         file = fopen(ROOM_FILE, "w");
         // Add default rooms with different types and facilities
         fprintf(file, "101:Single:100.00:Available:WiFi,TV,AC\n");
         fprintf(file, "102:Double:150.00:Available:WiFi,TV,AC,Meal Service\n");
         fprintf(file, "103:Suite:300.00:Available:WiFi,TV,AC,Meal Service,Jacuzzi\n");
         fprintf(file, "104:Single:120.00:Available:WiFi,TV,AC,Balcony\n");
         fprintf(file, "105:Double:180.00:Available:WiFi,TV,AC,Meal Service,Balcony\n");
         fclose(file);
     } else {
         fclose(file);
     }
 
     // Create empty booking file if not exists
     file = fopen(BOOKING_FILE, "r");
     if (file == NULL) {
         file = fopen(BOOKING_FILE, "w");
         fclose(file);
     } else {
         fclose(file);
     }
 
     // Create empty user file if not exists
     file = fopen(USER_FILE, "r");
     if (file == NULL) {
         file = fopen(USER_FILE, "w");
         fclose(file);
     } else {
         fclose(file);
     }
 
     // Create empty user profile file if not exists
     file = fopen(USER_PROFILE_FILE, "r");
     if (file == NULL) {
         file = fopen(USER_PROFILE_FILE, "w");
         fclose(file);
     } else {
         fclose(file);
     }
 
     // Create admin password file if not exists
     file = fopen(ADMIN_PASS_FILE, "r");
     if (file == NULL) {
         file = fopen(ADMIN_PASS_FILE, "w");
         fprintf(file, "admin123\n");  // Default admin password
         fclose(file);
     } else {
         fclose(file);
     }
 }
 
 /**
  * brief Changes the administrator password
  * 
  * Overwrites the admin password file with the new password.
  * Note: In a production system, passwords should be hashed.
  */
 void change_admin_password(const char *new_password) {
     FILE *file = fopen(ADMIN_PASS_FILE, "w");
     if (file == NULL) {
         return; // Silently fail - error handling would be better
     }
     fprintf(file, "%s\n", new_password);
     fclose(file);
 }
 
 /**
  * brief Saves user profile information
  * 
  * Saves or updates a user's profile information in the profiles file.
  * If the profile already exists, it's updated; otherwise, it's added.
  */
 void save_user_profile(const char *username, const char *full_name, 
                       const char *id_number, const char *email, 
                       const char *address, const char *phone) {
     FILE *file = fopen(USER_PROFILE_FILE, "r");
     if (file != NULL) {
         char line[MAX_LEN * 4];
         char temp_file[] = "data/temp_profiles.txt";
         FILE *temp = fopen(temp_file, "w");
         
         int found = 0;
         // Read existing profiles
         while (fgets(line, sizeof(line), file)) {
             char saved_username[MAX_LEN];
             sscanf(line, "%[^:]:", saved_username);
             
             if (strcmp(username, saved_username) == 0) {
                 // Replace existing profile
                 fprintf(temp, "%s:%s:%s:%s:%s:%s\n", 
                         username, full_name, id_number, email, address, phone);
                 found = 1;
             } else {
                 // Keep other profiles unchanged
                 fputs(line, temp);
             }
         }
         
         if (!found) {
             // Add new profile if not found
             fprintf(temp, "%s:%s:%s:%s:%s:%s\n", 
                     username, full_name, id_number, email, address, phone);
         }
         
         fclose(file);
         fclose(temp);
         
         // Replace old file with updated file
         remove(USER_PROFILE_FILE);
         rename(temp_file, USER_PROFILE_FILE);
         return;
     }
     
     // If file doesn't exist, create it with the new profile
     file = fopen(USER_PROFILE_FILE, "a");
     if (file == NULL) return;
     fprintf(file, "%s:%s:%s:%s:%s:%s\n", 
             username, full_name, id_number, email, address, phone);
     fclose(file);
 }
 
 /**
  * brief Retrieves user profile information
  * 
  * Looks up a user's profile information by username and returns
  * the data in the provided buffers.
  */
 void get_user_profile(const char *username, char *full_name, char *id_number, 
                      char *email, char *address, char *phone) {
     FILE *file = fopen(USER_PROFILE_FILE, "r");
     if (file == NULL) {
         // Return empty strings if file doesn't exist
         strcpy(full_name, "");
         strcpy(id_number, "");
         strcpy(email, "");
         strcpy(address, "");
         strcpy(phone, "");
         return;
     }
 
     char line[MAX_LEN * 4];
     char current_username[MAX_LEN];
     // Read each profile from file
     while (fgets(line, sizeof(line), file)) {
         // Parse profile data
         sscanf(line, "%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%[^\n]", 
                current_username, full_name, id_number, email, address, phone);
         
         if (strcmp(username, current_username) == 0) {
             fclose(file);
             return; // Profile found
         }
     }
     fclose(file);
     
     // Profile not found, return empty strings
     strcpy(full_name, "");
     strcpy(id_number, "");
     strcpy(email, "");
     strcpy(address, "");
     strcpy(phone, "");
 }
 
 /**
  * brief Registers a new user
  * 
  * Adds a new user to the user credentials file if the username
  * is not already taken.
  */
 bool register_user(const char *username, const char *phone) {
     if (username_exists(username)) {
         return false; // Username already taken
     }
     
     FILE *file = fopen(USER_FILE, "a");
     if (file == NULL) return false;
     
     // Save username:phone format
     fprintf(file, "%s:%s\n", username, phone);
     fclose(file);
     return true;
 }
 
 /**
  * brief Validates user credentials
  * 
  * Checks if the provided username and phone match a user
  * in the credentials file.
  */
 bool validate_user_credentials(const char *username, const char *phone) {
     FILE *file = fopen(USER_FILE, "r");
     if (file == NULL) return false;
 
     char line[MAX_LEN];
     // Read each user from file
     while (fgets(line, sizeof(line), file)) {
         char saved_username[MAX_LEN], saved_phone[15];
         // Parse username:phone format
         sscanf(line, "%[^:]:%s", saved_username, saved_phone);
         if (strcmp(username, saved_username) == 0 && strcmp(phone, saved_phone) == 0) {
             fclose(file);
             return true; // Credentials match
         }
     }
     fclose(file);
     return false; // No matching credentials found
 }
 
 /**
  * brief Checks if a username already exists
  * 
  * Searches the user credentials file to determine if a username
  * is already taken.
  */
 bool username_exists(const char *username) {
     FILE *file = fopen(USER_FILE, "r");
     if (file == NULL) return false;
 
     char line[MAX_LEN];
     // Read each user from file
     while (fgets(line, sizeof(line), file)) {
         char saved_username[MAX_LEN], saved_phone[15];
         // Parse username:phone format
         sscanf(line, "%[^:]:%s", saved_username, saved_phone);
         if (strcmp(username, saved_username) == 0) {
             fclose(file);
             return true; // Username found
         }
     }
     fclose(file);
     return false; // Username not found
 }