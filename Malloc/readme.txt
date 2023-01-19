The code in this file recreates the functionality of malloc using 4 different memory allocating algorithms( First Fit, Best Fit, Worst Fit, Next Fit).You would first need to
call mavalloc_init to initilaize your malloc and after that's done just call mavalloc_alloc and input in the size how much memory you wish to allocate.
In the function mavalloc_init, the 2nd parameter is your choosen algorithm, mavalloc.h has an enum containing the names of what you would need to input for your 2nd parameter.
And the 1st parameter is just the size of your memory pool.
