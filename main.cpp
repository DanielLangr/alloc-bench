#include <iostream>
#include <random>

#include <boost/container/small_vector.hpp>

#include <gnu/libc-version.h>

#include "colors.h"
#include "timer.h"

//using vector = std::vector<double>; 
using vector = boost::container::small_vector<double, 100>;

void benchmark(size_t n, size_t m)
{
#pragma omp parallel
   {
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<size_t> dist(1, 100);

      std::vector<vector> v(m);
      
      for (size_t i = 0; i < n; i++)
         for (size_t j = 0; j < m; j++) 
            v[j] = vector{dist(gen), 1.0};
   }
}

int main()
{
   static const size_t n = 10000;
   static const size_t m = 10000;

   chrono_timer<> timer(chrono_timer<>::start_now);

   benchmark(n, m);

   timer.stop();
   auto d = timer.seconds();
   std::cout << cyan << "Absolute runtime = " << d << " [s]" << reset << std::endl;
   auto nalloc = n * m;
   auto speed = (double)nalloc / d;
   std::cout << yellow <<  "Allocation speed per thread = " << speed << " [allocs/s]" << reset << std::endl;
   auto pace = (double)timer.nanoseconds() / (double)nalloc;
   std::cout << magenta << "Average time per alloc = " << pace << " [ns]" << reset << std::endl;
   std::cout << red << "GNU libc version: " << gnu_get_libc_version() << reset << std::endl;
}
