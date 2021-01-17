#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    int i;
    int count;
    char buf[1024];
    if (argc != 3)
    {
        printf("\n Usage: %s <input-raw-audio-file> <output-rsf-file>\n", argv[0]);
        return -1;
    }
    char *infile = argv[1];
    int fd = open(infile, O_RDONLY|O_BINARY);
    if (fd < 0)
    {
        fprintf(stderr, "open for read error\n");
        return -1;
    }
    char *outfile = argv[2];
    int fdo = open(outfile, O_CREAT|O_WRONLY|O_BINARY);
    if (fdo < 0)
    {
        fprintf(stderr, "open for write error\n");
        return -1;
    }

    off_t fsize = lseek(fd, 0, SEEK_END);
    if (fsize < 0)
    {
        fprintf(stderr, "lseek error\n");
        return -1;
    }

    if (fsize > 65535)
    {
        fprintf(stderr, "Error: input file large than 65535 bytes\n");
        return -1;
    }
    lseek(fd, 0, SEEK_SET);

    buf[0] = 0x01;
    buf[1] = 0x00;
    buf[2] = fsize/256;
    buf[3] = fsize%256;
    buf[4] = 0x1f;
    buf[5] = 0x40;
    buf[6] = 0x00;
    buf[7] = 0x00;
    write(fdo, buf, 8);

    while ((count = read(fd, buf, sizeof(buf))) > 0)
    {
        write(fdo, buf, count);
    }

    close(fd);
    close(fdo);

    return 0;
}
