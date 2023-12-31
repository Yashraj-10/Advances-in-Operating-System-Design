# Steps to install the Linux Kernel 5.10.191

PS: Make sure that the GRUB_TIMEOUT is not set to 0 seconds in ```/etc/default/grub```. If it is set to 0, then you will not be able to see the grub menu while booting. To change the value of GRUB_TIMEOUT, run the following command

```bash
sudo nano /etc/default/grub
```

## Step 1: Download the kernel from the official website

```bash
wget https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.10.191.tar.xz
```

## Step 2: Install the dependencies

```bash
sudo apt-get install build-essential ncurses-dev bison flex libssl-dev libelf-dev bc gcc
sudo apt install dwarves
sudo apt install -y zstd
```

## Step 3: Extract the tar file

```bash
mkdir linux-5.10.191
tar -xf linux-5.10.191.tar.xz -C linux-5.10.191 --strip-components=1
```

## Step 4: Change the directory to the extracted folder

```bash
cd linux-5.10.191
```

## Step 5: Copy the config file from the boot directory

```bash
cp /boot/config-$(uname -r) .config
```

After running this command, the ```.config``` directory will be 
created in the current directory, ie, ```linux-5.10.191```

## Step 6: Run the following command to open the menuconfig 

```bash
make menuconfig
```

After running this command you will be redirected to the configuration menu. Here, you will use the arrow keys to navigate to ```Save``` and press enter. Then press enter again to save the configuration file. After this ```Exit``` the menuconfig.

Now run the following command 
```bash
make localmodconfig
```

## Step 7: Compile the Kernel

For installing the kernel version 5.10.191, first we have to disable the ```CONFIG_SYSTEM_TRUSTED_KEYS``` and ```CONFIG_SYSTEM_REVOCATION_KEYS``` option. For this, run the following command

```bash
scripts/config --disable SYSTEM_TRUSTED_KEYS
scripts/config --disable SYSTEM_REVOCATION_KEYS
```

After this, run the following command to compile the kernel

```bash
make
```

To speed up the process you can use the following command

```bash
make -j $(nproc)
```

After this we would have to install the modules that we enabled in previous steps
```bash
sudo make modules_install
```

## Step 8: Installing the Kernel

```bash
sudo make install
```

## Step 9: Enabling the kernel for boot

To enable Kernel for boot, run the following command

```bash
sudo update-initramfs -c -k 5.10.191
```

Now run the following command to update the grub

```bash
sudo update-grub
```

## Step 10: Reboot the system

```bash
sudo reboot
```

## Step 11: Check the kernel version

```bash
uname -mrs
```
