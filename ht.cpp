#include <iostream>
#include <random>

#ifdef HAVE_BOOST
// #include <boost/container/small_vector.hpp>
   #include <boost/pool/pool.hpp>
#endif

#include <gnu/libc-version.h>

#include "colors.h"
#include "timer.h"

//using value_type = double;
using value_type = char;

//using vector = std::vector<value_type>; 
//using vector = boost::container::small_vector<value_type, 100>;
//using vector = std::vector<value_type, my_pool_alloc<value_type>>;

template <typename T>
struct new_delete_policy {
   T* alloc(size_t n) { return new T[n]; }
   void free(T* ptr) { delete[] ptr; }
};

#ifdef HAVE_BOOST
template <typename T, size_t N>
struct boost_pool_policy {
   boost::pool<> pool_;
   boost_pool_policy() : pool_(sizeof(T) * N) { }
   T* alloc(size_t n) { return (T*)pool_.malloc(); }
   void free(T* ptr) { pool_.free(ptr); }
};
#endif

template <typename alloc_policy>
void benchmark(size_t n, size_t m)
{
#pragma omp parallel
   {
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<size_t> dist(1, 100);

      alloc_policy ap;
   // Pool pool(sizeof(value_type));
   // std::vector<vector> v(m);
   // std::vector<vector> v(m, vector(pool));

      std::vector<value_type*> v(m, nullptr);
      
      for (size_t j = 0; j < m; j++) v[j] = ap.alloc(dist(gen));

      for (size_t i = 1; i < n; i++)
         for (size_t j = 0; j < m; j++) 
         {
            ap.free(v[j]);
            v[j] = ap.alloc(dist(gen));
         }

      for (size_t j = 0; j < m; j++) ap.free(v[j]);
   }
}

int main()
{
   static const size_t n = 10000;
   static const size_t m = 10000;

   chrono_timer<> timer(chrono_timer<>::start_now);

   using alloc_policy = new_delete_policy<value_type>;
// using alloc_policy = boost_pool_policy<value_type, 100>;

   benchmark<alloc_policy>(n, m);

   timer.stop();
   auto d = timer.seconds();
   std::cout << cyan << "Absolute runtime = " << d << " [s]" << reset << std::endl;
   auto nalloc = n * m;
   auto speed = (double)nalloc / d / 1000000;
   std::cout << yellow <<  "Allocation speed per thread = " << speed << " [Mallocs/s]" << reset << std::endl;
   auto pace = (double)timer.nanoseconds() / (double)nalloc;
   std::cout << magenta << "Average time per alloc = " << pace << " [ns]" << reset << std::endl;
   std::cout << red << "GNU libc version: " << gnu_get_libc_version() << reset << std::endl;
}
