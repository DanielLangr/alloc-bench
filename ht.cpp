#include <iostream>
#include <random>
#include <string>
#include <unordered_map>

#include "colors.h"
#include "timer.h"

#ifdef HAVE_BOOST

#include <boost/pool/pool.hpp>

template <typename T>
class pool_alloc {
   boost::pool<boost::default_user_allocator_malloc_free> pool_;

public:
   using value_type = T;

   pool_alloc() : pool_(sizeof(T)) { }
   template <typename U> pool_alloc(const pool_alloc<U>&) : pool_alloc() { }

   T *allocate(const size_t n) {
      T* ret;
      ret = static_cast<T*>( (n == 1) ? pool_.malloc() : std::malloc(n * sizeof(T)) );
      if (!ret && n) throw std::bad_alloc();
      return ret;
   }

   void deallocate(T* ptr, const size_t n) {
      if (ptr && n)
         if (n == 1) pool_.free(ptr); else std::free(ptr);
   }
};

template <typename T, typename U> bool operator==(const pool_alloc<T>& a, const pool_alloc<U>& b) { return &a == &b; }
template <typename T, typename U> bool operator!=(const pool_alloc<T>& a, const pool_alloc<U>& b) { return &a != &b; }

#endif /* HAVE_BOOST */

void benchmark(size_t n, size_t m)
{
#pragma omp parallel
   {
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<size_t> keydist(0, 65535);
      std::uniform_int_distribution<size_t> lendist(50, 100);
   // std::uniform_int_distribution<size_t> lendist(5, 10);

      std::unordered_map<int, std::string> um;
   // std::unordered_map<int, std::string, std::hash<int>, std::equal_to<int>, pool_alloc<std::pair<const int, std::string>>> um;

      size_t l = 0;

      for (size_t i = 0; i < n; i++) {
         for (size_t j = 0; j < m; j++) {
         // um[keydist(gen)] = std::string('a', lendist(gen)); 
            auto p = um.emplace(std::make_pair(keydist(gen), std::string(lendist(gen), 'a')));
         // donotopt += p.first->second.length();
         }
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

   auto nupdates = n * m;
   auto rate = (double)nupdates / d / 1000000;
   std::cout << yellow <<  "Update rate per thread = " << rate << " [Mupdate/s]" << reset << std::endl;

   auto nalloc = n * m * 2;
   auto speed = (double)nalloc / d / 1000000;
   std::cout << yellow <<  "Allocation rate per thread = " << speed << " [Malloc/s]" << reset << std::endl;
   auto pace = (double)timer.nanoseconds() / (double)nalloc;
   std::cout << magenta << "Average time per alloc = " << pace << " [ns]" << reset << std::endl;

// std::cout << donotopt << std::endl;
}
