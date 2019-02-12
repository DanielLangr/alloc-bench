#include <iostream>
#include <gnu/libc-version.h>

int main()
{
   std::cout << "GNU libc version: " << gnu_get_libc_version() << std::endl;
}
