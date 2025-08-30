/**
 * file room_management.c
 * brief Implementation of room management functions
 * 
 * Contains functions for loading, filtering, and sorting hotel rooms
 * from the data file.
 */

 #include "room_management.h"
 #include "file_operations.h"  // Add this include for ROOM_FILE
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 /**
  * @brief Comparison function for sorting rooms by price
  * 
  * Used by qsort() to sort rooms in ascending order by price.
  * Returns negative if a < b, positive if a > b, zero if equal.
  */
 int compare_rooms_by_price(const void *a, const void *b) {
     const Room *room_a = (const Room *)a;
     const Room *room_b = (const Room *)b;
     if (room_a->price < room_b->price) return -1;
     if (room_a->price > room_b->price) return 1;
     return 0;
 }
 
 /**
  * @brief Loads rooms from file with optional filtering
  * 
  * Reads rooms from the room data file, applies optional filters,
  * sorts them by price, and makes them available for display.
  * 
  * Note: This function prepares data but doesn't directly display it.
  * The actual display is handled by GUI functions.
  */
 void load_rooms(RoomFilter *filter) {
     FILE *file = fopen(ROOM_FILE, "r");  // Now ROOM_FILE is defined
     if (file == NULL) return;
 
     // This function will be implemented in GUI context
     // since it needs to interact with GTK widgets
     fclose(file);
 }