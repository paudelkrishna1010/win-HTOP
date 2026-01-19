# win-HTOP

A terminal-based process and resource monitoring tool for Windows, inspired by `htop`, built using native Win32 APIs.

This project focuses on understanding Windows system programming concepts such as process enumeration, memory ownership, and OS-level resource querying through documented Win32 interfaces.

---

## Motivation

Most system monitoring tools on Windows are GUI-based.  
This project explores how similar functionality can be implemented in a **command-line environment**, while learning how user-mode applications interact with the Windows operating system.

The goal is **learning and correctness**, not replacing existing tools.
---

## Tech Stack

- Language: C++
- Platform: Windows
- APIs: Win32, PSAPI
- Interface: Command Line (CLI)

---

## Learning Focus

- Win32 API design and usage
- `[in]` / `[out]` parameter contracts
- Stack vs heap memory allocation
- Byte-size vs element-count handling
- Proper error handling with `GetLastError()`

---

## Status

🚧 **Work in progress**

Planned improvements include:
- CPU usage per process
- Sorting and filtering

---

## Notes

This project is intentionally built without a GUI to keep the focus on **system-level logic** rather than presentation.

---

## License

No license specified (educational project).

