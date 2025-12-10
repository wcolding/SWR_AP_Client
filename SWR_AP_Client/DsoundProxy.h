#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

HMODULE originalDsound = nullptr;

struct dsound_dll {
    FARPROC _DirectSoundCaptureCreate;
    FARPROC _DirectSoundCaptureCreate8;
    FARPROC _DirectSoundCaptureEnumerateA;
    FARPROC _DirectSoundCaptureEnumerateW;
    FARPROC _DirectSoundCreate;
    FARPROC _DirectSoundCreate8;
    FARPROC _DirectSoundEnumerateA;
    FARPROC _DirectSoundEnumerateW;
    FARPROC _DirectSoundFullDuplexCreate;
    FARPROC _DllCanUnloadNow;
    FARPROC _DllGetClassObject;
    FARPROC _GetDeviceID;

    void Setup() {
        WCHAR originalDsoundPath[MAX_PATH];
        GetEnvironmentVariableW(L"windir", originalDsoundPath, MAX_PATH);
        std::wstring fullDsoundPath = std::wstring(originalDsoundPath);
        fullDsoundPath += L"\\System32\\dsound.dll";
        
        originalDsound = LoadLibrary(fullDsoundPath.c_str());
        if (originalDsound) {
            _DirectSoundCaptureCreate = GetProcAddress(originalDsound, "DirectSoundCaptureCreate");
            _DirectSoundCaptureCreate8 = GetProcAddress(originalDsound, "DirectSoundCaptureCreate8");
            _DirectSoundCaptureEnumerateA = GetProcAddress(originalDsound, "DirectSoundCaptureEnumerateA");
            _DirectSoundCaptureEnumerateW = GetProcAddress(originalDsound, "DirectSoundCaptureEnumerateW");
            _DirectSoundCreate = GetProcAddress(originalDsound, "DirectSoundCreate");
            _DirectSoundCreate8 = GetProcAddress(originalDsound, "DirectSoundCreate8");
            _DirectSoundEnumerateA = GetProcAddress(originalDsound, "DirectSoundEnumerateA");
            _DirectSoundEnumerateW = GetProcAddress(originalDsound, "DirectSoundEnumerateW");
            _DirectSoundFullDuplexCreate = GetProcAddress(originalDsound, "DirectSoundFullDuplexCreate");
            _DllCanUnloadNow = GetProcAddress(originalDsound, "DllCanUnloadNow");
            _DllGetClassObject = GetProcAddress(originalDsound, "DllGetClassObject");
            _GetDeviceID = GetProcAddress(originalDsound, "GetDeviceID");
        }
    }
} originalDllCalls;

__declspec(naked) void DirectSoundCaptureCreate() {
    _asm jmp[originalDllCalls._DirectSoundCaptureCreate]
}

__declspec(naked) void DirectSoundCaptureCreate8() {
    _asm jmp[originalDllCalls._DirectSoundCaptureCreate8]
}

__declspec(naked) void DirectSoundCaptureEnumerateA() {
    _asm jmp[originalDllCalls._DirectSoundCaptureEnumerateA]
}

__declspec(naked) void DirectSoundCaptureEnumerateW() {
    _asm jmp[originalDllCalls._DirectSoundCaptureEnumerateW]
}

__declspec(naked) void DirectSoundCreate() {
    _asm jmp[originalDllCalls._DirectSoundCreate]
}

__declspec(naked) void DirectSoundCreate8() {
    _asm jmp[originalDllCalls._DirectSoundCreate8]
}

__declspec(naked) void DirectSoundEnumerateA() {
    _asm jmp[originalDllCalls._DirectSoundEnumerateA]
}

__declspec(naked) void DirectSoundEnumerateW() {
    _asm jmp[originalDllCalls._DirectSoundEnumerateW]
}

__declspec(naked) void DirectSoundFullDuplexCreate() {
    _asm jmp[originalDllCalls._DirectSoundFullDuplexCreate]
}

__declspec(naked) void DllCanUnloadNow() {
    _asm jmp[originalDllCalls._DllCanUnloadNow]
}

__declspec(naked) void DllGetClassObject() {
    _asm jmp[originalDllCalls._DllGetClassObject]
}

__declspec(naked) void GetDeviceID() {
    _asm jmp[originalDllCalls._GetDeviceID]
}