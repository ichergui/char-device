# Linux character device
A basic communication between Linux kernel and user application via character device.

## Get and build the software
### Download the source code
    $ cd $HOME
    $ mkdir devel
    $ cd devel
    $ git clone https://github.com/ichergui/char-device.git

### Build
##### kernel side

```bash
$ cd $HOME/devel/char-device/kernel
$ make all
```
##### User side
```bash
$ cd $HOME/devel/char-device/user
$ gcc -o chrdev_user chrdev_user.c
```

## Test
#### Insert it into the Linux kernel
Specify the __user Access__ to the character device using Udev Rules.
This permit us to give a read and write permissions.

```bash
$ cd $HOME/devel/char-device/
$ sudo cp udev/rules.d/99-chrdev.rules /etc/udev/rules.d/
```

* Insert the character device

```bash
$ cd $HOME/devel/char-device/kernel
$ sudo insmod chrdev_kernel.ko
```
* __verification__

```bash
$ lsmod | grep -i "chrdev_kernel"
chrdev_kernel          16384  0
$
$ udevadm info -a -p /sys/class/mycdev/mycdev-0

Udevadm info starts with the device specified by the devpath and then
walks up the chain of parent devices. It prints for every device
found, all possible attributes in the udev rules key format.
A rule to match, can be composed by the attributes of the device
and the attributes from one single parent device.

  looking at device '/devices/virtual/mycdev/mycdev-0':
    KERNEL=="mycdev-0"
    SUBSYSTEM=="mycdev"
    DRIVER==""

$ ls -lrta /dev/mycdev-0
crw-rw-rw- 1 root root 236, 0 oct.  17 15:11 /dev/mycdev-0
$
```

Try with the following command line to see what's happened.

```bash
$ tail -f /var/log/kern.log
Oct 17 15:13:10 icg-machine kernel: [1467534.560338] Entering: init_module
Oct 17 15:13:10 icg-machine kernel: [1467534.560342] device major number is: 236
```


#### Launch the application
__1- Send a message to the Linux kernel module (chrdev_kernel)__

```bash
$ cd $HOME/devel/char-device/user
$ ./chrdev_user -d /dev/mycdev-0 -w "Hello: ilies CHERGUI"
Writing message to the device [Hello: ilies CHERGUI]
$
```

```bash
$ tail -f /var/log/kern.log

Oct 17 15:20:46 icg-machine kernel: [1467990.299123] Entering: cdev_open
Oct 17 15:20:46 icg-machine kernel: [1467990.299233] Entering: cdev_write
Oct 17 15:20:46 icg-machine kernel: [1467990.299236] Copied 20 bytes from the user
Oct 17 15:20:46 icg-machine kernel: [1467990.299238] Receive data from user: Hello: ilies CHERGUI
Oct 17 15:20:46 icg-machine kernel: [1467990.299243] Entering: cdev_release
```

__2- Read the latest message from Linux kernel module (chrdev_kernel)__

```bash
$ cd $HOME/devel/char-device/user
$ ./chrdev_user -d /dev/mycdev-0 -r
Reading from the device...
The received message is: [Hello: ilies CHERGUI]
$
```

```bash
$ tail -f /var/log/kern.log

Oct 17 15:22:24 icg-machine kernel: [1468088.887023] Entering: cdev_open
Oct 17 15:22:24 icg-machine kernel: [1468088.887121] Entering: cdev_read
Oct 17 15:22:24 icg-machine kernel: [1468088.887123] user data len: 20
Oct 17 15:22:24 icg-machine kernel: [1468088.887151] Entering: cdev_release
```

#### Remove it from Linux kernel
```bash
$ sudo rmmod chrdev_kernel
```
