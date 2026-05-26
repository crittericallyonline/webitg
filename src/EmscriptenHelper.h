#ifndef EMSCRIPTEN_HELPER
#define EMSCRIPTEN_HELPER

#if defined(__EMSCRIPTEN__)
#ifndef NOT_EM
#define NOT_EM(x) do {} while (0)
#endif
#ifndef IS_EM
#define IS_EM(x) do { x } while (0)
#endif
#else
#ifndef NOT_EM
#define NOT_EM(x) do { x } while (0)
#endif
#ifndef IS_EM
#define IS_EM(x) do {} while (0)
#endif

#endif


#endif