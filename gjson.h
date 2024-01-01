/**
 * \file
 * Embedded JSON builder and parser
 * PROJECT: GJSON library
 * TARGET SYSTEM: Arduino
 */

#ifndef GJSON_H
#define GJSON_H

namespace gjson {

#if !defined(GJSON_PRINT)

#if !defined(_MSC_VER)
/**
 * Macro to print a fixed string.  Define to override all the GJSON_PRINT macros
 *
 * \param[in] s String
 */
#define GJSON_PRINT(s)                      Serial.print(s)


/**
 * Macro to print a data string.  Define to override
 *
 * \param[in] s String
 */
#define GJSON_PRINTSTR(s)                   Serial.print(s)


/**
 * Macro to print a fixed string with newline.  Define to override
 *
 * \param[in] s String
 */
#define GJSON_PRINTLINE(s)                  Serial.println(s)

#else // defined(_MSC_VER)

#define GJSON_PRINT(s)                      printf(s)
#define GJSON_PRINTSTR(s)                   printf("%s", s)
#define GJSON_PRINTLINE(s)                  printf(s ## "\n")

#endif // defined(_MSC_VER)

#endif // !defined(GJSON_PRINT)


#if !defined(GJSON_PACKING)

#if defined(ARDUINO_ARCH_AVR)
/**
 * Macro to control structure and class element packing, define if not using AVR or you wish packing to be something other than 1 (default)
 */
#define    GJSON_PACKING                    1            // Optimised for AVR
#endif // defined(ARDUINO_ARCH_AVR)

#endif // !defined(GJSON_PACKING)


#if !defined(GJSON_NEW_OBJECT)

#if !defined(_MSC_VER)
/**
 * Macro to allocate new JSON object on heap.  Some RTOS employ their own memory allocation and deallocation process - define these 
 * to implement a custom process to meet your requirements otherwise leave with these heap allocations as defaults.  For release 
 * see \ref GJSON_RELEASE_OBJECT
 *
 * \param[in] o Type with any initialisation
 * \param[in] t Type only
 * \return Pointer to object or NULL
 */
#define GJSON_NEW_OBJECT(o,t)               (new o)
//#define GJSON_NEW_OBJECT(o,t)               ((std::nothrow) new o)
#else
#define GJSON_NEW_OBJECT(o,t)               (new o)
#endif


/**
 * Macro to release existing JSON object off heap.  Define your own with accompanying macro \ref GJSON_NEW_OBJECT if heap not desired
 *
 * \param[in] o Object pointer
 * \return Pointer to object or NULL
 */
#define GJSON_RELEASE_OBJECT(o)             (delete o)

#endif // !defined(GJSON_NEW_OBJECT)

#if !defined(GJSON_DEFAULT_EXPONENTIAL_NOTATION)
/**
 * Build macro to control default acceptance of exponential notation in JSON literal strings
 *
 * \note There maybe a reason your client doesnt support numeric literal strings with exponential notation
 * \attention Define before include to override
 */
#define GJSON_DEFAULT_EXPONENTIAL_NOTATION  true
#endif // defined(GJSON_DEFAULT_EXPONENTIAL_NOTATION)


#if !defined(GJSON_REPLACE_STRNCMPI)
/**
 * Macro string compare case sensitive.  Redirect to library calls if they exist in your build
 *
 * \param[in] s1 String 1
 * \param[in] s2 String 2
 * \param[in] n Length in characters (optional including NULL)
 * \return int Difference.  s1 == s2 => 0, s1 < s2 => -n, s2 > s2 => +n
 */
#define GJSON_REPLACE_STRNCMPI(s1, s2, n)   (gjson::JSONSupport::StrNCmp(s1, s2, n, false))
#endif


#if !defined(GJSON_REPLACE_STRNCMP)
/**
 * Macro string compare aid for library function strncmp, update as required
 *
 * \param[in] s1 String 1
 * \param[in] s2 String 2
 * \param[in] n Length in characters (optional including NULL)
 * \return int Difference.  s1 == s2 => 0, s1 < s2 => -n, s2 > s2 => +n
 */
#define GJSON_REPLACE_STRNCMP(s1, s2, n)    (gjson::JSONSupport::StrNCmp(s1, s2, n, true))
#endif


}; // namespace gjson

#if defined(GJSON_PACKING)
#pragma pack(push, GJSON_PACKING)
#endif

// Library includes
#include "gjson_support.h"
#include "gjson_elements.h"
#include "gjson_builders.h"
#include "gjson_io_buffer.h"
#include "gjson_parser.h"

#if defined(GJSON_PACKING)
#pragma pack(pop)
#endif

#endif // GJSON_H
