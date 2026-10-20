#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iphlpapi.h>
#include <netioapi.h>
#include <objidl.h>
#include <dwmapi.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>
using std::max;
using std::min;
#include <gdiplus.h>

struct Rgba {
    int r, g, b, a;
};

struct Curve {
    double x1, y1, x2, y2;
};

struct Anim {
    Curve c;
    double dur;
};

struct Named {
    std::string name;
    Curve c;
};

enum Disp { D_EXEC, D_KILL, D_FLOAT, D_EXIT, D_WS, D_MOVEWS, D_FOCUS, D_SWAP, D_RELOAD, D_WALL };

struct Bind {
    unsigned mods;
    DWORD vk;
    Disp d;
    std::string arg;
};

struct Rule {
    std::string kind, target;
};

struct ModDef {
    std::string name, type, format, command;
    int interval = 5;
};

struct WidgetDef {
    std::string name;
    int x, y, w, h, px;
};

struct Config {
    int gapsIn = 6, gapsOut = 12;
    std::string material = "blur";
    double activeOp = 0.88, inactiveOp = 0.75;
    int blurSize = 12, blurPasses = 3;
    Rgba tint = {12, 14, 24, 110};
    bool barEnabled = true;
    int barHeight = 30;
    Rgba accent = {51, 204, 255, 255};
    bool animEnabled = true;
    bool wEn = true, sEn = true;
    double wSpeed = 4.5, sSpeed = 5.0;
    std::string wCurve = "myBezier", sCurve = "default";
    std::vector<Named> curves = {{"default", {0.23, 1, 0.32, 1}}, {"myBezier", {0.05, 0.9, 0.1, 1.05}}};
    std::vector<Bind> binds;
    std::vector<Rule> rules;
    std::vector<std::string> execOnce;
    int borderSize = 2, rounding = 10, actAngle = 45;
    Rgba actA = {51, 204, 255, 238}, actB = {0, 255, 153, 238}, inact = {89, 89, 89, 170};
    std::vector<std::string> matExceptions;
    bool barClock = true, barVolume = true;
    bool accentFromWall = true, actFromWall = true;
    std::vector<std::string> barRight = {"cpu", "ram", "volume", "clock"};
    std::vector<ModDef> mods;
    std::vector<WidgetDef> widgets;
    bool altTab = true, wpApplySystem = true;
    std::string wpPath, wpMode = "fill", wpFolder;
    Rgba wpColor = {16, 16, 24, 255};
    int wpInterval = 0;
    std::string clockFormat = "%a %H:%M";
};

static const char* kDefaultConf = R"CONF(# hypr.conf
# material: none | blur | acrylic | mica (mica needs Windows 11, falls back to blur)

general {
    gaps_in = 6
    gaps_out = 12
    border_size = 2
    col.active_border = wallpaper 45deg
    col.inactive_border = rgba(595959aa)
    alt_tab = true
}

decoration {
    material = blur
    active_opacity = 0.88
    inactive_opacity = 0.75
    rounding = 10
    exceptions_material_on_app = {}
    blur {
        size = 12
        passes = 3
        tint = rgba(0c0e186e)
    }
}

bar {
    enabled = true
    height = 30
    accent = wallpaper
    modules_right = cpu, ram, volume, clock
}

module = clock, format, %a %d %b  %H:%M
# module = temp, type, exec
# module = temp, command, echo hello
# module = temp, interval, 10
# widget = clock, 60, 60, 300, 120, 56
# widget = cpu, -60, 60, 220, 90, 32

wallpaper {
    mode = fill
    color = rgb(101018)
    interval = 0
    apply_system = true
    # path = C:\Users\you\Pictures\wall.jpg
    # folder = C:\Users\you\Pictures\wallpapers
}

animations {
    enabled = true
    bezier = default, 0.23, 1, 0.32, 1
    bezier = myBezier, 0.05, 0.9, 0.1, 1.05
    animation = windows, 1, 4.5, myBezier
    animation = workspaces, 1, 5, default
}

# windowrule = float, calc
# windowrule = noblur, chrome
# windowrule = tile, someapp
# windowrule = noborder, mpv
# windowrule = norounding, mpv
# exec-once = wt.exe

$mainMod = SUPER

bind = $mainMod, Q, exec, wt.exe
bind = $mainMod, E, exec, explorer.exe shell:MyComputerFolder
bind = $mainMod, C, killactive
bind = $mainMod, V, togglefloating
bind = $mainMod, M, exit
bind = $mainMod SHIFT, R, reload
bind = $mainMod, W, wallpaper, next

bind = $mainMod, left, movefocus, l
bind = $mainMod, right, movefocus, r
bind = $mainMod, up, movefocus, u
bind = $mainMod, down, movefocus, d
bind = $mainMod SHIFT, left, swapwindow, l
bind = $mainMod SHIFT, right, swapwindow, r
bind = $mainMod SHIFT, up, swapwindow, u
bind = $mainMod SHIFT, down, swapwindow, d

bind = $mainMod, 1, workspace, 1
bind = $mainMod, 2, workspace, 2
bind = $mainMod, 3, workspace, 3
bind = $mainMod, 4, workspace, 4
bind = $mainMod, 5, workspace, 5
bind = $mainMod, 6, workspace, 6
bind = $mainMod, 7, workspace, 7
bind = $mainMod, 8, workspace, 8
bind = $mainMod, 9, workspace, 9
bind = $mainMod SHIFT, 1, movetoworkspace, 1
bind = $mainMod SHIFT, 2, movetoworkspace, 2
bind = $mainMod SHIFT, 3, movetoworkspace, 3
bind = $mainMod SHIFT, 4, movetoworkspace, 4
bind = $mainMod SHIFT, 5, movetoworkspace, 5
bind = $mainMod SHIFT, 6, movetoworkspace, 6
bind = $mainMod SHIFT, 7, movetoworkspace, 7
bind = $mainMod SHIFT, 8, movetoworkspace, 8
bind = $mainMod SHIFT, 9, movetoworkspace, 9
)CONF";

static const int NUM_WS = 9;
static const int PARK_X = -30000;
static const double POPIN = 0.87;
static const int BLUR_DIV = 2;
static const unsigned MOD_SUPER_ = 1, MOD_SHIFT_ = 2, MOD_CTRL_ = 4, MOD_ALT_ = 8;
static const wchar_t* OV_CLASS = L"hypr_ov";
static const wchar_t* BAR_CLASS = L"hypr_bar";

struct Win {
    HWND hwnd;
    HWND ov;
    HICON icon;
    bool floating;
    bool fresh;
    bool parked;
    bool anim;
    bool park;
    bool noBlur;
    bool layeredByUs;
    bool accentOn;
    bool micaOn;
    bool noBorder;
    bool noRound;
    bool rounded;
    int rgnW, rgnH;
    int ws;
    int enterDir;
    RECT cur, from, to, floatRect, margin;
    double t0;
    Anim a;
    std::string exe, cls;
};

struct BarItem {
    int kind;
    RECT r;
    int ws;
    HWND hwnd;
    bool active;
    bool occupied;
    HICON icon;
    std::wstring text;
    bool muted;
    std::wstring glyph;
};

static Config g_cfg;
static Anim g_animW = {{0.05, 0.9, 0.1, 1.05}, 0.45};
static Anim g_animS = {{0.23, 1.0, 0.32, 1.0}, 0.5};
static std::vector<Win> g_wins;
static HWND g_lastFocus[NUM_WS];
static HWND g_active;
static int g_ws;
static HWINEVENTHOOK g_eventHook;
static HHOOK g_kbHook;
static unsigned g_mods;
static bool g_winUsed;
static bool g_swallowed[256];
static bool g_win11;
static std::string g_sig;
static FILETIME g_cfgTime;
static HBITMAP g_blurBmp;
static HDC g_blurDC;
static uint32_t* g_bits;
static int g_bw, g_bh;
static HWND g_bar;
static int g_barReserve;
static std::vector<BarItem> g_items;
static ULONG_PTR g_gdiToken;
static bool g_shellSession;
static HWND g_border, g_wall;
static HDC g_bdc, g_wallDC;
static HBITMAP g_bbmp, g_wallBmp;
static uint32_t* g_bbits;
static uint32_t* g_wallBits;
static Gdiplus::Bitmap* g_bgBmp;
static Gdiplus::Graphics* g_bgGfx;
static RECT g_bdirty;
static const wchar_t* BORDER_CLASS = L"hypr_border";
static const wchar_t* WALL_CLASS = L"hypr_wall";
static const wchar_t* SW_CLASS = L"hypr_switch";
static std::wstring g_wpCur;
static std::vector<std::wstring> g_wpList;
static size_t g_wpIdx;
static ULONGLONG g_wpTick;
static HWND g_sw;
static std::vector<HWND> g_mru, g_swList;
static std::vector<HTHUMBNAIL> g_swThumbs;
static int g_swSel;
static bool g_swOpen;
static bool g_altUsed, g_altTab;
static Rgba g_dynA = {51, 204, 255, 238}, g_dynB = {0, 255, 153, 238};
static bool g_dynOk, g_fs, g_expAlive = true;
static HWND g_lastKill;
static ULONGLONG g_lastKillTick;
static const wchar_t* WIDGET_CLASS = L"hypr_widget";
static IAudioEndpointVolume* g_epv;
static float g_volLevel = -1.0f;
static BOOL g_volMute;
static HFONT g_font, g_glyph;

static std::string Trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

static std::string Lower(std::string s) {
    for (auto& c : s) c = (char)tolower((unsigned char)c);
    return s;
}

static std::vector<std::string> Split(const std::string& s, char d, size_t maxParts = 0) {
    std::vector<std::string> out;
    size_t start = 0;
    for (;;) {
        size_t p = s.find(d, start);
        if (p == std::string::npos || (maxParts && out.size() + 1 == maxParts)) {
            out.push_back(Trim(s.substr(start)));
            break;
        }
        out.push_back(Trim(s.substr(start, p - start)));
        start = p + 1;
    }
    return out;
}

static bool ParseBool(const std::string& v) {
    std::string l = Lower(v);
    return l == "true" || l == "yes" || l == "on" || l == "1";
}

static Rgba ParseColor(const std::string& in, Rgba def) {
    std::string s = Trim(in);
    auto hex = [](const std::string& h) { return strtoul(h.c_str(), nullptr, 16); };
    size_t close = s.find(')');
    if (s.rfind("rgba(", 0) == 0 && close != std::string::npos) {
        unsigned long v = hex(s.substr(5, close - 5));
        return {(int)((v >> 24) & 255), (int)((v >> 16) & 255), (int)((v >> 8) & 255), (int)(v & 255)};
    }
    if (s.rfind("rgb(", 0) == 0 && close != std::string::npos) {
        unsigned long v = hex(s.substr(4, close - 4));
        return {(int)((v >> 16) & 255), (int)((v >> 8) & 255), (int)(v & 255), 255};
    }
    if (s.rfind("0x", 0) == 0) {
        unsigned long v = hex(s.substr(2));
        return {(int)((v >> 16) & 255), (int)((v >> 8) & 255), (int)(v & 255), (int)((v >> 24) & 255)};
    }
    return def;
}

static DWORD KeyFromName(const std::string& n) {
    std::string k = Lower(n);
    if (k.size() == 1) {
        char c = k[0];
        if (c >= 'a' && c <= 'z') return (DWORD)(c - 'a' + 'A');
        if (c >= '0' && c <= '9') return (DWORD)c;
    }
    if (k.size() >= 2 && k[0] == 'f') {
        int n2 = atoi(k.c_str() + 1);
        if (n2 >= 1 && n2 <= 12) return (DWORD)(VK_F1 + n2 - 1);
    }
    struct P {
        const char* n;
        DWORD v;
    };
    static const P tbl[] = {{"left", VK_LEFT},      {"right", VK_RIGHT},   {"up", VK_UP},
                            {"down", VK_DOWN},      {"return", VK_RETURN}, {"enter", VK_RETURN},
                            {"space", VK_SPACE},    {"tab", VK_TAB},       {"escape", VK_ESCAPE},
                            {"esc", VK_ESCAPE},     {"backspace", VK_BACK}, {"delete", VK_DELETE},
                            {"minus", VK_OEM_MINUS}, {"equal", VK_OEM_PLUS}, {"comma", VK_OEM_COMMA},
                            {"period", VK_OEM_PERIOD}, {"slash", VK_OEM_2}, {"semicolon", VK_OEM_1},
                            {"grave", VK_OEM_3},    {"print", VK_SNAPSHOT}};
    for (auto& p : tbl)
        if (k == p.n) return p.v;
    return 0;
}

static unsigned ParseMods(const std::string& s) {
    unsigned m = 0;
    std::string cur;
    std::string all = s + " ";
    for (char c : all) {
        if (c == ' ' || c == '_' || c == '+') {
            std::string t = Lower(cur);
            if (t == "super" || t == "win" || t == "mod4") m |= MOD_SUPER_;
            else if (t == "shift") m |= MOD_SHIFT_;
            else if (t == "ctrl" || t == "control") m |= MOD_CTRL_;
            else if (t == "alt") m |= MOD_ALT_;
            cur.clear();
        } else {
            cur += c;
        }
    }
    return m;
}

