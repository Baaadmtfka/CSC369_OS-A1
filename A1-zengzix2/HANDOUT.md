# Assignment - Processes

In this assignment, you design and implement a "kernel" that runs processes on a simulated (i.e., in software) processor.
Your code loads a program and runs it on the simulated processor, which means your code "gives up control" to a library that executes these instructions.
Your code regains control through a trap handler, which is a function that the simulated processor calls when an exception occurs.
Note that this assignment focuses on the *mechanisms* and *data structures* of process management, not the *policies*.

Before starting to program *anything*, you should:
- Read the entirety of this handout
- Review the source and header files in the `workloads/src` directory.
  These are examples of "workloads" (i.e., user programs) that will run on your kernel.

The only files you should modify for submission are `src/csc369_process.h` and `src/csc369_kernel.c`.
In addition, you must submit an assignment report (`a1_report.pdf`; see the `a1_report.tex` starter file) that answers questions left throughout this handout.
Make sure to submit your files to MarkUs before the due date;
see the Syllabus for our policy on late submissions.

## Simplifications, assumptions, and constraints

Operating systems consist of a number of different sub-systems that rely on each other.
However, this assignment makes a number of simplifications so that the kernel you write focuses on key parts of process management.
Here, the kernel is completely separate from a user process.
That is, we are **not** implementing the "unix way", where kernel code and data are in the reserved part of a user's address space.
Put another way, the only code that should be executed on the simulated processor, which is managed by your kernel, is user code.

This assignment also avoids the complexities of virtual memory.
The simulated processor is initialized with 8,192 bytes of memory.
And your kernel should only keep **one** process in the simulated processor's memory at a time.
So there are no translations between virtual and physical addresses.

We have compiled the workloads to be "simple"; you can view their high-level code in `workloads/src`.
But note that the executables (in `workloads/bin`) do **not** include the C runtime or standard library.
They also do not include a data segment and, by extension, do not use the heap.
However, the workloads do use the stack, and they do make system calls.

