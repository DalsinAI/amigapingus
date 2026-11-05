/* A Shell gives 4 KB of stack; libnix's startup swaps to this much instead
 * (Pingus' C++ readers and screens are deep). */
unsigned long __stack = 1024 * 1024;