static void ApplyKey(Config& c, const std::string& full, const std::string& val) {
    size_t lc = full.rfind(':');
    std::string last = lc == std::string::npos ? full : full.substr(lc + 1);
    if (last == "bezier") {
        auto p = Split(val, ',');
        if (p.size() < 5) return;
        Named n{p[0], {atof(p[1].c_str()), atof(p[2].c_str()), atof(p[3].c_str()), atof(p[4].c_str())}};
        for (auto& e : c.curves)
            if (e.name == n.name) {
                e.c = n.c;
                return;
            }
        c.curves.push_back(n);
        return;
    }
    if (last == "animation") {
        auto p = Split(val, ',', 4);
        if (p.size() < 3) return;
        bool en = atoi(p[1].c_str()) != 0;
        double sp = atof(p[2].c_str());
        std::string cv = p.size() > 3 ? p[3] : "default";
        if (p[0] == "windows") {
            c.wEn = en;
            c.wSpeed = sp;
            c.wCurve = cv;
        } else if (p[0] == "workspaces") {
            c.sEn = en;
            c.sSpeed = sp;
            c.sCurve = cv;
        }
        return;
    }
    if (last == "bind") {
        auto p = Split(val, ',', 4);
        if (p.size() < 3) return;
        Bind b{};
        b.mods = ParseMods(p[0]);
        b.vk = KeyFromName(p[1]);
        std::string d = Lower(p[2]);
        b.arg = p.size() > 3 ? p[3] : "";
        if (d == "exec") b.d = D_EXEC;
        else if (d == "killactive") b.d = D_KILL;
        else if (d == "togglefloating") b.d = D_FLOAT;
        else if (d == "exit") b.d = D_EXIT;
        else if (d == "workspace") b.d = D_WS;
        else if (d == "movetoworkspace") b.d = D_MOVEWS;
        else if (d == "movefocus") b.d = D_FOCUS;
        else if (d == "swapwindow") b.d = D_SWAP;
        else if (d == "reload") b.d = D_RELOAD;
        else if (d == "wallpaper") b.d = D_WALL;
        else return;
        if (b.vk) c.binds.push_back(b);
        return;
    }
    if (last == "windowrule") {
        auto p = Split(val, ',', 2);
        if (p.size() < 2) return;
        std::string t = Lower(p[1]);
        if (t.rfind("class:", 0) == 0) t = t.substr(6);
        c.rules.push_back({Lower(p[0]), t});
        return;
    }
    if (last == "module") {
        auto p = Split(val, ',', 3);
        if (p.size() < 3) return;
        std::string n = Lower(p[0]), k = Lower(p[1]);
        ModDef* d = nullptr;
        for (auto& e : c.mods)
            if (e.name == n) d = &e;
        if (!d) {
            c.mods.push_back(ModDef());
            d = &c.mods.back();
            d->name = n;
        }
        if (k == "type") d->type = Lower(p[2]);
        else if (k == "format") d->format = p[2];
        else if (k == "command") d->command = p[2];
        else if (k == "interval") d->interval = atoi(p[2].c_str());
        return;
    }
    if (last == "widget") {
        auto p = Split(val, ',');
        if (p.size() < 6) return;
        WidgetDef w{Lower(p[0]), atoi(p[1].c_str()), atoi(p[2].c_str()), atoi(p[3].c_str()), atoi(p[4].c_str()),
                    atoi(p[5].c_str())};
        if (w.w > 0 && w.h > 0 && w.px > 0) c.widgets.push_back(w);
        return;
    }
    if (last == "exec-once") {
        c.execOnce.push_back(val);
        return;
    }
    if (last == "exceptions_material_on_app") {
        std::string v = val;
        size_t a = v.find('{'), b = v.rfind('}');
        if (a != std::string::npos) v = v.substr(a + 1, b == std::string::npos || b < a ? std::string::npos : b - a - 1);
        for (auto& e : Split(v, ',')) {
            std::string n = Lower(e);
            if (n.size() > 4 && n.compare(n.size() - 4, 4, ".exe") == 0) n.resize(n.size() - 4);
            if (!n.empty()) c.matExceptions.push_back(n);
        }
        return;
    }
    if (full == "general:col.active_border") {
        std::vector<std::string> tk;
        size_t st = 0;
        while (st < val.size()) {
            size_t e = val.find(' ', st);
            if (e == std::string::npos) e = val.size();
            std::string t = Trim(val.substr(st, e - st));
            if (!t.empty()) tk.push_back(t);
            st = e + 1;
        }
        int n = 0;
        c.actFromWall = false;
        for (auto& t : tk) {
            if (Lower(t) == "wallpaper") c.actFromWall = true;
            else if (t.size() > 3 && t.compare(t.size() - 3, 3, "deg") == 0) c.actAngle = atoi(t.c_str());
            else if (n == 0) c.actA = c.actB = ParseColor(t, c.actA), n++;
            else if (n == 1) c.actB = ParseColor(t, c.actB), n++;
        }
        return;
    }
    if (full == "general:col.inactive_border") {
        c.inact = ParseColor(val, c.inact);
        return;
    }
    if (full == "general:gaps_in") c.gapsIn = atoi(val.c_str());
    else if (full == "general:gaps_out") c.gapsOut = atoi(val.c_str());
    else if (full == "general:border_size") c.borderSize = atoi(val.c_str());
    else if (full == "decoration:rounding") c.rounding = atoi(val.c_str());
    else if (full == "decoration:material") c.material = Lower(val);
    else if (full == "decoration:active_opacity") c.activeOp = atof(val.c_str());
    else if (full == "decoration:inactive_opacity") c.inactiveOp = atof(val.c_str());
    else if (full == "decoration:blur:size") c.blurSize = atoi(val.c_str());
    else if (full == "decoration:blur:passes") c.blurPasses = atoi(val.c_str());
    else if (full == "decoration:blur:tint") c.tint = ParseColor(val, c.tint);
    else if (full == "bar:enabled") c.barEnabled = ParseBool(val);
    else if (full == "bar:height") c.barHeight = atoi(val.c_str());
    else if (full == "bar:accent") {
        if (Lower(val).rfind("wallpaper", 0) == 0) {
            c.accentFromWall = true;
        } else {
            c.accentFromWall = false;
            c.accent = ParseColor(val, c.accent);
        }
    } else if (full == "bar:modules_right") {
        c.barRight.clear();
        for (auto& e : Split(val, ',')) {
            std::string n = Lower(e);
            if (!n.empty()) c.barRight.push_back(n);
        }
    }
    else if (full == "bar:clock") c.barClock = ParseBool(val);
    else if (full == "bar:clock_format") c.clockFormat = val;
    else if (full == "bar:volume") c.barVolume = ParseBool(val);
    else if (full == "general:alt_tab") c.altTab = ParseBool(val);
    else if (full == "wallpaper:path") c.wpPath = val;
    else if (full == "wallpaper:mode") c.wpMode = Lower(val);
    else if (full == "wallpaper:color") c.wpColor = ParseColor(val, c.wpColor);
    else if (full == "wallpaper:folder") c.wpFolder = val;
    else if (full == "wallpaper:interval") c.wpInterval = atoi(val.c_str());
    else if (full == "wallpaper:apply_system") c.wpApplySystem = ParseBool(val);
    else if (full == "animations:enabled") c.animEnabled = ParseBool(val);
}

static void ParseConfig(const std::string& text, Config& c) {
    std::vector<std::string> sect;
    std::vector<std::pair<std::string, std::string>> vars;
    size_t pos = 0;
    while (pos <= text.size()) {
        size_t e = text.find('\n', pos);
        if (e == std::string::npos) e = text.size();
        std::string line = text.substr(pos, e - pos);
        pos = e + 1;
        size_t h = line.find('#');
        if (h != std::string::npos) line = line.substr(0, h);
        line = Trim(line);
        if (line.empty()) continue;
        if (line.back() == '{' && line.find('=') == std::string::npos) {
            sect.push_back(Lower(Trim(line.substr(0, line.size() - 1))));
            continue;
        }
        if (line == "}") {
            if (!sect.empty()) sect.pop_back();
            continue;
        }
        size_t eq = line.find('=');
        if (eq == std::string::npos) {
            std::string k = Lower(line);
            if (k == "reload") continue;
            continue;
        }
        std::string key = Lower(Trim(line.substr(0, eq)));
        std::string val = Trim(line.substr(eq + 1));
        if (!val.empty() && val[0] == '{' && val.find('}') == std::string::npos) {
            while (pos <= text.size()) {
                size_t e2 = text.find('\n', pos);
                if (e2 == std::string::npos) e2 = text.size();
                std::string l2 = text.substr(pos, e2 - pos);
                pos = e2 + 1;
                size_t h2 = l2.find('#');
                if (h2 != std::string::npos) l2 = l2.substr(0, h2);
                val += " " + Trim(l2);
                if (l2.find('}') != std::string::npos) break;
            }
        }
        if (!key.empty() && key[0] == '$') {
            vars.push_back({Trim(line.substr(0, eq)), val});
            continue;
        }
        for (auto& v : vars) {
            size_t p = 0;
            while ((p = val.find(v.first, p)) != std::string::npos) {
                val.replace(p, v.first.size(), v.second);
                p += v.second.size();
            }
        }
        std::string full;
        for (auto& s : sect) full += s + ":";
        full += key;
        ApplyKey(c, full, val);
    }
}

static const Curve* FindCurve(const Config& c, const std::string& n) {
    for (auto& e : c.curves)
        if (e.name == n) return &e.c;
    return &c.curves[0].c;
}

static void Resolve(Config& c) {
    bool all = c.animEnabled;
    g_animW = {*FindCurve(c, c.wCurve), (all && c.wEn) ? c.wSpeed * 0.1 : 0.0};
    g_animS = {*FindCurve(c, c.sCurve), (all && c.sEn) ? c.sSpeed * 0.1 : 0.0};
}

static double Now() {
    static LARGE_INTEGER f;
    if (!f.QuadPart) QueryPerformanceFrequency(&f);
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart / (double)f.QuadPart;
}

static double Bezier(const Curve& c, double x) {
    double lo = 0, hi = 1, t = x;
    for (int i = 0; i < 20; i++) {
        double u = 1 - t;
        double bx = 3 * u * u * t * c.x1 + 3 * u * t * t * c.x2 + t * t * t;
        if (bx < x) lo = t; else hi = t;
        t = (lo + hi) / 2;
    }
    double u = 1 - t;
    return 3 * u * u * t * c.y1 + 3 * u * t * t * c.y2 + t * t * t;
}

static RECT GetFrame(HWND h) {
    RECT r;
    if (FAILED(DwmGetWindowAttribute(h, DWMWA_EXTENDED_FRAME_BOUNDS, &r, sizeof(r)))) GetWindowRect(h, &r);
    return r;
}

static RECT Margins(HWND h) {
    RECT wr;
    GetWindowRect(h, &wr);
    RECT fr = GetFrame(h);
    return {fr.left - wr.left, fr.top - wr.top, wr.right - fr.right, wr.bottom - fr.bottom};
}

static RECT Shift(RECT r, int dx) {
    OffsetRect(&r, dx, 0);
    return r;
}

static int ScreenW() {
    return GetSystemMetrics(SM_CXSCREEN);
}

static int ScreenH() {
    return GetSystemMetrics(SM_CYSCREEN);
}

static RECT Lerp(const RECT& a, const RECT& b, double e) {
    auto f = [&](LONG x, LONG y) { return (LONG)std::lround(x + (y - x) * e); };
    return {f(a.left, b.left), f(a.top, b.top), f(a.right, b.right), f(a.bottom, b.bottom)};
}

static std::wstring Widen(const std::string& s) {
    if (s.empty()) return L"";
    int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    std::wstring w(n, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &w[0], n);
    w.resize(n - 1);
    return w;
}

static std::string Narrow(const wchar_t* w) {
    int n = WideCharToMultiByte(CP_UTF8, 0, w, -1, nullptr, 0, nullptr, nullptr);
    if (n <= 1) return "";
    std::string s(n, 0);
    WideCharToMultiByte(CP_UTF8, 0, w, -1, &s[0], n, nullptr, nullptr);
    s.resize(n - 1);
    return s;
}

static void BoxPass(const uint32_t* src, uint32_t* dst, int len, int lines, int step, int lineStep, int r) {
    int n = 2 * r + 1;
    for (int l = 0; l < lines; l++) {
        const uint32_t* s = src + (size_t)l * lineStep;
        uint32_t* d = dst + (size_t)l * lineStep;
        long sb = 0, sg = 0, sr = 0;
        for (int i = -r; i <= r; i++) {
            uint32_t p = s[(size_t)std::clamp(i, 0, len - 1) * step];
            sb += p & 255;
            sg += (p >> 8) & 255;
            sr += (p >> 16) & 255;
        }
        for (int x = 0; x < len; x++) {
            d[(size_t)x * step] = 0xFF000000u | ((uint32_t)(sr / n) << 16) | ((uint32_t)(sg / n) << 8) | (uint32_t)(sb / n);
            uint32_t a = s[(size_t)std::clamp(x + r + 1, 0, len - 1) * step];
            uint32_t b = s[(size_t)std::clamp(x - r, 0, len - 1) * step];
            sb += (long)(a & 255) - (long)(b & 255);
            sg += (long)((a >> 8) & 255) - (long)((b >> 8) & 255);
            sr += (long)((a >> 16) & 255) - (long)((b >> 16) & 255);
        }
    }
}

static std::wstring CurrentWallpaper() {
    if (!g_wpCur.empty()) return g_wpCur;
    wchar_t p[MAX_PATH] = {};
    SystemParametersInfoW(SPI_GETDESKWALLPAPER, MAX_PATH, p, 0);
    return p;
}

static void DrawWallpaper(uint32_t* bits, int bw, int bh) {
    uint32_t bg = 0xFF000000u | ((uint32_t)g_cfg.wpColor.r << 16) | ((uint32_t)g_cfg.wpColor.g << 8) |
                  (uint32_t)g_cfg.wpColor.b;
    size_t cnt = (size_t)bw * bh;
    for (size_t i = 0; i < cnt; i++) bits[i] = bg;
    std::wstring path = CurrentWallpaper();
    if (path.empty()) return;
    Gdiplus::Bitmap img(path.c_str());
    if (img.GetLastStatus() != Gdiplus::Ok || !img.GetWidth() || !img.GetHeight()) return;
    Gdiplus::Bitmap dst(bw, bh, bw * 4, PixelFormat32bppPARGB, (BYTE*)bits);
    Gdiplus::Graphics g(&dst);
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBilinear);
    double iw = img.GetWidth(), ih = img.GetHeight();
    double k = (double)bw / ScreenW();
    const std::string& m = g_cfg.wpMode;
    if (m == "stretch") {
        g.DrawImage(&img, 0.0f, 0.0f, (Gdiplus::REAL)bw, (Gdiplus::REAL)bh);
    } else if (m == "tile") {
        double tw = iw * k, th = ih * k;
        if (tw < 1 || th < 1) return;
        for (double y = 0; y < bh; y += th)
            for (double x = 0; x < bw; x += tw)
                g.DrawImage(&img, (Gdiplus::REAL)x, (Gdiplus::REAL)y, (Gdiplus::REAL)tw, (Gdiplus::REAL)th);
    } else {
        double sc = m == "center" ? k : (m == "fit" ? min((double)bw / iw, (double)bh / ih)
                                                     : max((double)bw / iw, (double)bh / ih));
        double dw = iw * sc, dh = ih * sc;
        g.DrawImage(&img, (Gdiplus::REAL)((bw - dw) / 2), (Gdiplus::REAL)((bh - dh) / 2), (Gdiplus::REAL)dw,
                    (Gdiplus::REAL)dh);
    }
}

