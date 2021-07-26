#pragma once

/*
defer implementation. It can be useful to free resources.

Usage:
  void dosomething()
  {
    char* data = malloc(100);
    DEFER({ free(data); });

    dosomethingwith(data);
  }

Reference: https://gist.github.com/baruch/f005ce51e9c5bd5c1897ab24ea1ecf3b
*/

#define DEFER_MERGE(a, b) a##b
#define DEFER_VARNAME(a) DEFER_MERGE(defer_scopevar_, a)
#define DEFER_FUNCNAME(a) DEFER_MERGE(defer_scopefunc_, a)
#define DEFER(BLOCK)                                                   \
  void DEFER_FUNCNAME(__LINE__)(__attribute__((unused)) int *a) BLOCK; \
  __attribute__((cleanup(DEFER_FUNCNAME(__LINE__)))) int DEFER_VARNAME(__LINE__)

// Add this because Visual Studio Code's Intellisense
// doesn't like anonymous functions.
#ifdef __INTELLISENSE__
#pragma diag_suppress 65
#endif
