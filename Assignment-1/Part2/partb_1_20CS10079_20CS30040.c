/*
 * CS60038: Advances in Operating Systems
 * Assignment 1, Part B: Loadable Kernel Module(LKM) for a Deque
 * Members:
 * > 20CS10079 - Yashraj Singh
 * > 20CS30040 - Rishi Raj
*/

#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#define PROCFS_NAME "partb_1_20CS10079_20CS30040"
#define PROCFS_MAX_SIZE 1024

struct deque
{
    int *arr;
    int front;
    int rear;
    int capacity;
};

enum process_state
{
    PROC_FILE_OPEN,
    PROC_READ,
};

struct process_node
{
    pid_t pid;
    enum process_state state;
    struct deque *process_deque;
    struct process_node *next;
};

static struct proc_dir_entry *proc_file;
static char procfs_buffer[PROCFS_MAX_SIZE];
static size_t procfs_buffer_size = 0;
static struct process_node *process_list = NULL;

DEFINE_MUTEX(procfs_mutex);

// Initialize a deque
static struct deque *deque_init(int capacity)
{
    struct deque *d = kmalloc(sizeof(struct deque), GFP_KERNEL);
    if (!d)
    {
        printk(KERN_ALERT "E: Memory allocation for deque failed\n");
        return NULL;
    }

    d->arr = kmalloc(capacity * sizeof(int), GFP_KERNEL);
    if (!d->arr)
    {
        printk(KERN_ALERT "E: Memory allocation for deque array failed\n");
        return NULL;
    }

    d->front = -1;
    d->rear = -1;
    d->capacity = capacity;

    return d;
}

// Insert an element in the deque
static int deque_insert(struct deque *d, int val)
{
    if (val % 2 != 0)
    {
        if ((d->front == 0 && d->rear == d->capacity - 1) || (d->front == d->rear + 1))
        {
            printk(KERN_ALERT "E: Deque is full\n");
            return -EACCES;
        }
        else if ((d->front == -1) && (d->rear == -1))
        {
            d->front = d->rear = 0;
            d->arr[d->front] = val;
        }
        else if (d->front == 0)
        {
            d->front = d->capacity - 1;
            d->arr[d->front] = val;
        }
        else
        {
            d->front = d->front - 1;
            d->arr[d->front] = val;
        }
    }
    else
    {
        if ((d->front == 0 && d->rear == d->capacity - 1) || (d->front == d->rear + 1))
        {
            printk(KERN_ALERT "E: Deque is full\n");
            return -EACCES;
        }
        else if ((d->front == -1) && (d->rear == -1))
        {
            d->front = d->rear = 0;
            d->arr[d->rear] = val;
        }
        else if (d->rear == d->capacity - 1)
        {
            d->rear = 0;
            d->arr[d->rear] = val;
        }
        else
        {
            d->rear = d->rear + 1;
            d->arr[d->rear] = val;
        }
    }

    return 0;
}

// Read an element from the deque
static int deque_read(struct deque *d)
{
    int ret_val;
    if ((d->front == -1) && (d->rear == -1))
    {
        printk(KERN_ALERT "E: Deque is empty\n");
        return -EACCES;
    }
    else if (d->front == d->rear)
    {
        ret_val = d->arr[d->front];
        d->front = -1;
        d->rear = -1;
        return ret_val;
    }
    else if (d->front == (d->capacity - 1))
    {
        ret_val = d->arr[d->front];
        d->front = 0;
        return ret_val;
    }
    else
    {
        ret_val = d->arr[d->front];
        d->front = d->front + 1;
        return ret_val;
    }
}

// Delete the deque
static void deque_delete(struct deque *d)
{
    kfree(d->arr);
    kfree(d);
}

// Finding the process
static struct process_node *process_find(pid_t pid)
{
    struct process_node *curr = process_list;

    while (curr != NULL)
    {
        if (curr->pid == pid)
            return curr;

        curr = curr->next;
    }

    return NULL;
}

// Insert a process node with the given pid
static struct process_node *process_insert(pid_t pid)
{
    struct process_node *node = kmalloc(sizeof(struct process_node), GFP_KERNEL);