static void RgbToHsv(double r, double g, double b, double& h, double& sat, double& v) {
    double mx = max(r, max(g, b)), mn = min(r, min(g, b));
    v = mx;
    double d = mx - mn;
    sat = mx > 0 ? d / mx : 0;
    if (d == 0) {
        h = 0;
        return;
    }
    if (mx == r) h = 60 * std::fmod((g - b) / d, 6.0);
    else if (mx == g) h = 60 * ((b - r) / d + 2);
    else h = 60 * ((r - g) / d + 4);
    if (h < 0) h += 360;
}

static void HsvToRgb(double h, double sat, double v, int& r, int& g, int& b) {
    double c = v * sat;
    double hh = std::fmod(h + 360.0, 360.0) / 60.0;
    double x = c * (1 - std::fabs(std::fmod(hh, 2.0) - 1));
    double rr = 0, gg = 0, bb = 0;
    switch ((int)hh) {
    case 0: rr = c; gg = x; break;
    case 1: rr = x; gg = c; break;
    case 2: gg = c; bb = x; break;
    case 3: gg = x; bb = c; break;
    case 4: rr = x; bb = c; break;
    default: rr = c; bb = x; break;
    }
    double m = v - c;
    r = (int)std::lround((rr + m) * 255);
    g = (int)std::lround((gg + m) * 255);
    b = (int)std::lround((bb + m) * 255);
}

static void ExtractAccent(const uint32_t* bits, int w, int h) {
    struct B {
        double w, r, g, b;
    };
    B bk[12] = {};
    size_t n = (size_t)w * h;
    size_t stride = max((size_t)1, n / 40000);
    for (size_t i = 0; i < n; i += stride) {
        uint32_t p = bits[i];
        double r = ((p >> 16) & 255) / 255.0, g = ((p >> 8) & 255) / 255.0, b = (p & 255) / 255.0;
        double hu, sa, va;
        RgbToHsv(r, g, b, hu, sa, va);
        if (va < 0.22 || sa < 0.28) continue;
        double wt = sa * sa * va;
        int k = ((int)(hu / 30)) % 12;
        bk[k].w += wt;
        bk[k].r += r * wt;
        bk[k].g += g * wt;
        bk[k].b += b * wt;
    }
    int a = -1;
    double best = 0;
    for (int k = 0; k < 12; k++)
        if (bk[k].w > best) {
            best = bk[k].w;
            a = k;
        }
    if (a < 0) {
        g_dynOk = false;
        return;
    }
    int bi = -1;
    double second = 0;
    for (int k = 0; k < 12; k++) {
        int dk = min((k - a + 12) % 12, (a - k + 12) % 12);
        if (dk >= 2 && bk[k].w > second) {
            second = bk[k].w;
            bi = k;
        }
    }
    auto fin = [&](const B& x, int& r, int& g, int& b) {
        double hu, sa, va;
        RgbToHsv(x.r / x.w, x.g / x.w, x.b / x.w, hu, sa, va);
        sa = std::clamp(sa * 1.1, 0.55, 1.0);
        va = std::clamp(va * 1.15, 0.78, 1.0);
        HsvToRgb(hu, sa, va, r, g, b);
    };
    int r1, g1, b1, r2, g2, b2;
    fin(bk[a], r1, g1, b1);
    if (bi >= 0 && second > best * 0.12) {
        fin(bk[bi], r2, g2, b2);
    } else {
        double hu, sa, va;
        RgbToHsv(r1 / 255.0, g1 / 255.0, b1 / 255.0, hu, sa, va);
        HsvToRgb(hu + 40, sa, va, r2, g2, b2);
    }
    g_dynA = {r1, g1, b1, 238};
    g_dynB = {r2, g2, b2, 238};
    g_dynOk = true;
}

static Rgba EffA() {
    return (g_cfg.actFromWall && g_dynOk) ? g_dynA : g_cfg.actA;
}

static Rgba EffB() {
    return (g_cfg.actFromWall && g_dynOk) ? g_dynB : g_cfg.actB;
}

static Rgba EffAccent() {
    if (g_cfg.accentFromWall && g_dynOk) return {g_dynA.r, g_dynA.g, g_dynA.b, 255};
    return g_cfg.accent;
}

static void FreeBlur() {
    if (g_blurDC) DeleteDC(g_blurDC);
    if (g_blurBmp) DeleteObject(g_blurBmp);
    g_blurDC = nullptr;
    g_blurBmp = nullptr;
    g_bits = nullptr;
}

static void BuildBlur() {
    FreeBlur();
    g_bw = (ScreenW() + BLUR_DIV - 1) / BLUR_DIV;
    g_bh = (ScreenH() + BLUR_DIV - 1) / BLUR_DIV;
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = g_bw;
    bi.bmiHeader.biHeight = -g_bh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    HDC sdc = GetDC(nullptr);
    g_blurBmp = CreateDIBSection(sdc, &bi, DIB_RGB_COLORS, (void**)&g_bits, nullptr, 0);
    g_blurDC = CreateCompatibleDC(sdc);
    ReleaseDC(nullptr, sdc);
    if (!g_blurBmp || !g_bits) {
        FreeBlur();
        return;
    }
    SelectObject(g_blurDC, g_blurBmp);
    size_t count = (size_t)g_bw * g_bh;
    DrawWallpaper(g_bits, g_bw, g_bh);
    ExtractAccent(g_bits, g_bw, g_bh);

    int r = g_cfg.blurSize / BLUR_DIV;
    if (r > 0 && g_cfg.blurPasses > 0) {
        std::vector<uint32_t> tmp(count);
        for (int p = 0; p < g_cfg.blurPasses; p++) {
            BoxPass(g_bits, tmp.data(), g_bw, g_bh, 1, g_bw, r);
            BoxPass(tmp.data(), g_bits, g_bh, g_bw, g_bw, 1, r);
        }
    }
    int ta = g_cfg.tint.a;
    if (ta > 0) {
        for (size_t i = 0; i < count; i++) {
            uint32_t p = g_bits[i];
            int b = (int)(p & 255), gg = (int)((p >> 8) & 255), rr = (int)((p >> 16) & 255);
            b = (b * (255 - ta) + g_cfg.tint.b * ta) / 255;
            gg = (gg * (255 - ta) + g_cfg.tint.g * ta) / 255;
            rr = (rr * (255 - ta) + g_cfg.tint.r * ta) / 255;
            g_bits[i] = 0xFF000000u | ((uint32_t)rr << 16) | ((uint32_t)gg << 8) | (uint32_t)b;
        }
    }
}

static LRESULT CALLBACK WallProc(HWND h, UINT m, WPARAM wp, LPARAM lp) {
    switch (m) {
    case WM_ERASEBKGND:
        return 1;
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(h, &ps);
        if (g_wallDC) BitBlt(dc, 0, 0, ScreenW(), ScreenH(), g_wallDC, 0, 0, SRCCOPY);
        EndPaint(h, &ps);
        return 0;
    }
    }
    return DefWindowProcW(h, m, wp, lp);
}

static void BuildWallpaper() {
    if (g_wallDC) DeleteDC(g_wallDC);
    if (g_wallBmp) DeleteObject(g_wallBmp);
    g_wallDC = nullptr;
    g_wallBmp = nullptr;
    int sw = ScreenW(), sh = ScreenH();
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = sw;
    bi.bmiHeader.biHeight = -sh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    HDC sdc = GetDC(nullptr);
    g_wallBmp = CreateDIBSection(sdc, &bi, DIB_RGB_COLORS, (void**)&g_wallBits, nullptr, 0);
    g_wallDC = CreateCompatibleDC(sdc);
    ReleaseDC(nullptr, sdc);
    if (!g_wallBmp) return;
    SelectObject(g_wallDC, g_wallBmp);
    DrawWallpaper(g_wallBits, sw, sh);
    if (!g_wall) {
        g_wall = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, WALL_CLASS, L"", WS_POPUP, 0, 0, sw, sh, nullptr,
                                 nullptr, GetModuleHandleW(nullptr), nullptr);
        SetWindowPos(g_wall, HWND_BOTTOM, 0, 0, sw, sh, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
    InvalidateRect(g_wall, nullptr, FALSE);
}

static void BlitBackdrop(HDC dst, int dx, int dy, RECT sr) {
    if (!g_blurDC) return;
    int l = max((int)sr.left, 0), t = max((int)sr.top, 0);
    int r = min((int)sr.right, ScreenW()), b = min((int)sr.bottom, ScreenH());
    if (r <= l || b <= t) return;
    int sl = l / BLUR_DIV, st = t / BLUR_DIV;
    int sw = min((r - l + BLUR_DIV - 1) / BLUR_DIV, g_bw - sl);
    int sh = min((b - t + BLUR_DIV - 1) / BLUR_DIV, g_bh - st);
    if (sw <= 0 || sh <= 0) return;
    StretchBlt(dst, dx + (l - (int)sr.left), dy + (t - (int)sr.top), r - l, b - t, g_blurDC, sl, st, sw, sh, SRCCOPY);
}

static void FillSolid(HDC dc, RECT r, Rgba c) {
    HBRUSH br = CreateSolidBrush(RGB(c.r, c.g, c.b));
    FillRect(dc, &r, br);
    DeleteObject(br);
}

static LRESULT CALLBACK OvProc(HWND h, UINT m, WPARAM wp, LPARAM lp) {
    switch (m) {
    case WM_ERASEBKGND:
        return 1;
    case WM_NCHITTEST:
        return HTTRANSPARENT;
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(h, &ps);
        RECT wr, cr;
        GetWindowRect(h, &wr);
        GetClientRect(h, &cr);
        FillSolid(dc, cr, g_cfg.tint);
        BlitBackdrop(dc, 0, 0, wr);
        EndPaint(h, &ps);
        return 0;
    }
    }
    return DefWindowProcW(h, m, wp, lp);
}

struct AccentPolicy {
    int state;
    int flags;
    unsigned color;
    int anim;
};

struct WcaData {
    int attr;
    void* data;
    ULONG size;
};

typedef BOOL(WINAPI* SetWcaFn)(HWND, WcaData*);

static void SetAccent(HWND h, int state, Rgba c) {
    static SetWcaFn fn = (SetWcaFn)(void*)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
    if (!fn) return;
    AccentPolicy ap{state, 0, ((unsigned)c.a << 24) | ((unsigned)c.b << 16) | ((unsigned)c.g << 8) | (unsigned)c.r, 0};
    WcaData d{19, &ap, sizeof(ap)};
    fn(h, &d);
}

static bool DetectWin11() {
    typedef LONG(WINAPI * Fn)(OSVERSIONINFOW*);
    Fn fn = (Fn)(void*)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion");
    if (!fn) return false;
    OSVERSIONINFOW vi{};
    vi.dwOSVersionInfoSize = sizeof(vi);
    fn(&vi);
    return vi.dwBuildNumber >= 22621;
}

static std::string EffMaterial() {
    if (g_cfg.material == "mica" && !g_win11) return "blur";
    return g_cfg.material;
}

