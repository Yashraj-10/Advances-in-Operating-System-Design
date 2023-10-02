#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <bits/stdc++.h>

/*
TESTS CHECKED -> each carries 10 marks
General working with N = 1
General working with N = 50
General working with N = 100
*/

static int marks = 0;
const int total_marks = 30;

inline void success()
{
    marks += 10;
    std::cout << "\033[1;32m PASS \033[0m\n";
}

inline void failure(std::string error_str = "")
{
    std::cout << "\033[1;31m FAIL \033[0m\n";
    if (error_str != "")
    {
        std::cout << error_str << "\n";
    }
}

int simulate_queue(char length, const char *procfile_name)
{
    srand(time(NULL));
    std::cout << "TEST FOR N = " << static_cast<int>(length) << " : ";
    int fd = open(procfile_name, O_RDWR);
    std::deque<int> dq;
    int ret;

    if (fd < 0)
    {
        failure("OPEN FAILED");
        goto failureCase;
        ;
    }
    ret = write(fd, &length, sizeof(length));
    if (!(ret == sizeof(char) || ret == 0))
    {
        failure("INITIALIZATION FAILED");
        goto failureCase;
        ;
    }

    for (int ops = 1; ops <= 1000; ops++)
    {
        int op = rand() % 2;
        if (op == 0) // WRITE
        {
            int valid_write_data = rand();
            int ret = write(fd, (char *)&valid_write_data, sizeof(valid_write_data));
            if (dq.size() == length)
            {
                if (!(ret == -1))
                {
                    failure("ALLOWED WRITE BEYOND SIZE");
                    goto failureCase;
                    ;
                }
                continue;
            }
            else if (ret != sizeof(int))
            {
                failure("NORMAL WRITE FAILED");
                goto failureCase;
                ;
            }

            if (valid_write_data % 2) // odd
            {
                dq.push_front(valid_write_data);
            }
            else // even
            {
                dq.push_back(valid_write_data);
            }
        }
        else
        {
            int valid_read_data;
            int ret = read(fd, (char *)&valid_read_data, sizeof(valid_read_data));

            if (dq.size() == 0)
            {
                if (!(ret == -1))
                {
                    failure("READ FROM EMPTY QUEUE DID NOT RETURN ERROR");
                    goto failureCase;
                    ;
                }
                continue;
            }
            if (ret != sizeof(int))
            {
                failure("NORMAL READ FAILED DUE TO ERROR");
                goto failureCase;
                ;
            }

            int valid_read_data_dq = dq.front();
            dq.pop_front();
            if (valid_read_data_dq != valid_read_data)
            {
                failure("NORMAL READ FAILED DUE TO INCORRECT OUTPUT");
                std::cout << "Expected : " << valid_read_data_dq << " Got : " << valid_read_data << "after " << ops << " operations\n";
                goto failureCase;
                ;
            }
        }
    }

successCase:
    if (!(close(fd) == 0))
    {
        failure("FILE CLOSE FAILED");
        goto failureCase;
    }
    success();
    return 0;

failureCase:
    close(fd);
    return 1;
}

int main(int argc, char *argv[])
{
    const char *procfile_name = argv[1];
    char lengths[] = {1, 50, 100};
    for (auto length : lengths)
    {
        simulate_queue(length, procfile_name);
    }

    std::cout << "MARKS: " << marks << " / " << total_marks << "\n";

    return 0;
}