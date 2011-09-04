/* Copyright 2010 Matthew Arsenault, Travis Desell, Boleslaw
Szymanski, Heidi Newberg, Carlos Varela, Malik Magdon-Ismail and
Rensselaer Polytechnic Institute.

This file is part of Milkway@Home.

Milkyway@Home is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Milkyway@Home is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Milkyway@Home.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _WIN32
  #include <sys/time.h>
  #include <sys/resource.h>
#else
  #include <stdlib.h>
  #include <malloc.h>
  #include <windows.h>
#endif /* _WIN32 */

#include <time.h>
#include <errno.h>
#include <float.h>

#ifdef __SSE__
  #include <xmmintrin.h>
#endif /* __SSE__ */


#include "milkyway_util.h"
#include "mw_boinc_util.h"

#if MW_IS_X86
  #if HAVE_FPU_CONTROL_H
    #include <fpu_control.h>
  #endif
  #ifndef _FPU_SETCW
    #define _FPU_SETCW(cw) __asm__ ("fldcw %0" : : "m" (*&cw))
  #endif
  #ifndef _FPU_GETCW
    #define _FPU_GETCW(cw) __asm__ ("fnstcw %0" : "=m" (*&cw))
  #endif

  #ifndef _FPU_EXTENDED
    #define _FPU_EXTENDED 0x300
  #endif
  #ifndef _FPU_DOUBLE
    #define _FPU_DOUBLE 0x200
  #endif
  #if defined(__FreeBSD__) && !defined(_FPU_DEFAULT)
    #include <machine/fpu.h>
    #define _FPU_DEFAULT __INITIAL_FPUCW__
  #endif
#endif /* MW_IS_X86 */


void* mwCalloc(size_t count, size_t size)
{
    void* mem = (void*) calloc(count, size);
    if (mem == NULL)
        fail("calloc failed: "ZU" bytes\n", count * size);
    return mem;
}

void* mwMalloc(size_t size)
{
    void* mem = (void*) malloc(size);
    if (mem == NULL)
        fail("malloc failed: "ZU" bytes\n", size);
    return mem;
}


#ifndef __APPLE__

void* mwCallocA(size_t count, size_t size)
{
    void* p;
    size_t totalSize = count * size;

    p = mwMallocA(totalSize);
    memset(p, 0, totalSize);

    return p;
}

#else

void* mwCallocA(size_t count, size_t size)
{
    return mwCalloc(count, size);
}


#endif /* __APPLE__ */

#if defined(__APPLE__)

/* OS X already aligns everything to 16 bytes */
void* mwMallocA(size_t size)
{
    return mwMalloc(size);
}

#elif !defined(_WIN32)

void* mwMallocA(size_t size)
{
    void* p;

    if (posix_memalign(&p, 16, size))
    {
        perror(__func__);
        fail("Failed to allocate block of size %zu aligned to 16\n", size);
    }

    if (!p)
        fail("%s: NULL\n", __func__);

    return p;
}

#else

#if defined(__MINGW32__) && !defined(__MINGW64__)
  #define _aligned_malloc __mingw_aligned_malloc
#endif

void* mwMallocA(size_t size)
{
    void* p;

    p = _aligned_malloc(size, 16);
    if (!p)
        fail("%s: NULL: _aligned_malloc error = %ld\n", FUNC_NAME, GetLastError());

    return p;
}
#endif /* defined(__APPLE__) */

void* mwRealloc(void* ptr, size_t size)
{
    void* mem = (void*) realloc(ptr, size);
    if (mem == NULL)
        fail("realloc failed: "ZU" bytes\n", size);
    return mem;
}

static char* fcloseVerbose(FILE* f, const char* err)
{
    if (fclose(f))
        perror(err);

    return NULL;
}

char* mwFreadFileWithSize(FILE* f, const char* filename, size_t* sizeOut)
{
    long fsize;
    size_t readSize;
    char* buf;

    if (!f)
    {
        warn("Failed to open file '%s' for reading\n", filename);
        return NULL;
    }

     /* Find size of file */
    if (fseek(f, 0, SEEK_END) == -1)
        return fcloseVerbose(f, "Seeking file end");

    fsize = ftell(f);
    if (fsize == -1)
        return fcloseVerbose(f, "Getting file size");

    fseek(f, 0, SEEK_SET);

    buf = (char*) mwMalloc((fsize + 1) * sizeof(char));
    buf[fsize] = '\0';

    readSize = fread(buf, sizeof(char), fsize, f);
    if (readSize != (size_t) fsize)
    {
        warn("Failed to read file '%s': Expected to read %ld, but got "ZU"\n",
             filename, fsize, readSize);
        free(buf);
        buf = NULL;
    }

    fcloseVerbose(f, "Closing read file");

    if (sizeOut)
        *sizeOut = readSize;

    return buf;
}

