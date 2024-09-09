#include <stdio.h>
#include "jsonwrite.h"

#define BUFSIZE (16 * 1024)

static char buf[BUFSIZE];

int main(int argc, char **argv)
{
    jwr_init(buf, sizeof buf);

    __TESTFILE__

    jwr_finish();
    puts(buf);
    return 0;
}
