#ifndef _MEMCHECK_
#define _MEMCHECK_

class MemCheck {

    public:

        // Return the total virtual memory available on the system
        static long getTotalVirtualMem();

        // Return the total virtual memory in use by the current process
        static long getProcessVirtualMem();

        // Return the total physical memory (RAM) available on
        //   the system
        static long getTotalPhysicalMem();

        // Return the total physical memory (RAM) in use by the
        //   current process
        static long getProcessPhysicalMem();

};

#endif
