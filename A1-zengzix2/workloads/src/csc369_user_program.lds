/*
 * This is a Linker script. It tells the linker where to put different sections from object files. You will see the
 * symbol "." below, which is actually a variable called the location counter. As we add sections, the location counter
 * increases.
 */
OUTPUT_FORMAT("elf64-x86-64")

/*The entry point of our executable.*/
ENTRY(_CSC369_start)

SECTIONS
{
    /* Start the text segment at address 0. */
    . = 0x00000000;
    .text : {
        /* Match the .text section of every input file. */
        *(.text)
    }

    /* Ignore all other sections. */
}
