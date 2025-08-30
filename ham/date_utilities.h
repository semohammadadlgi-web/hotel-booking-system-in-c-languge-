/**
 * file date_utilities.h
 * brief Date handling and validation functions for the hotel booking system
 * 
 * This header provides functions for date validation, parsing, and calculations
 * used throughout the booking system to handle check-in/check-out dates.
 */

 #ifndef DATE_UTILITIES_H
 #define DATE_UTILITIES_H
 
 #include <stdbool.h>
 #include <time.h>
 
 /**
  * brief Validates a date string in YYYY-MM-DD format
  * param date The date string to validate
  * return true if date is valid, false otherwise
  */
 bool validate_date(const char *date);
 
 /**
  * brief Parses various date formats into standard YYYY-MM-DD format
  * param input The input date string (can be YYYY-MM-DD or DD/MM/YYYY)
  * param output Buffer to store the formatted date (must be at least 20 chars)
  */
 void parse_date(const char *input, char *output);
 
 /**
  * brief Calculates the number of nights between two dates
  * param check_in Check-in date in YYYY-MM-DD format
  * param check_out Check-out date in YYYY-MM-DD format
  * return Number of nights between dates
  */
 int calculate_duration(const char *check_in, const char *check_out);
 
 /**
  * brief Checks if a date is today or in the future
  * param date Date to check in YYYY-MM-DD format
  * return true if date is today or future, false if past
  */
 bool is_date_in_future(const char *date);
 
 /**
  * brief Calculates the start and end dates of the week for a given date
  * param date Any date within the week
  * param start_date Buffer for start date (Monday) of the week
  * param end_date Buffer for end date (Sunday) of the week
  */
 void get_week_range(const char *date, char *start_date, char *end_date);
 
 #endif