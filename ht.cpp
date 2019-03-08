#include <cmath>
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "colors.h"
#include "timer.h"

#ifdef HAVE_BOOST

#include <boost/container/small_vector.hpp>
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

void benchmark(size_t n)
{
#pragma omp parallel
   {
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<int> keydist;
      std::gamma_distribution<> lendist(2.0); 

#if defined(HAVE_BOOST) && defined(SBO)
      using value_t = boost::container::small_vector<float, SBO>;
#else
      using value_t = std::vector<float>;
#endif

#if defined(HAVE_BOOST) && defined(MP)
      std::unordered_map<int, value_t, std::hash<int>, std::equal_to<int>, pool_alloc<std::pair<const int, value_t>>> um;
#else
      std::unordered_map<key_t, value_t> um;
#endif
      um.reserve(n);

      for (size_t i = 0; i < n; i++) {
         size_t len = 1 + static_cast<size_t>(lendist(gen));
         auto p = um.emplace(std::make_pair(keydist(gen), value_t(len)));
      }
   }
}

int main()
{
   static const size_t n = 10'000'000;

   chrono_timer<> timer(chrono_timer<>::start_now);

   benchmark(n);

   timer.stop();
   auto d = timer.seconds();
   std::cout << cyan << "Absolute runtime = " << d << " [s]" << reset << std::endl;

   auto rate = (double)n / d / 1'000'000;
   std::cout << yellow <<  "Update rate per thread = " << rate << " [Mupdate/s]" << reset << std::endl;
}
