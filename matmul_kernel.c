#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/completion.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sayan");

#define MAT_SIZE 100
#define SUBMAT_SIZE 10  // Submatrix size for parallel computation

#define MATRIX_IOCTL_MAGIC 'm'
#define MATRIX_IOCTL_SET_A _IOW(MATRIX_IOCTL_MAGIC, 1, int)
#define MATRIX_IOCTL_SET_B _IOW(MATRIX_IOCTL_MAGIC, 2, int)
#define MATRIX_IOCTL_CALCULATE _IO(MATRIX_IOCTL_MAGIC, 3)

static int matrix_a[MAT_SIZE][MAT_SIZE];
static int matrix_b[MAT_SIZE][MAT_SIZE];
static int result[MAT_SIZE][MAT_SIZE];
static struct mutex matrix_mutex;
static struct completion computation_done;  // For synchronization

static int worker_thread(void *data) {
    int start_row = *(int *)data;
    int end_row = start_row + SUBMAT_SIZE;
    int i, j, k;

    for (i = start_row; i < end_row; ++i) {
        for (j = 0; j < MAT_SIZE; ++j) {
            result[i][j] = 0;
            for (k = 0; k < MAT_SIZE; ++k) {
                result[i][j] += matrix_a[i][k] * matrix_b[k][j];
            }
        }
    }

    complete(&computation_done);
    return 0;
}

static long matrix_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case MATRIX_IOCTL_SET_A:
            // Copy user data to kernel buffer (matrix_a)
            if (copy_from_user(matrix_a, (int *)arg, sizeof(matrix_a)))
                return -EFAULT;
            break;
        
        case MATRIX_IOCTL_SET_B:
            // Copy user data to kernel buffer (matrix_b)
            if (copy_from_user(matrix_b, (int *)arg, sizeof(matrix_b)))
                return -EFAULT;
            break;
        
        case MATRIX_IOCTL_CALCULATE:
        {
            int i;
            mutex_lock(&matrix_mutex);
            
            init_completion(&computation_done);
            
            // Create worker threads for each submatrix
            for (i = 0; i < MAT_SIZE; i += SUBMAT_SIZE) {
                int *thread_arg = kmalloc(sizeof(int), GFP_KERNEL);
                *thread_arg = i;
                kthread_run(worker_thread, thread_arg, "worker_thread");
            }
            
            // Wait for all threads to complete
            for (i = 0; i < MAT_SIZE; i += SUBMAT_SIZE) {
                wait_for_completion(&computation_done);
            }
            
            mutex_unlock(&matrix_mutex);
            break;
        }

        
        default:
            return -EINVAL;
    }
    
    return 0;
}

static ssize_t matrix_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    if (*pos >= sizeof(result))
        return 0;  // End of file
    
    if (*pos + count > sizeof(result))
        count = sizeof(result) - *pos;
    
    if (copy_to_user(buf, (char *)result + *pos, count))
        return -EFAULT;
    
    *pos += count;
    return count;
}

static const struct file_operations matrix_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = matrix_ioctl,
    .read = matrix_read,
};

static struct proc_dir_entry *proc_entry = NULL;

static int __init matrix_init(void) {
    mutex_init(&matrix_mutex);
    
    // Create proc entry
    proc_entry = proc_create("matrix_multiplication", 0666, NULL, &matrix_fops);
    if (!proc_entry) {
        printk(KERN_ALERT "Failed to create proc entry\n");
        return -ENOMEM;
    }
    
    printk(KERN_INFO "Matrix multiplication module loaded\n");
    return 0;
}

static void __exit matrix_exit(void) {
    if (proc_entry)
        proc_remove(proc_entry);
    
    mutex_destroy(&matrix_mutex);
    
    printk(KERN_INFO "Matrix multiplication module unloaded\n");
}

module_init(matrix_init);
module_exit(matrix_exit);
