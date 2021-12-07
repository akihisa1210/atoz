#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/uinput.h>

#define INPUT_DEVICE "/dev/input/event6"

void die(const char *s)
{
    perror(s);
    exit(1);
}

void emit(int fd, int type, int code, int val)
{
    struct input_event ie;

    ie.type = type;
    ie.code = code;
    ie.value = val;
    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;

    write(fd, &ie, sizeof(ie));
}

int create_uinput_device()
{
    struct uinput_setup usetup;
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd == -1) {
        die("failed to open /dev/uinput");
    }

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, KEY_Z);

    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234; /* sample vendor */
    usetup.id.product = 0x5678; /* sample product */
    strcpy(usetup.name, "Example device");

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);

    sleep(1);

    return fd;
}

int main(int argc, char *argv[])
{
    int fdi = open(INPUT_DEVICE, O_RDONLY);
    if (fdi == -1) {
        die("failed to open the input device");
    }

    int fdo = create_uinput_device();
    
    struct input_event ie;

    printf("Start atoz.\n");
    while(1) {
        ssize_t result = read(fdi, &ie, sizeof(ie));
        if (result == -1) {
            die("failed to read the input device");
        }
        switch(ie.type) {
        case EV_KEY:
            if (ie.code == KEY_A) {
                emit(fdo, EV_KEY, KEY_Z, 1);
                emit(fdo, EV_SYN, SYN_REPORT, 0);
                emit(fdo, EV_KEY, KEY_Z, 0);
                emit(fdo, EV_SYN, SYN_REPORT, 0);
            }
        }
    }
}
