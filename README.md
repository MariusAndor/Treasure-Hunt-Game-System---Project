# 🗺️ Treasure Hunt Game System - Phase 1

## 📌 Overview

This project represents **Phase 1** of a *Treasure Hunt* game system, developed in **C** for **UNIX/Linux** systems. The game logic is managed using the file system, storing each hunt and its associated treasures as files and directories.

The purpose of this phase is to create a file-based backend for treasure management, allowing interaction through command-line operations.

---

## 🧱 Project Structure

``` bash
treasure_hunt_project/ │
 ├── treasure_manager.c # Main source file
 ├── <hunt_id> # Directory for a specific treasure hunt
     └── <hunt_id>_treasures.dat # Binary file containing all treasures 
     └── logged_hunt.dat # Symbolic Link, ../logged_hunt_<hunt_id>.symlink
 ├── logged_hunt_<hunt_id>.symlink →  # Log File
```

Each hunt is represented by a **directory** named after the hunt ID. Treasures are stored in a binary file inside this directory. A **symbolic link** named `logged_hunt_<hunt_id>.symlink` points to the `logged_hunt.dat` file inside the hunt's directory for easier access.

---

## 💾 Treasure Data Structure

Each treasure is represented in the binary file as a struct like this:

```c
typedef struct {
    int id;                  // Unique identifier for the treasure
    char username[50];       // Name of the user who added it
    float x, y;              // Coordinates of the treasure
    float value;             // Value of the treasure
    char clueText[100];      // Clue or description for the treasure
} treasure_t;
```
🛠️ Features
✅ Add New Treasure
Creates or updates a hunt directory and adds a treasure to its binary file.

📋 List All Treasures
Reads all treasures from a hunt's binary file and displays them.

🔍 View a Single Treasure
Fetches details of a treasure by its ID from a specific hunt.

❌ Delete Treasure
Removes a treasure entry from the binary file by ID.

🔗 Symbolic Link Management
Creates symbolic links for logs to simplify tracking and logging for each hunt.

📂 Directory & File Example
Assuming a hunt with ID hunt42, the following structure will be created:
``` bash
hunt42/
├── hunt42_treasures.dat
└── logged_hunt.dat

logged_hunt_hunt42.symlink → hunt42/logged_hunt.dat
```


##  How to run the project?

Just type in terminal make, and you will get to executables:

``` bash
── treasure_manager
── treasure_hub

```

If you want to run the functions from 
    Phase 1 type in ./treasure_manager
    Phase 2 type in ./treasure_hub


##  Phase Status

Phase 1: ✅
Phase 2: ✅
Phase 3: ✅
