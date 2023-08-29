# Matrix Multiplication using Linux Kernel Module

This repository contains code for performing matrix multiplication using a Linux kernel module. The matrix multiplication is implemented with parallel computation using submatrices and multiple threads.

## Files

1. **matmul_kernel.c**: This file contains the kernel code responsible for matrix multiplication. It receives and instantiates matrices A and B from user space to the kernel. The code splits the matrices into submatrices if the matrix size is greater than a certain threshold (`SUBMAT_SIZE`). It computes the multiplication results in each worker thread, synchronizes the thread results (until completion), combines the results, and sends them to user space. All kernel processes happen within the `/proc` directory and are later offloaded to user space.

2. **matmul_user.c**: This file contains the user space code. When executed, it prompts the user for the matrix size, inputs for matrices A and B, and then displays the resulting matrix. The matrices A and B are sent to kernel space for computation.

3. **Makefile**: This Makefile is used to build the kernel modules and compile the user space C code. It streamlines the compilation and building process.

4. **load_kernel.sh**: This script runs the Makefile, inserts the `matmul` module into the kernel, and executes the compiled user space code to perform the matrix multiplication.

5. **rem_kernel.sh**: This script first unloads the `matmul` module from the kernel and then runs `make clean` to remove all kernel-related files.

## Usage

To run the matrix multiplication, follow these steps in a terminal in the `matmul` directory:

1. Use the `sudo su` command to become a superuser or switch to root.

2. Execute the `load_kernel.sh` script to compile and load the kernel module, and then run the user script.

3. If needed, execute the `rem_kernel.sh` script to unload the kernel module and clean up.

Please ensure you have a compatible environment and necessary permissions before running these scripts.

## Acknowledgements

This project was inspired by various resources:

- OpenAI's ChatGPT for providing assistance and guidance on developing the kernel module and user space code.
- Stack Overflow community for answering questions related to Linux kernel programming.
- [Linux Kernel Module Programming Guide](https://sysprog21.github.io/lkmpg/#hello-world) for its detailed guide on creating kernel modules.

I am grateful for the insights gained from these sources during the development of this project.
