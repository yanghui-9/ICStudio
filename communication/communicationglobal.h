#ifndef COMMUNICATION_GLOBAL_H
#define COMMUNICATION_GLOBAL_H
#include <stdint.h>

#if defined(WIN32)	/* { */
#if defined(COMMUNICATION_LIBRARY)	/* { */
#define _API_COMM  extern "C" __declspec(dllexport)
#else
#define _API_COMM  extern "C" __declspec(dllimport)
#endif
#else
#define _API_COMM	 extern
#endif


#endif // COMMUNICATION_GLOBAL_H
