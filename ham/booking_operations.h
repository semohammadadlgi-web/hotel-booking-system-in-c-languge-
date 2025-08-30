/**
 * file booking_operations.h
 * brief Booking data structures and operations
 * 
 * Defines booking structures and functions for managing reservations,
 * calculating revenue, and handling booking-related operations.
 */

 #ifndef BOOKING_OPERATIONS_H
 #define BOOKING_OPERATIONS_H
 
 #include "validation.h"
 #include "date_utilities.h"
 
 /**
  * brief Booking structure representing a hotel reservation
  */
 typedef struct {
     int booking_id;             ///< Unique booking identifier
     int room_number;            ///< Booked room number
     char username[MAX_LEN];     ///< Username who made the booking
     char booking_date[20];      ///< Date when booking was made
     char check_in_date[20];     ///< Planned check-in date
     char check_out_date[20];    ///< Planned check-out date
     float total_price;          ///< Total price for the stay
     char status[20];            ///< Booking status (active, canceled)
 } Booking;
 
 /**
  * brief Booking filter structure for searching bookings
  */
 typedef struct {
     int booking_id;             ///< Filter by booking ID
     char username[MAX_LEN];     ///< Filter by username
     char start_date[20];        ///< Filter by start date
     char end_date[20];          ///< Filter by end date
 } BookingFilter;
 
 /**
  * brief Loads bookings for a specific user
  * param username User whose bookings to load
  */
 void load_user_bookings(const char *username);
 
 /**
  * brief Loads all bookings with optional filters
  * param filter Optional filter to apply to bookings
  */
 void load_all_bookings(BookingFilter *filter);
 
 /**
  * brief Calculates daily revenue for a specific date
  * param date Date to calculate revenue for
  * return Total revenue for the specified date
  */
 float calculate_daily_revenue(const char *date);
 
 /**
  * brief Calculates weekly revenue for a specific date
  * param date Any date within the week to calculate revenue for
  * return Total revenue for the week containing the specified date
  */
 float calculate_weekly_revenue(const char *date);
 
 /**
  * brief Displays a booking receipt
  * param booking Booking to display receipt for
  */
 void display_receipt(Booking *booking);
 
 /**
  * brief Loads receipt history for a user
  * param username User whose receipt history to load
  */
 void load_receipt_history(const char *username);
 
 #endif