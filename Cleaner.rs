use std::fs;
use std::path::{Path, PathBuf};

fn clear_directory(dir_path: &Path) -> u64 {
    let mut freed_bytes: u64 = 0;
    if let Ok(entries) = fs::read_dir(dir_path) {
        for entry in entries.flatten() {
            let path = entry.path();
            if path.is_dir() {
                freed_bytes += clear_directory(&path);
                let _ = fs::remove_dir(&path);
            } else {
                let size = fs::metadata(&path).map(|m| m.len()).unwrap_or(0);
                if fs::remove_file(&path).is_ok() {
                    freed_bytes += size;
                }
            }
        }
    }
    freed_bytes
}

fn get_real_user_downloads() -> Option<PathBuf> {
    if let Ok(profile) = std::env::var("USERPROFILE") {
        let path = Path::new(&profile).join("Downloads");
        if path.exists() && !profile.to_lowercase().contains("administrator") {
            return Some(path);
        }
    }

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
pub extern "C" fn RunCleaning(c1: bool, c2: bool, c3: bool, c4: bool, c5: bool) -> u64 {
    let mut total_freed: u64 = 0;

    if c1 {
        if let Some(temp_dir) = get_real_user_temp() {
            total_freed += clear_directory(&temp_dir);
        } else if let Ok(temp_dir) = std::env::var("TEMP") {
            total_freed += clear_directory(Path::new(&temp_dir));
        }
    }

    if c2 {
        total_freed += clear_directory(Path::new("C:\\Windows\\Temp"));
    }

    if c3 {
        if let Some(downloads_dir) = get_real_user_downloads() {
            total_freed += clear_directory(&downloads_dir);
        }
    }

    if c4 {
        total_freed += clear_directory(Path::new("C:\\Windows\\SoftwareDistribution\\Download"));
    }

    if c5 {
        total_freed += clear_directory(Path::new("C:\\Windows\\Prefetch"));
    }

    total_freed
}
