#!/bin/bash
# sudo su before running this file
make
insmod matmul_kernel.ko
dmesg | tail
#./matmul_user