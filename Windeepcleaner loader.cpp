#include <windows.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <shlobj.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

// Global Değişkenler
HWND hMainWnd = NULL;
HWND chkWinTemp = NULL, chkSysTemp = NULL, chkDownloads = NULL, chkSoftwareDist = NULL, chkPrefetch = NULL;
HWND btnClean = NULL, btnBack = NULL, comboLang = NULL, btnTheme = NULL;
HBRUSH hDarkBrush = NULL;
bool isDarkMode = true;
std::string currentLang = "en";

struct Translations {
    std::wstring title;
    std::wstring lblWinTemp;
    std::wstring lblSysTemp;
    std::wstring lblDownloads;
    std::wstring lblSoftDist;
    std::wstring lblPrefetch;
    std::wstring btnCleanText;
    std::wstring btnBackText;
    std::wstring confirmTitle;
    std::wstring confirmMsg;
    std::wstring successTitle;
    std::wstring successPrefix;
};

std::wstring FormatBytes(unsigned long long bytes) {
    const wchar_t* units[] = { L"B", L"KB", L"MB", L"GB", L"TB" };
    double size = static_cast<double>(bytes);
    int unitIdx = 0;
    while (size >= 1024.0 && unitIdx < 4) {
        size /= 1024.0;
        unitIdx++;
    }
    std::wostringstream oss;
    if (unitIdx == 0) {
        oss << bytes << L" " << units[unitIdx];
    }
    else {
        oss << std::fixed << std::setprecision(2) << size << L" " << units[unitIdx];
    }
    return oss.str();
}

Translations GetTranslations(const std::string& lang) {
    Translations t;
    if (lang == "tr") {
        t.title = L"WinDeepCleaner - Sistem Temizleyici";
        t.lblWinTemp = L"Kullanıcı Temp Klasörü (%TEMP%)";
        t.lblSysTemp = L"Windows Sistem Temp (C:\\Windows\\Temp)";
        t.lblDownloads = L"İndirilenler Klasörü (Downloads)";
        t.lblSoftDist = L"Windows Güncelleme Kalıntıları (SoftwareDistribution)";
        t.lblPrefetch = L"Prefetch Önbellek Dosyaları";
        t.btnCleanText = L"Temizliği Başlat";
        t.btnBackText = L"Geri";
        t.confirmTitle = L"Emin misiniz?";
        t.confirmMsg = L"Seçilen klasörlerin içi kalıcı olarak silinecek. Devam etmek istiyor musunuz?";
        t.successTitle = L"Başarılı";
        t.successPrefix = L"Temizlik başarıyla tamamlandı!\nTemizlenen alan: ";
    }
    else if (lang == "de") {
        t.title = L"WinDeepCleaner - System Cleaner";
        t.lblWinTemp = L"Benutzer Temp Ordner (%TEMP%)";
        t.lblSysTemp = L"Windows System Temp (C:\\Windows\\Temp)";
        t.lblDownloads = L"Downloads Ordner";
        t.lblSoftDist = L"Windows Update Reste (SoftwareDistribution)";
        t.lblPrefetch = L"Prefetch Cache Dateien";
        t.btnCleanText = L"Bereinigung Starten";
        t.btnBackText = L"Zurück";
        t.confirmTitle = L"Sind Sie sicher?";
        t.confirmMsg = L"Der Inhalt der ausgewählten Ordner wird dauerhaft gelöscht. Fortfahren?";
        t.successTitle = L"Erfolg";
        t.successPrefix = L"Bereinigung erfolgreich abgeschlossen!\nFreigegebener Speicher: ";
    }
    else if (lang == "fr") {
        t.title = L"WinDeepCleaner - Nettoyeur Système";
        t.lblWinTemp = L"Dossier Temp Utilisateur (%TEMP%)";
        t.lblSysTemp = L"Temp Système Windows (C:\\Windows\\Temp)";
        t.lblDownloads = L"Dossier Téléchargements";
        t.lblSoftDist = L"Restes de Mise à Jour Windows";
        t.lblPrefetch = L"Fichiers Cache Prefetch";
        t.btnCleanText = L"Lancer le Nettoyage";
        t.btnBackText = L"Retour";
        t.confirmTitle = L"Êtes-vous sûr ?";
        t.confirmMsg = L"Le contenu des dossiers sélectionnés sera supprimé définitivement. Continuer ?";
        t.successTitle = L"Succès";
        t.successPrefix = L"Nettoyage terminé avec succès !\nEspace libéré : ";
    }
    else {
        t.title = L"WinDeepCleaner - System Cleaner";
        t.lblWinTemp = L"User Temp Folder (%TEMP%)";
        t.lblSysTemp = L"Windows System Temp (C:\\Windows\\Temp)";
        t.lblDownloads = L"Downloads Folder";
        t.lblSoftDist = L"Windows Update Cache (SoftwareDistribution)";
        t.lblPrefetch = L"Prefetch Cache Files";
        t.btnCleanText = L"Start Cleaning";
        t.btnBackText = L"Back";
        t.confirmTitle = L"Are you sure?";
        t.confirmMsg = L"Selected folders will be permanently cleared. Do you really want to proceed?";
        t.successTitle = L"Success";
        t.successPrefix = L"Cleaning completed successfully!\nSpace freed: ";
    }
    return t;
}

