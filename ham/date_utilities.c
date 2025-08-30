/**
 * file date_utilities.c
 * brief Implementation of date handling and validation functions
 * 
 * This file contains the implementation of date-related functions
 * for parsing, validating, and calculating date differences.
 */

 #include "date_utilities.h"
 #include <stdio.h>        // For sscanf, sprintf
 #include <string.h>       // For string manipulation functions
 #include <ctype.h>        // For character type functions
 #include <time.h>         // For time and date functions
 
 /**
  * brief Validates a date string in YYYY-MM-DD format
  * 
  * Checks if the date has the correct format and valid values for
  * year, month, and day. Year must be >= 2024, month between 1-12,
  * and day between 1-31 (no month-specific day validation).
  */
 bool validate_date(const char *date) {
     int y, m, d;
     // Parse the date string into components
     if (sscanf(date, "%d-%d-%d", &y, &m, &d) != 3) return false;
     // Validate ranges
     return (y >= 2024 && m >= 1 && m <= 12 && d >= 1 && d <= 31);
 }
 
 /**
  * brief Parses various date formats into standard YYYY-MM-DD format
  * 
  * Supports both YYYY-MM-DD and DD/MM/YYYY formats. If the input
  * format is invalid, outputs "invalid".
  */
 void parse_date(const char *input, char *output) {
     int y, m, d;
     // Try YYYY-MM-DD format first
     if (sscanf(input, "%d-%d-%d", &y, &m, &d) == 3) {
         sprintf(output, "%04d-%02d-%02d", y, m, d);
     } 
     // Try DD/MM/YYYY format
     else if (sscanf(input, "%d/%d/%d", &d, &m, &y) == 3) {
         sprintf(output, "%04d-%02d-%02d", y, m, d);
     } 
     // Invalid format
     else {
         strcpy(output, "invalid");
     }
 }
 
 /**
  * brief Calculates the number of nights between two dates
  * 
  * Uses the standard C time functions to calculate the exact
  * number of days between check-in and check-out dates.
  */
 int calculate_duration(const char *check_in, const char *check_out) {
     struct tm tm1 = {0}, tm2 = {0};
     // Parse check-in date
     sscanf(check_in, "%d-%d-%d", &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday);
     // Parse check-out date
     sscanf(check_out, "%d-%d-%d", &tm2.tm_year, &tm2.tm_mon, &tm2.tm_mday);
     
     // Adjust for struct tm conventions (years since 1900, months 0-11)
     tm1.tm_year -= 1900; 
     tm1.tm_mon -= 1;
     tm2.tm_year -= 1900;
     tm2.tm_mon -= 1;
     
     // Convert to time_t values
     time_t t1 = mktime(&tm1);
     time_t t2 = mktime(&tm2);
     
     // Calculate difference in days
     return (t2 - t1) / (60 * 60 * 24);
 }
 
 /**
  * brief Checks if a date is today or in the future
  * 
  * Compares the given date with the current system date
  * to determine if it's today or a future date.
  */
 bool is_date_in_future(const char *date) {
     time_t now = time(NULL);
     struct tm *now_tm = localtime(&now);
     char today[11];
     // Format current date as YYYY-MM-DD
     strftime(today, sizeof(today), "%Y-%m-%d", now_tm);
     
     // Compare with the given date
     return strcmp(date, today) >= 0;
 }
 
 /**
  * brief Calculates the start and end dates of the week for a given date
  * 
  * Finds the Monday and Sunday of the week containing the given date.
  * Useful for weekly revenue calculations.
  */
 void get_week_range(const char *date, char *start_date, char *end_date) {
     int year, month, day;
     // Parse the input date
     sscanf(date, "%d-%d-%d", &year, &month, &day);
 
     // Create a tm structure for the given date
     struct tm tm = {0};
     tm.tm_year = year - 1900;
     tm.tm_mon = month - 1;
     tm.tm_mday = day;
     // Normalize the tm structure and fill in week day
     mktime(&tm);
 
     // Calculate days to Monday (assuming Monday is start of week)
     // tm_wday: 0=Sunday, 1=Monday, ..., 6=Saturday
     int days_to_monday = (tm.tm_wday == 0) ? -6 : 1 - tm.tm_wday;
     
     // Create Monday date
     struct tm monday = tm;
     monday.tm_mday += days_to_monday;
     mktime(&monday); // Normalize
 
     // Create Sunday date (6 days after Monday)
     struct tm sunday = monday;
     sunday.tm_mday += 6;
     mktime(&sunday); // Normalize
 
     // Format dates as strings
     strftime(start_date, 11, "%Y-%m-%d", &monday);
     strftime(end_date, 11, "%Y-%m-%d", &sunday);
 }