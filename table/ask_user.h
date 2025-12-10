#ifndef ASK_USER_H
#define ASK_USER_H

#include <stdbool.h>

/**
 * @brief Reads a string from stdin.
 * @return Pointer to dynamically allocated string (must be freed).
 */
char *read_string(void);

/**
 * @brief Checks a string for numeric validity and range.
 * @param string Input string.
 * @return 'A', 'h', 'l', 'i', or 'f' depending on validity and range.
 */
char num_checkert(char *string);

/**
 * @brief Asks the user for minimum comfort temperature.
 * @return Pointer to validated input string (must be freed).
 */
char *minimum_temp(void);

/**
 * @brief Checks if the input string is a valid number for moving frequency.
 * @param nbuf Input string.
 * @return true if valid, false otherwise.
 */
bool num_checkerm(const char *nbuf);

/**
 * @brief Asks the user how often they need reminders to move.
 * @return Pointer to validated input string (must be freed).
 */
char *frequence_of_moving(void);

#endif // ASK_USER_H
