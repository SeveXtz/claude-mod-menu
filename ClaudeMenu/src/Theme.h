#pragma once
#include <cmath>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"

// Paleta do Claude (tema escuro)
namespace theme {

inline ImVec4 rgb(int r, int g, int b, float a = 1.f) { return ImVec4(r / 255.f, g / 255.f, b / 255.f, a); }
inline ImU32  u32(const ImVec4& c) { return ImGui::ColorConvertFloat4ToU32(c); }

inline const ImVec4 kOrange       = rgb(217, 119,  87);  // #D97757 laranja Claude
inline const ImVec4 kOrangeHover  = rgb(230, 138, 106);
inline const ImVec4 kOrangeActive = rgb(193,  95,  60);
inline const ImVec4 kBg           = rgb( 26,  25,  24);  // fundo
inline const ImVec4 kBg2          = rgb( 31,  30,  29);  // sidebar
inline const ImVec4 kPanel        = rgb( 38,  38,  36);  // cards / header
inline const ImVec4 kPanel2       = rgb( 50,  48,  45);  // campos / hover
inline const ImVec4 kBorder       = rgb( 61,  58,  54);
inline const ImVec4 kText         = rgb(245, 244, 238);
inline const ImVec4 kMuted        = rgb(168, 164, 155);
inline const ImVec4 kEye          = rgb( 20,  20,  19);

inline ImFont* Regular = nullptr;
inline ImFont* Bold    = nullptr;
inline ImFont* BoldFont() { return Bold ? Bold : ImGui::GetFont(); }

inline void Apply() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 0;  s.ChildRounding = 8;  s.FrameRounding = 6;
    s.GrabRounding   = 6;  s.PopupRounding = 6;  s.ScrollbarRounding = 6;
    s.WindowBorderSize = 0; s.ChildBorderSize = 1; s.FrameBorderSize = 0;
    s.FramePadding = ImVec2(10, 6);
    s.ItemSpacing  = ImVec2(8, 8);
    s.GrabMinSize  = 12;
    s.ScrollbarSize = 8;

    ImVec4* c = s.Colors;
    c[ImGuiCol_Text]                 = kText;
    c[ImGuiCol_TextDisabled]         = kMuted;
    c[ImGuiCol_WindowBg]             = kBg;
    c[ImGuiCol_ChildBg]              = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_PopupBg]              = kPanel;
    c[ImGuiCol_Border]               = kBorder;
    c[ImGuiCol_FrameBg]              = kPanel2;
    c[ImGuiCol_FrameBgHovered]       = rgb(60, 57, 53);
    c[ImGuiCol_FrameBgActive]        = rgb(68, 65, 60);
    c[ImGuiCol_SliderGrab]           = kOrange;
    c[ImGuiCol_SliderGrabActive]     = kOrangeHover;
    c[ImGuiCol_CheckMark]            = kOrange;
    c[ImGuiCol_Button]               = kOrange;
    c[ImGuiCol_ButtonHovered]        = kOrangeHover;
    c[ImGuiCol_ButtonActive]         = kOrangeActive;
    c[ImGuiCol_Header]               = kPanel2;
    c[ImGuiCol_HeaderHovered]        = rgb(60, 57, 53);
    c[ImGuiCol_HeaderActive]         = kOrange;
    c[ImGuiCol_Separator]            = kBorder;
    c[ImGuiCol_ScrollbarBg]          = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_ScrollbarGrab]        = kPanel2;
    c[ImGuiCol_ScrollbarGrabHovered] = kBorder;
    c[ImGuiCol_ScrollbarGrabActive]  = kOrange;
    c[ImGuiCol_TextSelectedBg]       = rgb(217, 119, 87, 0.35f);
}

// Mascote do Claude Code em pixel art (# corpo, E olho, . vazio)
inline void DrawClawd(ImDrawList* dl, ImVec2 pos, float px, double time) {
    static const char* art[] = {
        "..#######..",
        "..#E###E#..",
        "###E###E###",
        "###########",
        "..#######..",
        "..#.#.#.#..",
        "..#.#.#.#..",
    };
    const bool blink = std::fmod(time, 4.0) < 0.12;   // pisca a cada 4s
    for (int y = 0; y < 7; ++y)
        for (int x = 0; art[y][x]; ++x) {
            char ch = art[y][x];
            if (ch == '.') continue;
            ImVec4 col = kOrange;
            if (ch == 'E' && !(blink && y == 1)) col = kEye;
            ImVec2 a(pos.x + x * px, pos.y + y * px);
            dl->AddRectFilled(a, ImVec2(a.x + px, a.y + px), u32(col));
        }
}

} // namespace theme
