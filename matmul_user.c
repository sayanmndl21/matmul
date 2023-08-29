#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define MAT_SIZE 100
#define MATRIX_IOCTL_MAGIC 'm'
#define MATRIX_IOCTL_SET_A _IOW(MATRIX_IOCTL_MAGIC, 1, int)
#define MATRIX_IOCTL_SET_B _IOW(MATRIX_IOCTL_MAGIC, 2, int)
#define MATRIX_IOCTL_CALCULATE _IO(MATRIX_IOCTL_MAGIC, 3)

void fill_matrix(int matrix[MAT_SIZE][MAT_SIZE], int n) {
    printf("Enter matrix elements:\n");
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("Matrix[%d][%d]: ", i, j);
            scanf("%d", &matrix[i][j]);
        }
    }
}

int main() {
    int fd = open("/proc/matrix_multiplication", O_RDWR);
    if (fd < 0) {
        perror("Failed to open /proc/matrix_multiplication");
        return -1;
    }
    
    int n;  // Size of the matrix
    printf("Enter matrix size (n): ");
    scanf("%d", &n);
    
    if (n <= 0 || n > MAT_SIZE) {
        printf("Invalid matrix size\n");
        close(fd);
        return -1;
    }
    
    int matrix_a[MAT_SIZE][MAT_SIZE];
    int matrix_b[MAT_SIZE][MAT_SIZE];
    
    printf("Matrix A:\n");
    fill_matrix(matrix_a, n);
    
    printf("Matrix B:\n");
    fill_matrix(matrix_b, n);
    
    // Set matrix A
    if (ioctl(fd, MATRIX_IOCTL_SET_A, matrix_a)) {
        perror("MATRIX_IOCTL_SET_A failed");
        close(fd);
        return -1;
    }
    
    // Set matrix B
    if (ioctl(fd, MATRIX_IOCTL_SET_B, matrix_b)) {
        perror("MATRIX_IOCTL_SET_B failed");
        close(fd);
        return -1;
    }
    
    // Calculate matrix multiplication
    if (ioctl(fd, MATRIX_IOCTL_CALCULATE)) {
        perror("MATRIX_IOCTL_CALCULATE failed");
        close(fd);
        return -1;
    }
    
    /// Read the result matrix from the kernel
    int result[MAT_SIZE][MAT_SIZE];
    ssize_t bytes_read = read(fd, result, sizeof(result));
    if (bytes_read != sizeof(result)) {
        perror("Failed to read result from kernel");
        close(fd);
        return -1;
    }
    
    // Display the result matrix
    printf("Result matrix:\n");
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%d ", result[i][j]);
        }
        printf("\n");
    }
    
    close(fd);
    return 0;
}
