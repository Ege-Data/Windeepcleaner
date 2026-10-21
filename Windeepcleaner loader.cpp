#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <wchar.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "/SUBSYSTEM:WINDOWS") // Arkada CMD/Konsol penceresinin açılmasını kesin olarak engeller!

// Rust DLL Fonksiyon İmzası
typedef void(__cdecl* RunCleaningFunc)(bool, bool, bool, bool, bool);

// UI Eleman ID'leri (Ana Pencere)
#define ID_CHK_TEMP     101
#define ID_CHK_SYSTEM   102
#define ID_CHK_DOWNLOAD 103
#define ID_CHK_UPDATE   104
#define ID_CHK_PREFETCH 105
#define ID_BTN_CLEAN    106
#define ID_BTN_SETTINGS 107

// UI Eleman ID'leri (Ayarlar Penceresi)
#define ID_COMBO_LANG   201
#define ID_COMBO_THEME  202
#define ID_BTN_SAVE     203

// Global Değişkenler ve Tema Renkleri
HBRUSH hBrushBg = NULL;
COLORREF bgColor = RGB(30, 30, 30);      // Varsayılan Koyu Tema Arka Plan (#1E1E1E)
COLORREF textColor = RGB(220, 220, 220); // Varsayılan Koyu Tema Metin

HWND hMainWnd = NULL;
HWND hChk1, hChk2, hChk3, hChk4, hChk5, hBtnClean, hBtnSettings, hStatus;
HWND hSettingsWnd = NULL;
HWND hComboLang, hComboTheme, hBtnSave;

wchar_t currentLang[10] = L"tr";   // Varsayılan Dil
wchar_t currentTheme[10] = L"dark"; // Varsayılan Tema

// Dil Metinleri Değişkenleri
wchar_t strTitle[100];
wchar_t strChk1[100], strChk2[100], strChk3[100], strChk4[100], strChk5[100];
wchar_t strBtnClean[100], strBtnSettingsText[100];
wchar_t strStatusReady[100];
wchar_t strConfirmMsg[250], strConfirmTitle[100];
wchar_t strCleaning[100], strSuccess[100], strSuccessMsg[250];
wchar_t strErrDll[100], strErrNotFound[100], strErrTitle[100];
wchar_t strSettingsTitle[100], strSelectLang[100], strSelectTheme[100], strSaveBtn[100];

// Tema Renklerini Ayarlama
void UpdateThemeColors() {
    if (hBrushBg) DeleteObject(hBrushBg);

    if (_wcsicmp(currentTheme, L"light") == 0) {
        bgColor = RGB(245, 245, 245);  // Aydınlık Mod Arka Plan (Açık Gri)
        textColor = RGB(20, 20, 20);   // Aydınlık Mod Metin (Koyu Siyah)
    }
    else {
        bgColor = RGB(30, 30, 30);     // Karanlık Mod Arka Plan (#1E1E1E)
        textColor = RGB(220, 220, 220); // Karanlık Mod Metin (Açık Gri)
    }
    hBrushBg = CreateSolidBrush(bgColor);
}

