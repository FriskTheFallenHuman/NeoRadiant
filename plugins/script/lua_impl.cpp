// This file owns the minilua implementation.
// It must be compiled exactly once.  Every other .cpp that needs the Lua
// API should include  minilua.h  WITHOUT defining LUA_IMPL.
#define LUA_IMPL
#include "minilua.h"
