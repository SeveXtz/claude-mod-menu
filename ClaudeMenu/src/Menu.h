#pragma once
#include <windows.h>
#include <cmath>
#include <cstdio>
#include <span>
#include "imgui.h"
#include "Theme.h"
#include "Memory.h"

namespace menu {
using namespace theme;

struct Feature {
    const char*  label;
    const char*  desc;
    bool         isFloat;      // true = escreve float, false = escreve int
    float        value, vmin, vmax;
    mem::Pointer ptr;          // cadeia de ponteiros (veja abaixo)
    bool         on = false;   // ligado = congela o valor (escreve todo frame)
};

// =====================================================================
//  EDITE AQUI: coloque seus ponteiros no campo ptr de cada item.
//  Formato: { L"modulo.exe", 0xBASE, { 0xOFF1, 0xOFF2, ... } }
//  Exemplo: { L"game.exe", 0x0123ABC0, { 0x10, 0x48, 0x2C } }
//  Enquanto ptr estiver vazio ({}), o item mostra "sem ponteiro".
// =====================================================================
inline Feature g_player[] = {
    { "Vida infinita",    "Congela a vida no valor escolhido",       false, 100.f, 1.f, 9999.f, {} },
    { "Munição infinita", "Mantém o pente sempre cheio",             false, 999.f, 1.f, 9999.f, {} },
    { "Velocidade",       "Multiplicador de velocidade do jogador",  true,  1.f,   0.5f, 10.f,  {} },
};
inline Feature g_world[] = {
    { "Gravidade",        "Valor da gravidade do mundo",             true,  9.8f,  0.f,  30.f,  {} },
    { "Escala de tempo",  "Acelera ou desacelera o jogo",            true,  1.f,   0.1f, 5.f,   {} },
};
inline Feature g_misc[] = {
    { "Dinheiro",         "Define a quantidade de dinheiro",         false, 50000.f, 0.f, 999999.f, {} },
};

struct Tab { const char* name; std::span<Feature> items; };
inline Tab g_tabs[] = {
    { "Jogador", g_player },
    { "Mundo",   g_world  },
    { "Extras",  g_misc   },
    { "Config",  {}       },
};

inline constexpr float kHeaderH = 64.f;
inline constexpr float kFooterH = 30.f;
inline constexpr float kSideW   = 140.f;

inline mem::Process g_proc;
inline int  g_tab = 0;
inline bool g_topmost = true;
inline char g_procName[64] = "game.exe";
inline char g_status[128]  = "";

// ---------------------------------------------------------------------
// Aplica os valores congelados. Chamado todo loop (mesmo com menu oculto).
inline void Tick() {
    if (g_proc.h && !g_proc.Alive()) {
        g_proc.Detach();
        snprintf(g_status, sizeof(g_status), "O processo foi fechado.");
    }
    if (!g_proc.h) return;
    for (auto& tab : g_tabs)
        for (auto& f : tab.items) {
            if (!f.on || !f.ptr.valid()) continue;
            uintptr_t addr = g_proc.Resolve(f.ptr);
            if (!addr) continue;
            if (f.isFloat) g_proc.Write<float>(addr, f.value);
            else           g_proc.Write<int>(addr, static_cast<int>(f.value));
        }
}

// ---------------------------------------------------------------------
// Widgets
inline float ToggleWidth() { return ImGui::GetFontSize() * 2.2f; }

inline bool Toggle(const char* id, bool* v) {
    const float h = ImGui::GetFontSize() * 1.2f, w = ToggleWidth();
    ImVec2 p = ImGui::GetCursorScreenPos();
    bool pressed = ImGui::InvisibleButton(id, ImVec2(w, h));
    if (pressed) *v = !*v;

    float* t = ImGui::GetStateStorage()->GetFloatRef(ImGui::GetItemID(), *v ? 1.f : 0.f);
    *t += ((*v ? 1.f : 0.f) - *t) * (std::fmin)(1.f, ImGui::GetIO().DeltaTime * 14.f);

    ImVec4 off = ImGui::IsItemHovered() ? kBorder : kPanel2, c;
    c.x = off.x + (kOrange.x - off.x) * *t;
    c.y = off.y + (kOrange.y - off.y) * *t;
    c.z = off.z + (kOrange.z - off.z) * *t;
    c.w = 1.f;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(p, p + ImVec2(w, h), u32(c), h * 0.5f);
    float r = h * 0.5f - 3.f;
    dl->AddCircleFilled(ImVec2(p.x + h * 0.5f + (w - h) * *t, p.y + h * 0.5f), r, u32(kText));
    return pressed;
}

inline void Tag(const char* text) {
    ImVec2 ts = ImGui::CalcTextSize(text);
    ImVec2 p  = ImGui::GetCursorScreenPos();
    ImVec2 sz(ts.x + 14, ts.y + 2);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(p, p + sz, u32(kPanel2), sz.y * 0.5f);
    dl->AddText(ImVec2(p.x + 7, p.y + 1), u32(kMuted), text);
    ImGui::Dummy(sz);
}

inline bool SideTab(const char* label, int idx) {
    const bool active = g_tab == idx;
    ImGui::SetCursorPosX(10);
    ImVec2 p  = ImGui::GetCursorScreenPos();
    ImVec2 sz(kSideW - 20, 34);
    ImGui::PushID(idx);
    bool pressed = ImGui::InvisibleButton("##tab", sz);
    ImGui::PopID();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    if (active || ImGui::IsItemHovered())
        dl->AddRectFilled(p, p + sz, u32(active ? kPanel2 : kPanel), 6);
    if (active)
        dl->AddRectFilled(ImVec2(p.x, p.y + 8), ImVec2(p.x + 3, p.y + sz.y - 8), u32(kOrange), 2);
    dl->AddText(ImVec2(p.x + 14, p.y + (sz.y - ImGui::GetFontSize()) * 0.5f),
                u32(active ? kText : kMuted), label);
    if (pressed) g_tab = idx;
    return pressed;
}

inline bool CloseButton() {
    ImVec2 p = ImGui::GetCursorScreenPos(), sz(30, 30);
    bool pressed = ImGui::InvisibleButton("##close", sz);
    bool hov = ImGui::IsItemHovered();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    if (hov) dl->AddRectFilled(p, p + sz, u32(kPanel2), 6);
    ImU32 col = u32(hov ? kOrange : kMuted);
    ImVec2 c = p + sz * 0.5f; float s = 5.f;
    dl->AddLine(c - ImVec2(s, s), c + ImVec2(s, s), col, 1.8f);
    dl->AddLine(c + ImVec2(-s, s), c + ImVec2(s, -s), col, 1.8f);
    return pressed;
}

inline void SectionTitle(const char* title, const char* sub) {
    ImGui::PushFont(BoldFont());
    ImGui::TextUnformatted(title);
    ImGui::PopFont();
    if (sub) {
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, kMuted);
        ImGui::TextUnformatted(sub);
        ImGui::PopStyleColor();
    }
    ImGui::Spacing();
}