// Dil Metinlerini Güncelleme Fonksiyonu (TR, EN, DE, FR)
void UpdateLanguageTexts() {
    if (_wcsicmp(currentLang, L"tr") == 0) {
        wcscpy_s(strTitle, 100, L"WinDeepCleaner v1.0.1");
        wcscpy_s(strChk1, 100, L" Kullanıcı Temp Klasörü (%TEMP%)");
        wcscpy_s(strChk2, 100, L" Windows Sistem Temp (C:\\Windows\\Temp)");
        wcscpy_s(strChk3, 100, L" İndirilenler Klasörü (Downloads)");
        wcscpy_s(strChk4, 100, L" Windows Update Kalıntıları");
        wcscpy_s(strChk5, 100, L" Prefetch Önbellek Dosyaları");
        wcscpy_s(strBtnClean, 100, L"Sistemi Temizle");
        wcscpy_s(strBtnSettingsText, 100, L"⚙ Ayarlar");
        wcscpy_s(strStatusReady, 100, L"Hazır - Lütfen temizlenecek alanları seçin.");
        wcscpy_s(strConfirmMsg, 250, L"Seçilen sistem kalıntılarını temizlemek istediğinize emin misiniz?");
        wcscpy_s(strConfirmTitle, 100, L"WinDeepCleaner - Onay");
        wcscpy_s(strCleaning, 100, L"Temizlik yapılıyor, lütfen bekleyin...");
        wcscpy_s(strSuccess, 100, L"✔ Temizlik başarıyla tamamlandı!");
        wcscpy_s(strSuccessMsg, 250, L"Seçilen sistem kalıntıları güvenle temizlendi.");
        wcscpy_s(strErrDll, 100, L"❌ Hata: DLL fonksiyonu bulunamadı.");
        wcscpy_s(strErrNotFound, 100, L"❌ Hata: cleaner.dll dosyası bulunamadı!");
        wcscpy_s(strErrTitle, 100, L"Hata");
        wcscpy_s(strSettingsTitle, 100, L"Ayarlar");
        wcscpy_s(strSelectLang, 100, L"Uygulama Dili:");
        wcscpy_s(strSelectTheme, 100, L"Tema Modu:");
        wcscpy_s(strSaveBtn, 100, L"Kaydet ve Kapat");
    }
    else if (_wcsicmp(currentLang, L"de") == 0) {
        wcscpy_s(strTitle, 100, L"WinDeepCleaner v1.0.1");
        wcscpy_s(strChk1, 100, L" Benutzer-Temp-Ordner (%TEMP%)");
        wcscpy_s(strChk2, 100, L" Windows System-Temp (C:\\Windows\\Temp)");
        wcscpy_s(strChk3, 100, L" Downloads-Ordner");
        wcscpy_s(strChk4, 100, L" Windows Update Cache");
        wcscpy_s(strChk5, 100, L" Prefetch-Cache-Dateien");
        wcscpy_s(strBtnClean, 100, L"System bereinigen");
        wcscpy_s(strBtnSettingsText, 100, L"⚙ Einstellungen");
        wcscpy_s(strStatusReady, 100, L"Bereit - Bitte Elemente auswählen.");
        wcscpy_s(strConfirmMsg, 250, L"Möchten Sie die ausgewählten Elemente wirklich bereinigen?");
        wcscpy_s(strConfirmTitle, 100, L"WinDeepCleaner - Bestätigung");
        wcscpy_s(strCleaning, 100, L"Bereinigung läuft, bitte warten...");
        wcscpy_s(strSuccess, 100, L"✔ Bereinigung erfolgreich abgeschlossen!");
        wcscpy_s(strSuccessMsg, 250, L"Die ausgewählten Caches wurden sicher bereinigt.");
        wcscpy_s(strErrDll, 100, L"❌ Fehler: DLL-Funktion nicht gefunden.");
        wcscpy_s(strErrNotFound, 100, L"❌ Fehler: cleaner.dll nicht gefunden!");
        wcscpy_s(strErrTitle, 100, L"Fehler");
        wcscpy_s(strSettingsTitle, 100, L"Einstellungen");
        wcscpy_s(strSelectLang, 100, L"Sprache wählen:");
        wcscpy_s(strSelectTheme, 100, L"Themenmodus:");
        wcscpy_s(strSaveBtn, 100, L"Speichern & Schließen");
    }
    else if (_wcsicmp(currentLang, L"fr") == 0) {
        wcscpy_s(strTitle, 100, L"WinDeepCleaner v1.0.1");
        wcscpy_s(strChk1, 100, L" Dossier Temp utilisateur (%TEMP%)");
        wcscpy_s(strChk2, 100, L" Temp système Windows (C:\\Windows\\Temp)");
        wcscpy_s(strChk3, 100, L" Dossier Téléchargements");
        wcscpy_s(strChk4, 100, L" Cache Windows Update");
        wcscpy_s(strChk5, 100, L" Fichiers Cache Prefetch");
        wcscpy_s(strBtnClean, 100, L"Nettoyer le système");
        wcscpy_s(strBtnSettingsText, 100, L"⚙ Paramètres");
        wcscpy_s(strStatusReady, 100, L"Prêt - Veuillez sélectionner les éléments.");
        wcscpy_s(strConfirmMsg, 250, L"Êtes-vous sûr de vouloir nettoyer les éléments sélectionnés ?");
        wcscpy_s(strConfirmTitle, 100, L"WinDeepCleaner - Confirmation");
        wcscpy_s(strCleaning, 100, L"Nettoyage en cours, veuillez patienter...");
        wcscpy_s(strSuccess, 100, L"✔ Nettoyage terminé avec succès !");
        wcscpy_s(strSuccessMsg, 250, L"Les caches sélectionnés ont été nettoyés.");
        wcscpy_s(strErrDll, 100, L"❌ Erreur : Fonction DLL introuvable.");
        wcscpy_s(strErrNotFound, 100, L"❌ Erreur : cleaner.dll introuvable !");
        wcscpy_s(strErrTitle, 100, L"Erreur");
        wcscpy_s(strSettingsTitle, 100, L"Paramètres");
        wcscpy_s(strSelectLang, 100, L"Choisir la langue :");
        wcscpy_s(strSelectTheme, 100, L"Mode de thème :");
        wcscpy_s(strSaveBtn, 100, L"Enregistrer & Fermer");
    }
    else {
        wcscpy_s(strTitle, 100, L"WinDeepCleaner v1.0.1");
        wcscpy_s(strChk1, 100, L" User Temp Folder (%TEMP%)");
        wcscpy_s(strChk2, 100, L" Windows System Temp (C:\\Windows\\Temp)");
        wcscpy_s(strChk3, 100, L" Downloads Folder");
        wcscpy_s(strChk4, 100, L" Windows Update Cache");
        wcscpy_s(strChk5, 100, L" Prefetch Cache Files");
        wcscpy_s(strBtnClean, 100, L"Clean System");
        wcscpy_s(strBtnSettingsText, 100, L"⚙ Settings");
        wcscpy_s(strStatusReady, 100, L"Ready - Please select items to clean.");
        wcscpy_s(strConfirmMsg, 250, L"Are you sure you want to clean the selected system items?");
        wcscpy_s(strConfirmTitle, 100, L"WinDeepCleaner - Confirmation");
        wcscpy_s(strCleaning, 100, L"Cleaning in progress, please wait...");
        wcscpy_s(strSuccess, 100, L"✔ Cleaning completed successfully!");
        wcscpy_s(strSuccessMsg, 250, L"Selected system caches have been safely cleaned.");
        wcscpy_s(strErrDll, 100, L"❌ Error: DLL function not found.");
        wcscpy_s(strErrNotFound, 100, L"❌ Error: cleaner.dll not found!");
        wcscpy_s(strErrTitle, 100, L"Error");
        wcscpy_s(strSettingsTitle, 100, L"Settings");
        wcscpy_s(strSelectLang, 100, L"Select Language:");
        wcscpy_s(strSelectTheme, 100, L"Theme Mode:");
        wcscpy_s(strSaveBtn, 100, L"Save & Close");
    }
}

