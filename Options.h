#ifndef _OPTIONS_H
#define _OPTIONS_H

// TODO: Enable one or more options below as required

// Define to enable hardware-specific features based on a specific platform
// Most code compiled within USE_HARDWARE is for ARM
//#define USE_HARDWARE

// Define to enabled operating system specific features
//#define USE_OPERATING_SYSTEM

// Define to test a hardware fault; otherwise test software fault
//#define HARD_FAULT_TEST

// Define to use GCC __builtin_frame_address() for active call stack
//#define USE_BUILTIN_BACKTRACE

// Define to use GCC backtrace and backtrace_symbols for active call stack
#ifdef __linux__
#define USE_LINUX_BACKTRACE
#endif

// Define to use Windows backtrace method. Must link with DbgHelp.lib.
#ifdef WIN32
#define USE_WINDOWS_BACKTRACE
#endif

#endif 