void LoadSettings() {
    std::ifstream file("settings.cfg");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (line.find("lang=") == 0) {
                currentLang = line.substr(5);
            }
            else if (line.find("theme=") == 0) {
                isDarkMode = (line.substr(6) == "dark");
            }
        }
        file.close();
    }
    else {
        std::ofstream outfile("settings.cfg");
        outfile << "lang=en\ntheme=dark\n";
        outfile.close();
    }
}

void SaveSettings() {
    std::ofstream file("settings.cfg");
    file << "lang=" << currentLang << "\n";
    file << "theme=" << (isDarkMode ? "dark" : "light") << "\n";
    file.close();
}

void UpdateUILanguage(HWND hwnd) {
    Translations t = GetTranslations(currentLang);
    SetWindowTextW(hwnd, t.title.c_str());
    SetWindowTextW(chkWinTemp, t.lblWinTemp.c_str());
    SetWindowTextW(chkSysTemp, t.lblSysTemp.c_str());
    SetWindowTextW(chkDownloads, t.lblDownloads.c_str());
    SetWindowTextW(chkSoftwareDist, t.lblSoftDist.c_str());
    SetWindowTextW(chkPrefetch, t.lblPrefetch.c_str());
    SetWindowTextW(btnClean, t.btnCleanText.c_str());
    SetWindowTextW(btnBack, t.btnBackText.c_str());
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        hDarkBrush = CreateSolidBrush(RGB(30, 30, 30));
        LoadSettings();

        chkWinTemp = CreateWindowW(L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 30, 30, 420, 25, hwnd, (HMENU)1, NULL, NULL);
        chkSysTemp = CreateWindowW(L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 30, 70, 420, 25, hwnd, (HMENU)2, NULL, NULL);
        chkDownloads = CreateWindowW(L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 30, 110, 420, 25, hwnd, (HMENU)3, NULL, NULL);
        chkSoftwareDist = CreateWindowW(L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 30, 150, 420, 25, hwnd, (HMENU)4, NULL, NULL);
        chkPrefetch = CreateWindowW(L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 30, 190, 420, 25, hwnd, (HMENU)5, NULL, NULL);

        SendMessage(chkWinTemp, BM_SETCHECK, BST_CHECKED, 0);
        SendMessage(chkSysTemp, BM_SETCHECK, BST_CHECKED, 0);

        comboLang = CreateWindowW(L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_VISIBLE | WS_CHILD, 30, 240, 120, 200, hwnd, (HMENU)6, NULL, NULL);
        SendMessageW(comboLang, CB_ADDSTRING, 0, (LPARAM)L"English");
        SendMessageW(comboLang, CB_ADDSTRING, 0, (LPARAM)L"Türkçe");
        SendMessageW(comboLang, CB_ADDSTRING, 0, (LPARAM)L"Deutsch");
        SendMessageW(comboLang, CB_ADDSTRING, 0, (LPARAM)L"Français");

        if (currentLang == "en") SendMessage(comboLang, CB_SETCURSEL, 0, 0);
        else if (currentLang == "tr") SendMessage(comboLang, CB_SETCURSEL, 1, 0);
        else if (currentLang == "de") SendMessage(comboLang, CB_SETCURSEL, 2, 0);
        else if (currentLang == "fr") SendMessage(comboLang, CB_SETCURSEL, 3, 0);

        btnTheme = CreateWindowW(L"BUTTON", L"Theme: Dark/Light", WS_VISIBLE | WS_CHILD, 160, 240, 150, 30, hwnd, (HMENU)7, NULL, NULL);
        btnBack = CreateWindowW(L"BUTTON", L"", WS_VISIBLE | WS_CHILD, 30, 290, 100, 35, hwnd, (HMENU)8, NULL, NULL);
        btnClean = CreateWindowW(L"BUTTON", L"", WS_VISIBLE | WS_CHILD, 140, 290, 170, 35, hwnd, (HMENU)9, NULL, NULL);

        UpdateUILanguage(hwnd);
        break;
    }
    case WM_COMMAND: {
        int id = LOWORD(wp);
        if (id == 6 && HIWORD(wp) == CBN_SELCHANGE) {
            int sel = (int)SendMessage(comboLang, CB_GETCURSEL, 0, 0);
            if (sel == 0) currentLang = "en";
            else if (sel == 1) currentLang = "tr";
            else if (sel == 2) currentLang = "de";
            else if (sel == 3) currentLang = "fr";
            SaveSettings();
            UpdateUILanguage(hwnd);
        }
        else if (id == 7) {
            isDarkMode = !isDarkMode;
            SaveSettings();
            RedrawWindow(hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);
        }
        else if (id == 8) {
            MessageBoxW(hwnd, L"Ana menüye dönülüyor...", L"Info", MB_OK);
        }
        else if (id == 9) {
            Translations t = GetTranslations(currentLang);
            int res = MessageBoxW(hwnd, t.confirmMsg.c_str(), t.confirmTitle.c_str(), MB_YESNO | MB_ICONWARNING);
            if (res == IDYES) {
                HMODULE hDll = LoadLibraryA("cleaner.dll");
                if (hDll) {
                    typedef unsigned long long (*CleanFunc)(bool, bool, bool, bool, bool);
                    CleanFunc runClean = (CleanFunc)GetProcAddress(hDll, "RunCleaning");
                    if (runClean) {
                        bool c1 = SendMessage(chkWinTemp, BM_GETCHECK, 0, 0) == BST_CHECKED;
                        bool c2 = SendMessage(chkSysTemp, BM_GETCHECK, 0, 0) == BST_CHECKED;
                        bool c3 = SendMessage(chkDownloads, BM_GETCHECK, 0, 0) == BST_CHECKED;
                        bool c4 = SendMessage(chkSoftwareDist, BM_GETCHECK, 0, 0) == BST_CHECKED;
                        bool c5 = SendMessage(chkPrefetch, BM_GETCHECK, 0, 0) == BST_CHECKED;

                        unsigned long long freedBytes = runClean(c1, c2, c3, c4, c5);
                        std::wstring doneMsg = t.successPrefix + FormatBytes(freedBytes);
                        MessageBoxW(hwnd, doneMsg.c_str(), t.successTitle.c_str(), MB_OK | MB_ICONINFORMATION);
                    }
                    FreeLibrary(hDll);
                }
                else {
                    MessageBoxW(hwnd, L"cleaner.dll bulunamadı!", L"Hata", MB_OK | MB_ICONERROR);
                }
            }
        }
        break;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wp;
        if (isDarkMode) {
            SetTextColor(hdcStatic, RGB(255, 255, 255));
            SetBkColor(hdcStatic, RGB(30, 30, 30));
            return (INT_PTR)hDarkBrush;
        }
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
    case WM_ERASEBKGND: {
        if (isDarkMode) {
            HDC hdc = (HDC)wp;
            RECT rc;
            GetClientRect(hwnd, &rc);
            FillRect(hdc, &rc, hDarkBrush);
            return 1;
        }
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
    case WM_DESTROY:
        if (hDarkBrush) {
            DeleteObject(hDarkBrush);
            hDarkBrush = NULL;
        }
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
    return 0;
}

int main() {
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"WinDeepCleanerClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassW(&wc);

    hMainWnd = CreateWindowExW(0, L"WinDeepCleanerClass", L"WinDeepCleaner",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT, 485, 385, NULL, NULL, hInstance, NULL);

    ShowWindow(hMainWnd, SW_SHOW);
    UpdateWindow(hMainWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
