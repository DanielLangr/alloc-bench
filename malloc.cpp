#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <cstddef>
#include <iostream>
#include <unordered_map>

#include <dlfcn.h>
#include <omp.h>

static __thread int hooked = 0;
static __thread void *(*orig_malloc)(size_t) = NULL;

std::unordered_map<size_t, size_t>* maps[1024] = { NULL };

__attribute((destructor)) static void done(void)
{
   for (int tid = 0; tid < omp_get_max_threads(); tid++)
   {
      size_t nalloc = 0;
      for (const auto& p : *maps[tid])
         nalloc += p.second;
      std::cout << nalloc << std::endl;
   }
}

void hook(size_t len)
{
   auto tid = omp_get_thread_num();
   if (!maps[tid])
      maps[tid] = new std::unordered_map<size_t, size_t>;
   maps[tid]->operator[](len)++;
}

extern "C"
{

void* malloc(size_t len)
{
   void* ret;

   if (!orig_malloc) 
      orig_malloc = (void*(*)(size_t))dlsym(RTLD_NEXT, "malloc"); // invokes malloc internally?

   if (hooked)
      return (*orig_malloc)(len);

   hooked = 1;
   hook(len);
   ret = (*orig_malloc)(len);
   hooked = 0;
   return ret;
}

} // extern "C"
