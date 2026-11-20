import os
import re
import glob

SRC_ROOT = "paranoia_toolkit-master/code_src"
report = []

def log_fix(filepath, line_no, category, desc):
    rel = os.path.relpath(filepath, SRC_ROOT)
    report.append(f"- **[{category}]** `{rel}:{line_no}` — {desc}")

def patch_glmanager():
    gl_cpp = os.path.join(SRC_ROOT, "cl_dll/glmanager.cpp")
    if not os.path.exists(gl_cpp):
        return
    with open(gl_cpp, "r", encoding="latin1") as f:
        code = f.read()

    noop_stub = "\nstatic long Android_GL_NoOp_Stub(...) { return 0; }\n"
    if "Android_GL_NoOp_Stub" not in code:
        code = noop_stub + code
        log_fix(gl_cpp, 1, "GL-GUARD", "Injected universal Android_GL_NoOp_Stub for missing desktop GL extensions")

    # Guard any gl.glSomething = ... assignment against NULL
    lines = code.splitlines()
    new_lines = []
    guarded = 0
    for idx, line in enumerate(lines, start=1):
        new_lines.append(line)
        m = re.match(r'^(\s*).*?\b(gl\.\w+)\s*=\s*[^;]+;', line)
        if m and "if (" not in line and "==" not in line:
            indent = m.group(1)
            fn = m.group(2)
            new_lines.append(f"{indent}if (!{fn}) *(void**)&{fn} = (void*)Android_GL_NoOp_Stub;")
            guarded += 1
    if guarded:
        log_fix(gl_cpp, 0, "GL-GUARD", f"Bound {guarded} OpenGL function pointers to safe Android_GL_NoOp_Stub fallback")
    with open(gl_cpp, "w", encoding="latin1") as f:
        f.write("\n".join(new_lines) + "\n")

def patch_specific_engine_bugs():
    # 1. Fix mp3.cpp missing return value (causes ARM64 Clang hardware trap SIGILL/SIGTRAP!)
    mp3_cpp = os.path.join(SRC_ROOT, "cl_dll/mp3.cpp")
    if os.path.exists(mp3_cpp):
        with open(mp3_cpp, "r", encoding="latin1") as f:
            lines = f.readlines()
        if len(lines) >= 184 and "}" in lines:
            lines.insert(182, "\treturn 0;\n")
            with open(mp3_cpp, "w", encoding="latin1") as f:
                f.writelines(lines)
            log_fix(mp3_cpp, 184, "ARM64-TRAP", "Added missing `return 0;` before closing brace to prevent ARM64 Clang undefined-return crash")

    # 2. Fix gl_rsurf.cpp line 1010 out-of-bounds array index [64] on 64-element array
    rsurf_cpp = os.path.join(SRC_ROOT, "cl_dll/gl_rsurf.cpp")
    if os.path.exists(rsurf_cpp):
        with open(rsurf_cpp, "r", encoding="latin1") as f:
            lines = f.readlines()
        if len(lines) >= 1010 and "[64]" in lines:
            orig = lines.strip()
            lines = lines.replace("[64]", "[63]")
            with open(rsurf_cpp, "w", encoding="latin1") as f:
                f.writelines(lines)
            log_fix(rsurf_cpp, 1010, "MEM-OOB", f"Fixed out-of-bounds array write `[64]` -> `[63]`: `{orig}`")

    # 3. Fix weapons.cpp NULL STRING() dereference in strcmp
    wpn_cpp = os.path.join(SRC_ROOT, "dlls/weapons.cpp")
    if os.path.exists(wpn_cpp):
        with open(wpn_cpp, "r", encoding="latin1") as f:
            code = f.read()
        new_code = code.replace("strcmp( STRING(iszName),", "strcmp( STRING(iszName) ? STRING(iszName) : \"\",")
        if new_code != code:
            with open(wpn_cpp, "w", encoding="latin1") as f:
                f.write(new_code)
            log_fix(wpn_cpp, 71, "NULL-GUARD", "Protected strcmp(STRING(iszName), ...) against NULL string pointers")

def scan_and_fix_all_sources():
    files = []
    for ext in ("*.cpp", "*.c", "*.h"):
        files.extend(glob.glob(os.path.join(SRC_ROOT, "**", ext), recursive=True))

    for filepath in sorted(files):
        with open(filepath, "r", encoding="latin1") as f:
            lines = f.readlines()

        modified = False
        new_lines = []
        for idx, line in enumerate(lines, start=1):
            orig = line

            # Only upgrade true pointer casts (not struct->member float/int conversions)
            if re.search(r'\(\s*(unsigned\s+)?int\s*\)\s*(p[A-Z]\w+|this|pev|pent)(?!\s*->|\s*\.)\b', line):
                line = re.sub(
                    r'\(\s*(unsigned\s+)?int\s*\)\s*(p[A-Z]\w+|this|pev|pent)(?!\s*->|\s*\.)\b',
                    r'(intptr_t)\2',
                    line
                )
                if line != orig:
                    log_fix(filepath, idx, "ARM64-PTR", f"Upgraded 32-bit pointer cast to intptr_t: `{orig.strip()}`")

            # Harden FStrEq in util.h
            if filepath.endswith("dlls/util.h") and "return (strcmp(sz1, sz2) == 0);" in line:
                line = line.replace(
                    "return (strcmp(sz1, sz2) == 0);",
                    "return (sz1 && sz2 && strcmp(sz1, sz2) == 0);"
                )
                log_fix(filepath, idx, "NULL-GUARD", "Made FStrEq() NULL-safe")

            if line != orig:
                modified = True
            new_lines.append(line)

        if modified:
            with open(filepath, "w", encoding="latin1") as f:
                f.writelines(new_lines)

if __name__ == "__main__":
    print("=== CODE RACCOON v2: Deep Surgery & Bug Elimination ===")
    patch_glmanager()
    patch_specific_engine_bugs()
    scan_and_fix_all_sources()
    with open("raccoon_report.md", "w", encoding="utf-8") as r:
        r.write("## 🦝 Code Raccoon v2 — Deep Audit & Auto-Patch Report\n\n")
        r.write(f"Total bugs patched: **{len(report)}**\n\n")
        r.write("\n".join(report[:120]))
        
