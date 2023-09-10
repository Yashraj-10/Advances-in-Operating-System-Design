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
    int values1[] = {0, 1, -2, 3, 4};

    int length2 = 10;
    int values2[] = { -97, 1, -2, 3, -46, 5, -6, 7, 78, 9 };

    int length3 = 8;
    int values3[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

    int length4 = 3;
    int values4[] = { -1, -2, -3 };

    int pid_lvl1 = fork();
    if (pid_lvl1 == 0) {
        int pid_lvl2 = fork();
        if (pid_lvl2 == 0) {
            test(values1, length1);
        } else {
            test(values2, length2);
        }
    } else {
        int pid_lvl2 = fork();
        if (pid_lvl2 == 0) {
            test(values3, length3);
        } else {
            test(values4, length4);
        }
    }

    return 0;
}