char* mwFreadFile(FILE* f, const char* filename)
{
    return mwFreadFileWithSize(f, filename, NULL);
}

char* mwReadFileWithSize(const char* filename, size_t* sizeOut)
{
    return mwFreadFileWithSize(mw_fopen(filename, "rb"), filename, sizeOut);
}

char* mwReadFile(const char* filename)
{
    return mwFreadFileWithSize(mw_fopen(filename, "rb"), filename, NULL);
}


int mwWriteFile(const char* filename, const char* str)
{
    FILE* f;
    int rc;

    f = mw_fopen(filename, "w");
    if (!f)
    {
        perror("Writing file");
        return 1;
    }

    rc = fputs(str, f);
    if (rc == EOF)
        warn("Error writing file '%s'\n", filename);

    fcloseVerbose(f, "Closing write file");
    return rc;
}


#ifdef _WIN32

double mwGetTime()
{
    LARGE_INTEGER t, f;
    QueryPerformanceCounter(&t);
    QueryPerformanceFrequency(&f);
    return (double)t.QuadPart/(double)f.QuadPart;
}

double mwGetTimeMilli()
{
    return 1.0e3 * mwGetTime();
}

static UINT mwGetMinTimerResolution()
{
    TIMECAPS tc;

    if (timeGetDevCaps(&tc, sizeof(tc)) != TIMERR_NOERROR)
    {
        warn("Failed to get timer resolution\n");
        return 0;
    }

	/* target 1-millisecond target resolution */
    return MIN(MAX(tc.wPeriodMin, 1), tc.wPeriodMax);
}

int mwSetTimerMinResolution()
{
    if (timeBeginPeriod(mwGetMinTimerResolution()) != TIMERR_NOERROR)
    {
        warn("Failed to set timer resolution\n");
        return 1;
    }

    return 0;
}

int mwResetTimerResolution()
{
    if (timeEndPeriod(mwGetMinTimerResolution()) != TIMERR_NOERROR)
    {
        warn("Failed to end timer resolution\n");
        return 1;
    }

    return 0;
}

#else

/* Seconds */
double mwGetTime()
{
    struct timeval t;
    struct timezone tzp;
    gettimeofday(&t, &tzp);
    /* Prevent weird breakage when building with -fsingle-precision-constant */
    return t.tv_sec + t.tv_usec * (double) 1.0e-6;
}

/* Get time in microseconds */
long mwGetTimeMicro()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return 1000000 * t.tv_sec  + t.tv_usec;
}

/* Get time in milliseconds */
double mwGetTimeMilli()
{
    return (double) mwGetTimeMicro() / 1.0e3;
}

int mwSetTimerMinResolution()
{
	return 0;
}

int mwResetTimerResolution()
{
    return 0;
}

#endif

#if defined(__SSE__) && DISABLE_DENORMALS

int mwDisableDenormalsSSE()
{
    int oldMXCSR = _mm_getcsr();
    int newMXCSR = oldMXCSR | 0x8040;
    _mm_setcsr(newMXCSR);

    warn("Disabled denormals\n");
    return oldMXCSR;
}

#endif /* defined(__SSE__) && DISABLE_DENORMALS */


/* From the extra parameters, read them as doubles */
real* mwReadRestArgs(const char** rest, unsigned int n)
{
    unsigned int i;
    real* parameters = NULL;

    if (!rest)
        return NULL;

    parameters = (real*) mwMalloc(n * sizeof(real));

    errno = 0;
    for (i = 0; i < n; ++i)
    {
        parameters[i] = (real) strtod(rest[i], NULL);
        if (errno)
        {
            perror("Error parsing command line fit parameters");
            free(parameters);
            return NULL;
        }
    }

    return parameters;
}

/* Take subset of argv not used by the application to forward on to the Lua scripts.
   Need to free return value.
 */
