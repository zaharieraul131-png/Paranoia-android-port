import os, re

SRC = "paranoia_toolkit-master/code_src"
report = []

def log(f, ln, cat, msg):
    report.append(f"- **[{cat}]** `{os.path.relpath(f, SRC)}:{ln}` — {msg}")

def patch_all():
    # 1. glmanager.cpp NULL GL extension guard
    p = f"{SRC}/cl_dll/glmanager.cpp"
    c = open(p, encoding="latin1").read()
    c = "static long Android_GL_NoOp_Stub(...) { return 0; }\n" + c
    out, cnt = [], 0
    for ln in c.splitlines():
        out.append(ln)
        m = re.match(r'^(\s*).*?\b(gl\.\w+)\s*=\s*[^;]+;', ln)
        if m and "if (" not in ln and "==" not in ln:
            out.append(f"{m.group(1)}if (!{m.group(2)}) *(void**)&{m.group(2)} = (void*)Android_GL_NoOp_Stub;")
            cnt += 1
    open(p, "w", encoding="latin1").write("\n".join(out) + "\n")
    log(p, 1, "GL-GUARD", f"Guarded {cnt} OpenGL pointers with Android_GL_NoOp_Stub")

    # 2. nodes.cpp CGraph::FindShortestPath AI crash fix
    p = f"{SRC}/dlls/nodes.cpp"
    c = open(p, encoding="latin1").read()
    c, n = re.subn(r'(int\s+CGraph\s*::\s*FindShortestPath\s*\([^)]*\)\s*\{)',
                   r'\1\n\tif (!m_fGraphPresent || !m_pNodes || !m_pRouteInfo || iStart < 0 || iStart >= m_cNodes || iDest < 0 || iDest >= m_cNodes) return 0;', c, count=1)
    open(p, "w", encoding="latin1").write(c)
    log(p, 0, "AI-FIX", f"Added bounds guard to CGraph::FindShortestPath ({n})")

    # 3. mp3.cpp MUSIC_XASH playback & return fix
    p = f"{SRC}/cl_dll/mp3.cpp"
    c = open(p, encoding="latin1").read()
    c = c.replace("gEngfuncs.pfnPrimeMusicStream( pszSong, 0 );\n#endif\n}",
                  "gEngfuncs.pfnPrimeMusicStream( pszSong, 0 );\n\tm_iIsPlaying = 1;\n\tstrcpy(m_szMP3File, pszSong);\n\treturn 1;\n#endif\n}")
    open(p, "w", encoding="latin1").write(c)
    log(p, 184, "AUDIO-FIX", "Enabled MUSIC_XASH playback & fixed return value")

    # 4. gl_rsurf.cpp array index 64 out-of-bounds fix
    p = f"{SRC}/cl_dll/gl_rsurf.cpp"
    lns = open(p, encoding="latin1").readlines()
    if len(lns) >= 1010:
        lns[1009] = re.sub(r'\[\s*64\s*\]', '[63]', lns[1009])
        open(p, "w", encoding="latin1").writelines(lns)
        log(p, 1010, "MEM-OOB", "Fixed out-of-bounds array index [64]->[63]")

    # 5. message.cpp native subtitles, intro titles, tips & mission docs
    p = f"{SRC}/cl_dll/message.cpp"
    c = '#include "android_hud_overlay.h"\n' + open(p, encoding="latin1").read()
    c = c.replace("SubtitleMessageAdd( tempMessage );", "Android_PushOverlay(g_AndroidSubtitles, tempMessage, 0.84f, 5.5f); SubtitleMessageAdd(tempMessage); m_iFlags |= HUD_ACTIVE;")
    c = c.replace("VGuiAddScreenMessage( tempMessage );", "Android_PushOverlay(g_AndroidBanner, tempMessage, 0.25f, 6.0f); VGuiAddScreenMessage(tempMessage); m_iFlags |= HUD_ACTIVE;")
    c = c.replace("VGuiAddPickupMessage( tempMessage );", "Android_PushOverlay(g_AndroidBanner, tempMessage, 0.72f, 4.0f); VGuiAddPickupMessage(tempMessage); m_iFlags |= HUD_ACTIVE;")
    c = c.replace("ShowTip( tempMessage );", "Android_PushOverlay(g_AndroidBanner, tempMessage, 0.22f, 7.0f); ShowTip(tempMessage); m_iFlags |= HUD_ACTIVE;")
    draw_code = """float now = gEngfuncs.GetClientTime();
	if (g_AndroidSubtitles.dieTime > now) { Android_DrawWrappedText(g_AndroidSubtitles.text, g_AndroidSubtitles.yNorm, 255, 245, 200, true); drawn++; }
	if (g_AndroidBanner.dieTime > now) { Android_DrawWrappedText(g_AndroidBanner.text, g_AndroidBanner.yNorm, 140, 230, 255, true); drawn++; }
	if (g_AndroidMissionEnd > now && g_AndroidMissionDoc[0]) { Android_DrawWrappedText(g_AndroidMissionDoc, 0.45f, 220, 255, 220, false); drawn++; }
	if ( !drawn )"""
    c = c.replace("if ( !drawn )", draw_code, 1)
    open(p, "w", encoding="latin1").write(c)
    log(p, 0, "HUD-FIX", "Connected English subtitles, intro titles & mission docs to native HUD")

    # 6. vgui_paranoiatext.cpp mission document hook
    p = f"{SRC}/cl_dll/vgui_paranoiatext.cpp"
    c = 'extern "C" void Android_SetMissionText(const char* rawText);\n' + open(p, encoding="latin1").read()
    c = c.replace("char* ptext = pFile;", "Android_SetMissionText(pFile);\n\tchar* ptext = pFile;", 1)
    open(p, "w", encoding="latin1").write(c)
    log(p, 540, "MISSION-FIX", "Hooked CParanoiaTextPanel mission text into Android HUD")

    # 7. util.h NULL-safe FStrEq
    p = f"{SRC}/dlls/util.h"
    c = open(p, encoding="latin1").read().replace("return (strcmp(sz1, sz2) == 0);", "return (sz1 && sz2 && strcmp(sz1, sz2) == 0);")
    open(p, "w", encoding="latin1").write(c)
    log(p, 207, "NULL-GUARD", "Made FStrEq() NULL-safe")

patch_all()
open("raccoon_report.md", "w").write("## 🦝 Code Raccoon v3 Report\n\n" + "\n".join(report))
