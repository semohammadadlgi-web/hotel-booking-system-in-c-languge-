/**
 * file validation.h
 * brief Input validation functions for the hotel booking system
 * 
 * Provides validation for user inputs, credentials, and business rules
 * to ensure data integrity throughout the application.
 */

 #ifndef VALIDATION_H
 #define VALIDATION_H
 
 #include <stdbool.h>
 
 // Maximum length constants for various inputs
 #define MAX_LEN 100
 #define MAX_USERS 100
 #define MAX_ROOMS 100
 #define MAX_BOOKINGS 100
 
 /**
  * brief Checks if a room is available for given dates
  * param room_number The room number to check
  * param check_in Check-in date in YYYY-MM-DD format
  * param check_out Check-out date in YYYY-MM-DD format
  * return true if room is available, false otherwise
  */
 bool is_room_available(int room_number, const char *check_in, const char *check_out);
 
 /**
  * brief Validates administrator password
  * param password Password to validate
  * return true if password matches stored admin password, false otherwise
  */
 bool validate_admin_password(const char *password);
 
 /**
  * brief Checks if a user profile exists
  * param username Username to check
  * return true if profile exists, false otherwise
  */
 bool profile_exists(const char *username);
 
 /**
  * brief Validates username format
  * param username Username to validate
  * return true if username meets format requirements, false otherwise
  * 
  * Requirements: 3-20 characters, starts with letter, 
  * only letters, numbers, and underscores
  */
 bool is_valid_username(const char *username);
 
 /**
  * brief Validates phone number format
  * param phone Phone number to validate
  * return true if phone meets format requirements, false otherwise
  * 
  * Requirements: 10-15 digits, numbers only
  */
 bool is_valid_phone(const char *phone);
 
 #endif