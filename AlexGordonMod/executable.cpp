#include "executable.h"


Executable::Executable() : base((uintptr_t)GetModuleHandle(NULL)) {}