inline void FeatureCard(Feature& f, int i) {
    ImGui::PushID(i);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, kPanel);
    ImGui::PushStyleColor(ImGuiCol_Border, f.on ? rgb(217, 119, 87, 0.55f) : kBorder);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14, 12));
    ImGui::BeginChild("##card", ImVec2(0, 0),
                      ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding,
                      ImGuiWindowFlags_NoScrollbar);

    const float x0 = ImGui::GetCursorPosX();
    const float avail = ImGui::GetContentRegionAvail().x;
    ImGui::TextUnformatted(f.label);
    if (!f.ptr.valid()) { ImGui::SameLine(); Tag("sem ponteiro"); }
    ImGui::SameLine(x0 + avail - ToggleWidth());
    Toggle("##on", &f.on);

    ImGui::PushStyleColor(ImGuiCol_Text, kMuted);
    ImGui::TextUnformatted(f.desc);
    ImGui::PopStyleColor();

    ImGui::SetNextItemWidth(-FLT_MIN);
    if (f.isFloat) {
        ImGui::SliderFloat("##val", &f.value, f.vmin, f.vmax, "%.1f");
    } else {
        int v = static_cast<int>(f.value);
        if (ImGui::SliderInt("##val", &v, static_cast<int>(f.vmin), static_cast<int>(f.vmax)))
            f.value = static_cast<float>(v);
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
    ImGui::PopID();
}