const char** mwGetForwardedArguments(const char** args, unsigned int* nForwardedArgs)
{
    unsigned int i, argCount = 0;
    const char** forwardedArgs;

    if (!args)
    {
        *nForwardedArgs = 0;
        return NULL;
    }

    while (args[++argCount]);  /* Count number of parameters */

    forwardedArgs = (const char**) mwMalloc(sizeof(const char*) * argCount);

    for (i = 0; i < argCount; ++i)
        forwardedArgs[i] = args[i];

    *nForwardedArgs = argCount;

    return forwardedArgs;
}

void _mw_time_prefix(char* buf, size_t bufSize)
{
    time_t x = time(NULL);
    struct tm* tm = localtime(&x);
    if (!strftime(buf, bufSize - 1, "%H:%M:%S", tm))
        buf[0] = '\0';
}

/* Returns < 0 on error, otherwise bitwise or'd val flags from arguments encountered.
   We want to be able to some behaviour without the flag, but simply using some default value for the flag is undesirable.
*/
int mwReadArguments(poptContext context)
{
    int o;
    int rc = 0;

    while ((o = poptGetNextOpt(context)) >= 0)
    {
         rc |= o;
    }

    if (o < -1)
    {
        warn("Argument parsing error: %s: %s\n",
             poptBadOption(context, 0),
             poptStrerror(o));
        return -1;
    }

    return rc;
}


/* This is a really stupid workaround: The server sends double
 * arguments. Some of them happen to be negative numbers. Since BOINC
 * is stupid and appends extra arguments, e.g. --nthreads 4 or
 * --device 0 for multithreaded or GPU applications to the command
 * line instead of prepending like it should do (also user arguments
 * in app_info.xml), you can't use POPT_CONTEXT_POSIXMEHARDER. Popt
 * tries to interpret negative numbers as options when not using
 * POPT_CONTEXT_POSIXMEHARDER, which results in them being interpreted
 * as wrong options and then erroring.
 */
const char** mwFixArgv(int argc, const char* argv[])
{
    const char** argvCopy = NULL;
    const char** p = NULL;
    char* endP = NULL;

    int i, j;
    int np, remaining, appendedCount;
    int npCheck = 0;

    p = argv;
    while (*p && strncmp(*p, "-np", 3)) /* Find how many numbers we're expected to find */
        ++p, ++npCheck;

    if (!*p)  /* Probably not using the server arguments */
        return NULL;

    if (!*(++p)) /* Go to the actual value of np */
        return NULL;

    /* The next argument after np should be the number */
    np = strtoul(*p, &endP, 10);
    if (*endP != '\0')
    {
        perror("Reading np");
        return NULL;
    }

    /* -2: -1 from argv[0], -1 from -p arg, -1 from np value */
    remaining = (argc - 3) - npCheck;  /* All remaining arguments */
    if (np > remaining || np >= argc)  /* Careful of underflow */
    {
        warn("Warning: Number of parameters remaining can't match expected: -np = %d\n", np);
        return NULL;
    }

    ++p;   /* Move on to the p argument */
    if (*p && strncmp(*p, "-p", 2))
    {
        warn("Didn't find expected p argument\n");
        return NULL;
    }

    if (!*(++p))  /* Should be first actual number argument */
        return NULL;

    /* FIXME: Have no dependence on np. FIXME: BOINC really, really,
     * really shouldn't ever append arguments ever (should prepend)
     * and it should be fixed. */

    argvCopy = (const char**) mwCalloc(argc, sizeof(const char*));
    i = j = 0;  /* Index into copy argv, original argv */
    argvCopy[i++] = argv[j++];
    p += np;  /* Skip the arguments */

    appendedCount = remaining - np;  /* Extras added by BOINC */
    while (*p && i <= appendedCount)
        argvCopy[i++] = *p++;

    while (i < argc && j < argc)  /* Copy the rest of the arguments into an appropriate order */
        argvCopy[i++] = argv[j++];

    return argvCopy;
}

/* Pick from unit cube. i.e. not normalized */
mwvector mwRandomUnitPoint(dsfmt_t* dsfmtState)
{
    mwvector vec;

    X(vec) = mwUnitRandom(dsfmtState);
    Y(vec) = mwUnitRandom(dsfmtState);
    Z(vec) = mwUnitRandom(dsfmtState);
    W(vec) = 0.0;

    return vec;
}