You do not need to implement any new workloads.
But you should learn more about the workloads using command-line tools like [objdump](https://www.man7.org/linux/man-pages/man1/objdump.1.html) and [readelf](https://man7.org/linux/man-pages/man1/readelf.1.html).
The `objdump` utility helps show you the corresponding instructions the program has compiled to.
The `readelf` utility helps you learn more about the contents of the executable file's metadata.
For example,

```zsh
wolf:~/369/0test/A1/workloads/bin$ readelf -h return_3
```

Should produce the output below.
Note the "Entry point address:" field, which indicates the address your processor should begin executing the program after it has been loaded.
Hint: it is not zero, and it is not the same for every workload.

```
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Entry point address:               0xf
  Start of program headers:          64 (bytes into file)
  Start of section headers:          6840 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           56 (bytes)
  Number of program headers:         5
  Size of section headers:           64 (bytes)
  Number of section headers:         15
  Section header string table index: 14
```

Before you begin, we highly recommend reading both the interface and implementation of the CSC369 CPU module (`csc369_cpu.h`, `csc369_cpu.c`).
In addition, we recommend having `unicorn/unicorn.h` open so that you can look through [its documentation](https://github.com/unicorn-engine/unicorn).
While you can use `CSC369_CPUExposeUnicorn` to access the unicorn engine, you should not need to do anything "advanced".
The documentation for the aforementioned function gives more guidance.
When in doubt, please ask on Piazza (without sharing your solution).

## Part 1: Loading and running one program

Your first goal is to load a program on to the simulated processor and run it.
To do so, you need to begin the design of your kernel (the struct `CSC369_Kernel`) and what should be tracked by your processes (the struct `CSC369_PCB`).
We recommend you do this incrementally, adding to the data structures "as you go" rather than all at once.
Then, try to load the program into your own kernel's memory; see `csc369_executable.h`.
Feel free to create helper functions as needed.

Once you have the program loaded into your kernel's memory, you need set it up for execution on the processor.
Which registers do you need to set?
What should you write to memory?
To accomplish the answers to the previous questions, you will need to learn more about the Unicorn API.
When you think everything is in order (the program is loaded and ready to go), then you can give up control for the first time.
You **must** do this using `CSC369_CPUResume` (this applies to all parts).

Perform the following steps:
1. In `src`, implement the code so that `procsim` loads and runs one program.
    Remember that you are restricted to `src/csc369_kernel.c` and `src/csc369_process.h`.
    While you may wish to change other files (e.g., to add trace statements), make sure that your code does not depend on those changes.
2. Test out your changes by running one of the simplest workloads: `./procsim return_3`.
    See the [README](README.md) for more details on running the simulator.
    Your goal is to reach a point where the trap handler is called due to a system call.
    In Part 2, you begin to implement the trap handler.
3. When you have something working, even something small, we recommend you commit and push your changes to keep a history that you can roll back to, just in case.

In your assignment report, answer the question:
"How is a program loaded and run on the Unicorn Processor?"
using 300 words or fewer.
Another way to phrase this question is "How did you turn a program executable into a running process?".
You may include up to one figure.
Your answer should describe the sequence of steps that *you* take (i.e., this should not be an answer about a generic OS), including how you interact with the data structures you have designed and their attributes.

## Part 2: Supporting system calls

Notice that the `return_3` workload (and, in fact, every workload) uses a system call to exit the process.
Your second goal is to support these system calls using the calling conventions for x86.
Before starting, you should be able to answer questions (based on x86 system call conventions) like:
How do you know what the system call number is?
Where can you find the arguments to the system call?
Where do you put the return value to a system call?

In `src/csc369_system_call.h`, you will see the system calls you need to support.
You implement these functions in `csc369_kernel.c`, and you must call them from your trap handler.
We also recommend you review (at least) `workloads/src/syscall.c`.
The code shows you how the user code is making the system call.
You may also want to disassemble the compiled files in `workloads/bin` using `objdump`.

What should you do when the last user program is finished running?
You should call `exit(0)`.
But note the implications:
this will not return control to the simulator.
So remember to clean up (memory) after yourself.
By convention, you should only be cleaning up memory you allocated.
This could be through your own explicit call (e.g., `malloc`), or through a given helper function (e.g., `CSC369_LoadProgram`).
However, you should **not** clean up an object that is "shared", like the `CSC369_CPU` object passed to your kernel.

Perform the following steps:
1. In `src`, implement the code so that `procsim` loads and runs one program to completion (i.e., it exits).
2. Test out your changes by running one of the simplest workloads: `./procsim return_3`.
    As you support more system calls, try some other workloads.

    For example, you may have a trace that looks like this (you do not need to match this trace output):

    ```
    [TRACE] [csc369_executable.c:64] Loaded return_3 into kernel memory (7800 bytes read)
    [TRACE] [csc369_executable.c:95] Saving .text section (size: 103 bytes).
    [TRACE] [csc369_simulation.c:61] Created new process with pid 369 for program return_3.
    [TRACE] [csc369_kernel.c:xxx] [pid 369] CSC369_exit(3)
    [TRACE] [csc369_kernel.c:xxx] Last process finished.
    ```
3. When you have something working, even something small, we recommend you commit and push your changes to keep a history that you can roll back to, just in case.

In your assignment report, answer the question:
"How would you design a system call that prints a pointer to an integer?"
using 300 words or fewer.
You can assume that the pointer contains an address on the stack (since our user programs do not have a heap).
Your answer may include up to two short "listings" (i.e., code snippets), not included in the word count.
For example, your listings could show how the user makes the system call and how the kernel would handle the system call.
You should **not** implement this in your code.

## Part 3: Process switch on exit

At this point, you should be able to load and run a single workload until it exits.
In this part, when one process finishes you will load and run the next process (based on the order `CSC369_CreateProcess` was called).
You can actually begin Part 3 after supporting only the exit system call from Part 2.
So if you are struggling with some of Part 2, you can still move on to Part 3 using only the `return_3` workload.

Perform the following steps:
1. In `src`, implement the code so that `procsim` loads and runs every program given to it.
2. Test out your changes by running two or more of the simplest workloads: `./procsim return_3 return_3`.
    For example, you may have a trace that looks like this (you do not need to match this trace output):

    ```
    [TRACE] [csc369_executable.c:64] Loaded return_3 into kernel memory (7800 bytes read)
    [TRACE] [csc369_executable.c:95] Saving .text section (size: 103 bytes).
    [TRACE] [csc369_simulation.c:61] Created new process with pid 369 for program return_3.
    [TRACE] [csc369_executable.c:64] Loaded return_3 into kernel memory (7800 bytes read)
    [TRACE] [csc369_executable.c:95] Saving .text section (size: 103 bytes).
    [TRACE] [csc369_simulation.c:61] Created new process with pid 370 for program return_3.
    [TRACE] [csc369_kernel.c:xxx] [pid 369] CSC369_exit(3)
    [TRACE] [csc369_kernel.c:xxx] Switched 369 with 370.
    [TRACE] [csc369_kernel.c:xxx] [pid 370] CSC369_exit(3)
    [TRACE] [csc369_kernel.c:xxx] Last process finished.
    ```
3. When you have something working, even something small, we recommend you commit and push your changes to keep a history that you can roll back to, just in case.

In your assignment report, answer the question:
"How and when did you clean up a process that had finished?"
using 300 words or fewer.
You may include up to one figure.
Your answer should describe the sequence of steps that *you* take (i.e., this should not be an answer about a generic OS), including how you interact with the data structures you have designed and their attributes.

## Part 4: Cooperative process switching

In this part, you enable "cooperation" between user programs.
That is, if there are other processes on the ready queue during a system call, your kernel will switch contexts to that process.
Later, our kernel will resume the process that was originally running.
This is done in round-robin fashion (here, round-robin refers to order, not the pre-emptive scheduling policy) until all processes have exited.

Perform the following steps:
1. In `src`, implement the code so that `procsim` loads and runs every program given to it.
    In addition, if a running program makes a system call, it should switch to another process (assuming one is ready).
2. Test out your changes by running two or more of the simplest workloads: `./procsim hello_pid hello_42`.
   For example, you may have a trace that looks like this (you do not need to match this trace output):

    ```
    [TRACE] [csc369_executable.c:64] Loaded hello_pid into kernel memory (7960 bytes read)
    [TRACE] [csc369_executable.c:95] Saving .text section (size: 125 bytes).
    [TRACE] [csc369_simulation.c:61] Created new process with pid 369 for program hello_pid.
    [TRACE] [csc369_executable.c:64] Loaded hello_42 into kernel memory (7872 bytes read)
    [TRACE] [csc369_executable.c:95] Saving .text section (size: 113 bytes).
    [TRACE] [csc369_simulation.c:61] Created new process with pid 370 for program hello_42.
    [TRACE] [csc369_kernel.c:xxx] [pid 369] CSC369_pid()
    [TRACE] [csc369_kernel.c:xxx] Switched 369 with 370.
    [TRACE] [csc369_kernel.c:xxx] [pid 370] CSC369_print_integer(42)
    [TRACE] [csc369_kernel.c:xxx] Switched 370 with 369.
    [TRACE] [csc369_kernel.c:xxx] [pid 369] CSC369_print_integer(369)
    [TRACE] [csc369_kernel.c:xxx] Switched 369 with 370.
    [TRACE] [csc369_kernel.c:xxx] [pid 370] CSC369_exit(0)
    [TRACE] [csc369_kernel.c:xxx] Switched 370 with 369.
    [TRACE] [csc369_kernel.c:xxx] [pid 369] CSC369_exit(0)
    [TRACE] [csc369_kernel.c:xxx] Last process finished.
    ```
3. When you have something working, even something small, we recommend you commit and push your changes to keep a history that you can roll back to, just in case.

In your assignment report, answer the question:
"What are the different states a process in your kernel can be in, and how does a process transition between states?"
using 300 words or fewer.
You may include up to one figure.
You may find it useful to model your answer (and figure) on how it is presented in lecture or the textbook.

## Assessment

Your submission for this assignment will be assessed by graders (i.e., teaching assistants) and automated tests.
Teaching assistants assess your assignment report.
And a series of automated tests assess your code.

The answers in your assignment report are assessed based on their correctness, communication, and (where appropriate) design.
So a response should not only be correct, but also concisely and clearly articulate the answer.
The use of figures is very helpful in communication, and we recommend that you not only include figures but refer to the (parts of the) figure in the text.
It is possible to answer these questions without the code working (or working perfectly).
In these scenarios, your answer should describe what you intended or what you believe would work.

We assess your code with automated testing.
This includes, for example, checking the state of registers and/or memory after calls to different functions in your kernel.
Our testing framework relies on submissions not circumventing the `csc369_cpu.h` interface.
The most important constraint is ensuring that you only use `CSC369_CPUResume` to "give up control" and run a user program.
But, once again, please do not get creative with advanced usage of the unicorn engine.
When in doubt, please ask on Piazza (without sharing your solution).

We do not use "trace-based automated testing" in this assessment.
So your standard output does not need to conform to a specific pattern of text.
However, please prefer the `CSC369_LOG_TRACE` macro rather than calling `printf` (or its variants) directly.
And, while trace statements can be useful in debugging, don't forget to also use the debugger.
