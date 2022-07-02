#include <cstdint>
#include <cstdio>

#include <libkern/OSCacheControl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>

#include "oaknut/oaknut.hpp"

int main()
{
    const size_t page_size = getpagesize();
    std::printf("page size: %zu\n", page_size);

    std::uint32_t* mem = (std::uint32_t*)mmap(nullptr, page_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_JIT | MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    pthread_jit_write_protect_np(false);

    using namespace oaknut;
    using namespace oaknut::util;

    CodeGenerator code{mem};
    code.MOVZ(W0, 42);
    code.RET(X30);

    pthread_jit_write_protect_np(true);
    sys_icache_invalidate(mem, page_size);

    std::printf("%i\n", ((int (*)())mem)());

    return 0;
}
