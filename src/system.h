#ifndef system_included
#define system_included
//----------------------------------------------------------------------

// system-specific stuff..
// wrap in #ifdefs
// (like clap/macros.h)

//----------------------------------------------------------------------

//SKEI: is this linux specific or portable?
#include <dlfcn.h>

//----------

void* open_library(const char* filename) {
  return dlopen(filename,RTLD_LAZY | RTLD_LOCAL ); // RTLD_NOW, RTLD_LAZY
}

//----------

void close_library(void* lib) {
  dlclose(lib);
}

//----------

void* get_library_symbol(void* lib, const char* sym) {
  return dlsym(lib,sym);
}

//----------------------------------------------------------------------
#endif
