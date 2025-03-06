#pragma once

#ifndef ARDUINO

/**
 * @brief Convert integer to string
 *
 * This is a replacement for the standard itoa function which may not be
 * available on all systems
 */
inline char *itoa(int value, char *result, int base) {
  if (base < 2 || base > 36) {
    *result = '\0';
    return result;
  }

  char *ptr = result, *ptr1 = result, tmp_char;
  int tmp_value;

  do {
    tmp_value = value;
    value /= base;
    *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[abs(tmp_value - value * base)];
  } while (value);

  // Apply negative sign
  if (tmp_value < 0) *ptr++ = '-';
  *ptr-- = '\0';

  // Reverse the string
  while (ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr-- = *ptr1;
    *ptr1++ = tmp_char;
  }

  return result;
}

/**
 * @brief Convert unsigned integer to string
 */
inline char *utoa(unsigned int value, char *result, int base) {
  if (base < 2 || base > 36) {
    *result = '\0';
    return result;
  }

  char *ptr = result, *ptr1 = result, tmp_char;
  unsigned int tmp_value;

  do {
    tmp_value = value;
    value /= base;
    *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[tmp_value - value * base];
  } while (value);

  *ptr-- = '\0';

  // Reverse the string
  while (ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr-- = *ptr1;
    *ptr1++ = tmp_char;
  }

  return result;
}

/**
 * @brief Convert long integer to string
 */
inline char *ltoa(long value, char *result, int base) {
  if (base < 2 || base > 36) {
    *result = '\0';
    return result;
  }

  char *ptr = result, *ptr1 = result, tmp_char;
  long tmp_value;

  do {
    tmp_value = value;
    value /= base;
    *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[abs(static_cast<int>(tmp_value - value * base))];
  } while (value);

  // Apply negative sign
  if (tmp_value < 0) *ptr++ = '-';
  *ptr-- = '\0';

  // Reverse the string
  while (ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr-- = *ptr1;
    *ptr1++ = tmp_char;
  }

  return result;
}

/**
 * @brief Convert unsigned long integer to string
 */
inline char *ultoa(unsigned long value, char *result, int base) {
  if (base < 2 || base > 36) {
    *result = '\0';
    return result;
  }

  char *ptr = result, *ptr1 = result, tmp_char;
  unsigned long tmp_value;

  do {
    tmp_value = value;
    value /= base;
    *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[static_cast<int>(tmp_value - value * base)];
  } while (value);

  *ptr-- = '\0';

  // Reverse the string
  while (ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr-- = *ptr1;
    *ptr1++ = tmp_char;
  }

  return result;
}

#endif
