#!/bin/bash
# sudo su before running this file
rmmod matmul_kernel
make clean
dmesg | tail