static bool IsTileable(HWND h) {
    if (!IsWindow(h) || !IsWindowVisible(h)) return false;
    if (GetAncestor(h, GA_ROOT) != h) return false;
    if (GetWindow(h, GW_OWNER)) return false;
    LONG_PTR ex = GetWindowLongPtrW(h, GWL_EXSTYLE);
    if (ex & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return false;
    LONG_PTR st = GetWindowLongPtrW(h, GWL_STYLE);
    if (!(st & WS_CAPTION)) return false;
    if (IsIconic(h)) return false;
    BOOL cloaked = FALSE;
    DwmGetWindowAttribute(h, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (cloaked) return false;
    wchar_t cls[64];
    GetClassNameW(h, cls, 64);
    if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW")) return false;
    return true;
}

static Win* Find(HWND h) {
    for (auto& w : g_wins)
        if (w.hwnd == h) return &w;
    return nullptr;
}

static void InvalidateBar();

static bool MakeLayered(Win& w) {
    LONG_PTR ex = GetWindowLongPtrW(w.hwnd, GWL_EXSTYLE);
    if (ex & WS_EX_LAYERED) return w.layeredByUs;
    SetWindowLongPtrW(w.hwnd, GWL_EXSTYLE, ex | WS_EX_LAYERED);
    SetPropW(w.hwnd, L"hypr_l", (HANDLE)1);
    w.layeredByUs = true;
    return true;
}

static void SetOpacity(Win& w, bool active) {
    if (!w.layeredByUs || !IsWindow(w.hwnd)) return;
    double o = active ? g_cfg.activeOp : g_cfg.inactiveOp;
    BYTE a = (BYTE)std::clamp((int)std::lround(o * 255.0), 40, 255);
    SetLayeredWindowAttributes(w.hwnd, 0, a, LWA_ALPHA);
}

static void MakeOverlay(Win& w) {
    if (w.ov) return;
    RECT r = GetFrame(w.hwnd);
    w.ov = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, OV_CLASS, L"", WS_POPUP, r.left, r.top,
                           r.right - r.left, r.bottom - r.top, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (w.ov)
        SetWindowPos(w.ov, w.hwnd, r.left, r.top, r.right - r.left, r.bottom - r.top,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

static void ClearMaterial(Win& w) {
    if (w.ov) {
        DestroyWindow(w.ov);
        w.ov = nullptr;
    }
    if (!IsWindow(w.hwnd)) return;
    if (w.layeredByUs) {
        LONG_PTR ex = GetWindowLongPtrW(w.hwnd, GWL_EXSTYLE);
        SetWindowLongPtrW(w.hwnd, GWL_EXSTYLE, ex & ~(LONG_PTR)WS_EX_LAYERED);
        RedrawWindow(w.hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
        RemovePropW(w.hwnd, L"hypr_l");
        w.layeredByUs = false;
    }
    if (w.accentOn) {
        SetAccent(w.hwnd, 0, {0, 0, 0, 0});
        w.accentOn = false;
    }
    if (w.micaOn) {
        int v = 0;
        DwmSetWindowAttribute(w.hwnd, 38, &v, sizeof(v));
        w.micaOn = false;
    }
}

static void ApplyMaterial(Win& w) {
    if (w.noBlur) return;
    std::string m = EffMaterial();
    if (m == "blur") {
        if (MakeLayered(w)) {
            MakeOverlay(w);
            SetOpacity(w, w.hwnd == g_active);
        }
    } else if (m == "acrylic") {
        SetAccent(w.hwnd, 4, g_cfg.tint);
        w.accentOn = true;
    } else if (m == "mica") {
        int v = 2;
        DwmSetWindowAttribute(w.hwnd, 38, &v, sizeof(v));
        w.micaOn = true;
    }
}

static void ApplyRounding(Win& w, int fw, int fh, bool force = false) {
    if (g_cfg.rounding <= 0 || w.noRound || w.parked || IsZoomed(w.hwnd)) return;
    if (!force && w.rgnW == fw && w.rgnH == fh) return;
    HRGN rg = CreateRoundRectRgn(w.margin.left, w.margin.top, w.margin.left + fw + 1, w.margin.top + fh + 1,
                                 g_cfg.rounding * 2, g_cfg.rounding * 2);
    if (SetWindowRgn(w.hwnd, rg, FALSE)) {
        w.rounded = true;
        w.rgnW = fw;
        w.rgnH = fh;
        SetPropW(w.hwnd, L"hypr_r", (HANDLE)1);
    } else {
        DeleteObject(rg);
    }
}

static void ClearRounding(Win& w) {
    if (w.rounded && IsWindow(w.hwnd)) {
        SetWindowRgn(w.hwnd, nullptr, TRUE);
        RemovePropW(w.hwnd, L"hypr_r");
    }
    w.rounded = false;
    w.rgnW = w.rgnH = 0;
}

static std::string ExeOf(HWND h) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    HANDLE p = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!p) return "";
    wchar_t buf[MAX_PATH];
    DWORD n = MAX_PATH;
    std::string out;
    if (QueryFullProcessImageNameW(p, 0, buf, &n)) {
        std::wstring ws(buf, n);
        size_t s = ws.find_last_of(L"\\/");
        if (s != std::wstring::npos) ws = ws.substr(s + 1);
        out = Lower(Narrow(ws.c_str()));
        if (out.size() > 4 && out.compare(out.size() - 4, 4, ".exe") == 0) out.resize(out.size() - 4);
    }
    CloseHandle(p);
    return out;
}

static bool RuleHit(const Win& w, const char* kind) {
    for (auto& r : g_cfg.rules) {
        if (r.kind != kind) continue;
        std::string t = r.target;
        if (t.size() > 4 && t.compare(t.size() - 4, 4, ".exe") == 0) t.resize(t.size() - 4);
        if (t == w.exe || t == w.cls) return true;
    }
    return false;
}

static bool MatExcluded(const Win& w) {
    for (auto& e : g_cfg.matExceptions)
        if (e == w.exe) return true;
    return RuleHit(w, "noblur");
}

static HICON GetIcon(HWND h) {
    DWORD_PTR r = 0;
    HICON i = nullptr;
    if (SendMessageTimeoutW(h, WM_GETICON, ICON_SMALL2, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 40, &r) && r) i = (HICON)r;
    if (!i && SendMessageTimeoutW(h, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 40, &r) && r) i = (HICON)r;
    if (!i) i = (HICON)GetClassLongPtrW(h, GCLP_HICONSM);
    if (!i) i = (HICON)GetClassLongPtrW(h, GCLP_HICON);
    if (!i) i = LoadIconW(nullptr, IDI_APPLICATION);
    return i;
}

static void Rescue(HWND h) {
    RECT f = GetFrame(h);
    if (f.left > -20000) return;
    RECT wa;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &wa, 0);
    RECT m = Margins(h);
    SetWindowPos(h, nullptr, wa.left + 60 - m.left, wa.top + 60 - m.top, (f.right - f.left) + m.left + m.right,
                 (f.bottom - f.top) + m.top + m.bottom, SWP_NOZORDER | SWP_NOACTIVATE);
}

static void AddWin(HWND h, bool fresh) {
    Rescue(h);
    Win w{};
    w.hwnd = h;
    w.ws = g_ws;
    w.fresh = fresh;
    w.cur = GetFrame(h);
    w.floatRect = w.cur;
    w.a = g_animW;
    w.icon = GetIcon(h);
    w.exe = ExeOf(h);
    wchar_t cls[128] = {};
    GetClassNameW(h, cls, 128);
    w.cls = Lower(Narrow(cls));
    w.floating = RuleHit(w, "float") || (!RuleHit(w, "tile") && IsFixedSize(h));
    w.noBlur = MatExcluded(w);
    w.noBorder = RuleHit(w, "noborder");
    w.noRound = RuleHit(w, "norounding");
    w.margin = Margins(h);
    if (GetPropW(h, L"hypr_l")) w.layeredByUs = true;
    if (GetPropW(h, L"hypr_r")) {
        SetWindowRgn(h, nullptr, FALSE);
        RemovePropW(h, L"hypr_r");
    }
    g_wins.push_back(w);
    if (std::find(g_mru.begin(), g_mru.end(), h) == g_mru.end()) g_mru.push_back(h);
    Win& ref = g_wins.back();
    ApplyMaterial(ref);
    ApplyRounding(ref, w.cur.right - w.cur.left, w.cur.bottom - w.cur.top, true);
}

static void SetTarget(Win& w, RECT to, const Anim& a, bool park, bool popin) {
    if (IsZoomed(w.hwnd)) ShowWindow(w.hwnd, SW_RESTORE);
    w.margin = Margins(w.hwnd);
    if (w.parked) {
        w.cur = Shift(to, w.enterDir * ScreenW());
        w.parked = false;
    } else if (popin) {
        int cw = to.right - to.left;
        int ch = to.bottom - to.top;
        int nw = (int)(cw * POPIN);
        int nh = (int)(ch * POPIN);
        w.cur.left = to.left + (cw - nw) / 2;
        w.cur.top = to.top + (ch - nh) / 2;
        w.cur.right = w.cur.left + nw;
        w.cur.bottom = w.cur.top + nh;
    }
    w.from = w.cur;
    w.to = to;
    w.t0 = Now();
    w.a = a;
    w.park = park;
    w.anim = !EqualRect(&w.from, &to) || park;
}

static void RoundPath(Gdiplus::GraphicsPath& p, float x, float y, float w, float h, float r) {
    if (r <= 0) {
        p.AddRectangle(Gdiplus::RectF(x, y, w, h));
        return;
    }
    float d = 2 * r;
    p.AddArc(x, y, d, d, 180, 90);
    p.AddArc(x + w - d, y, d, d, 270, 90);
    p.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    p.AddArc(x, y + h - d, d, d, 90, 90);
    p.CloseFigure();
}

static void FreeBorderLayer() {
    delete g_bgGfx;
    delete g_bgBmp;
    g_bgGfx = nullptr;
    g_bgBmp = nullptr;
    if (g_border) DestroyWindow(g_border);
    if (g_bdc) DeleteDC(g_bdc);
    if (g_bbmp) DeleteObject(g_bbmp);
    g_border = nullptr;
    g_bdc = nullptr;
    g_bbmp = nullptr;
    g_bbits = nullptr;
    SetRectEmpty(&g_bdirty);
}

static void EnsureBorderLayer() {
    bool want = g_cfg.borderSize > 0;
    if (!want && g_border) {
        FreeBorderLayer();
        return;
    }
    if (!want || g_border) return;
    int sw = ScreenW(), sh = ScreenH();
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = sw;
    bi.bmiHeader.biHeight = -sh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    HDC sdc = GetDC(nullptr);
    g_bbmp = CreateDIBSection(sdc, &bi, DIB_RGB_COLORS, (void**)&g_bbits, nullptr, 0);
    g_bdc = CreateCompatibleDC(sdc);
    ReleaseDC(nullptr, sdc);
    if (!g_bbmp || !g_bbits) {
        FreeBorderLayer();
        return;
    }
    SelectObject(g_bdc, g_bbmp);
    memset(g_bbits, 0, (size_t)sw * sh * 4);
    g_bgBmp = new Gdiplus::Bitmap(sw, sh, sw * 4, PixelFormat32bppPARGB, (BYTE*)g_bbits);
    g_bgGfx = new Gdiplus::Graphics(g_bgBmp);
    g_bgGfx->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g_border = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST,
                               BORDER_CLASS, L"", WS_POPUP, 0, 0, sw, sh, nullptr, nullptr, GetModuleHandleW(nullptr),
                               nullptr);
    ShowWindow(g_border, SW_SHOWNOACTIVATE);
    SetRectEmpty(&g_bdirty);
}

static void DrawBorders() {
    if (!g_border || !g_bbits) return;
    struct Ring {
        RECT r;
        bool act;
    };
    std::vector<Ring> rings;
    int bs = g_cfg.borderSize;
    for (auto& w : g_wins) {
        if (w.noBorder || w.ws != g_ws || w.parked) continue;
        if (!w.anim && !IsWindowVisible(w.hwnd)) continue;
        rings.push_back({w.anim ? w.cur : GetFrame(w.hwnd), w.hwnd == g_active});
    }
    int sw = ScreenW(), sh = ScreenH();
    int pad = bs + 3;
    RECT nu{};
    bool hn = false;
    for (auto& g : rings) {
        RECT q = {g.r.left - pad, g.r.top - pad, g.r.right + pad, g.r.bottom + pad};
        if (!hn) {
            nu = q;
            hn = true;
        } else {
            nu.left = min(nu.left, q.left);
            nu.top = min(nu.top, q.top);
            nu.right = max(nu.right, q.right);
            nu.bottom = max(nu.bottom, q.bottom);
        }
    }
    RECT tot = nu;
    bool hasOld = !IsRectEmpty(&g_bdirty);
    if (hasOld) {
        if (!hn) {
            tot = g_bdirty;
        } else {
            tot.left = min(tot.left, g_bdirty.left);
            tot.top = min(tot.top, g_bdirty.top);
            tot.right = max(tot.right, g_bdirty.right);
            tot.bottom = max(tot.bottom, g_bdirty.bottom);
        }
    } else if (!hn) {
        return;
    }
    tot.left = max((int)tot.left, 0);
    tot.top = max((int)tot.top, 0);
    tot.right = min((int)tot.right, sw);
    tot.bottom = min((int)tot.bottom, sh);
    if (tot.right <= tot.left || tot.bottom <= tot.top) return;
    for (int y = tot.top; y < tot.bottom; y++)
        memset(g_bbits + (size_t)y * sw + tot.left, 0, (size_t)(tot.right - tot.left) * 4);

    for (auto& g : rings) {
        float infl = bs * 0.5f - 1.0f;
        float x = g.r.left - infl, y = g.r.top - infl;
        float w = (g.r.right - g.r.left) + 2 * infl, h = (g.r.bottom - g.r.top) + 2 * infl;
        float rad = max(0.0f, (float)g_cfg.rounding + infl);
        Gdiplus::GraphicsPath path;
        RoundPath(path, x, y, w, h, rad);
        if (g.act) {
            double ang = EffA()ngle * 3.14159265358979 / 180.0;
            float cx = x + w / 2, cy = y + h / 2;
            float half = (float)((std::fabs(w * std::cos(ang)) + std::fabs(h * std::sin(ang))) / 2.0);
            Gdiplus::PointF p1(cx - (float)std::cos(ang) * half, cy - (float)std::sin(ang) * half);
            Gdiplus::PointF p2(cx + (float)std::cos(ang) * half, cy + (float)std::sin(ang) * half);
            Gdiplus::Color c1((BYTE)EffA().a, (BYTE)EffA().r, (BYTE)EffA().g, (BYTE)EffA().b);
            Gdiplus::Color c2((BYTE)EffB().a, (BYTE)EffB().r, (BYTE)EffB().g, (BYTE)EffB().b);
            Gdiplus::LinearGradientBrush br(p1, p2, c1, c2);
            Gdiplus::Pen pen(&br, (Gdiplus::REAL)bs);
            g_bgGfx->DrawPath(&pen, &path);
        } else {
            Gdiplus::Color c((BYTE)g_cfg.inact.a, (BYTE)g_cfg.inact.r, (BYTE)g_cfg.inact.g, (BYTE)g_cfg.inact.b);
            Gdiplus::Pen pen(c, (Gdiplus::REAL)bs);
            g_bgGfx->DrawPath(&pen, &path);
        }
    }

    SIZE sz = {sw, sh};
    POINT src = {0, 0};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UPDATELAYEREDWINDOWINFO ul{};
    ul.cbSize = sizeof(ul);
    ul.hdcSrc = g_bdc;
    ul.psize = &sz;
    ul.pptSrc = &src;
    ul.pblend = &bf;
    ul.dwFlags = ULW_ALPHA;
    ul.prcDirty = &tot;
    UpdateLayeredWindowIndirect(g_border, &ul);
    if (hn) {
        nu.left = max((int)nu.left, 0);
        nu.top = max((int)nu.top, 0);
        nu.right = min((int)nu.right, sw);
        nu.bottom = min((int)nu.bottom, sh);
        g_bdirty = nu;
    } else {
        SetRectEmpty(&g_bdirty);
    }
}

static bool Animating() {
    for (auto& w : g_wins)
        if (w.anim) return true;
    return false;
}

static void Step() {
    int n = 0;
    for (auto& w : g_wins)
        if (w.anim) n += w.ov ? 2 : 1;
    if (!n) return;
    double now = Now();
    HDWP dp = BeginDeferWindowPos(n);
    std::vector<HWND> dirty;
    for (auto& w : g_wins) {
        if (!w.anim) continue;
        double p = w.a.dur > 0 ? (now - w.t0) / w.a.dur : 2.0;
        bool done = p >= 1.0;
        RECT r = done ? w.to : Lerp(w.from, w.to, Bezier(w.a.c, p));
        w.cur = r;
        if (g_cfg.rounding > 0) ApplyRounding(w, r.right - r.left, r.bottom - r.top);
        int x = r.left - w.margin.left;
        int y = r.top - w.margin.top;
        int cw = (r.right - r.left) + w.margin.left + w.margin.right;
        int ch = (r.bottom - r.top) + w.margin.top + w.margin.bottom;
        int ox = r.left;
        if (done) {
            w.anim = false;
            if (w.park) {
                x = PARK_X;
                ox = PARK_X;
                w.park = false;
                w.parked = true;
            }
        }
        if (dp) dp = DeferWindowPos(dp, w.hwnd, nullptr, x, y, cw, ch, SWP_NOZORDER | SWP_NOACTIVATE);
        if (w.ov && dp) {
            dp = DeferWindowPos(dp, w.ov, w.hwnd, ox, r.top, r.right - r.left, r.bottom - r.top, SWP_NOACTIVATE);
            dirty.push_back(w.ov);
        }
    }
    if (dp) EndDeferWindowPos(dp);
    for (HWND o : dirty) InvalidateRect(o, nullptr, FALSE);
    DrawBorders();
}

static std::vector<RECT> Dwindle(size_t n, RECT area) {
    std::vector<RECT> out;
    RECT cur = area;
    int gap = g_cfg.gapsIn;
    for (size_t i = 0; i < n; i++) {
        if (i == n - 1) {
            out.push_back(cur);
            break;
        }
        int w = cur.right - cur.left;
        int h = cur.bottom - cur.top;
        RECT a = cur, b = cur;
        if (w >= h) {
            int mid = cur.left + w / 2;
            a.right = mid - gap / 2;
            b.left = mid + gap / 2;
        } else {
            int mid = cur.top + h / 2;
            a.bottom = mid - gap / 2;
            b.top = mid + gap / 2;
        }
        out.push_back(a);
        cur = b;
    }
    return out;
}

static void Retile(const Anim* anim = nullptr) {
    struct Guard {
        ~Guard() {
            InvalidateBar();
            DrawBorders();
        }
    } guard;
    std::vector<Win*> t;
    for (auto& w : g_wins)
        if (w.ws == g_ws && !w.floating) t.push_back(&w);
    if (t.empty()) return;
    RECT wa;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &wa, 0);
    int go = g_cfg.gapsOut;
    wa.left += go;
    wa.right -= go;
    wa.bottom -= go;
    wa.top = max((int)wa.top + go, g_barReserve);
    std::vector<RECT> rs = Dwindle(t.size(), wa);
    const Anim& a = anim ? *anim : g_animW;
    for (size_t i = 0; i < t.size(); i++) {
        bool popin = t[i]->fresh;
        t[i]->fresh = false;
        SetTarget(*t[i], rs[i], a, false, popin);
    }
}

