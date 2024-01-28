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

	uint32_t* stackPointer = nullptr;	// TODO: Store the current stack pointer here
	uint32_t vectorNum = 0;				// TODO: Store the exection vector number here

	// Store hardware exception core dump data
	CoreDumpStore(stackPointer, __FILE__, __LINE__, vectorNum);

	// TODO: Reboot CPU here! After reboot, the core dump data is used.

	// If you hit this line, it means a hardware exception occurred.
	while (true);
}