// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

// 新增
int reference_count[(PHYSTOP - KERNBASE) / PGSIZE];

//
// **新增**
// 用来为reference_count增加或者减少
// 并且检查reference，若为0，free该内存
// n 可以为负数
//
void reference_add(uint64 *pa, int n) {
  int *count = &reference_count[(PHYSTOP - (uint64)pa) / PGSIZE];
  *count += n; 
  if (*count == 0) {
    kfree(pa);
  } 
  if (*count < 0)
    panic("reference");
}


void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  printf("start ~ end:%p ~ %p\n", p, pa_end);
  // for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE){
  //   /** 初始化ref_count  */
  //   reference[getrefindex(p)] = 0;
  //   kfree(p);
  // }
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  //
  // 修改
  //

  // 该页ref_count减一，若减后为0，执行free
  int *count = &reference_count[(PHYSTOP - PGROUNDDOWN((uint64)pa)) / PGSIZE];
  if (*count > 0)
      *count += 1;
  if (*count == 0)
      return;

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk

  //
  // 修改
  //

  // 设置初始reference计数器
  reference_count[(PHYSTOP - (uint64)r) / PGSIZE] = 1;

  return (void*)r;
}
