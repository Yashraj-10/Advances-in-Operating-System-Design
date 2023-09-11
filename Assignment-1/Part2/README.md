### Running the Kernel Module
```bash
make
```

### Installing the Kernel Module

```bash
sudo insmod partb_1_20CS10079_20CS30040.ko
```

### Removing the Kernel Module

```bash
sudo rmmod partb_1_20CS10079_20CS30040.ko
```

### Running the tests

```bash
gcc test1.c -o test1
gcc test2.c -o test2

./test1 >> output1.txt
./test2 >> output2.txt
```

### Cleaning the directory

```bash
make clean
```