    if (node == NULL)
        return NULL;

    node->pid = pid;
    node->state = PROC_FILE_OPEN;
    node->process_deque = NULL;
    node->next = process_list;
    process_list = node;

    return node;
}

// Delete a process node with the given pid
static int process_delete(pid_t pid)
{
    struct process_node *curr = process_list;
    struct process_node *previous = NULL;

    while (curr != NULL)
    {
        if (curr->pid == pid)
        {
            if (previous == NULL)
                process_list = curr->next;
            else
                previous->next = curr->next;

            if (curr != NULL)
            {
                if (curr->process_deque != NULL)
                    deque_delete(curr->process_deque);

                kfree(curr);
            }

            return 0;
        }

        previous = curr;
        curr = curr->next;
    }

    return -EACCES;
}

// Delete the process list
static void process_list_delete(void)
{
    struct process_node *curr = process_list;
    struct process_node *next = NULL;

    while (curr != NULL)
    {
        next = curr->next;

        if (curr->process_deque != NULL)
            deque_delete(curr->process_deque);

        kfree(curr);

        curr = next;
    }

    process_list = NULL;
}

// Process File Open Handler
static int procfile_open(struct inode *inode, struct file *file)
{
    pid_t pid;
    int ret_val;
    struct process_node *node;

    mutex_lock(&procfs_mutex);

    pid = current->pid;
    printk(KERN_INFO "I: Process %d opening the file\n", pid);
    ret_val = 0;

    node = process_find(pid);
    if (node == NULL)
    {
        node = process_insert(pid);
        if (node == NULL)
        {
            printk(KERN_ALERT "E: Process %d could not be allocated node\n", pid);
            ret_val = -ENOMEM;
        }
        else
        {
            printk(KERN_INFO "I: Process %d inserted\n", pid);
        }
    }
    else
    {
        printk(KERN_ALERT "E: Process %d already has the process file open\n", pid);
        ret_val = -EACCES;
    }

    mutex_unlock(&procfs_mutex);

    return ret_val;
}

// Process File Close Handler
static int procfile_close(struct inode *inode, struct file *file)
{
    pid_t pid;
    int ret_val;
    struct process_node *node;

    mutex_lock(&procfs_mutex);

    pid = current->pid;
    printk(KERN_INFO "I: Process %d closing the file\n", pid);
    ret_val = 0;

    node = process_find(pid);
    if (node == NULL)
    {
        printk(KERN_ALERT "E: Process %d does not have the process file open\n", pid);
        ret_val = -EACCES;
    }
    else
    {
        process_delete(pid);
        printk(KERN_INFO "I: Process %d deleted\n", pid);
    }

    mutex_unlock(&procfs_mutex);

    return ret_val;
}

// Process File Read Handler
static ssize_t procfile_read(struct file *filep, char __user *buffer, size_t length, loff_t *offset)
{
    pid_t pid;
    int ret_val, min_val;
    struct process_node *node;

    mutex_lock(&procfs_mutex);

    pid = current->pid;
    printk(KERN_INFO "I: Process %d reading from the file\n", pid);
    ret_val = 0;

    node = process_find(pid);
    if (node == NULL)
    {
        printk(KERN_ALERT "E: Process %d does not have the process file open\n", pid);
        ret_val = -EACCES;
    }
    else
    {
        procfs_buffer_size = min(length, (size_t)PROCFS_MAX_SIZE);

        if (node->state == PROC_FILE_OPEN)
        {
            printk(KERN_ALERT "E: Process File of the process %d is still empty\n", pid);
            ret_val = -EACCES;
        }
        else if (node->process_deque->front == -1 && node->process_deque->rear == -1)
        {
            printk(KERN_ALERT "E: Deque is empty\n");
            ret_val = -EACCES;
        }
        else
        {
            min_val = deque_read(node->process_deque);
            memcpy(procfs_buffer, (const char *)&min_val, sizeof(int));
            procfs_buffer[sizeof(int)] = '\0';
            procfs_buffer_size = sizeof(int);
            ret_val = sizeof(int);
        }
        
        if (ret_val >= 0)
        {
            if (copy_to_user(buffer, procfs_buffer, procfs_buffer_size) != 0)
            {
                printk(KERN_ALERT "E: Could not copy data to user\n");
                ret_val = -EFAULT;
            }
            else
            {
                ret_val = procfs_buffer_size;
            }
        }
    }

    mutex_unlock(&procfs_mutex);

    return ret_val;
}

