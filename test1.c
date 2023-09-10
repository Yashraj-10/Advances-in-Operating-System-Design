#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

void test(int values[], int length)
{
    int fd = open("/proc/partb_1_3", O_RDWR);
    char c = (char)length;
    write(fd, &c, 1);
    for (int i = 0; i < length; i++) {
        int ret = write(fd, &values[i], sizeof(int));
        printf("[Proc %d] Write: %d, Return: %d\n", getpid(), values[i], ret);
        usleep(100);
    }
    for (int i = 0; i < length; i++) {
        int out;
        int ret = read(fd, &out, sizeof(int));
        printf("[Proc %d] Read: %d, Return: %d\n", getpid(), out, ret);
        usleep(100);
    }
    close(fd);
}

int main() {
    int length1 = 5;
    int valuesues1[] = {0, 1, -2, 3, 4};

    test(valuesues1, length1);

    return 0;
}