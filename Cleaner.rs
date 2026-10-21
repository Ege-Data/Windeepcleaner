use std::fs;
use std::path::{Path, PathBuf};

// Klasörün içindekileri silen yardımcı fonksiyon
fn clear_directory(dir_path: &Path) {
    if let Ok(entries) = fs::read_dir(dir_path) {
        for entry in entries.flatten() {
            let path = entry.path();
            if path.is_dir() {
                let _ = fs::remove_dir_all(&path);
            } else {
                let _ = fs::remove_file(&path);
            }
        }
    }
}

// UAC (Yönetici) modunda bile gerçek kullanıcının Downloads klasörünü bulan fonksiyon
fn get_real_user_downloads() -> Option<PathBuf> {
    if let Ok(profile) = std::env::var("USERPROFILE") {
        let path = Path::new(&profile).join("Downloads");
        if path.exists() && !profile.to_lowercase().contains("administrator") {
            return Some(path);
        }
    }

    // Administrator modundaysa C:\Users altındaki gerçek kullanıcıyı tara
    if let Ok(entries) = fs::read_dir("C:\\Users") {
        for entry in entries.flatten() {
            let path = entry.path();
            if path.is_dir() {
                if let Some(name) = path.file_name().and_then(|n| n.to_str()) {
                    let lower = name.to_lowercase();
                    if lower != "public" && lower != "default" && lower != "administrator" && lower != "all users" {
                        let downloads = path.join("Downloads");
                        if downloads.exists() {
                            return Some(downloads);
                        }
                    }
                }
            }
        }
    }
    None
}

// Gerçek kullanıcının Temp klasörünü bulan fonksiyon
fn get_real_user_temp() -> Option<PathBuf> {
    if let Ok(temp) = std::env::var("TEMP") {
        let path = Path::new(&temp);
        if path.exists() && !temp.to_lowercase().contains("administrator") {
            return Some(path.to_path_buf());
        }
    }

    if let Ok(entries) = fs::read_dir("C:\\Users") {
        for entry in entries.flatten() {
            let path = entry.path();
            if path.is_dir() {
                if let Some(name) = path.file_name().and_then(|n| n.to_str()) {
                    let lower = name.to_lowercase();
                    if lower != "public" && lower != "default" && lower != "administrator" && lower != "all users" {
                        let temp_path = path.join("AppData\\Local\\Temp");
                        if temp_path.exists() {
                            return Some(temp_path);
                        }
                    }
                }
            }
        }
    }
    None
}

#[no_mangle]
pub extern "C" fn RunCleaning(c1: bool, c2: bool, c3: bool, c4: bool, c5: bool) {
    // 1. Kullanıcı Temp Klasörü
    if c1 {
        if let Some(temp_dir) = get_real_user_temp() {
            clear_directory(&temp_dir);
        } else if let Ok(temp_dir) = std::env::var("TEMP") {
            clear_directory(Path::new(&temp_dir));
        }
    }

    // 2. Windows Sistem Temp (C:\Windows\Temp)
    if c2 {
        clear_directory(Path::new("C:\\Windows\\Temp"));
    }

    // 3. İndirilenler Klasörü (Downloads)
    if c3 {
        if let Some(downloads_dir) = get_real_user_downloads() {
            clear_directory(&downloads_dir);
        }
    }

    // 4. Windows Güncelleme Kalıntıları (SoftwareDistribution\Download)
    if c4 {
        clear_directory(Path::new("C:\\Windows\\SoftwareDistribution\\Download"));
    }

    // 5. Prefetch Önbellek Dosyaları
    if c5 {
        clear_directory(Path::new("C:\\Windows\\Prefetch"));
    }
}
