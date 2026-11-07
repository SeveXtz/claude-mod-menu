#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

// Leitura/escrita externa de memoria + cadeia de ponteiros
// Obs.: compile em x86 para jogos 32-bit e em x64 para jogos 64-bit.
namespace mem {

struct Pointer {
    const wchar_t* module = nullptr;      // ex.: L"game.exe" (nullptr = base absoluta)
    uintptr_t base = 0;                   // offset a partir do modulo
    std::vector<uintptr_t> offsets;       // ex.: { 0x10, 0x48, 0x2C }
    bool valid() const { return base != 0; }
};

class Process {
public:
    DWORD  pid = 0;
    HANDLE h   = nullptr;

    bool Attach(const wchar_t* exeName) {
        Detach();
        pid = FindPid(exeName);
        if (!pid) return false;
        h = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
                        PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!h) pid = 0;
        return h != nullptr;
    }

    void Detach() {
        if (h) CloseHandle(h);
        h = nullptr; pid = 0; modCache_.clear();
    }

    bool Alive() const {
        DWORD code = 0;
        return h && GetExitCodeProcess(h, &code) && code == STILL_ACTIVE;
    }

    template <class T> bool Read(uintptr_t addr, T& out) const {
        return ReadProcessMemory(h, reinterpret_cast<LPCVOID>(addr), &out, sizeof(T), nullptr);
    }
    template <class T> bool Write(uintptr_t addr, const T& value) const {
        return WriteProcessMemory(h, reinterpret_cast<LPVOID>(addr), &value, sizeof(T), nullptr);
    }

    uintptr_t ModuleBase(const wchar_t* name) {
        auto it = modCache_.find(name);
        if (it != modCache_.end()) return it->second;
        uintptr_t base = 0;
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
        if (snap != INVALID_HANDLE_VALUE) {
            MODULEENTRY32W me{ sizeof(me) };
            for (BOOL ok = Module32FirstW(snap, &me); ok; ok = Module32NextW(snap, &me))
                if (!_wcsicmp(me.szModule, name)) { base = reinterpret_cast<uintptr_t>(me.modBaseAddr); break; }
            CloseHandle(snap);
        }
        if (base) modCache_[name] = base;
        return base;
    }

    // modulo + base -> [le] + off1 -> [le] + off2 ... -> endereco final
    uintptr_t Resolve(const Pointer& p) {
        if (!h || !p.valid()) return 0;
        uintptr_t addr = p.base;
        if (p.module) {
            uintptr_t mb = ModuleBase(p.module);
            if (!mb) return 0;
            addr += mb;
        }
        for (uintptr_t off : p.offsets) {
            uintptr_t next = 0;
            if (!Read(addr, next) || !next) return 0;
            addr = next + off;
        }
        return addr;
    }

private:
    std::unordered_map<std::wstring, uintptr_t> modCache_;

    static DWORD FindPid(const wchar_t* exeName) {
        DWORD found = 0;
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snap == INVALID_HANDLE_VALUE) return 0;
        PROCESSENTRY32W pe{ sizeof(pe) };
        for (BOOL ok = Process32FirstW(snap, &pe); ok; ok = Process32NextW(snap, &pe))
            if (!_wcsicmp(pe.szExeFile, exeName)) { found = pe.th32ProcessID; break; }
        CloseHandle(snap);
        return found;
    }
};

} // namespace mem
