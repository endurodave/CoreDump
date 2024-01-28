#include "CoreDump.h"
#include "Options.h"
#include <cstring>

// Core dump data stored in RAM.
// TODO: This data structure must not be zero-initialized at startup!
// The data stored here must persist through a CPU reset. Platform-specific 
// implementation detail to place this data structure outside of BSS data section. 
// See your compiler linker documentation on how to place data into a different 
// section. The data should remain uninitialized by the C runtime. Also, if a 
// bootloader is used, the bootloader application must also not initialize this data. 
// In short, the bootload and main application must agree upon a non zero-initialized
// section to hold the CoreDumpData below.
static CoreDumpData _coreDumpData;

static void StoreCallStack(uint32_t* stackPointer, uint32_t* stackStoreArr, int stackStoreArrLen)
{
    int stackDepth = 0;
    int depth = 0;

    // Clear the core dump call stack storage
    memset(stackStoreArr, 0, sizeof(uint32_t) * stackStoreArrLen);

    // Ensure the stack pointer is within RAM address range
    if (((uint32_t)stackPointer < RAM_BEGIN || (uint32_t)stackPointer > RAM_END))
        return;

    // Search the stack for address values within the flash address range. 
    // We're looking for stored LR (link register) values pushed onto the stack.
    for (depth = 0; depth < MAX_STACK_DEPTH_SEARCH; depth++)
    {
        // Get a 32-bit value from the stack
        uint32_t stackData = *(stackPointer + depth);

        // Have we reached the beginning of the stack?
        if (stackData == STACK_MARKER && *(stackPointer + depth + 1) == STACK_MARKER)
            break;

        // Is the stack value within the flash address range? This is the 
        // check to determine if the address stored within the stack is a 
        // return address. Later, a PC addr2line tool can convert each
        // address to a file name/line number.
        if (stackData >= FLASH_BASE && stackData <= FLASH_END)
        {
            // Save the function address within the core dump structure
            stackStoreArr[stackDepth++] = stackData;
        }

        // Have we reached the maximum stack depth to store?
        if (stackDepth >= stackStoreArrLen)
            break;
    }
}

// Store all thread call stacks into core dump 
static void StoreThreadCallStacks()
{
#ifdef USE_OPERATING_SYSTEM
    // TODO: The code below assumes that each task has a Task Control Block (TCB).
    // The stack starting location is obtained from the TCB (ie

    int taskCnt = 0;

    memset(_coreDumpData.ThreadCallStacks, 0, sizeof(_coreDumpData.ThreadCallStacks));

    // For each system task
    for (int t = 0; t <= OS_TASKCNT; t++)
    {
        // Is there a task control block?
        if (os_active_TCB[t] == NULL)
            continue;

        P_TCB p_TCB = (P_TCB)(os_active_TCB[t]);
        uint32_t* stackPointer = p_TCB->tsk_stack;

        // Store the call stack for this task
        StoreCallStack(stackPointer, &_coreDumpData.ThreadCallStacks[taskCnt][0], CALL_STACK_SIZE);

        if (++taskCnt >= CRASH_DUMP_TASK_SIZE)
            break;
    }
#endif
}

void CoreDumpStore(uint32_t* stackPointer, const char* fileName,
    uint32_t lineNumber, uint32_t auxCode)
{
    // Is a core dump already stored? Then don't overwrite. The first  
    // core dump is what is needed, not any subsequent crashes detected
    // after the first one.
    if (IsCoreDumpSaved())
        return;

    // Set the key indicating a core dump is stored 
    _coreDumpData.Key = KEY_CORE_DUMP_STORED;
    _coreDumpData.NotKey = ~KEY_CORE_DUMP_STORED;

    _coreDumpData.SoftwareVersion = SOFTWARE_VERSION;
    _coreDumpData.AuxCode = auxCode;

    // If the caller provides a stack pointer it means that this
    // function is being called from an ISR handler and register 
    // values are pushed onto the stack by the CPU. 
    if (stackPointer != 0)
    {
        // Hardware exception occurred!
        _coreDumpData.Type = FAULT_EXCEPTION;

        // Store all the registers located on the stack
        // TODO: Registers pushed onto stack is a platform-specific implementation detail.
        // See your CPU processor, BSP, and/or compiler documentation. 
#ifdef USE_HARDWARE
        _coreDumpData.R0_register = *stackPointer;
        _coreDumpData.R1_register = *(stackPointer + 1);
        _coreDumpData.R2_register = *(stackPointer + 2);
        _coreDumpData.R3_register = *(stackPointer + 3);
        _coreDumpData.R12_register = *(stackPointer + 4);
        _coreDumpData.LR_register = *(stackPointer + 5);
        _coreDumpData.PC_register = *(stackPointer + 6);
        _coreDumpData.XPSR_register = *(stackPointer + 7);
#endif

#ifdef USE_HARDWARE
        // Store other registers to assist with exception decoding as necessary
        // TODO: Obtaining registers and storing is a platform-specific implementation detail.
        // See your CPU processor, BSP, and/or compiler documentation.
        _coreDumpData.CFSR_register = SCB->CFSR;
        _coreDumpData.HFSR_register = SCB->HFSR;
        _coreDumpData.MMFAR_register = SCB->MMFAR;
        _coreDumpData.BFAR_register = SCB->BFAR;
        _coreDumpData.AFSR_register = SCB->AFSR;
#endif
    }
    else
    {
        // Software assertion occurred!
        _coreDumpData.Type = SOFTWARE_ASSERTION;
    }

    // Save file name and line number
    _coreDumpData.LineNumber = lineNumber;
    if (fileName != NULL)
    {
        strncpy(_coreDumpData.FileName, fileName, FILE_NAME_LEN);
        _coreDumpData.FileName[FILE_NAME_LEN - 1] = 0;
    }

    // Get the stack pointer if none passed in
    if (stackPointer == 0)
    {
#ifdef USE_HARDWARE
        // Get current stack location using SP register
        // TODO: Obtaining registers and storing is a platform-specific implementation detail.
        // See your CPU processor, BSP, and/or compiler documentation.
        stackPointer = (uint32_t*)SP;
#endif
    }

    // Save the current call stack
    StoreCallStack(stackPointer, &_coreDumpData.ActiveCallStack[0], CALL_STACK_SIZE);
}

bool IsCoreDumpSaved()
{
    if (_coreDumpData.Key == KEY_CORE_DUMP_STORED &&
        _coreDumpData.NotKey == ~KEY_CORE_DUMP_STORED)
        return true;
    else
        return false;
}

CoreDumpData* CoreDumpGet()
{
    return &_coreDumpData;
}

void CoreDumpReset()
{
    _coreDumpData.Key = 0;
    _coreDumpData.NotKey = 0;
}