void LoadSettings() {
    GetPrivateProfileStringW(L"Settings", L"Language", L"tr", currentLang, 10, L".\\settings.cfg");
    GetPrivateProfileStringW(L"Settings", L"Theme", L"dark", currentTheme, 10, L".\\settings.cfg");
    UpdateThemeColors();
    UpdateLanguageTexts();
}

void SaveSettings() {
    WritePrivateProfileStringW(L"Settings", L"Language", currentLang, L".\\settings.cfg");
    WritePrivateProfileStringW(L"Settings", L"Theme", currentTheme, L".\\settings.cfg");
}

// Yönetici Yetki Kontrolü
BOOL IsRunAsAdmin() {
    BOOL fIsRunAsAdmin = FALSE;
    PSID pAdminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pAdminGroup)) {
        CheckTokenMembership(NULL, pAdminGroup, &fIsRunAsAdmin);
        FreeSid(pAdminGroup);
    }
    return fIsRunAsAdmin;
}

void RelaunchAsAdmin() {
    wchar_t szPath[MAX_PATH];
    if (GetModuleFileNameW(NULL, szPath, MAX_PATH)) {
        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.lpVerb = L"runas";
        sei.lpFile = szPath;
        sei.hwnd = NULL;
        sei.nShow = SW_NORMAL;

        if (ShellExecuteExW(&sei)) {
            ExitProcess(0);
        }
    }
}

