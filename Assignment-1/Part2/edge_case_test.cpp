#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <bits/stdc++.h>

/*
TESTS CHECKED -> each test carries 5 marks

1. File open success test
2. Reopen opened file without closing results in error
3. Doing read on dequeue without initialising error
4. Doing first write on dequeue with out of range N error beyond byte
5. Doing first write on dequeue with out of range N error within byte
6. Doing first write on dequeue with N = 0 error
7. Doing first write on dequeue with valid N = 100
8. Trying to write data with invalid type : long long error
9. Trying to write data with invalid type : char error
10. Valid write of data
11. Valid read of data
12. Excess read call error
13. Excess write calls error
14. File close success test
15. Reopen after closing success test
16. Queue reset after reopen success test

*/

static int marks = 0;
const int total_marks = 80;

inline void success()
{
    marks += 5;
    std::cout << "\033[1;32m PASS \033[0m\n";
}
inline void failure(std::string error_str = "")
{
    std::cout << "\033[1;31m FAIL \033[0m\n";
    if(error_str != "")
    {
        std::cout<<error_str<<"\n";
    }
    
}

int main(int argc, char *argv[])
{
    const char *procfile_name = argv[1];
    int test_cnt = 1;
    int readBuff = 0;
    // TEST 1
    std::cout << "[ " << std::right << std::setw(2) << test_cnt++ << std::left << std::setw(2) << " ] File Open Test : ";
    int fd = open(procfile_name, O_RDWR);
    if(fd<0)
    {
        failure(strerror(errno));
        return 1;
    }
    success();

    // TEST 2 
    std::cout << "[ " << std::right << std::setw(2) << test_cnt++ << std::left << std::setw(2) << " ] File Reopen Test : ";
    if(open(procfile_name, O_RDWR) < 0 )
    {
        success();
    }
    else 
    {
        failure();
    }

    // TEST 3
    std::cout << "[ " << std::right << std::setw(2) << test_cnt++ << std::left << std::setw(2) << " ] Read Without Initialization Test : ";
    if(read(fd, (char *)&readBuff, sizeof(readBuff)) == -1)
    {
        success();
    }
    else 
    {
        failure();
    }

    // TEST 4
    std::cout << "[ " << std::right << std::setw(2) << test_cnt++ << std::left << std::setw(2) << " ] Initialization With non-byte N Test : ";
    int out_of_range_N = 1000; 
    if(write(fd, (char*)&out_of_range_N, sizeof(out_of_range_N)) == -1)
    {
        success();
    }
    else 
    {
        failure();
    }

    // TEST 5
    std::cout << "[ " << std::right << std::setw(2) << test_cnt++ << std::left << std::setw(2) << " ] Initialization With Out of Range byte N Test : ";
    char out_of_range_N_byte = 101; 
    if(write(fd, (char*)&out_of_range_N_byte, sizeof(out_of_range_N_byte)) == -1)
    {
        success();
    }
    else 
    {
        failure();
    }

    // TEST 6
    std::cout << "[ " << std::right << std::setw(2) << test_cnt++ << std::left << std::setw(2) << " ] Initialization With Zero N Test : ";
    char zero_N = 0; 
    if(write(fd, (char*)&zero_N, sizeof(zero_N)) == -1)
    {
        success();
    }
    else 
    {
        failure();
    }

    // TEST 7
    std::cout << "[ " << std::right << std::setw(2) << test_cnt++ << std::left << std::setw(2) << " ] Initialization With N = 100 Test : ";
    char valid_N = 100; 
    int ret = write(fd, (char*)&valid_N, sizeof(valid_N));
    if( ret == sizeof(char)) 
    {
        success();
    }
    else 
    {
        failure(strerror(errno));
    }

    // TEST 8
    std::cout << "[ " << std::right << std::setw(2) << test_cnt++ << std::left << std::setw(2) << " ] Incorrect Data Write with Long Long Test : ";
    long long invalid_write_ll = 10000000000000; 
    if(write(fd, (char*)&invalid_write_ll, sizeof(invalid_write_ll)) == -1)
    {
        success();
    }
    else 
    {
        failure();
    }

    // TEST 9
    std::cout << "[ " << std::right << std::setw(2) << test_cnt++ << std::left << std::setw(2) << " ] Incorrect Data Write with Char Test : ";
    char invalid_write_char = 1; 
    if(write(fd, (char*)&invalid_write_char, sizeof(invalid_write_char)) == -1)
    {
        success();
    }
    else 
    {
        failure();
    }

    // TEST 10
    std::cout << "[ " << std::right << std::setw(2) << test_cnt++ << std::left << std::setw(2) << " ] Correct Data Write Test : ";
    int valid_write_data = 1000; 
    if(write(fd, (char*)&valid_write_data, sizeof(valid_write_data)) == sizeof(int))
    {
        success();
    }
    else 
    {
        failure(strerror(errno));
    }

    // TEST 11
    std::cout << "[ " << std::right << std::setw(2) << test_cnt++ << std::left << std::setw(2) << " ] Correct Data Read Test : ";
    int valid_read_data; 
    if(read(fd, (char*)&valid_read_data, sizeof(valid_read_data)) == sizeof(int))
    {
        success();
    }
    else 
    {
        failure(strerror(errno));
    }

    // TEST 12
    std::cout << "[ " << std::right << std::setw(2) << test_cnt++ << std::left << std::setw(2) << " ] Excess Data Read Test : ";
    int invalid_read_data; 
    if(read(fd, (char*)&invalid_read_data, sizeof(invalid_read_data)) == -1)
    {
        success();
    }
    else 
    {
        failure();
    }

    // TEST 13
    std::cout << "[ " << std::right << std::setw(2) << test_cnt++ << std::left << std::setw(2) << " ] Excess Data Write Test : ";
    
    for( int i = 1; i<= valid_N + 1 ; i++)
    {
        int valid_write_data = rand(); 
        if(i == valid_N + 1)
        {
            if(write(fd, (char*)&valid_write_data, sizeof(valid_write_data)) == -1)
            {
                success();
            }
            else 
            {
                failure(strerror(errno));
            }
            break;
        }
        if(!(write(fd, (char*)&valid_write_data, sizeof(valid_write_data)) == sizeof(int)))
        {
            failure(strerror(errno));
            break;
        }
    }

    // TEST 14
    std::cout << "[ " << std::right << std::setw(2) << test_cnt++ << std::left << std::setw(2) << " ] File Close Test : ";
    if(close(fd) == 0)
    {
        success();
    }
    else 
    {
        failure();
    }

    // TEST 15
    std::cout << "[ " << std::right << std::setw(2) << test_cnt++ << std::left << std::setw(2) << " ] Reopen After Close Test : ";
    fd = open(procfile_name, O_RDWR);
    if(fd<0)
    {
        failure(strerror(errno));
        return 1;
    }
    success();

    // TEST 16
    std::cout << "[ " << std::right << std::setw(2) << test_cnt++ << std::left << std::setw(2) << " ] Read After Reopen Test : ";
    if(read(fd, (char *)&readBuff, sizeof(readBuff)) == -1)
    {
        success();
    }
    else 
    {
        failure();
    }

    std::cout<<"MARKS: " << marks << " / " << total_marks << "\n";

    return 0;
}