// Handle the write operation
static ssize_t handleWrite(struct process_node *node)
{
    size_t capacity;
    int value, ret_val;

    if (node->state == PROC_FILE_OPEN)
    {
        if (procfs_buffer_size > 1ul)
        {
            printk(KERN_ALERT "E: Capacity buffer size must be at max 1 byte\n", node->pid);
            return -EINVAL;
        }

        capacity = (size_t)procfs_buffer[0];
        if (capacity < 1 || capacity > 100)
        {
            printk(KERN_ALERT "E: Capacity must be between 1 and 100\n");
            return -EINVAL;
        }

        node->process_deque = deque_init(capacity);
        if (node->process_deque == NULL)
        {
            printk(KERN_ALERT "E: Could not initialize deque for process %d\n", node->pid);
            return -ENOMEM;
        }

        printk(KERN_INFO "I: Deque initialized for process %d\n", node->pid);
        node->state = PROC_READ;
    }
    else if (node->state == PROC_READ)
    {
        if (procfs_buffer_size != 4ul)
        {
            printk(KERN_ALERT "E: Value buffer size must be at max 4 bytes\n", node->pid);
            return -EINVAL;
        }
    
        value = *((int *)procfs_buffer);

        ret_val = deque_insert(node->process_deque, value);
        if (ret_val < 0)
        {
            printk(KERN_ALERT "E: Could not insert value %d in deque for process %d\n", value, node->pid);
            return -EACCES;
        }

        printk(KERN_INFO "I: Value %d inserted in deque for process %d\n", value, node->pid);
    }

    return procfs_buffer_size;
}

// Process File Write Handler
static ssize_t procfile_write(struct file *filep, const char __user *buffer, size_t length, loff_t *offset)
{
    pid_t pid;
    int ret_val;
    struct process_node *node;

    mutex_lock(&procfs_mutex);

    pid = current->pid;
    printk(KERN_INFO "I: Process %d writing to the file\n", pid);
    ret_val = 0;

    node = process_find(pid);
    if (node == NULL)
    {
        printk(KERN_ALERT "E: Process %d does not have the process file open\n", pid);
        ret_val = -EACCES;
    }
    else
    {
        if (buffer == NULL || length == 0)
        {
            printk(KERN_ALERT "E: No data to write\n");
            ret_val = -EINVAL;
        }
        else
        {
            procfs_buffer_size = min(length, (size_t)PROCFS_MAX_SIZE);

            if (copy_from_user(procfs_buffer, buffer, procfs_buffer_size))
            {
                printk(KERN_ALERT "E: Could not copy data from user\n");
                ret_val = -EFAULT;
            }
            else
            {
                ret_val = handleWrite(node);
            }
        }
    }

    mutex_unlock(&procfs_mutex);

    return ret_val;
}

static const struct proc_ops proc_fops = {
    .proc_open = procfile_open,
    .proc_read = procfile_read,
    .proc_write = procfile_write,
    .proc_release = procfile_close,
};

// Initialize Module
static int __init lkm_init(void) {
    printk(KERN_INFO "I: LKM for partb_1_20CS10079_20CS30040 loaded\n");

    proc_file = proc_create(PROCFS_NAME, 0666, NULL, &proc_fops);

    if (proc_file == NULL) {
        printk(KERN_ALERT "E: Could not create process file\n");
        return -ENOENT;
    }

    printk(KERN_INFO "I: /proc/%s created\n", PROCFS_NAME);

    return 0;
}

// Clean Module
static void __exit lkm_exit(void) {
    process_list_delete();
    remove_proc_entry(PROCFS_NAME, NULL);
    printk(KERN_INFO "I: LKM for partb_1_20CS10079_20CS30040 unloaded\n");
    printk(KERN_INFO "I: /proc/%s removed\n", PROCFS_NAME);
}

module_init(lkm_init);
module_exit(lkm_exit);