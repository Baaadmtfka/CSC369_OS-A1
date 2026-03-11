# Assignment - Processes

This README describes how to compile this project and run the compiled executables.
For the instructions on what to do for this exercise, please see [the handout](HANDOUT.md).

## Dependencies

This project uses CMake to configure and build its binaries.
The `teach.cs` machines have CMake version 3.22 already installed.
But if you are working locally, you will need to install CMake as well.

The project uses the [Unicorn Engine](https://github.com/unicorn-engine) (version 2.0.1.post1).
The Unicorn Engine allows us to simulate a processor by executing instructions on it.
If you are working locally, you will need to install this as well.
But it has already been installed on the `teach.cs` machines.
The `cmake/FindUnicorn.cmake` file should be able to find it.

## Compiling

CMake can configure the project for different build systems and IDEs (type `cmake --help` for a list of generators available for your platform).
You can also work via the command line.
We recommend you create a build directory before invoking CMake to configure the project (`cmake -B`).
For example, we can perform the configuration step from the project root directory:

```zsh
wolf:~/369/0test/A1$ cmake -H. -Bcmake-build-debug -DCMAKE_BUILD_TYPE=Debug
```

After the configuration step, you can ask CMake to build the project:

```zsh
wolf:~/369/0test/A1$ cmake --build cmake-build-debug/ --target all
```


## Running

If compilation is successful, the `procsim` binary should be created.
But before you run it, you will want to copy over the workloads:

```zsh
wolf:~/369/0test/A1$ cp workloads/bin/* cmake-build-debug/bin/
```

Then, you can run the simulator with the `return_3` workload:

```zsh
wolf:~/369/0test/A1$ cd cmake-build-debug
wolf:~/369/0test/A1/cmake-build-debug$ cd bin/
wolf:~/369/0test/A1/cmake-build-debug/bin$ ./procsim return_3
```

Which produces output like:

```
[ERROR]	[csc369_simulation.c:58] Could not create process for program return_3.
```

You can also run it with more than one workload:

```zsh
wolf:~/369/0test/A1/cmake-build-debug/bin$ ./procsim return_3 return_pid
```

Which produces output like:

```
[ERROR]	[csc369_simulation.c:58] Could not create process for program return_3.
[ERROR]	[csc369_simulation.c:58] Could not create process for program return_pid.
```