static void ReleaseWin(Win& w) {
    ClearMaterial(w);
    ClearRounding(w);
}

static void Prune() {
    for (size_t i = 0; i < g_wins.size();) {
        if (!IsTileable(g_wins[i].hwnd)) {
            ReleaseWin(g_wins[i]);
            g_wins.erase(g_wins.begin() + (long)i);
        } else {
            i++;
        }
    }
}

static void Scan() {
    EnumWindows([](HWND h, LPARAM) -> BOOL {
        if (IsTileable(h) && !Find(h)) AddWin(h, false);
        return TRUE;
    }, 0);
}

static void FocusWs() {
    HWND t = g_lastFocus[g_ws];
    Win* w = t ? Find(t) : nullptr;
    if (!w || w->ws != g_ws) {
        w = nullptr;
        for (auto& x : g_wins)
            if (x.ws == g_ws) {
                w = &x;
                break;
            }
    }
    if (w) SetForegroundWindow(w->hwnd);
}

static void Leave(Win& w, int dir) {
    if (w.parked) return;
    w.cur = GetFrame(w.hwnd);
    w.floatRect = w.cur;
    SetTarget(w, Shift(w.cur, -dir * ScreenW()), g_animS, true, false);
}

static void SwitchWs(int n, bool focus) {
    if (n == g_ws || n < 0 || n >= NUM_WS) return;
    int dir = n > g_ws ? 1 : -1;
    for (auto& w : g_wins)
        if (w.ws == g_ws) Leave(w, dir);
    g_ws = n;
    for (auto& w : g_wins) {
        if (w.ws != g_ws) continue;
        w.enterDir = dir;
        if (w.floating) SetTarget(w, w.floatRect, g_animS, false, false);
    }
    Retile(&g_animS);
    if (focus) FocusWs();
}

static void MoveToWs(int n) {
    Win* w = Find(GetForegroundWindow());
    if (!w || n == g_ws || n < 0 || n >= NUM_WS) return;
    int dir = n > g_ws ? 1 : -1;
    w->ws = n;
    w->enterDir = dir;
    Leave(*w, dir);
    Retile();
    FocusWs();
}

static void FocusStep(int dir) {
    std::vector<HWND> t;
    for (auto& w : g_wins)
        if (w.ws == g_ws) t.push_back(w.hwnd);
    if (t.empty()) return;
    HWND fg = GetForegroundWindow();
    size_t idx = 0;
    for (size_t i = 0; i < t.size(); i++)
        if (t[i] == fg) idx = i;
    SetForegroundWindow(t[(idx + t.size() + dir) % t.size()]);
}

static void SwapStep(int dir) {
    Win* cur = Find(GetForegroundWindow());
    if (!cur || cur->floating) return;
    size_t n = g_wins.size();
    size_t i = cur - g_wins.data();
    for (size_t k = 1; k < n; k++) {
        size_t j = (i + (dir > 0 ? k : n - k)) % n;
        if (g_wins[j].ws == g_ws && !g_wins[j].floating) {
            std::swap(g_wins[i], g_wins[j]);
            Retile();
            return;
        }
    }
}

static void Spawn(const std::string& cmd) {
    std::wstring w = Widen(cmd);
    STARTUPINFOW si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};
    if (CreateProcessW(nullptr, &w[0], nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
}

static void RestoreAll() {
    for (auto& w : g_wins) {
        if (!w.parked && !w.anim) continue;
        RECT r = w.parked ? w.floatRect : w.to;
        SetWindowPos(w.hwnd, nullptr, r.left - w.margin.left, r.top - w.margin.top,
                     (r.right - r.left) + w.margin.left + w.margin.right,
                     (r.bottom - r.top) + w.margin.top + w.margin.bottom, SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

static bool VolAcquire() {
    if (g_epv) return true;
    IMMDeviceEnumerator* en = nullptr;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                                (void**)&en)))
        return false;
    IMMDevice* dev = nullptr;
    HRESULT hr = en->GetDefaultAudioEndpoint(eRender, eMultimedia, &dev);
    en->Release();
    if (FAILED(hr)) return false;
    hr = dev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&g_epv);
    dev->Release();
    return SUCCEEDED(hr) && g_epv;
}

static bool VolRefresh() {
    if (!VolAcquire()) return false;
    float lv = 0;
    BOOL mu = FALSE;
    if (FAILED(g_epv->GetMasterVolumeLevelScalar(&lv)) || FAILED(g_epv->GetMute(&mu))) {
        g_epv->Release();
        g_epv = nullptr;
        return false;
    }
    bool ch = lv != g_volLevel || mu != g_volMute;
    g_volLevel = lv;
    g_volMute = mu;
    return ch;
}

static void VolAdjust(float d) {
    if (!VolAcquire()) return;
    float lv = 0;
    if (FAILED(g_epv->GetMasterVolumeLevelScalar(&lv))) return;
    g_epv->SetMasterVolumeLevelScalar(std::clamp(lv + d, 0.0f, 1.0f), nullptr);
    if (d > 0) g_epv->SetMute(FALSE, nullptr);
    VolRefresh();
    InvalidateBar();
}

static void VolToggleMute() {
    if (!VolAcquire()) return;
    BOOL m = FALSE;
    g_epv->GetMute(&m);
    g_epv->SetMute(!m, nullptr);
    VolRefresh();
    InvalidateBar();
}

static int TextW(HFONT f, const std::wstring& str) {
    HDC dc = GetDC(nullptr);
    HGDIOBJ o = SelectObject(dc, f);
    SIZE sz{};
    GetTextExtentPoint32W(dc, str.c_str(), (int)str.size(), &sz);
    SelectObject(dc, o);
    ReleaseDC(nullptr, dc);
    return sz.cx;
}

static bool IsFixedSize(HWND h) {
    LONG_PTR st = GetWindowLongPtrW(h, GWL_STYLE);
    wchar_t cls[64];
    GetClassNameW(h, cls, 64);
    return !(st & WS_THICKFRAME) || !wcscmp(cls, L"#32770");
}

struct ExecJob {
    std::string cmd;
    std::string out;
    volatile LONG done;
};

struct ModRt {
    std::string name;
    std::wstring text, glyph;
    bool hidden = false;
    ULONGLONG last = 0, pIdle = 0, pTotal = 0, pIn = 0, pOut = 0, pTick = 0;
    ExecJob* job = nullptr;
};

static std::vector<ModRt*> g_rt;

static ModRt* GetRt(const std::string& n) {
    for (auto* m : g_rt)
        if (m->name == n) return m;
    ModRt* m = new ModRt();
    m->name = n;
    g_rt.push_back(m);
    return m;
}

static const ModDef* DefOf(const std::string& n) {
    for (auto& d : g_cfg.mods)
        if (d.name == n) return &d;
    return nullptr;
}

static std::string TypeOf(const std::string& n) {
    const ModDef* d = DefOf(n);
    if (d && !d->type.empty()) return d->type;
    return n;
}

static std::wstring ModTemplate(const std::string& type) {
    if (type == "cpu") return L"CPU 100%";
    if (type == "ram") return L"RAM 100%";
    if (type == "battery") return L"CHG 100%";
    if (type == "volume") return L"100%";
    if (type == "net") return L"\x2193 999.9 MB/s  \x2191 999.9 MB/s";
    return L"";
}

static std::wstring Rate(double v) {
    if (v >= 1048576.0) {
        int t = (int)std::lround(v / 104857.6);
        return std::to_wstring(t / 10) + L"." + std::to_wstring(t % 10) + L" MB/s";
    }
    return std::to_wstring((int)std::lround(v / 1024.0)) + L" KB/s";
}