mwvector mwRandomPoint(dsfmt_t* dsfmtState, real s)
{
    mwvector v = mwRandomUnitPoint(dsfmtState);
    mw_incmulvs(v, s);
    return v;
}

/* Random direction */
mwvector mwRandomUnitVector(dsfmt_t* dsfmtState)
{
    mwvector v = mwRandomUnitPoint(dsfmtState);
    mw_normalize(v);
    return v;
}

mwvector mwRandomVector(dsfmt_t* dsfmtState, real r)
{
    mwvector v = mwRandomUnitVector(dsfmtState);
    mw_incmulvs(v, r);
    return v;
}


/* Check for a timesteps etc. which will actually finish. */
int mwCheckNormalPosNumEps(real n)
{
    return !isfinite(n) || n <= 0.0 || n <= REAL_EPSILON;
}

/* Check for positive, real numbers that can be any size */
int mwCheckNormalPosNum(real n)
{
    return !isfinite(n) || n <= 0.0;
}

/* Disable the stupid "Windows is checking for a solution to the
 * problem" boxes from showing up if this crashes */
void mwDisableErrorBoxes()
{
  #ifdef _WIN32

    DWORD mode, setMode;

    setMode = SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX;
    mode = SetErrorMode(setMode);
    SetErrorMode(mode | setMode);
  #endif /* _WIN32 */
}


#ifdef _WIN32

static DWORD mwPriorityToPriorityClass(MWPriority x)
{
    switch (x)
    {
        case MW_PRIORITY_IDLE:
            return IDLE_PRIORITY_CLASS;
        case MW_PRIORITY_BELOW_NORMAL:
            return BELOW_NORMAL_PRIORITY_CLASS;
        case MW_PRIORITY_NORMAL:
            return NORMAL_PRIORITY_CLASS;
        case MW_PRIORITY_ABOVE_NORMAL:
            return ABOVE_NORMAL_PRIORITY_CLASS;
        case MW_PRIORITY_HIGH:
            return HIGH_PRIORITY_CLASS;
        default:
            warn("Invalid priority: %d. Using default.\n", (int) x);
            return mwPriorityToPriorityClass(MW_PRIORITY_DEFAULT);
    }
}

int mwSetProcessPriority(MWPriority priority)
{
    HANDLE handle;

    if (!DuplicateHandle(GetCurrentProcess(),
                         GetCurrentProcess(),
                         GetCurrentProcess(),
                         &handle,
                         0,
                         FALSE,
                         DUPLICATE_SAME_ACCESS))
    {
        warn("Failed to get process handle: %ld\n", GetLastError());
        return 1;
    }

    if (!SetPriorityClass(handle, mwPriorityToPriorityClass(priority)))
    {
        warn("Failed to set process priority class: %ld\n", GetLastError());
        return 1;
    }

    return 0;
}
#else

int mwSetProcessPriority(MWPriority priority)
{
    if (setpriority(PRIO_PROCESS, getpid(), priority))
    {
        perror("Setting process priority");
        return 1;
    }

    return 0;
}

#endif /* _WIN32 */


/*  From crlibm */
unsigned long long mwFixFPUPrecision()
{
#if MW_IS_X86

  #if defined(_MSC_VER) || defined(__MINGW32__)
  unsigned int oldcw, cw;

  /* CHECKME */
  oldcw = _controlfp(0, 0);

  #ifdef __MINGW32__
  _controlfp(_PC_53, _MCW_PC);
  #else
  _controlfp(_PC_53, MCW_PC);
  #endif

  return (unsigned long long) oldcw;

  #elif defined(__SUNPRO_C)  /* Sun Studio  */
  unsigned short oldcw, cw;

  __asm__ ("movw    $639, -22(%ebp)");
  __asm__ ("fldcw -22(%ebp)");

  return (unsigned long long) oldcw;
  #elif !defined(__APPLE__) /* GCC, clang */
  /* x87 FPU never used on OS X */
  unsigned short oldcw, cw;

    /* save old state */
  _FPU_GETCW(oldcw);
  /* Set FPU flags to use double, not double extended,
     with rounding to nearest */
  cw = (_FPU_DEFAULT & ~_FPU_EXTENDED)|_FPU_DOUBLE;
  _FPU_SETCW(cw);
  return (unsigned long long) oldcw;
  #else
  return 0;
  #endif /* defined(_MSC_VER) || defined(__MINGW32__) */
#else /* */
  return 0;
#endif /* MW_IS_X86 */
}

