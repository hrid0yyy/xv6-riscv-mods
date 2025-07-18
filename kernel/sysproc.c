#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "pstat.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_settickets(void)
{
    int tickets;
    
    // Get the number of tickets from user space
    argint(0, &tickets);
    
    // Check if tickets is valid (must be >= 1)
    if(tickets < 1) {
        tickets = 1; // Default to 1 if invalid
    }
    
    // Set tickets for current process
    struct proc *p = myproc();
    acquire(&p->lock);
    p->tickets_original = tickets;
    p->tickets_current = tickets;
    release(&p->lock);
    
    return 0;
}

uint64
sys_getpinfo(void)
{
    uint64 addr;
    struct pstat ps;
    
    // Get pointer from user space
    argaddr(0, &addr);
    
    // Fill pstat structure
    if(getpinfo(&ps) < 0)
        return -1;
    
    // Copy to user space
    if(copyout(myproc()->pagetable, addr, (char*)&ps, sizeof(ps)) < 0)
        return -1;
    
    return 0;
}