// -----------------------------------------------------------------------------
// AYARLAR PENCERESİ PROC
// -----------------------------------------------------------------------------
LRESULT CALLBACK SettingsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        // Dil Seçimi Etiketi ve Combobox
        CreateWindowW(L"STATIC", strSelectLang, WS_VISIBLE | WS_CHILD, 20, 20, 230, 20, hwnd, NULL, NULL, NULL);
        hComboLang = CreateWindowW(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS, 20, 45, 230, 150, hwnd, (HMENU)ID_COMBO_LANG, NULL, NULL);

        SendMessageW(hComboLang, CB_ADDSTRING, 0, (LPARAM)L"Türkçe (TR)");
        SendMessageW(hComboLang, CB_ADDSTRING, 0, (LPARAM)L"English (EN)");
        SendMessageW(hComboLang, CB_ADDSTRING, 0, (LPARAM)L"Deutsch (DE)");
        SendMessageW(hComboLang, CB_ADDSTRING, 0, (LPARAM)L"Français (FR)");

        if (_wcsicmp(currentLang, L"tr") == 0) SendMessageW(hComboLang, CB_SETCURSEL, 0, 0);
        else if (_wcsicmp(currentLang, L"en") == 0) SendMessageW(hComboLang, CB_SETCURSEL, 1, 0);
        else if (_wcsicmp(currentLang, L"de") == 0) SendMessageW(hComboLang, CB_SETCURSEL, 2, 0);
        else if (_wcsicmp(currentLang, L"fr") == 0) SendMessageW(hComboLang, CB_SETCURSEL, 3, 0);

        // Tema Seçimi Etiketi ve Combobox
        CreateWindowW(L"STATIC", strSelectTheme, WS_VISIBLE | WS_CHILD, 20, 85, 230, 20, hwnd, NULL, NULL, NULL);
        hComboTheme = CreateWindowW(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS, 20, 110, 230, 100, hwnd, (HMENU)ID_COMBO_THEME, NULL, NULL);

        SendMessageW(hComboTheme, CB_ADDSTRING, 0, (LPARAM)L"Karanlık Mod (Dark)");
        SendMessageW(hComboTheme, CB_ADDSTRING, 0, (LPARAM)L"Aydınlık Mod (Light)");

        if (_wcsicmp(currentTheme, L"light") == 0) SendMessageW(hComboTheme, CB_SETCURSEL, 1, 0);
        else SendMessageW(hComboTheme, CB_SETCURSEL, 0, 0);

        // Kaydet Butonu
        hBtnSave = CreateWindowW(L"BUTTON", strSaveBtn, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 20, 155, 230, 35, hwnd, (HMENU)ID_BTN_SAVE, NULL, NULL);
        break;
    }

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORBTN: {
        HDC hdc = (HDC)wParam;
        SetTextColor(hdc, textColor);
        SetBkColor(hdc, bgColor);
        return (INT_PTR)hBrushBg;
    }

    case WM_COMMAND: {
        if (LOWORD(wParam) == ID_BTN_SAVE) {
            // Dili Al
            int selLang = (int)SendMessageW(hComboLang, CB_GETCURSEL, 0, 0);
            if (selLang == 0) wcscpy_s(currentLang, 10, L"tr");
            else if (selLang == 1) wcscpy_s(currentLang, 10, L"en");
            else if (selLang == 2) wcscpy_s(currentLang, 10, L"de");
            else if (selLang == 3) wcscpy_s(currentLang, 10, L"fr");

            // Temayı Al
            int selTheme = (int)SendMessageW(hComboTheme, CB_GETCURSEL, 0, 0);
            if (selTheme == 1) wcscpy_s(currentTheme, 10, L"light");
            else wcscpy_s(currentTheme, 10, L"dark");

            SaveSettings();
            UpdateThemeColors();
            UpdateLanguageTexts();

            // Ana pencereyi ve kontrolleri anında güncelle
            SetWindowTextW(hMainWnd, strTitle);
            SetWindowTextW(hChk1, strChk1);
            SetWindowTextW(hChk2, strChk2);
            SetWindowTextW(hChk3, strChk3);
            SetWindowTextW(hChk4, strChk4);
            SetWindowTextW(hChk5, strChk5);
            SetWindowTextW(hBtnClean, strBtnClean);
            SetWindowTextW(hBtnSettings, strBtnSettingsText);
            SetWindowTextW(hStatus, strStatusReady);

            InvalidateRect(hMainWnd, NULL, TRUE); // Ana pencereyi yeniden boya

            DestroyWindow(hwnd);
            hSettingsWnd = NULL;
        }
        break;
    }

    case WM_DESTROY:
        hSettingsWnd = NULL;
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// -----------------------------------------------------------------------------
// ANA PENCERE PROC
// -----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        UpdateThemeColors();

        hChk1 = CreateWindowW(L"BUTTON", strChk1, WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 25, 20, 380, 25, hwnd, (HMENU)ID_CHK_TEMP, NULL, NULL);
        hChk2 = CreateWindowW(L"BUTTON", strChk2, WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 25, 50, 380, 25, hwnd, (HMENU)ID_CHK_SYSTEM, NULL, NULL);
        hChk3 = CreateWindowW(L"BUTTON", strChk3, WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 25, 80, 380, 25, hwnd, (HMENU)ID_CHK_DOWNLOAD, NULL, NULL);
        hChk4 = CreateWindowW(L"BUTTON", strChk4, WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 25, 110, 380, 25, hwnd, (HMENU)ID_CHK_UPDATE, NULL, NULL);
        hChk5 = CreateWindowW(L"BUTTON", strChk5, WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 25, 140, 380, 25, hwnd, (HMENU)ID_CHK_PREFETCH, NULL, NULL);

        SendMessage(hChk1, BM_SETCHECK, BST_CHECKED, 0);
        SendMessage(hChk2, BM_SETCHECK, BST_CHECKED, 0);
        SendMessage(hChk3, BM_SETCHECK, BST_CHECKED, 0);
        SendMessage(hChk4, BM_SETCHECK, BST_CHECKED, 0);
        SendMessage(hChk5, BM_SETCHECK, BST_CHECKED, 0);

        hBtnClean = CreateWindowW(L"BUTTON", strBtnClean, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 25, 185, 380, 35, hwnd, (HMENU)ID_BTN_CLEAN, NULL, NULL);
        hBtnSettings = CreateWindowW(L"BUTTON", strBtnSettingsText, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 25, 230, 380, 30, hwnd, (HMENU)ID_BTN_SETTINGS, NULL, NULL);
        hStatus = CreateWindowW(L"STATIC", strStatusReady, WS_VISIBLE | WS_CHILD | SS_CENTER, 25, 275, 380, 25, hwnd, NULL, NULL, NULL);
        break;
    }

    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN: {
        HDC hdc = (HDC)wParam;
        SetTextColor(hdc, textColor);
        SetBkColor(hdc, bgColor);
        return (INT_PTR)hBrushBg;
    }

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);

        // Ayarlar Penceresini Aç
        if (wmId == ID_BTN_SETTINGS) {
            if (!hSettingsWnd) {
                WNDCLASSW wcSet = { 0 };
                wcSet.lpfnWndProc = SettingsWndProc;
                wcSet.hInstance = GetModuleHandle(NULL);
                wcSet.lpszClassName = L"WinDeepCleanerSettingsClass";
                wcSet.hbrBackground = CreateSolidBrush(bgColor);
                wcSet.hCursor = LoadCursor(NULL, IDC_ARROW);
                RegisterClassW(&wcSet);

                hSettingsWnd = CreateWindowExW(
                    0, L"WinDeepCleanerSettingsClass", strSettingsTitle,
                    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                    CW_USEDEFAULT, CW_USEDEFAULT, 285, 245,
                    hwnd, NULL, GetModuleHandle(NULL), NULL
                );
                ShowWindow(hSettingsWnd, SW_SHOW);
            }
            else {
                SetForegroundWindow(hSettingsWnd);
            }
            break;
        }

        // Sistemi Temizle
        if (wmId == ID_BTN_CLEAN) {
            int confirm = MessageBoxW(hwnd, strConfirmMsg, strConfirmTitle, MB_YESNO | MB_ICONQUESTION);
            if (confirm != IDYES) {
                break;
            }

            bool c1 = (SendMessage(hChk1, BM_GETCHECK, 0, 0) == BST_CHECKED);
            bool c2 = (SendMessage(hChk2, BM_GETCHECK, 0, 0) == BST_CHECKED);
            bool c3 = (SendMessage(hChk3, BM_GETCHECK, 0, 0) == BST_CHECKED);
            bool c4 = (SendMessage(hChk4, BM_GETCHECK, 0, 0) == BST_CHECKED);
            bool c5 = (SendMessage(hChk5, BM_GETCHECK, 0, 0) == BST_CHECKED);

            SetWindowTextW(hStatus, strCleaning);
            EnableWindow(hBtnClean, FALSE);
            EnableWindow(hBtnSettings, FALSE);

            HMODULE hDll = LoadLibraryW(L"cleaner.dll");
            if (hDll) {
                RunCleaningFunc RunCleaning = (RunCleaningFunc)GetProcAddress(hDll, "RunCleaning");
                if (RunCleaning) {
                    RunCleaning(c1, c2, c3, c4, c5);
                    SetWindowTextW(hStatus, strSuccess);
                    MessageBoxW(hwnd, strSuccessMsg, strTitle, MB_OK | MB_ICONINFORMATION);
                }
                else {
                    SetWindowTextW(hStatus, strErrDll);
                }
                FreeLibrary(hDll);
            }
            else {
                SetWindowTextW(hStatus, strErrNotFound);
                MessageBoxW(hwnd, L"cleaner.dll dosyası loader.exe ile aynı dizinde bulunmalıdır!", strErrTitle, MB_OK | MB_ICONERROR);
            }

            EnableWindow(hBtnClean, TRUE);
            EnableWindow(hBtnSettings, TRUE);
        }
        break;
    }

    case WM_DESTROY:
        if (hBrushBg) DeleteObject(hBrushBg);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    if (!IsRunAsAdmin()) {
        RelaunchAsAdmin();
        return 0;
    }

    LoadSettings();

    const wchar_t CLASS_NAME[] = L"WinDeepCleanerDark";

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(bgColor);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassW(&wc);

    hMainWnd = CreateWindowExW(
        0, CLASS_NAME, strTitle,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 445, 345,
        NULL, NULL, hInstance, NULL
    );

    if (!hMainWnd) return 0;

    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
