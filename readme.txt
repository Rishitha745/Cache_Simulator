This folder consists of a file named code.c. It simulates cache with various write policies and replacements policies.

To run the program:

    To compile ,open terminal in this folder and type
    gcc code.c -lm

    and then to execute type
    ./a.out

User will be asked to provide a configuration file and an access file

Format of configuration file
    SIZE_OF_CACHE (number)
    BLOCK_SIZE (number)
    ASSOCIATIVITY (number)
    REPLACEMENT_POLICY (FIFO or LRU or RANDOM) 
    WRITEBACK_POLICY (WB or WT)

    each of above attributes must be given in a newline

Format of access file
    Mode: Address
    Mode: Address 
    ...

    Mode should be 
        R for Read
        W for write
    Address should be a hexadecimal number (maximum of 32 bits) starting with 0x