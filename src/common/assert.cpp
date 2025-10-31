#include <iostream>
#include <common/assert.hpp>
#include <common/compiler.hpp>
#include <common/platform.hpp>

namespace transmute::common
{
#if TR_COMPILER_IS(CLANG) || TR_COMPILER_IS(GCC)
  void BreakPoint()
  {
#if TR_PLATFORM_IS(X86)
    __asm__ __volatile__("int $3\n\t");
#elif TR_PLATFORM_IS(ARM32)
    __asm__ __volatile__("bkpt 0");
#elif TR_PLATFORM_IS(ARM64)
    __asm__ __volatile__("brk 0xf000");
#elif TR_PLATFORM_IS(LOONGARCH)
    __asm__ __volatile__("break 0");
#elif TR_PLATFORM_IS(RISCV)
    __asm__ __volatile__("ebreak");
#elif TR_PLATFORM_IS(MIPS)
    __asm__ __volatile__("break");
#elif TR_PLATFORM_IS(S390) || TR_PLATFORM_IS(S390X)
    __asm__ __volatile__(".word 0x0001");
#elif TR_PLATFORM_IS(PPC) || TR_PLATFORM_IS(PPC64)
    __asm__ __volatile__("twge 2,2");
#elif TR_PLATFORM_IS(WASM32) || TR_PLATFORM_IS(WASM64)
    EM_ASM(debugger;);
#else
#error "Unsupported platform"
#endif
  }

#elif TR_COMPILER_IS(MSVC)
  void BreakPoint()
  {
    __debugbreak();
  }

#else
#error "Unsupported compiler"
#endif

  void HandleAssertionFailure(const char *file,
                              const char *function,
                              int line,
                              const char *condition)
  {
    std::cerr << "Assertion failure at " << file << ":" << line << " (" << function
              << "): " << condition;
#if defined(TR_ABORT_ON_ASSERT)
    abort();
#else
    BreakPoint();
#endif
  }
}
