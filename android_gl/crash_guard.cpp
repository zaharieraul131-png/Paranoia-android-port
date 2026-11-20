#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <unwind.h>
#include <cxxabi.h>
#include <ucontext.h>
#include <android/log.h>

#define LOG_TAG "ParanoiaGuard"
#define CRASH_LOG_PATH "/storage/emulated/0/xash/paranoia_crash.txt"

struct BacktraceState {
    void** current;
    void** end;
};

static _Unwind_Reason_Code unwindCallback(struct _Unwind_Context* context, void* arg) {
    BacktraceState* state = static_cast<BacktraceState*>(arg);
    uintptr_t pc = _Unwind_GetIP(context);
    if (pc) {
        if (state->current == state->end) {
            return _URC_END_OF_STACK;
        }
        *state->current++ = reinterpret_cast<void*>(pc);
    }
    return _URC_NO_REASON;
}

static void writeCrashReport(int sig, siginfo_t* info, void* ucontext) {
    FILE* f = fopen(CRASH_LOG_PATH, "w");
    auto logLine = [&](const char* msg) {
        __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, "%s", msg);
        if (f) {
            fprintf(f, "%s\n", msg);
            fflush(f);
        }
    };

    char buf[512];
    logLine("=== PARANOIA NATIVE DIAGNOSTIC CRASH REPORT ===");
    snprintf(buf, sizeof(buf), "Signal: %d (%s) | Code: %d | Fault Addr: %p",
             sig, strsignal(sig), info ? info->si_code : 0, info ? info->si_addr : nullptr);
    logLine(buf);

#if defined(__aarch64__)
    if (ucontext) {
        ucontext_t* uc = static_cast<ucontext_t*>(ucontext);
        snprintf(buf, sizeof(buf), "ARM64 Registers: PC=0x%llx | LR=0x%llx | SP=0x%llx | X0=0x%llx",
                 (unsigned long long)uc->uc_mcontext.pc,
                 (unsigned long long)uc->uc_mcontext.regs[30],
                 (unsigned long long)uc->uc_mcontext.sp,
                 (unsigned long long)uc->uc_mcontext.regs[0]);
        logLine(buf);
    }
#endif

    void* buffer[48];
    BacktraceState state = {buffer, buffer + 48};
    _Unwind_Backtrace(unwindCallback, &state);
    size_t count = state.current - buffer;

    logLine("--- Demangled Call Stack ---");
    for (size_t idx = 0; idx < count; ++idx) {
        const void* addr = buffer[idx];
        const char* symbol = "<unknown>";
        const char* libname = "<unknown>";
        uintptr_t offset = 0;
        char* demangled = nullptr;

        Dl_info dlinfo;
        if (dladdr(addr, &dlinfo)) {
            if (dlinfo.dli_fname) libname = dlinfo.dli_fname;
            if (dlinfo.dli_fbase) offset = (uintptr_t)addr - (uintptr_t)dlinfo.dli_fbase;
            if (dlinfo.dli_sname) {
                int status = 0;
                demangled = abi::__cxa_demangle(dlinfo.dli_sname, nullptr, nullptr, &status);
                symbol = (status == 0 && demangled) ? demangled : dlinfo.dli_sname;
            }
        }

        snprintf(buf, sizeof(buf), "#%02zu pc 0x%lx  %s (%s)", idx, (unsigned long)offset, libname, symbol);
        logLine(buf);
        if (demangled) free(demangled);
    }

    if (f) fclose(f);
    _exit(128 + sig);
}

__attribute__((constructor))
static void installCrashGuard() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = writeCrashReport;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGSEGV, &sa, nullptr);
    sigaction(SIGABRT, &sa, nullptr);
    sigaction(SIGILL,  &sa, nullptr);
    sigaction(SIGBUS,  &sa, nullptr);

    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Native CrashGuard armed and monitoring!");
}
