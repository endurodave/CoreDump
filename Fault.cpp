#include "Fault.h"
#include "CoreDump.h"

void FaultHandler(const char* file, unsigned short line)
{
	// Store software assertion core dump data
	CoreDumpStore(0, file, line, 0);

	// TODO: Reboot CPU here! After reboot, the core dump data is used.
	
	// If you hit this line, it means one of the ASSERT macros failed.
	while (true);
}

void HardFaultHandler(void)
{
	// TODO: Called if a hardware exception is generated. Platform 
	// specific detail. Connect exceptions to call this function.

	INTEGER_TYPE* stackPointer = nullptr;	// TODO: Store the current stack pointer here
	uint32_t vectorNum = 0;					// TODO: Store the exception vector number here

#ifdef USE_HARDWARE
	// Determine if main stack or process stack is being used. Bit 2 of the 
	// LR (link register) indicates if MSP or PSP stack is used.
	if ((LR & 0x4) == 0)
		stackPointer = (unsigned int*)MSP;
	else
		stackPointer = (unsigned int*)PSP;
#endif

	// Store hardware exception core dump data
	CoreDumpStore(stackPointer, __FILE__, __LINE__, vectorNum);

	// TODO: Reboot CPU here! After reboot, the core dump data is used.

	// If you hit this line, it means a hardware exception occurred.
	while (true);
}