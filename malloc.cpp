#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <cassert>
#include <cstddef>
#include <iostream>
#include <unordered_map>

#include <dlfcn.h>
#include <omp.h>

#include "colors.h"

static __thread int hooked = 0;
static __thread void *(*orig_malloc)(size_t) = NULL;

static constexpr size_t MAX_MAPS = 1024;
std::unordered_map<size_t, size_t>* maps[MAX_MAPS] = { NULL };
//static __thread size_t count = 0;

__attribute((destructor)) static void done(void)
{
   for (int tid = 0; tid < omp_get_max_threads(); tid++)
   {
      size_t nalloc = 0;
      for (const auto& p : *maps[tid]) {
         std::cerr << p.first << " : " << p.second << std::endl;
         nalloc += p.second;
      }
      std::cerr << "Total number of allocations: " << nalloc << std::endl;
   }
// std::cerr << count << std::endl;
}

void hook(size_t len)
{
   auto tid = omp_get_thread_num();
   assert(tid < MAX_MAPS);
   if (!maps[tid])
      maps[tid] = new std::unordered_map<size_t, size_t>;
   maps[tid]->operator[](len)++;
// count++;
}

extern "C"
{

void* malloc(size_t len)
{
   void* ret;

   if (!orig_malloc) 
      orig_malloc = (void*(*)(size_t))dlsym(RTLD_NEXT, "malloc"); // invokes calloc internally?

   if (hooked)
      return (*orig_malloc)(len);

   hooked = 1;
   hook(len);
   ret = (*orig_malloc)(len);
   hooked = 0;
   return ret;
}

} // extern "C"
