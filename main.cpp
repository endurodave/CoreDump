// @see https://github.com/endurodave/CoreDump
// David Lafreniere, January 2024.

// Search TODO in source code for platform-specific details required for deployment
// on an embedded system

#include "Fault.h"
#include "CoreDump.h"
#include "Options.h"

#ifdef HARD_FAULT_TEST
static int val = 2, zero = 0, result;
#endif

//----------------------------------------------------------------------------
// Call3
//----------------------------------------------------------------------------
int Call3()
{
    unsigned int stackArr3[5] = { 0x33333333, 0x33333333, 0x33333333, 0x33333333, 0x33333333 };

#ifdef HARD_FAULT_TEST
    // Cause a divide by 0 exception. HardFaultHandler will be called.
    result = val / zero;
#else
    // Cause a software fault. FaultHandler() will be called. 
    ASSERT();
#endif

    return stackArr3[0];
}

//----------------------------------------------------------------------------
// Call2
//----------------------------------------------------------------------------
int Call2()
{
    unsigned int stackArr2[5] = { 0x22222222, 0x22222222, 0x22222222, 0x22222222, 0x22222222 };
    Call3();
    return stackArr2[0];
}

//----------------------------------------------------------------------------
// Call1
//----------------------------------------------------------------------------
int Call1()
{
    unsigned int stackArr1[5] = { 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111 };
    Call2();
    return stackArr1[0];
}

//----------------------------------------------------------------------------
// main
//----------------------------------------------------------------------------
int main(void)
{
    // Mark the beginning of the stack with a marker pattern. Each task in the 
    // system needs a marker to define the stack top. Your OS might already do 
    // this, but just incase here is a manual method. 
    unsigned int stackArr0[5] = { STACK_MARKER, STACK_MARKER, STACK_MARKER, STACK_MARKER, STACK_MARKER };

#ifdef USE_HARDWARE
    // Enable divide by 0 hardware exception
    SCB->CCR |= 0x10;
#endif

    // Did a core dump get saved? i.e. Did CPU start due to a FaultHandler or
    // HardFaultHandler reset?
    if (IsCoreDumpSaved() == true)
    {
        // Get the saved core dump data structure
        CoreDumpData* coreDumpData = CoreDumpGet();

        // TODO: Save core dump to persistent storage or transmit.
        // the data to a remote device. Platform-specific implementation detail
        // on where to persist the RAM core dump data to a permanent storage device.

        // Reset core dump for next time. 
        CoreDumpReset();
    }

    // Create call stack by calling a few functions
    Call1();

    return stackArr0[0];
}
