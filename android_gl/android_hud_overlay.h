#ifndef ANDROID_HUD_OVERLAY_H
#define ANDROID_HUD_OVERLAY_H

struct AndroidOverlayMsg {
    char text[2048];
    float dieTime;
    float yNorm;
    int r, g, b;
};

static AndroidOverlayMsg g_AndroidSubtitles = {{0}, 0, 0.84f, 255, 245, 200};
static AndroidOverlayMsg g_AndroidBanner = {{0}, 0, 0.24f, 140, 230, 255};
static char g_AndroidMissionDoc[4096] = {0};
static float g_AndroidMissionEnd = 0.0f;

extern "C" inline void Android_SetMissionText(const char* rawText) {
    if (!rawText) return;
    int out = 0;
    bool inTag = false;
    for (int i = 0; rawText[i] && out < 4090; i++) {
        if (rawText[i] == '<') { inTag = true; continue; }
        if (rawText[i] == '>') { inTag = false; g_AndroidMissionDoc[out++] = ' '; continue; }
        if (!inTag) g_AndroidMissionDoc[out++] = rawText[i];
    }
    g_AndroidMissionDoc[out] = 0;
    g_AndroidMissionEnd = gEngfuncs.GetClientTime() + 12.0f;
}

static inline void Android_PushOverlay(AndroidOverlayMsg& slot, client_textmessage_t* msg, float yPos, float defHold) {
    if (!msg || !msg->pMessage) return;
    strncpy(slot.text, msg->pMessage, sizeof(slot.text) - 1);
    slot.text[sizeof(slot.text) - 1] = 0;
    float hold = msg->holdtime > 0.5f ? msg->holdtime : defHold;
    slot.dieTime = gEngfuncs.GetClientTime() + hold + msg->fadein + msg->fadeout;
    slot.yNorm = yPos;
    slot.r = msg->r1 ? msg->r1 : 255;
    slot.g = msg->g1 ? msg->g1 : 255;
    slot.b = msg->b1 ? msg->b1 : 255;
}

static inline void Android_DrawWrappedText(const char* text, float yNorm, int r, int g, int b, bool centerBox) {
    if (!text || !text[0]) return;
    int maxChars = (ScreenWidth > 800) ? 72 : 48;
    int lineH = (gHUD.m_scrinfo.iCharHeight > 0) ? gHUD.m_scrinfo.iCharHeight + 4 : 22;
    char lines[24][96];
    int numLines = 0;
    const char* p = text;
    while (*p && numLines < 24) {
        while (*p == '\r' || *p == '\n') p++;
        if (!*p) break;
        int len = 0, lastSpace = -1;
        while (p[len] && p[len] != '\n' && p[len] != '\r' && len < maxChars) {
            if (p[len] == ' ') lastSpace = len;
            len++;
        }
        if (p[len] && p[len] != '\n' && p[len] != '\r' && lastSpace > 10) len = lastSpace;
        memcpy(lines[numLines], p, len);
        lines[numLines][len] = 0;
        numLines++;
        p += len;
        if (*p == ' ') p++;
    }
    int startY = (int)(ScreenHeight * yNorm) - (numLines * lineH) / 2;
    if (startY < 40) startY = 40;
    if (startY + numLines * lineH > ScreenHeight - 30) startY = ScreenHeight - 30 - numLines * lineH;
    for (int i = 0; i < numLines; i++) {
        int charW = (gHUD.m_scrinfo.charWidths['A'] > 0) ? gHUD.m_scrinfo.charWidths['A'] : 10;
        int x = centerBox ? (ScreenWidth - (int)strlen(lines[i]) * charW) / 2 : (int)(ScreenWidth * 0.08f);
        if (x < 20) x = 20;
        gEngfuncs.pfnDrawSetTextColor(r / 255.0f, g / 255.0f, b / 255.0f);
        gEngfuncs.pfnDrawConsoleString(x, startY + i * lineH, lines[i]);
    }
}

#endif