inline void ConfigPage(HWND hwnd) {
    SectionTitle("Processo", "alvo da escrita de memória");
    ImGui::SetNextItemWidth(220);
    ImGui::InputText("##proc", g_procName, sizeof(g_procName));
    ImGui::SameLine();
    if (!g_proc.h) {
        if (ImGui::Button("Conectar")) {
            wchar_t w[64]{};
            MultiByteToWideChar(CP_UTF8, 0, g_procName, -1, w, 64);
            if (g_proc.Attach(w)) snprintf(g_status, sizeof(g_status), "Conectado (PID %lu)", g_proc.pid);
            else snprintf(g_status, sizeof(g_status), "Processo não encontrado ou sem permissão.");
        }
    } else if (ImGui::Button("Desconectar")) {
        g_proc.Detach();
        snprintf(g_status, sizeof(g_status), "Desconectado.");
    }
    if (g_status[0]) {
        ImGui::PushStyleColor(ImGuiCol_Text, kMuted);
        ImGui::TextUnformatted(g_status);
        ImGui::PopStyleColor();
    }

    ImGui::Dummy(ImVec2(0, 10));
    SectionTitle("Janela", nullptr);
    const float x0 = ImGui::GetCursorPosX(), avail = ImGui::GetContentRegionAvail().x;
    ImGui::TextUnformatted("Sempre no topo");
    ImGui::SameLine(x0 + avail - ToggleWidth());
    if (Toggle("##top", &g_topmost))
        SetWindowPos(hwnd, g_topmost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    ImGui::TextUnformatted("Mostrar / ocultar");
    ImGui::SameLine(x0 + avail - ImGui::CalcTextSize("INSERT").x - 14);
    Tag("INSERT");
}

// ---------------------------------------------------------------------
// Desenha o menu. Retorna false quando o usuario clica em fechar.
inline bool Render(HWND hwnd) {
    bool keepOpen = true;
    ImGuiIO& io = ImGui::GetIO();
    const float W = io.DisplaySize.x, H = io.DisplaySize.y;
    const double t = ImGui::GetTime();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##claude", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::PopStyleVar();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const ImVec2 o = ImGui::GetWindowPos();

    // ---- Header ----
    dl->AddRectFilled(o, o + ImVec2(W, kHeaderH), u32(kPanel));
    dl->AddRectFilled(o + ImVec2(0, kHeaderH - 2), o + ImVec2(W, kHeaderH), u32(kOrange));
    const float bob = static_cast<float>(std::sin(t * 2.5)) * 1.5f;
    DrawClawd(dl, o + ImVec2(18, 17 + bob), 4.f, t);
    dl->AddText(BoldFont(), BoldFont()->FontSize, o + ImVec2(78, 13), u32(kText), "Claude Code");
    dl->AddText(o + ImVec2(78, 34), u32(kMuted), "mod menu");

    // status
    const bool attached = g_proc.h != nullptr;
    const char* st = attached ? g_procName : "desconectado";
    ImVec2 ts = ImGui::CalcTextSize(st);
    ImVec2 pillSz(ts.x + 30, 24);
    ImVec2 pill = o + ImVec2(W - 52 - pillSz.x, (kHeaderH - pillSz.y) * 0.5f);
    dl->AddRectFilled(pill, pill + pillSz, u32(kPanel2), 12);
    dl->AddCircleFilled(pill + ImVec2(13, 12), 4, u32(attached ? kOrange : kBorder));
    dl->AddText(pill + ImVec2(22, (pillSz.y - ts.y) * 0.5f), u32(attached ? kText : kMuted), st);

    ImGui::SetCursorPos(ImVec2(W - 44, (kHeaderH - 30) * 0.5f));
    if (CloseButton()) keepOpen = false;

    // ---- Sidebar ----
    const float bodyH = H - kHeaderH - kFooterH;
    ImGui::SetCursorPos(ImVec2(0, kHeaderH));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, kBg2);
    ImGui::BeginChild("##side", ImVec2(kSideW, bodyH), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    ImGui::Dummy(ImVec2(0, 6));
    for (int i = 0; i < IM_ARRAYSIZE(g_tabs); ++i) SideTab(g_tabs[i].name, i);
    ImGui::EndChild();
    ImGui::PopStyleColor();
    dl->AddLine(o + ImVec2(kSideW, kHeaderH), o + ImVec2(kSideW, kHeaderH + bodyH), u32(kBorder));

    // ---- Conteudo ----
    ImGui::SetCursorPos(ImVec2(kSideW + 1, kHeaderH));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(18, 16));
    ImGui::BeginChild("##content", ImVec2(W - kSideW - 1, bodyH), ImGuiChildFlags_AlwaysUseWindowPadding);
    ImGui::PopStyleVar();
    Tab& tab = g_tabs[g_tab];
    if (tab.items.empty()) {
        ConfigPage(hwnd);
    } else {
        char sub[32];
        snprintf(sub, sizeof(sub), "%d opções", static_cast<int>(tab.items.size()));
        SectionTitle(tab.name, sub);
        for (int i = 0; i < static_cast<int>(tab.items.size()); ++i) FeatureCard(tab.items[i], i);
    }
    ImGui::EndChild();

    // ---- Footer ----
    const ImVec2 f0 = o + ImVec2(0, H - kFooterH);
    dl->AddRectFilled(f0, o + ImVec2(W, H), u32(kPanel));
    dl->AddLine(f0, f0 + ImVec2(W, 0), u32(kBorder));
    const float fy = f0.y + (kFooterH - ImGui::GetFontSize()) * 0.5f;
    dl->AddText(ImVec2(f0.x + 14, fy), u32(kMuted), "INSERT  ocultar / mostrar");
    const char* ver = "v0.1";
    dl->AddText(ImVec2(o.x + W - 14 - ImGui::CalcTextSize(ver).x, fy), u32(kMuted), ver);

    // borda externa
    ImGui::GetForegroundDrawList()->AddRect(o, o + ImVec2(W, H), u32(kBorder));

    ImGui::End();
    return keepOpen;
}

} // namespace menu