static DWORD WINAPI ExecThread(LPVOID p) {
    ExecJob* j = (ExecJob*)p;
    SECURITY_ATTRIBUTES sa{sizeof(sa), nullptr, TRUE};
    HANDLE rd = nullptr, wr = nullptr;
    if (CreatePipe(&rd, &wr, &sa, 0)) {
        SetHandleInformation(rd, HANDLE_FLAG_INHERIT, 0);
        STARTUPINFOW si{};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
        si.hStdOutput = wr;
        si.hStdError = wr;
        si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        PROCESS_INFORMATION pi{};
        std::wstring cmd = L"cmd.exe /c " + Widen(j->cmd);
        if (CreateProcessW(nullptr, &cmd[0], nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
            CloseHandle(wr);
            wr = nullptr;
            char buf[512];
            DWORD got = 0;
            std::string o;
            while (o.size() < 400 && ReadFile(rd, buf, sizeof(buf) - 1, &got, nullptr) && got) o.append(buf, got);
            WaitForSingleObject(pi.hProcess, 5000);
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
            size_t nl = o.find_first_of("\r\n");
            if (nl != std::string::npos) o.resize(nl);
            j->out = o;
        }
        if (wr) CloseHandle(wr);
        CloseHandle(rd);
    }
    InterlockedExchange(&j->done, 1);
    return 0;
}

static bool ModTick(const std::string& name) {
    ModRt* m = GetRt(name);
    std::string type = TypeOf(name);
    const ModDef* d = DefOf(name);
    ULONGLONG now = GetTickCount64();
    std::wstring text = m->text, glyph = m->glyph;
    bool hidden = m->hidden;
    if (type == "clock") {
        std::string f = (d && !d->format.empty()) ? d->format : "%a %H:%M";
        time_t t = time(nullptr);
        struct tm* lt = localtime(&t);
        wchar_t b[96] = {};
        wcsftime(b, 96, Widen(f).c_str(), lt);
        text = b;
        hidden = false;
    } else if (type == "volume") {
        VolRefresh();
        hidden = g_volLevel < 0;
        if (!hidden) {
            text = g_volMute ? L"--" : std::to_wstring((int)std::lround(g_volLevel * 100)) + L"%";
            glyph = g_volMute ? L"\xE74F" : L"\xE767";
        }
    } else if (type == "cpu") {
        hidden = false;
        if (!m->last || now - m->last >= 1000) {
            FILETIME i, k, u;
            GetSystemTimes(&i, &k, &u);
            auto f64 = [](FILETIME x) { return ((ULONGLONG)x.dwHighDateTime << 32) | x.dwLowDateTime; };
            ULONGLONG idle = f64(i), total = f64(k) + f64(u);
            if (m->pTotal && total > m->pTotal) {
                double busy = 1.0 - (double)(idle - m->pIdle) / (double)(total - m->pTotal);
                text = L"CPU " + std::to_wstring((int)std::lround(std::clamp(busy, 0.0, 1.0) * 100)) + L"%";
            } else {
                text = L"CPU --";
            }
            m->pIdle = idle;
            m->pTotal = total;
            m->last = now;
        }
    } else if (type == "ram") {
        hidden = false;
        if (!m->last || now - m->last >= 2000) {
            MEMORYSTATUSEX ms{};
            ms.dwLength = sizeof(ms);
            GlobalMemoryStatusEx(&ms);
            text = L"RAM " + std::to_wstring((int)ms.dwMemoryLoad) + L"%";
            m->last = now;
        }
    } else if (type == "battery") {
        if (!m->last || now - m->last >= 10000) {
            SYSTEM_POWER_STATUS sp{};
            if (!GetSystemPowerStatus(&sp) || sp.BatteryFlag == 128 || sp.BatteryLifePercent == 255) {
                hidden = true;
            } else {
                hidden = false;
                text = std::wstring(sp.ACLineStatus == 1 ? L"CHG " : L"BAT ") + std::to_wstring((int)sp.BatteryLifePercent) + L"%";
            }
            m->last = now;
        }
    } else if (type == "net") {
        hidden = false;
        if (!m->last || now - m->last >= 1000) {
            MIB_IF_TABLE2* tbl = nullptr;
            ULONGLONG in = 0, out = 0;
            if (GetIfTable2(&tbl) == NO_ERROR && tbl) {
                for (ULONG k = 0; k < tbl->NumEntries; k++) {
                    const MIB_IF_ROW2& r = tbl->Table[k];
                    if (r.OperStatus == IfOperStatusUp && r.Type != IF_TYPE_SOFTWARE_LOOPBACK) {
                        in += r.InOctets;
                        out += r.OutOctets;
                    }
                }
                FreeMibTable(tbl);
            }
            if (m->pTick && now > m->pTick && in >= m->pIn && out >= m->pOut) {
                double dt = (double)(now - m->pTick) / 1000.0;
                text = L"\x2193 " + Rate((double)(in - m->pIn) / dt) + L"  \x2191 " + Rate((double)(out - m->pOut) / dt);
            } else {
                text = L"\x2193 --  \x2191 --";
            }
            m->pIn = in;
            m->pOut = out;
            m->pTick = now;
            m->last = now;
        }
    } else if (type == "exec") {
        hidden = false;
        if (m->job && m->job->done) {
            text = Widen(m->job->out);
            delete m->job;
            m->job = nullptr;
            m->last = now;
        }
        if (!m->job && d && !d->command.empty() &&
            (!m->last || now - m->last >= (ULONGLONG)max(1, d->interval) * 1000ULL)) {
            m->job = new ExecJob();
            m->job->cmd = d->command;
            m->job->done = 0;
            HANDLE th = CreateThread(nullptr, 0, ExecThread, m->job, 0, nullptr);
            if (th) {
                CloseHandle(th);
            } else {
                delete m->job;
                m->job = nullptr;
            }
            m->last = now;
        }
    } else {
        hidden = true;
    }
    bool changed = text != m->text || glyph != m->glyph || hidden != m->hidden;
    m->text = text;
    m->glyph = glyph;
    m->hidden = hidden;
    return changed;
}

static bool ModulesTick() {
    std::vector<std::string> names = g_cfg.barRight;
    for (auto& w : g_cfg.widgets)
        if (std::find(names.begin(), names.end(), w.name) == names.end()) names.push_back(w.name);
    bool ch = false;
    for (auto& n : names)
        if (ModTick(n)) ch = true;
    return ch;
}

struct WidgetWin {
    HWND h;
    WidgetDef d;
    HFONT font;
};

static std::vector<WidgetWin> g_widgets;

static HWND DesktopHost() {
    HWND p = GetShellWindow();
    if (p && FindWindowExW(p, nullptr, L"SHELLDLL_DefView", nullptr)) return p;
    HWND w = nullptr;
    while ((w = FindWindowExW(nullptr, w, L"WorkerW", nullptr)) != nullptr)
        if (FindWindowExW(w, nullptr, L"SHELLDLL_DefView", nullptr)) return w;
    return p;
}

static void ReorderWidgets() {
    UINT f = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE;
    if (g_shellSession && g_wall) {
        for (auto& w : g_widgets) SetWindowPos(w.h, HWND_BOTTOM, 0, 0, 0, 0, f);
        SetWindowPos(g_wall, HWND_BOTTOM, 0, 0, 0, 0, f);
        return;
    }
    HWND host = DesktopHost();
    for (auto& w : g_widgets) {
        HWND above = host ? GetWindow(host, GW_HWNDPREV) : nullptr;
        SetWindowPos(w.h, above && above != w.h ? above : HWND_BOTTOM, 0, 0, 0, 0, f);
    }
}

static LRESULT CALLBACK WidgetProc(HWND h, UINT m, WPARAM wp, LPARAM lp) {
    switch (m) {
    case WM_ERASEBKGND:
        return 1;
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(h, &ps);
        RECT wr, cr;
        GetWindowRect(h, &wr);
        GetClientRect(h, &cr);
        HDC mem = CreateCompatibleDC(dc);
        HBITMAP bmp = CreateCompatibleBitmap(dc, cr.right, cr.bottom);
        HGDIOBJ old = SelectObject(mem, bmp);
        FillSolid(mem, cr, g_cfg.tint);
        BlitBackdrop(mem, 0, 0, wr);
        for (auto& w : g_widgets) {
            if (w.h != h) continue;
            ModRt* r = GetRt(w.d.name);
            SetBkMode(mem, TRANSPARENT);
            SetTextColor(mem, RGB(235, 237, 245));
            HGDIOBJ of = SelectObject(mem, w.font);
            RECT tr = cr;
            if (!r->hidden) DrawTextW(mem, r->text.c_str(), -1, &tr, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
            SelectObject(mem, of);
        }
        BitBlt(dc, 0, 0, cr.right, cr.bottom, mem, 0, 0, SRCCOPY);
        SelectObject(mem, old);
        DeleteObject(bmp);
        DeleteDC(mem);
        EndPaint(h, &ps);
        return 0;
    }
    }
    return DefWindowProcW(h, m, wp, lp);
}

static void EnsureWidgets() {
    for (auto& w : g_widgets) {
        DestroyWindow(w.h);
        DeleteObject(w.font);
    }
    g_widgets.clear();
    int sw = ScreenW(), sh = ScreenH();
    for (auto& d : g_cfg.widgets) {
        int x = d.x < 0 ? sw + d.x - d.w : d.x;
        int y = d.y < 0 ? sh + d.y - d.h : d.y;
        HWND h = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, WIDGET_CLASS, L"", WS_POPUP, x, y, d.w, d.h,
                                 nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
        if (!h) continue;
        int rad = max(g_cfg.rounding, 12) * 2;
        SetWindowRgn(h, CreateRoundRectRgn(0, 0, d.w + 1, d.h + 1, rad, rad), TRUE);
        HFONT f = CreateFontW(-d.px, 0, 0, 0, FW_LIGHT, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        g_widgets.push_back({h, d, f});
        ShowWindow(h, SW_SHOWNOACTIVATE);
    }
    ReorderWidgets();
}

static int BarTop() {
    return 8;
}

static void LayoutBar() {
    g_items.clear();
    if (!g_bar) return;
    const int ICON = 20, IGAP = 10, PAD = 16, DW = 8, DAW = 22, DGAP = 7, DH = 8;
    bool occ[NUM_WS] = {};
    for (auto& w : g_wins) occ[w.ws] = true;
    int H = g_cfg.barHeight;
    int x = PAD;
    for (int i = 0; i < NUM_WS; i++) {
        if (!occ[i] && i != g_ws) continue;
        int w = i == g_ws ? DAW : DW;
        BarItem it{};
        it.kind = 0;
        it.r = {x, (H - DH) / 2, x + w, (H - DH) / 2 + DH};
        it.ws = i;
        it.active = i == g_ws;
        it.occupied = occ[i];
        g_items.push_back(it);
        x += w + DGAP;
    }
    bool any = false;
    for (auto& w : g_wins) {
        if (w.ws != g_ws) continue;
        if (!any) {
            x += 10;
            any = true;
        }
        BarItem it{};
        it.kind = 1;
        it.r = {x, (H - ICON) / 2 - 1, x + ICON, (H - ICON) / 2 - 1 + ICON};
        it.hwnd = w.hwnd;
        it.icon = w.icon;
        it.active = w.hwnd == g_active;
        g_items.push_back(it);
        x += ICON + IGAP;
    }
    int end = x - (any ? IGAP : DGAP);
    for (auto& name : g_cfg.barRight) {
        ModRt* mr = GetRt(name);
        if (mr->hidden || (mr->text.empty() && mr->glyph.empty())) continue;
        std::string type = TypeOf(name);
        std::wstring tpl = ModTemplate(type);
        int tw = TextW(g_font, tpl.empty() ? mr->text : tpl);
        int gw = mr->glyph.empty() ? 0 : 20;
        end += 16;
        BarItem it{};
        it.kind = type == "volume" ? 2 : 3;
        it.r = {end, 0, end + gw + tw, H};
        it.text = mr->text;
        it.glyph = mr->glyph;
        g_items.push_back(it);
        end += gw + tw;
    }
    int total = max(end + PAD, 90);
    int sx = (ScreenW() - total) / 2;
    RECT cur;
    GetWindowRect(g_bar, &cur);
    if (cur.left != sx || cur.right - cur.left != total || cur.bottom - cur.top != H) {
        SetWindowPos(g_bar, HWND_TOPMOST, sx, BarTop(), total, H, SWP_NOACTIVATE | (g_fs ? 0 : SWP_SHOWWINDOW));
        HRGN rg = CreateRoundRectRgn(0, 0, total + 1, H + 1, H, H);
        SetWindowRgn(g_bar, rg, TRUE);
    }
}

static void InvalidateBar() {
    if (!g_bar) return;
    LayoutBar();
    InvalidateRect(g_bar, nullptr, FALSE);
}

static void PaintBar(HWND h) {
    PAINTSTRUCT ps;
    HDC dc = BeginPaint(h, &ps);
    RECT wr, cr;
    GetWindowRect(h, &wr);
    GetClientRect(h, &cr);
    HDC mem = CreateCompatibleDC(dc);
    HBITMAP bmp = CreateCompatibleBitmap(dc, cr.right, cr.bottom);
    HGDIOBJ old = SelectObject(mem, bmp);
    FillSolid(mem, cr, g_cfg.tint);
    BlitBackdrop(mem, 0, 0, wr);
    HGDIOBJ oldPen = SelectObject(mem, GetStockObject(NULL_PEN));
    SetBkMode(mem, TRANSPARENT);
    SetTextColor(mem, RGB(225, 228, 240));
    for (auto& it : g_items) {
        if (it.kind == 0) {
            Rgba c = it.active ? EffAccent() : (it.occupied ? Rgba{205, 208, 220, 255} : Rgba{110, 114, 130, 255});
            HBRUSH br = CreateSolidBrush(RGB(c.r, c.g, c.b));
            HGDIOBJ ob = SelectObject(mem, br);
            RoundRect(mem, it.r.left, it.r.top, it.r.right + 1, it.r.bottom + 1, 8, 8);
            SelectObject(mem, ob);
            DeleteObject(br);
        } else if (it.kind == 1) {
            DrawIconEx(mem, it.r.left, it.r.top, it.icon, 20, 20, 0, nullptr, DI_NORMAL);
            if (it.active) {
                RECT u = {it.r.left + 3, it.r.bottom + 2, it.r.right - 3, it.r.bottom + 4};
                FillSolid(mem, u, EffAccent());
            }
        } else {
            HGDIOBJ of = SelectObject(mem, g_font);
            if (it.kind == 2) {
                SelectObject(mem, g_glyph);
                RECT gr = {it.r.left, it.r.top, it.r.left + 16, it.r.bottom};
                DrawTextW(mem, it.glyph.c_str(), -1, &gr, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
                SelectObject(mem, g_font);
                RECT tr = {it.r.left + 20, it.r.top, it.r.right, it.r.bottom};
                DrawTextW(mem, it.text.c_str(), -1, &tr, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
            } else {
                RECT tr = it.r;
                DrawTextW(mem, it.text.c_str(), -1, &tr, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
            }
            SelectObject(mem, of);
        }
    }
    SelectObject(mem, oldPen);
    BitBlt(dc, 0, 0, cr.right, cr.bottom, mem, 0, 0, SRCCOPY);
    SelectObject(mem, old);
    DeleteObject(bmp);
    DeleteDC(mem);
    EndPaint(h, &ps);
}

static LRESULT CALLBACK BarProc(HWND h, UINT m, WPARAM wp, LPARAM lp) {
    switch (m) {
    case WM_ERASEBKGND:
        return 1;
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;
    case WM_PAINT:
        PaintBar(h);
        return 0;
    case WM_LBUTTONDOWN: {
        POINT p = {(short)LOWORD(lp), (short)HIWORD(lp)};
        for (auto& it : g_items) {
            if (!PtInRect(&it.r, p)) continue;
            if (it.kind == 0) SwitchWs(it.ws, true);
            else if (it.kind == 1) SetForegroundWindow(it.hwnd);
            else if (it.kind == 2) VolToggleMute();
            break;
        }
        return 0;
    }
    case WM_MOUSEWHEEL: {
        bool up = GET_WHEEL_DELTA_WPARAM(wp) > 0;
        POINT p = {(short)LOWORD(lp), (short)HIWORD(lp)};
        ScreenToClient(h, &p);
        for (auto& it : g_items) {
            if (it.kind == 2 && PtInRect(&it.r, p)) {
                VolAdjust(up ? 0.05f : -0.05f);
                return 0;
            }
        }
        SwitchWs(g_ws + (up ? -1 : 1), true);
        return 0;
    }
    }
    return DefWindowProcW(h, m, wp, lp);
}

static void EnsureBar() {
    if (!g_font) {
        g_font = CreateFontW(-14, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        g_glyph = CreateFontW(-14, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0,
                              L"Segoe MDL2 Assets");
    }
    g_barReserve = g_cfg.barEnabled ? BarTop() + g_cfg.barHeight + 8 : 0;
    if (g_cfg.barEnabled && !g_bar) {
        g_bar = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST, BAR_CLASS, L"", WS_POPUP, 0, 0,
                                100, g_cfg.barHeight, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    } else if (!g_cfg.barEnabled && g_bar) {
        DestroyWindow(g_bar);
        g_bar = nullptr;
    }
}

static void Dispatch(const Bind& b);

static std::string CfgPath() {
    char buf[MAX_PATH];
    DWORD n = GetEnvironmentVariableA("USERPROFILE", buf, MAX_PATH);
    if (!n || n >= MAX_PATH) return "";
    return std::string(buf) + "\\.config\\hypr\\hypr.conf";
}

static bool ReadAll(const std::string& path, std::string& out) {
    HANDLE f = CreateFileW(Widen(path).c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    DWORD sz = GetFileSize(f, nullptr);
    out.assign(sz, 0);
    DWORD got = 0;
    bool ok = ReadFile(f, &out[0], sz, &got, nullptr) != 0;
    CloseHandle(f);
    out.resize(got);
    return ok;
}

static void EnsureDefaultConfig(const std::string& path) {
    if (path.empty() || GetFileAttributesW(Widen(path).c_str()) != INVALID_FILE_ATTRIBUTES) return;
    std::string dir = path.substr(0, path.find_last_of('\\'));
    CreateDirectoryW(Widen(dir.substr(0, dir.find_last_of('\\'))).c_str(), nullptr);
    CreateDirectoryW(Widen(dir).c_str(), nullptr);
    HANDLE f = CreateFileW(Widen(path).c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return;
    DWORD w = 0;
    WriteFile(f, kDefaultConf, (DWORD)strlen(kDefaultConf), &w, nullptr);
    CloseHandle(f);
}

static void StatCfg(const std::string& path) {
    WIN32_FILE_ATTRIBUTE_DATA d{};
    if (GetFileAttributesExW(Widen(path).c_str(), GetFileExInfoStandard, &d)) g_cfgTime = d.ftLastWriteTime;
}

static void KillActive() {
    HWND fg = GetForegroundWindow();
    if (!fg) return;
    HWND root = GetAncestor(fg, GA_ROOT);
    if (root) fg = root;
    wchar_t cls[64] = {};
    GetClassNameW(fg, cls, 64);
    if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW") || !wcscmp(cls, L"Shell_TrayWnd")) return;
    ULONGLONG now = GetTickCount64();
    bool again = fg == g_lastKill && now - g_lastKillTick < 2500 && IsWindow(fg);
    g_lastKill = fg;
    g_lastKillTick = now;
    if (again && IsHungAppWindow(fg)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(fg, &pid);
        HANDLE p = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        if (p) {
            TerminateProcess(p, 1);
            CloseHandle(p);
        }
        return;
    }
    DWORD_PTR r = 0;
    if (!SendMessageTimeoutW(fg, WM_SYSCOMMAND, SC_CLOSE, 0, SMTO_ABORTIFHUNG, 150, &r)) PostMessageW(fg, WM_CLOSE, 0, 0);
}

static void OnExplorerChange(bool alive) {
    if (alive) {
        if (!g_shellSession) WallpaperApply();
        ReorderWidgets();
        UINT f = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE;
        if (g_bar) SetWindowPos(g_bar, HWND_TOPMOST, 0, 0, 0, 0, f);
        if (g_border) SetWindowPos(g_border, HWND_TOPMOST, 0, 0, 0, 0, f);
    }
    Retile();
}

static bool IsFullscreen(HWND h) {
    if (!h) return false;
    wchar_t cls[64] = {};
    GetClassNameW(h, cls, 64);
    if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW") || !wcscmp(cls, L"Shell_TrayWnd")) return false;
    LONG_PTR st = GetWindowLongPtrW(h, GWL_STYLE);
    if ((st & WS_CAPTION) == WS_CAPTION) return false;
    RECT r;
    GetWindowRect(h, &r);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST), &mi)) return false;
    return r.left <= mi.rcMonitor.left && r.top <= mi.rcMonitor.top && r.right >= mi.rcMonitor.right &&
           r.bottom >= mi.rcMonitor.bottom;
}

static void UpdateFullscreen() {
    bool fs = IsFullscreen(GetForegroundWindow());
    if (fs == g_fs) return;
    g_fs = fs;
    if (g_bar) ShowWindow(g_bar, fs ? SW_HIDE : SW_SHOWNOACTIVATE);
    if (g_border) ShowWindow(g_border, fs ? SW_HIDE : SW_SHOWNOACTIVATE);
}

static void WallpaperScan() {
    g_wpList.clear();
    g_wpCur.clear();
    if (!g_cfg.wpFolder.empty()) {
        std::wstring dir = Widen(g_cfg.wpFolder);
        if (!dir.empty() && dir.back() != L'\\') dir += L'\\';
        WIN32_FIND_DATAW fd;
        HANDLE h = FindFirstFileW((dir + L"*").c_str(), &fd);
        if (h != INVALID_HANDLE_VALUE) {
            do {
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
                std::wstring n = fd.cFileName;
                size_t d = n.find_last_of(L'.');
                if (d == std::wstring::npos) continue;
                std::wstring ext = n.substr(d);
                for (auto& c : ext) c = (wchar_t)towlower(c);
                if (ext == L".jpg" || ext == L".jpeg" || ext == L".png" || ext == L".bmp") g_wpList.push_back(dir + n);
            } while (FindNextFileW(h, &fd));
            FindClose(h);
        }
        std::sort(g_wpList.begin(), g_wpList.end());
    }
    g_wpIdx = 0;
    if (!g_cfg.wpPath.empty()) g_wpCur = Widen(g_cfg.wpPath);
    else if (!g_wpList.empty()) g_wpCur = g_wpList[0];
    g_wpTick = GetTickCount64();
}

static void WallpaperApply() {
    BuildBlur();
    if (g_shellSession) BuildWallpaper();
    else if (g_cfg.wpApplySystem && !g_wpCur.empty())
        SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (PVOID)g_wpCur.c_str(), SPIF_SENDCHANGE);
    for (auto& w : g_wins)
        if (w.ov) InvalidateRect(w.ov, nullptr, FALSE);
    for (auto& w : g_widgets) InvalidateRect(w.h, nullptr, FALSE);
    InvalidateBar();
    DrawBorders();
}

static void WallpaperStep(int d) {
    if (g_wpList.empty()) return;
    size_t n = g_wpList.size();
    g_wpIdx = (g_wpIdx + n + (size_t)(d > 0 ? 1 : n - 1)) % n;
    g_wpCur = g_wpList[g_wpIdx];
    g_wpTick = GetTickCount64();
    WallpaperApply();
}

static void Reload(bool first) {
    std::string path = CfgPath();
    std::string txt;
    if (path.empty() || !ReadAll(path, txt) || txt.empty()) txt = kDefaultConf;
    StatCfg(path);
    Config n;
    ParseConfig(txt, n);
    if (n.binds.empty()) ParseConfig(kDefaultConf, n);
    std::string sig = n.material + "|" + std::to_string(n.blurSize) + "|" + std::to_string(n.blurPasses) + "|" +
                      std::to_string(n.tint.r) + "," + std::to_string(n.tint.g) + "," + std::to_string(n.tint.b) + "," +
                      std::to_string(n.tint.a) + "|" + n.wpPath + "|" + n.wpMode + "|" + n.wpFolder + "|" +
                      std::to_string(n.wpColor.r) + "," + std::to_string(n.wpColor.g) + "," + std::to_string(n.wpColor.b);
    bool rebuild = first || sig != g_sig;
    g_sig = sig;
    g_cfg = std::move(n);
    Resolve(g_cfg);
    if (rebuild) {
        WallpaperScan();
        WallpaperApply();
    }
    for (auto& w : g_wins) {
        bool was = w.noBlur;
        w.noBlur = MatExcluded(w);
        w.noBorder = RuleHit(w, "noborder");
        w.noRound = RuleHit(w, "norounding");
        if (rebuild || was != w.noBlur) {
            ClearMaterial(w);
            ApplyMaterial(w);
        } else {
            SetOpacity(w, w.hwnd == g_active);
        }
        ClearRounding(w);
        if (!w.parked) {
            RECT f = GetFrame(w.hwnd);
            w.margin = Margins(w.hwnd);
            ApplyRounding(w, f.right - f.left, f.bottom - f.top, true);
        }
    }
    ModulesTick();
    EnsureBar();
    EnsureWidgets();
    EnsureBorderLayer();
    Retile();
    if (first)
        for (auto& c : g_cfg.execOnce) Spawn(c);
}

static void Dispatch(const Bind& b) {
    int dir = (b.arg == "l" || b.arg == "u") ? -1 : 1;
    switch (b.d) {
    case D_EXEC:
        Spawn(b.arg);
        break;
    case D_KILL:
        KillActive();
        break;
    case D_FLOAT: {
        Win* w = Find(GetForegroundWindow());
        if (w) {
            w->floating = !w->floating;
            if (w->floating) w->floatRect = GetFrame(w->hwnd);
            Retile();
        }
        break;
    }
    case D_EXIT:
        PostQuitMessage(0);
        break;
    case D_WS:
        SwitchWs(atoi(b.arg.c_str()) - 1, true);
        break;
    case D_MOVEWS:
        MoveToWs(atoi(b.arg.c_str()) - 1);
        break;
    case D_FOCUS:
        FocusStep(dir);
        break;
    case D_SWAP:
        SwapStep(dir);
        break;
    case D_RELOAD:
        Reload(false);
        break;
    case D_WALL:
        WallpaperStep(b.arg == "prev" ? -1 : 1);
        break;
    }
}

static const int SW_TILE_W = 220, SW_TILE_H = 150, SW_GAP = 12, SW_PAD = 18, SW_COLS = 6, SWT_MAX = 24;

static void MruTouch(HWND h) {
    g_mru.erase(std::remove(g_mru.begin(), g_mru.end(), h), g_mru.end());
    g_mru.insert(g_mru.begin(), h);
}

static RECT SwTile(int i) {
    int n = (int)g_swList.size();
    int cols = min(n, SW_COLS);
    int x = SW_PAD + (i % cols) * (SW_TILE_W + SW_GAP);
    int y = SW_PAD + (i / cols) * (SW_TILE_H + SW_GAP);
    return {x, y, x + SW_TILE_W, y + SW_TILE_H};
}

static void SwitcherClose() {
    for (auto t : g_swThumbs) DwmUnregisterThumbnail(t);
    g_swThumbs.clear();
    if (g_sw) ShowWindow(g_sw, SW_HIDE);
    g_swOpen = false;
}

static LRESULT CALLBACK SwProc(HWND h, UINT m, WPARAM wp, LPARAM lp) {
    switch (m) {
    case WM_ERASEBKGND:
        return 1;
    case WM_NCHITTEST:
        return HTTRANSPARENT;
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(h, &ps);
        RECT wr, cr;
        GetWindowRect(h, &wr);
        GetClientRect(h, &cr);
        HDC mem = CreateCompatibleDC(dc);
        HBITMAP bmp = CreateCompatibleBitmap(dc, cr.right, cr.bottom);
        HGDIOBJ old = SelectObject(mem, bmp);
        FillSolid(mem, cr, g_cfg.tint);
        BlitBackdrop(mem, 0, 0, wr);
        {
            Gdiplus::Graphics g(mem);
            g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            for (int i = 0; i < (int)g_swList.size(); i++) {
                RECT t = SwTile(i);
                Gdiplus::GraphicsPath path;
                RoundPath(path, (float)t.left, (float)t.top, (float)(t.right - t.left), (float)(t.bottom - t.top), 14.0f);
                if (i == g_swSel) {
                    Gdiplus::SolidBrush fb(Gdiplus::Color(70, (BYTE)EffAccent().r, (BYTE)EffAccent().g, (BYTE)EffAccent().b));
                    g.FillPath(&fb, &path);
                    Gdiplus::Pen pen(Gdiplus::Color(255, (BYTE)EffAccent().r, (BYTE)EffAccent().g, (BYTE)EffAccent().b), 2.0f);
                    g.DrawPath(&pen, &path);
                } else {
                    Gdiplus::SolidBrush fb(Gdiplus::Color(22, 255, 255, 255));
                    g.FillPath(&fb, &path);
                }
            }
        }
        SetBkMode(mem, TRANSPARENT);
        SetTextColor(mem, RGB(235, 237, 245));
        HGDIOBJ of = SelectObject(mem, g_font);
        for (int i = 0; i < (int)g_swList.size(); i++) {
            RECT t = SwTile(i);
            Win* w = Find(g_swList[i]);
            if (w && w->icon) DrawIconEx(mem, t.left + 12, t.bottom - 32, w->icon, 20, 20, 0, nullptr, DI_NORMAL);
            wchar_t title[128] = {};
            GetWindowTextW(g_swList[i], title, 128);
            RECT tr = {t.left + 38, t.bottom - 36, t.right - 10, t.bottom - 8};
            DrawTextW(mem, title, -1, &tr, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS);
        }
        SelectObject(mem, of);
        BitBlt(dc, 0, 0, cr.right, cr.bottom, mem, 0, 0, SRCCOPY);
        SelectObject(mem, old);
        DeleteObject(bmp);
        DeleteDC(mem);
        EndPaint(h, &ps);
        return 0;
    }
    }
    return DefWindowProcW(h, m, wp, lp);
}

static void SwitcherOpen(int dir) {
    g_swList.clear();
    for (HWND h : g_mru)
        if (Find(h)) g_swList.push_back(h);
    for (auto& w : g_wins)
        if (std::find(g_swList.begin(), g_swList.end(), w.hwnd) == g_swList.end()) g_swList.push_back(w.hwnd);
    if (g_swList.size() > (size_t)SWT_MAX) g_swList.resize(SWT_MAX);
    int n = (int)g_swList.size();
    if (n < 2) return;
    g_swSel = dir > 0 ? 1 : n - 1;
    int cols = min(n, SW_COLS);
    int rows = (n + cols - 1) / cols;
    int W = SW_PAD * 2 + cols * SW_TILE_W + (cols - 1) * SW_GAP;
    int H = SW_PAD * 2 + rows * SW_TILE_H + (rows - 1) * SW_GAP;
    if (!g_sw)
        g_sw = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST, SW_CLASS, L"", WS_POPUP, 0, 0, W, H,
                               nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    SetWindowPos(g_sw, HWND_TOPMOST, (ScreenW() - W) / 2, (ScreenH() - H) / 2, W, H, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    SetWindowRgn(g_sw, CreateRoundRectRgn(0, 0, W + 1, H + 1, 28, 28), TRUE);
    for (auto t : g_swThumbs) DwmUnregisterThumbnail(t);
    g_swThumbs.clear();
    for (int i = 0; i < n; i++) {
        HTHUMBNAIL th = nullptr;
        if (FAILED(DwmRegisterThumbnail(g_sw, g_swList[i], &th))) continue;
        SIZE ss{};
        if (FAILED(DwmQueryThumbnailSourceSize(th, &ss)) || ss.cx <= 0 || ss.cy <= 0) {
            DwmUnregisterThumbnail(th);
            continue;
        }
        RECT t = SwTile(i);
        int aw = SW_TILE_W - 24, ah = SW_TILE_H - 56;
        double sc = min(1.0, min((double)aw / ss.cx, (double)ah / ss.cy));
        int tw = (int)(ss.cx * sc), tH = (int)(ss.cy * sc);
        int tx = t.left + 12 + (aw - tw) / 2, ty = t.top + 12 + (ah - tH) / 2;
        DWM_THUMBNAIL_PROPERTIES tp{};
        tp.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_OPACITY;
        tp.rcDestination = {tx, ty, tx + tw, ty + tH};
        tp.fVisible = TRUE;
        tp.opacity = 255;
        DwmUpdateThumbnailProperties(th, &tp);
        g_swThumbs.push_back(th);
    }
    g_swOpen = true;
    InvalidateRect(g_sw, nullptr, FALSE);
}

static void SwitcherStep(int dir) {
    int n = (int)g_swList.size();
    if (!g_swOpen || n < 2) return;
    g_swSel = (g_swSel + dir + n) % n;
    InvalidateRect(g_sw, nullptr, FALSE);
}

static void SwitcherCommit() {
    if (!g_swOpen) return;
    HWND t = g_swList[g_swSel];
    SwitcherClose();
    if (IsWindow(t)) SetForegroundWindow(t);
}

static void CALLBACK ReloadTimer(HWND, UINT, UINT_PTR, DWORD) {
    if (g_cfg.wpInterval > 0 && !g_wpList.empty() &&
        GetTickCount64() - g_wpTick > (ULONGLONG)g_cfg.wpInterval * 60000ULL)
        WallpaperStep(1);
    if (ModulesTick()) {
        InvalidateBar();
        for (auto& w : g_widgets) InvalidateRect(w.h, nullptr, FALSE);
    }
    bool alive = FindWindowW(L"Shell_TrayWnd", nullptr) != nullptr;
    if (alive != g_expAlive) {
        g_expAlive = alive;
        OnExplorerChange(alive);
    }
    if ((g_mods & MOD_SUPER_) && !(GetAsyncKeyState(VK_LWIN) & 0x8000) && !(GetAsyncKeyState(VK_RWIN) & 0x8000))
        g_mods &= ~MOD_SUPER_;
    if ((g_mods & MOD_ALT_) && !(GetAsyncKeyState(VK_MENU) & 0x8000)) {
        g_mods &= ~MOD_ALT_;
        g_altTab = false;
        if (g_swOpen) SwitcherCommit();
    }
    if ((g_mods & MOD_SHIFT_) && !(GetAsyncKeyState(VK_SHIFT) & 0x8000)) g_mods &= ~MOD_SHIFT_;
    if ((g_mods & MOD_CTRL_) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000)) g_mods &= ~MOD_CTRL_;
    std::string path = CfgPath();
    WIN32_FILE_ATTRIBUTE_DATA d{};
    if (!GetFileAttributesExW(Widen(path).c_str(), GetFileExInfoStandard, &d)) return;
    if (CompareFileTime(&d.ftLastWriteTime, &g_cfgTime) != 0) Reload(false);
}

static LRESULT CALLBACK KbProc(int code, WPARAM wp, LPARAM lp) {
    if (code != HC_ACTION) return CallNextHookEx(g_kbHook, code, wp, lp);
    auto* k = reinterpret_cast<KBDLLHOOKSTRUCT*>(lp);
    if (k->flags & LLKHF_INJECTED) return CallNextHookEx(g_kbHook, code, wp, lp);
    bool down = (wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN);
    bool up = (wp == WM_KEYUP || wp == WM_SYSKEYUP);
    DWORD vk = k->vkCode;

    unsigned bit = 0;
    if (vk == VK_LWIN || vk == VK_RWIN) bit = MOD_SUPER_;
    else if (vk == VK_LSHIFT || vk == VK_RSHIFT || vk == VK_SHIFT) bit = MOD_SHIFT_;
    else if (vk == VK_LCONTROL || vk == VK_RCONTROL || vk == VK_CONTROL) bit = MOD_CTRL_;
    else if (vk == VK_LMENU || vk == VK_RMENU || vk == VK_MENU) bit = MOD_ALT_;
    if (bit) {
        if (down) {
            if (!(g_mods & bit)) {
                if (bit == MOD_SUPER_) g_winUsed = false;
                if (bit == MOD_ALT_) g_altUsed = false;
            }
            g_mods |= bit;
        } else if (up) {
            g_mods &= ~bit;
            if (bit == MOD_ALT_ && g_altTab) {
                g_altTab = false;
                PostThreadMessageW(GetCurrentThreadId(), WM_APP + 2, 0, 0);
            }
            if ((bit == MOD_SUPER_ && g_winUsed) || (bit == MOD_ALT_ && g_altUsed)) {
                if (bit == MOD_ALT_) g_altUsed = false;
                INPUT in[2]{};
                in[0].type = INPUT_KEYBOARD;
                in[0].ki.wVk = VK_F24;
                in[1].type = INPUT_KEYBOARD;
                in[1].ki.wVk = VK_F24;
                in[1].ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(2, in, sizeof(INPUT));
            }
        }
        return CallNextHookEx(g_kbHook, code, wp, lp);
    }
    if (vk < 256 && up && g_swallowed[vk]) {
        g_swallowed[vk] = false;
        return 1;
    }
    if (down && g_cfg.altTab && (g_mods & MOD_ALT_) && !(g_mods & ~(MOD_ALT_ | MOD_SHIFT_))) {
        if (vk == VK_TAB) {
            g_swallowed[VK_TAB] = true;
            g_altUsed = true;
            g_altTab = true;
            PostThreadMessageW(GetCurrentThreadId(), WM_APP + 1, (g_mods & MOD_SHIFT_) ? 1 : 0, 0);
            return 1;
        }
        if (vk == VK_ESCAPE && g_altTab) {
            g_swallowed[VK_ESCAPE] = true;
            g_altTab = false;
            PostThreadMessageW(GetCurrentThreadId(), WM_APP + 3, 0, 0);
            return 1;
        }
    }
    if (down && g_mods) {
        for (size_t i = 0; i < g_cfg.binds.size(); i++) {
            const Bind& b = g_cfg.binds[i];
            if (b.vk == vk && b.mods == g_mods) {
                if (vk < 256) g_swallowed[vk] = true;
                if (g_mods & MOD_SUPER_) g_winUsed = true;
                PostThreadMessageW(GetCurrentThreadId(), WM_APP, (WPARAM)i, 0);
                return 1;
            }
        }
    }
    return CallNextHookEx(g_kbHook, code, wp, lp);
}

static void CALLBACK EventProc(HWINEVENTHOOK, DWORD ev, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hwnd) return;
    switch (ev) {
    case EVENT_OBJECT_SHOW:
    case EVENT_OBJECT_UNCLOAKED:
    case EVENT_SYSTEM_MINIMIZEEND:
        if (IsTileable(hwnd) && !Find(hwnd)) {
            AddWin(hwnd, true);
            Retile();
        }
        break;
    case EVENT_OBJECT_DESTROY:
    case EVENT_OBJECT_HIDE:
    case EVENT_OBJECT_CLOAKED:
    case EVENT_SYSTEM_MINIMIZESTART:
        if (Find(hwnd)) {
            Prune();
            Retile();
        }
        break;
    case EVENT_OBJECT_LOCATIONCHANGE: {
        if (hwnd == g_active) UpdateFullscreen();
        Win* w = Find(hwnd);
        if (w && !w->anim && !w->parked) {
            RECT r = GetFrame(hwnd);
            w->margin = Margins(hwnd);
            ApplyRounding(*w, r.right - r.left, r.bottom - r.top);
            if (w->ov) {
                SetWindowPos(w->ov, hwnd, r.left, r.top, r.right - r.left, r.bottom - r.top, SWP_NOACTIVATE);
                InvalidateRect(w->ov, nullptr, FALSE);
            }
            DrawBorders();
        }
        break;
    }
    case EVENT_SYSTEM_FOREGROUND: {
        HWND prev = g_active;
        g_active = hwnd;
        Win* p = prev ? Find(prev) : nullptr;
        Win* w = Find(hwnd);
        if (p) SetOpacity(*p, false);
        if (w) {
            MruTouch(hwnd);
            SetOpacity(*w, true);
            if (w->ov) SetWindowPos(w->ov, hwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            g_lastFocus[w->ws] = hwnd;
            if (w->ws != g_ws) SwitchWs(w->ws, false);
        }
        UpdateFullscreen();
        InvalidateBar();
        DrawBorders();
        break;
    }
    }
}

static const wchar_t* WINLOGON_KEY = L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon";

static int SetShell(bool install) {
    wchar_t self[MAX_PATH];
    GetModuleFileNameW(nullptr, self, MAX_PATH);
    HKEY k;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, WINLOGON_KEY, 0, nullptr, 0, KEY_SET_VALUE, nullptr, &k, nullptr) !=
        ERROR_SUCCESS) {
        MessageBoxW(nullptr, L"Could not open the Winlogon registry key.", L"hypr", MB_OK | MB_ICONERROR);
        return 1;
    }
    if (install) {
        std::wstring v = L"\"" + std::wstring(self) + L"\" --shell";
        RegSetValueExW(k, L"Shell", 0, REG_SZ, (const BYTE*)v.c_str(), (DWORD)((v.size() + 1) * sizeof(wchar_t)));
        MessageBoxW(nullptr, L"hypr is now your shell. Sign out and back in to use it.", L"hypr", MB_OK);
    } else {
        RegDeleteValueW(k, L"Shell");
        MessageBoxW(nullptr, L"Shell restored to explorer. Sign out and back in.", L"hypr", MB_OK);
    }
    RegCloseKey(k);
    return 0;
}

static void StartExplorer() {
    wchar_t cmd[] = L"explorer.exe";
    STARTUPINFOW si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};
    if (CreateProcessW(nullptr, cmd, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
}

static int Supervise() {
    wchar_t self[MAX_PATH];
    GetModuleFileNameW(nullptr, self, MAX_PATH);
    int crashes = 0;
    ULONGLONG windowStart = GetTickCount64();
    for (;;) {
        std::wstring cmd = L"\"" + std::wstring(self) + L"\" --shell-session";
        STARTUPINFOW si{};
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi{};
        if (!CreateProcessW(nullptr, &cmd[0], nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
            StartExplorer();
            return 1;
        }
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD code = 1;
        GetExitCodeProcess(pi.hProcess, &code);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        if (code == 0) {
            StartExplorer();
            return 0;
        }
        ULONGLONG now = GetTickCount64();
        if (now - windowStart > 30000) {
            crashes = 0;
            windowStart = now;
        }
        if (++crashes >= 3) {
            StartExplorer();
            return 1;
        }
    }
}

int WINAPI wWinMain(HINSTANCE inst, HINSTANCE, PWSTR cmdLine, int) {
    SetProcessDPIAware();
    std::wstring args = cmdLine ? cmdLine : L"";
    if (args.find(L"--install-shell") != std::wstring::npos) return SetShell(true);
    if (args.find(L"--uninstall-shell") != std::wstring::npos) return SetShell(false);
    g_shellSession = args.find(L"--shell-session") != std::wstring::npos;
    if (!g_shellSession && args.find(L"--shell") != std::wstring::npos) return Supervise();
    g_win11 = DetectWin11();
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    Gdiplus::GdiplusStartupInput gsi;
    Gdiplus::GdiplusStartup(&g_gdiToken, &gsi, nullptr);

    WNDCLASSW wc{};
    wc.hInstance = inst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpfnWndProc = OvProc;
    wc.lpszClassName = OV_CLASS;
    RegisterClassW(&wc);
    wc.lpfnWndProc = BarProc;
    wc.lpszClassName = BAR_CLASS;
    RegisterClassW(&wc);
    wc.lpfnWndProc = DefWindowProcW;
    wc.lpszClassName = BORDER_CLASS;
    RegisterClassW(&wc);
    wc.lpfnWndProc = WallProc;
    wc.lpszClassName = WALL_CLASS;
    RegisterClassW(&wc);
    wc.lpfnWndProc = SwProc;
    wc.lpszClassName = SW_CLASS;
    RegisterClassW(&wc);
    wc.lpfnWndProc = WidgetProc;
    wc.lpszClassName = WIDGET_CLASS;
    RegisterClassW(&wc);

    std::string path = CfgPath();
    EnsureDefaultConfig(path);
    g_active = GetForegroundWindow();
    g_expAlive = FindWindowW(L"Shell_TrayWnd", nullptr) != nullptr;
    Reload(true);
    Scan();
    Retile();

    g_eventHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_OBJECT_UNCLOAKED, nullptr, EventProc, 0, 0,
                                  WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    g_kbHook = SetWindowsHookExW(WH_KEYBOARD_LL, KbProc, GetModuleHandleW(nullptr), 0);
    SetTimer(nullptr, 0, 500, ReloadTimer);

    MSG msg;
    bool quit = false;
    while (!quit) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                quit = true;
                break;
            }
            if (!msg.hwnd && msg.message == WM_APP) {
                size_t i = (size_t)msg.wParam;
                if (i < g_cfg.binds.size()) {
                    Bind b = g_cfg.binds[i];
                    Dispatch(b);
                }
                continue;
            }
            if (!msg.hwnd && msg.message == WM_APP + 1) {
                int dir = msg.wParam ? -1 : 1;
                if (g_swOpen) SwitcherStep(dir);
                else SwitcherOpen(dir);
                continue;
            }
            if (!msg.hwnd && msg.message == WM_APP + 2) {
                SwitcherCommit();
                continue;
            }
            if (!msg.hwnd && msg.message == WM_APP + 3) {
                SwitcherClose();
                continue;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (quit) break;
        if (Animating()) {
            Step();
            DwmFlush();
        } else {
            WaitMessage();
        }
    }
    UnhookWindowsHookEx(g_kbHook);
    UnhookWinEvent(g_eventHook);
    RestoreAll();
    for (auto& w : g_wins) ReleaseWin(w);
    FreeBorderLayer();
    if (g_wall) DestroyWindow(g_wall);
    if (g_bar) DestroyWindow(g_bar);
    FreeBlur();
    if (g_epv) g_epv->Release();
    Gdiplus::GdiplusShutdown(g_gdiToken);
    return 0;
}
