#ifndef utils_included
#define utils_included
//----------------------------------------------------------------------

//#define MIN(a,b)  (((a) < (b)) ? (a) : (b))
//#define MAX(a,b)  (((a) > (b)) ? (a) : (b))

//----------------------------------------------------------------------

// these are a bit hacky..

//----------

// /home/skei/test.so -> test.so
// returns ptr to first character after last /

const char* get_filename_from_path(const char* path) {
  if (path) {
    const char* slash     = strrchr(path,'/');
    const char* backslash = strrchr(path,'\\');
    if (slash) {
      return slash + 1;
    }
    else if (backslash) {
      return backslash + 1;
    }
  }
  return NULL;
}

//----------

// /home/skei/test.so -> /home/skei/
// inserts a 0 after the last /

char* get_path_only(char* dst, const char* src) {
  if (dst && src) {
    strcpy(dst,src);
    char* slash     = strrchr(dst,'/');
    char* backslash = strrchr(dst,'\\');
    if (slash) {
      slash[1] = 0;
      return dst;
    }
    else if (backslash) {
      backslash[1] = 0;
      return dst;
    }
  }
  return NULL;
}

//----------------------------------------------------------------------
#endif
