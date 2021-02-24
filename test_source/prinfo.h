#ifndef _LINUX_PRINFO_H
#define _LINUX_PRINFO_H

#include <linux/types.h>

struct prinfo{
        long state;
        pid_t pid;
        pid_t parent_pid;
        pid_t first_child_pid;
        pid_t next_sibling_pid;
        char comm[64];
};
#endif
