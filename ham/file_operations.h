/**
 * file file_operations.h
 * brief File I/O operations and data persistence
 * 
 * Handles all file operations including user data, room data, 
 * bookings, and system configuration.
 */

 #ifndef FILE_OPERATIONS_H
 #define FILE_OPERATIONS_H
 
 #include "booking_operations.h"
 #include "room_management.h"
 
 // File path constants
 #define ROOM_FILE "data/rooms.txt"              ///< Room data file
 #define BOOKING_FILE "data/bookings.txt"        ///< Booking data file
 #define USER_FILE "data/users.txt"              ///< User credentials file
 #define USER_PROFILE_FILE "data/user_profiles.txt" ///< User profile data file
 #define ADMIN_PASS_FILE "data/admin_pass.txt"   ///< Admin password file
 
 /**
  * brief Initializes all data files required by the system
  * 
  * Creates data directory and initial files with default data if they don't exist
  */
 void initialize_files();
 
 /**
  * brief Changes the administrator password
  * param new_password New password to set
  */
 void change_admin_password(const char *new_password);
 
 /**
  * brief Saves user profile information
  * param username User's username
  * param full_name User's full name
  * param id_number User's ID number
  * param email User's email address
  * param address User's physical address
  * param phone User's phone number
  */
 void save_user_profile(const char *username, const char *full_name, 
                       const char *id_number, const char *email, 
                       const char *address, const char *phone);
 
 /**
  * brief Retrieves user profile information
  * param username Username to retrieve profile for
  * param full_name Buffer for full name
  * param id_number Buffer for ID number
  * param email Buffer for email address
  * param address Buffer for physical address
  * param phone Buffer for phone number
  */
 void get_user_profile(const char *username, char *full_name, char *id_number, 
                      char *email, char *address, char *phone);
 
 /**
  * brief Registers a new user
  * param username Desired username
  * param phone User's phone number
  * return true if registration successful, false if username already exists
  */
 bool register_user(const char *username, const char *phone);
 
 /**
  * brief Validates user credentials
  * param username Username to validate
  * param phone Phone number to validate
  * return true if credentials are valid, false otherwise
  */
 bool validate_user_credentials(const char *username, const char *phone);
 
 /**
  * brief Checks if a username already exists
  * param username Username to check
  * return true if username exists, false otherwise
  */
 bool username_exists(const char *username);
 
 #endif