/**
 * file room_management.h
 * brief Room data structures and management functions
 * 
 * Defines room structures and functions for loading, filtering, 
 * and managing hotel rooms in the booking system.
 */

 #ifndef ROOM_MANAGEMENT_H
 #define ROOM_MANAGEMENT_H
 
 #include "validation.h"
 
 /**
  * brief Room structure representing a hotel room
  */
 typedef struct {
     int room_number;        ///< Unique room number
     char type[20];          ///< Room type (Single, Double, Suite)
     float price;            ///< Price per night
     char status[20];        ///< Availability status (Available, Booked)
     char facilities[100];   ///< Comma-separated list of facilities
 } Room;
 
 /**
  * brief Room filter structure for searching and filtering rooms
  */
 typedef struct {
     float min_price;        ///< Minimum price filter
     float max_price;        ///< Maximum price filter
     char type[20];          ///< Room type filter
     char facilities[100];   ///< Facilities filter
 } RoomFilter;
 
 /**
  * brief Loads rooms from file with optional filtering
  * param filter Optional filter to apply to room results
  */
 void load_rooms(RoomFilter *filter);
 
 /**
  * brief Comparison function for sorting rooms by price
  * param a First room to compare
  * param b Second room to compare
  * return Negative if a < b, positive if a > b, zero if equal
  * 
  * Used with qsort() to sort rooms by price in ascending order
  */
 int compare_rooms_by_price(const void *a, const void *b);
 
 #endif