# Scheduler EDF in C for RP2040

This project is a C implementation of a real-time scheduler using the Earliest Deadline First algorithm.

This code is used as a basis for a translation into the Rust language: [Scheduler EDF in Rust](https://github.com/adiscepo/scheduler_memoire_rust)

It was produced as part of my Master's Thesis.

## Launch the project

You can flash the code directly onto the chip using 
```shell
openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program build/kernel.elf verify reset exit"
``` 
after compiling the code with

```shell
mkdir build
cd build
cmake ..
make -j4
cd ..
```


To obtain the code execution logs, the minicom software can be used. 
```shell
minicom -b 115200 -o -D /dev/name_chip
```

## Plot of the code

The plots of the program can be plotted with [this code](https://github.com/adiscepo/scheduler_memoire_plot)