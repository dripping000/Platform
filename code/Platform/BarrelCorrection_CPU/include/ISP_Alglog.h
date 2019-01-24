#include <stdio.h>
#include <string.h>

#define  LOG_TAG    "MEDIACTRL"
#define  LOG_TAG_PLATE    "CPUVIDEO_ALG_PLATE"


#ifdef RUN_ON_QUALC
#include <android/log.h>
#endif


//#define ALGLOG
#ifdef ALGLOG
#define ISPprintLog(...) printf(__VA_ARGS__)
//#define ISPprintLog(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#else
#define ISPprintLog(...) (void)0
#endif
// #define RUN_ON_QUALC
#ifdef RUN_ON_QUALC
#define ISPprintALog(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#else
#define ISPprintALog(...) printf(__VA_ARGS__)
#endif

//#define CONVINBLAS
//#define TIMTER_NOTE
//#define PRINT_PROCESS_DATA
//#define COMBINE_CPU45_FOR_8056
//#define COMBINE_CPU0123_FOR_8053

