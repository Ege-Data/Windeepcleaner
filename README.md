# Windeepcleaner
Modern, lightweight Windows system cleaner powered by a native C++ GUI and a secure Rust backend engine. Safely clears temp files, downloads, and cache.
🚀 WinDeepCleaner
WinDeepCleaner is a modern, lightweight open-source system utility designed to boost Windows system performance and securely clean unnecessary leftover files. It features a sleek C++ native UI and a high-performance Rust-based backend engine (cdylib).

✨ Key Features
C++ Win32 GUI: Zero bloatware, fast and lightweight native Windows interface.

Rust Backend: High-speed cleaning engine that smartly skips locked or in-use files to protect system integrity.

UAC Support: Secure execution with administrator privileges.

Multi-language Support: Turkish, English, German, and French language options (customizable via settings.cfg).

Theme Support: Eye-friendly modern Dark and Light mode switching.

🧹 Cleaned Locations
User Temp Folder (%TEMP%)

Windows System Temp (C:\Windows\Temp)

Downloads Folder (Downloads)

Windows Update Cache (SoftwareDistribution\Download)

Prefetch Cache Files (C:\Windows\Prefetch)

📦 System Requirements & Dependencies
To run WinDeepCleaner smoothly, your system requires:

Operating System: Windows 7, 8, 10, or 11 (x86 or x64)

Visual C++ Redistributable: Microsoft Visual C++ 2015–2022 Redistributable (Both x86 and x64 versions must be installed if you encounter missing DLL errors).

Download official VC++ Redistributable packages from Microsoft

🛠️ Technologies Used
Interface (Loader): C++, Win32 API

Backend: Rust (cdylib)

📄 License
This project is licensed under the MIT License. Check out the LICENSE file for more details.

⚠️ No Warranty: This software is provided "as is", without warranty of any kind, express or implied. The developer is not responsible for any data loss or system issues resulting from the use of this software. Usage responsibility lies entirely with the user.
