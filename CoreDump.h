#ifndef _CORE_DUMP_H
#define _CORE_DUMP_H

#include <stdint.h>

// A marker used to indicate the top of the stack
#define STACK_MARKER  0xEFEFEFEF

// A unique key to determine if a core dump data structure is valid
#define KEY_CORE_DUMP_STORED    0xDEADBEEF

// Software application version number
#define SOFTWARE_VERSION    1234

// Maximum file length name stored in core dump
#define FILE_NAME_LEN       128

// The depth of the core dump call stack stored
#define CALL_STACK_SIZE     8

// How far back into the stack to search in 32-bit words.
// e.g. 1024 x 4 = 4k search depth
#define MAX_STACK_DEPTH_SEARCH      1024

// TODO: Define the RAM start and stop addresses. Platform specific detail.
// See your processor memory map for values.
#define RAM_BEGIN   0x100000
#define RAM_END     0x200000

// TODO: Define the flash start and stop addresses. Platform specific detail.
// See your processor memory map for values.
#define FLASH_BASE  0x400000
#define FLASH_END   0x500000

// TODO: How many operating system tasks to store within the core dump.
#define OS_TASKCNT  5

enum FaultType
{
    FAULT_EXCEPTION,        // Hardware exception
    SOFTWARE_ASSERTION      // Software assertion 
};

/// Core dump data structure
class CoreDumpData
{
public:
    uint32_t Key;
    uint32_t NotKey;
    uint32_t SoftwareVersion;
    uint32_t AuxCode;
    FaultType Type;

    uint32_t LineNumber;
    char FileName[FILE_NAME_LEN];

#ifdef USE_HARDWARE
    uint32_t R0_register;
    uint32_t R1_register;
    uint32_t R2_register;
    uint32_t R3_register;
    uint32_t R12_register;
    uint32_t LR_register;
    uint32_t PC_register;
    uint32_t XPSR_register;
#endif

    uint32_t ActiveCallStack[CALL_STACK_SIZE];

#ifdef USE_OPERATING_SYSTEM
    uint32_t ThreadCallStacks[OS_TASKCNT][CALL_STACK_SIZE];
#endif
};

/// Store core dump data.
/// @param[in] stackPointer - the current call stack pointer or 0.
/// @param[in] fileName - file name causing error
/// @param[in] lineNumber - line number causing error
/// @param[in] auxCode - any additional number, or 0
void CoreDumpStore(uint32_t* stackPointer, const char* fileName,
    uint32_t lineNumber, uint32_t auxCode);

/// Get the core dump saved state
/// @return Returns true if core dump data is saved.
bool IsCoreDumpSaved();

/// Get core dump data structure
/// @return A pointer to the core dump data structure.
CoreDumpData* CoreDumpGet();

/// Reset core dump data structure.
void CoreDumpReset();

#endif 
