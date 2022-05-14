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
#include "mmap.h"

uint64 mmap(uint64 addr, int size, int prot, int flags, int fd, int offset) {
    struct file *f;
    struct proc *p = myproc();
    int i;

    if((f = p->ofile[fd]) == 0) return MAP_FAILED;
    filedup(f);

    for(i=0; i<MAXVMA;i++) {
        if(p->vma[i].vma_start == 0) break;
    }
    if(i==MAXVMA) return MAP_FAILED;

    if(flags&MAP_SHARED) {
        if((prot & PROT_READ) && !f->readable) return MAP_FAILED;
        if((prot & PROT_WRITE) && !f->writable) return MAP_FAILED;
    }

    p->vma[i].vma_end = PGROUNDDOWN(p->heap_end);
    p->vma[i].vma_start = p->heap_end = PGROUNDDOWN(p->heap_end-size);
    p->vma[i].fd = fd;
    uint pte_flags = 0;
    if(prot & PROT_READ) pte_flags |= PTE_R;
    if(prot & PROT_WRITE) pte_flags |= PTE_W;
    p->vma[i].prot=pte_flags;
    p->vma[i].flags = flags;
    p->vma[i].offset = offset;
    p->vma[i].f = f;
    return p->vma[i].vma_start;
}

uint64 munmap(uint64 addr, int len) {
    addr = PGROUNDDOWN(addr);

    struct vma_t *vma = 0;
    struct proc *p = myproc();
    struct file *f;
    uint64 pa;

    for(int i=0;i<MAXVMA;i++) {
        if(p->vma[i].vma_start != 0 && p->vma[i].vma_start <= addr && p->vma[i].vma_end >= addr) {
            vma = &p->vma[i];
            break;
        }
    }
    if(vma == 0) return -1;

    f = vma->f;
    filewrite(f, addr, len);

    if((pa = walkaddr(p->pagetable, addr)) != 0) {
        uvmunmap(p->pagetable, addr, len/PGSIZE, 1);
    }

    if(vma->vma_start == addr)
        vma->vma_start += len;
    else
        vma->vma_end -= len;
    if(vma->vma_end == vma->vma_start) {
        fileclose(f);

        vma->vma_start = 0;
        vma->f = 0;
    }
    return 0;
}
