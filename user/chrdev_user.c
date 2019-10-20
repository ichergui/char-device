#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#define MAX_BUF_SIZE 256

static void print_usage(const char *prog)
{
    fprintf(stdout,"Usage: %s [-hdwr]\n",prog);
    fprintf(stdout,"\t-d --device\t\t\t\t: device to use.\n");
    fprintf(stdout,"\t-w --write\t\t\t\t: write a new mesage .\n");
    fprintf(stdout,"\t-r --read\t\t\t\t: read the message manage by char device.\n");
    fprintf(stdout,"\t-h --help\t\t\t\t: print this message\n");
    return;
}

static const struct option lopts[] = {
    { "device", required_argument, 0, 'd' },
    { "write", required_argument, 0, 'w' },
    { "read", no_argument, 0, 'r' },
    { "help", no_argument, 0, 'h' },
    { 0, 0, 0, 0 },
};

int main(int argc, char* argv[])
{
    struct stat st_dev;
    char device[MAX_BUF_SIZE];
    char user_msg[MAX_BUF_SIZE];
    int option_index = 0;
    int c = -1;
    int msg_len = 0;
    int ret, fd = 0;

    if (argc == 1)
    {
        print_usage(argv[0]);
        return -1;
    }

    do {
        c = getopt_long(argc, argv, ":d:", lopts, &option_index);
        if ( c == 'd' ) {
            memset(device, 0, sizeof(device));
            strcpy(device, optarg);
        }
    } while( c != -1 );
    optind=0;

    if (stat(device, &st_dev) == -1)
    {
        fprintf(stderr,"stat: %s\n", strerror(errno));
        return -1;
    }

    fd = open(device, O_RDWR);
    if (fd < 0){
        fprintf(stderr,"open: %s\n", strerror(errno));
        return -1;
    }

    do {
        c = getopt_long(argc, argv, "hrd:w:",lopts, &option_index);
        if ( c == -1 )
            break;

        switch( c ) {
            case 'w':
                memset(user_msg, 0, sizeof(user_msg));
                strcpy(user_msg, optarg);
                fprintf(stdout,"Writing message to the device [%s]\n", user_msg);
                ret = write(fd, user_msg, strlen(user_msg));
                if (ret < 0) {
                    fprintf(stderr,"write: %s\n", strerror(errno));
                    close (fd);
                    return -1;
                }
            break;
            case 'r':
                fprintf(stdout,"Reading from the device...\n");
                memset(user_msg, 0, sizeof(user_msg));
                ret = read(fd, user_msg, MAX_BUF_SIZE);
                if (ret < 0){
                    fprintf(stderr,"read: %s\n", strerror(errno));
                    close(fd);
                    return -1;
                }
                fprintf(stdout,"The received message is: [%s]\n", user_msg);
            break;
            case 'd':
            break;
            case 'h':
                print_usage(argv[0]);
            break;
            default:
                print_usage(argv[0]);
            break;
        }
    } while(1);

    close(fd);
    return 0;
}
