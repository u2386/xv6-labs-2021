#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "fcntl.h"

int
sigalarm(int ticks, uint64 addr) {
    struct proc *p = myproc();

    if(ticks==0) {
        p->alarm->ticks = 0;
        p->alarm->remain = 0;
        p->alarm->handler = 0;
        return 0;
    }

    p->alarm->ticks = ticks;
    p->alarm->remain = ticks;
    p->alarm->handler = (void(*)())addr;
    return 0;
}

int
sigreturn(void) {
    struct proc *p = myproc();

    *p->trapframe = p->alarm->trapframe;
    p->alarm->remain = p->alarm->ticks;
    return 0;
}