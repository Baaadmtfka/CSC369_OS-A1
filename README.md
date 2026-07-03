# CSC369 – Assignment 1: Process Management

Individual assignment for University of Toronto's CSC369 (Operating Systems): implementing a simplified kernel, in C, that loads and runs programs as processes on an emulated x86 CPU (via the [Unicorn Engine](https://github.com/unicorn-engine)), handles system calls, and performs cooperative (Round-Robin) scheduling.

## Layout

- **`A1-zengzix2/`** — the actual submission: source (`src/`), sample user programs (`workloads/`), CMake build config, the assignment report, and the course-provided [HANDOUT.md](A1-zengzix2/HANDOUT.md) / [README.md](A1-zengzix2/README.md) (build & run instructions).
- **`A1 handout.txt`** — plain-text copy of the assignment handout.
- **`a1_report.tex` / `.pdf` / `.docx`** — the report source (also copied into `A1-zengzix2/` so the submission folder is self-contained).

## What's implemented (`A1-zengzix2/src/`)

- **Process creation** (`csc369_kernel.c`): `LoadProgram()` reads a program executable, allocating its code on the heap along with metadata (code size, entry point). A `CSC369_PCB` is allocated alongside it, storing a pointer to that code plus the process's register state (`rsp`, `rbp`, `rip`, `rax`, `rdi`) and initial `READY` state. The kernel holds a heap-allocated list of PCB pointers (`pcbs`).
- **Running a process** (`csc369_kernel.c`, `csc369_cpu.c`): the kernel picks a `READY` PCB, writes its code into CPU memory and loads its registers into the (Unicorn-emulated) CPU, then resumes execution.
- **System calls**: user code sets `rax` to the syscall number and `rdi` to its argument, then executes `syscall`; the kernel's handler dispatches on `rax` and reads the argument back out of the PCB's saved `rdi`.
- **Process cleanup on exit**: on an exit syscall, the kernel frees the process's code buffer and its PCB, and clears that slot in `pcbs`.
- **Scheduling**: processes cycle between `BLOCKED` → `READY` → `RUNNING` → (back to `READY` on a syscall, or → `EXIT`) in Round-Robin order via a ready queue.

Design rationale and the four short-answer questions (loading/running a program, syscall design, process cleanup, and scheduling states) are written up in [a1_report.tex](a1_report.tex) / [a1_report.pdf](a1_report.pdf).

## Building & running

See [A1-zengzix2/README.md](A1-zengzix2/README.md) for CMake build instructions and how to run the `procsim` binary against the sample workloads.

## Author

Zixuan Zeng
