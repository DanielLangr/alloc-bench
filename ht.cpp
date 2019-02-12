#include <iostream>
#include <random>
#include <string>
#include <unordered_map>

#include "colors.h"
#include "timer.h"

void benchmark(size_t n, size_t m)
{
#pragma omp parallel
   {
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<size_t> keydist(0, 4095);
      std::uniform_int_distribution<size_t> lendist(50, 100);

      std::unordered_map<int, std::string> um;

      for (size_t i = 0; i < n; i++) {
         for (size_t j = 0; j < m; j++) 
            um[keydist(gen)] = std::string('a', lendist(gen));
         um.clear();
      }
   }
}

int main()
{
   static const size_t n = 4000;
   static const size_t m = 10000;

   chrono_timer<> timer(chrono_timer<>::start_now);

   benchmark(n, m);

   timer.stop();
   auto d = timer.seconds();
   std::cout << cyan << "Absolute runtime = " << d << " [s]" << reset << std::endl;
   auto nalloc = n * m * 2;
   auto speed = (double)nalloc / d / 1000000;
   std::cout << yellow <<  "Allocation speed per thread = " << speed << " [Mallocs/s]" << reset << std::endl;
   auto pace = (double)timer.nanoseconds() / (double)nalloc;
   std::cout << magenta << "Average time per alloc = " << pace << " [ns]" << reset << std::endl;
}
