#!/usr/bin/env python3
"""
ProLife Release App
A GUI application for managing ProLife releases:
- Set versions for all submodules
- Build EXE using InnoSetup
"""

import tkinter as tk
from tkinter import ttk, messagebox, scrolledtext
import subprocess
import os
import sys
import json
import threading
from pathlib import Path


class ProLifeReleaseApp:
    def __init__(self, root):
        self.root = root
        self.root.title("ProLife Release App")
        self.root.geometry("900x700")
        
        # Get repository root
        self.repo_root = Path(__file__).parent.absolute()
        self.submodules_path = self.repo_root / "3rdParty"
        self.innosetup_script = self.repo_root / "Impl" / "ProLifeServer" / "Install" / "ProLifeServer.iss"
        
        # Store submodule data
        self.submodules = {}
        
        # Create UI
        self.create_ui()
        
        # Load submodules
        self.load_submodules()
    
    def create_ui(self):
        """Create the user interface"""
        # Main container
        main_frame = ttk.Frame(self.root, padding="10")
        main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # Configure grid weights
        self.root.columnconfigure(0, weight=1)
        self.root.rowconfigure(0, weight=1)
        main_frame.columnconfigure(0, weight=1)
        main_frame.rowconfigure(1, weight=1)
        
        # Title
        title_label = ttk.Label(main_frame, text="🚀 ProLife Release Manager", 
                                font=('Arial', 16, 'bold'))
        title_label.grid(row=0, column=0, pady=10)
        
        # Notebook for tabs
        notebook = ttk.Notebook(main_frame)
        notebook.grid(row=1, column=0, sticky=(tk.W, tk.E, tk.N, tk.S), pady=5)
        
        # Tab 1: Submodule Version Management
        self.submodules_frame = ttk.Frame(notebook, padding="10")
        notebook.add(self.submodules_frame, text="Submodule Versions")
        
        # Tab 2: Build Configuration
        self.build_frame = ttk.Frame(notebook, padding="10")
        notebook.add(self.build_frame, text="Build & Release")
        
        # Tab 3: Logs
        self.logs_frame = ttk.Frame(notebook, padding="10")
        notebook.add(self.logs_frame, text="Logs")
        
        # Setup each tab
        self.setup_submodules_tab()
        self.setup_build_tab()
        self.setup_logs_tab()
    
    def setup_submodules_tab(self):
        """Setup the submodules version management tab"""
        # Instructions
        instructions = ttk.Label(self.submodules_frame, 
                                text="Select versions for each submodule:",
                                font=('Arial', 10, 'bold'))
        instructions.grid(row=0, column=0, columnspan=3, pady=5, sticky=tk.W)
        
        # Create header
        ttk.Label(self.submodules_frame, text="Submodule", 
                 font=('Arial', 9, 'bold')).grid(row=1, column=0, padx=5, pady=5, sticky=tk.W)
        ttk.Label(self.submodules_frame, text="Current Version", 
                 font=('Arial', 9, 'bold')).grid(row=1, column=1, padx=5, pady=5, sticky=tk.W)
        ttk.Label(self.submodules_frame, text="Select Version", 
                 font=('Arial', 9, 'bold')).grid(row=1, column=2, padx=5, pady=5, sticky=tk.W)
        
        # Container for submodule rows
        self.submodule_widgets = {}
        
        # Buttons frame
        buttons_frame = ttk.Frame(self.submodules_frame)
        buttons_frame.grid(row=100, column=0, columnspan=3, pady=10)
        
        ttk.Button(buttons_frame, text="Refresh Versions", 
                  command=self.load_submodules).pack(side=tk.LEFT, padx=5)
        ttk.Button(buttons_frame, text="Update All to Latest", 
                  command=self.update_all_to_latest).pack(side=tk.LEFT, padx=5)
        ttk.Button(buttons_frame, text="Apply Selected Versions", 
                  command=self.apply_versions).pack(side=tk.LEFT, padx=5)
    
    def setup_build_tab(self):
        """Setup the build configuration tab"""
        # Version input
        version_frame = ttk.LabelFrame(self.build_frame, text="Release Version", padding="10")
        version_frame.grid(row=0, column=0, sticky=(tk.W, tk.E), pady=5)
        
        ttk.Label(version_frame, text="Version:").grid(row=0, column=0, padx=5, sticky=tk.W)
        self.version_entry = ttk.Entry(version_frame, width=20)
        self.version_entry.insert(0, "1.0.0")
        self.version_entry.grid(row=0, column=1, padx=5)
        ttk.Label(version_frame, text="(e.g., 1.0.0)").grid(row=0, column=2, padx=5, sticky=tk.W)
        
        # Build configuration
        config_frame = ttk.LabelFrame(self.build_frame, text="Build Configuration", padding="10")
        config_frame.grid(row=1, column=0, sticky=(tk.W, tk.E), pady=5)
        
        ttk.Label(config_frame, text="Build Type:").grid(row=0, column=0, padx=5, sticky=tk.W)
        self.build_type = tk.StringVar(value="Release")
        ttk.Radiobutton(config_frame, text="Release", variable=self.build_type, 
                       value="Release").grid(row=0, column=1, padx=5)
        ttk.Radiobutton(config_frame, text="Debug", variable=self.build_type, 
                       value="Debug").grid(row=0, column=2, padx=5)
        
        # Build directory
        ttk.Label(config_frame, text="Build Dir:").grid(row=1, column=0, padx=5, sticky=tk.W, pady=5)
        self.build_dir_entry = ttk.Entry(config_frame, width=40)
        self.build_dir_entry.insert(0, str(self.repo_root / "build"))
        self.build_dir_entry.grid(row=1, column=1, columnspan=2, padx=5, pady=5, sticky=(tk.W, tk.E))
        
        # Actions
        actions_frame = ttk.LabelFrame(self.build_frame, text="Actions", padding="10")
        actions_frame.grid(row=2, column=0, sticky=(tk.W, tk.E), pady=5)
        
        ttk.Button(actions_frame, text="1. Pin Submodules", 
                  command=self.pin_submodules).pack(fill=tk.X, pady=2)
        ttk.Button(actions_frame, text="2. Build Project (CMake)", 
                  command=self.build_project).pack(fill=tk.X, pady=2)
        ttk.Button(actions_frame, text="3. Build EXE (InnoSetup)", 
                  command=self.build_innosetup).pack(fill=tk.X, pady=2)
        
        ttk.Separator(actions_frame, orient='horizontal').pack(fill=tk.X, pady=10)
        
        ttk.Button(actions_frame, text="🚀 Complete Release (All Steps)", 
                  command=self.complete_release,
                  style='Accent.TButton').pack(fill=tk.X, pady=2)
        
        # Status
        self.status_label = ttk.Label(self.build_frame, text="Ready", 
                                     font=('Arial', 9, 'italic'))
        self.status_label.grid(row=3, column=0, pady=5)
        
        # Progress bar
        self.progress = ttk.Progressbar(self.build_frame, mode='indeterminate')
        self.progress.grid(row=4, column=0, sticky=(tk.W, tk.E), pady=5)
    
    def setup_logs_tab(self):
        """Setup the logs tab"""
        # Log display
        self.log_text = scrolledtext.ScrolledText(self.logs_frame, 
                                                   wrap=tk.WORD, 
                                                   width=80, 
                                                   height=30,
                                                   font=('Courier', 9))
        self.log_text.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # Configure grid weights for logs frame
        self.logs_frame.columnconfigure(0, weight=1)
        self.logs_frame.rowconfigure(0, weight=1)
        
        # Buttons
        button_frame = ttk.Frame(self.logs_frame)
        button_frame.grid(row=1, column=0, pady=5)
        
        ttk.Button(button_frame, text="Clear Logs", 
                  command=self.clear_logs).pack(side=tk.LEFT, padx=5)
        ttk.Button(button_frame, text="Save Logs", 
                  command=self.save_logs).pack(side=tk.LEFT, padx=5)
    
    def load_submodules(self):
        """Load submodules from .gitmodules"""
        self.log("Loading submodules...")
        
        try:
            # Parse .gitmodules
            gitmodules_path = self.repo_root / ".gitmodules"
            if not gitmodules_path.exists():
                self.log("ERROR: .gitmodules not found", error=True)
                return
            
            # Get submodule list
            result = subprocess.run(
                ["git", "submodule", "status"],
                cwd=self.repo_root,
                capture_output=True,
                text=True
            )
            
            if result.returncode != 0:
                self.log(f"ERROR: Failed to get submodule status: {result.stderr}", error=True)
                return
            
            # Parse submodule status
            row_num = 2
            for line in result.stdout.strip().split('\n'):
                if not line.strip():
                    continue
                
                parts = line.strip().split()
                if len(parts) < 2:
                    continue
                
                commit = parts[0].lstrip('+-')
                path = parts[1]
                
                # Only process 3rdParty submodules
                if not path.startswith("3rdParty/"):
                    continue
                
                name = os.path.basename(path)
                full_path = self.repo_root / path
                
                # Get current version
                current_version = "Not initialized"
                available_versions = ["main", "master"]
                
                if full_path.exists():
                    # Get current tag
                    tag_result = subprocess.run(
                        ["git", "describe", "--tags", "--exact-match"],
                        cwd=full_path,
                        capture_output=True,
                        text=True
                    )
                    
                    if tag_result.returncode == 0:
                        current_version = tag_result.stdout.strip()
                    else:
                        # Get branch
                        branch_result = subprocess.run(
                            ["git", "rev-parse", "--abbrev-ref", "HEAD"],
                            cwd=full_path,
                            capture_output=True,
                            text=True
                        )
                        if branch_result.returncode == 0:
                            current_version = branch_result.stdout.strip()
                            if current_version == "HEAD":
                                current_version = f"commit {commit[:7]}"
                    
                    # Get available tags
                    tags_result = subprocess.run(
                        ["git", "tag", "-l"],
                        cwd=full_path,
                        capture_output=True,
                        text=True
                    )
                    
                    if tags_result.returncode == 0:
                        tags = [tag.strip() for tag in tags_result.stdout.split('\n') if tag.strip()]
                        # Filter semantic version tags and sort
                        import re
                        sem_ver_tags = [tag for tag in tags if re.match(r'^v?\d+\.\d+\.\d+', tag)]
                        sem_ver_tags.sort(reverse=True)
                        if sem_ver_tags:
                            available_versions = sem_ver_tags[:20] + ["main", "master"]
                        else:
                            available_versions = tags[-20:] + ["main", "master"]
                
                # Create UI elements
                ttk.Label(self.submodules_frame, text=name).grid(
                    row=row_num, column=0, padx=5, pady=2, sticky=tk.W
                )
                
                ttk.Label(self.submodules_frame, text=current_version).grid(
                    row=row_num, column=1, padx=5, pady=2, sticky=tk.W
                )
                
                version_combo = ttk.Combobox(self.submodules_frame, 
                                            values=available_versions,
                                            width=25,
                                            state='readonly')
                if current_version in available_versions:
                    version_combo.set(current_version)
                elif available_versions:
                    version_combo.set(available_versions[0])
                
                version_combo.grid(row=row_num, column=2, padx=5, pady=2, sticky=tk.W)
                
                # Store reference
                self.submodule_widgets[path] = {
                    'name': name,
                    'path': path,
                    'full_path': full_path,
                    'current': current_version,
                    'combo': version_combo
                }
                
                row_num += 1
            
            self.log(f"Loaded {len(self.submodule_widgets)} submodules")
            
        except Exception as e:
            self.log(f"ERROR loading submodules: {str(e)}", error=True)
    
    def update_all_to_latest(self):
        """Update all submodules to their latest tagged version"""
        if not messagebox.askyesno("Confirm", "Update all submodules to latest tagged versions?"):
            return
        
        self.log("Updating all submodules to latest versions...")
        
        for path, data in self.submodule_widgets.items():
            combo = data['combo']
            values = combo['values']
            
            if values:
                # Find latest semantic version
                import re
                sem_vers = [v for v in values if re.match(r'^v?\d+\.\d+\.\d+', v)]
                if sem_vers:
                    combo.set(sem_vers[0])
                    self.log(f"  {data['name']}: {sem_vers[0]}")
        
        self.log("All submodules set to latest versions")
    
    def apply_versions(self):
        """Apply selected versions to submodules"""
        if not messagebox.askyesno("Confirm", "Apply selected versions to submodules?"):
            return
        
        self.log("Applying submodule versions...")
        
        for path, data in self.submodule_widgets.items():
            selected_version = data['combo'].get()
            if not selected_version:
                continue
            
            self.log(f"Updating {data['name']} to {selected_version}...")
            
            try:
                # Change to submodule directory
                full_path = data['full_path']
                
                if not full_path.exists():
                    self.log(f"  Initializing submodule...", error=False)
                    subprocess.run(
                        ["git", "submodule", "update", "--init", path],
                        cwd=self.repo_root,
                        check=True
                    )
                
                # Fetch latest
                subprocess.run(
                    ["git", "fetch", "--all", "--tags"],
                    cwd=full_path,
                    check=True,
                    capture_output=True
                )
                
                # Checkout version
                subprocess.run(
                    ["git", "checkout", selected_version],
                    cwd=full_path,
                    check=True,
                    capture_output=True
                )
                
                # Stage changes in main repo
                subprocess.run(
                    ["git", "add", path],
                    cwd=self.repo_root,
                    check=True
                )
                
                self.log(f"  ✓ {data['name']} updated to {selected_version}")
                
            except subprocess.CalledProcessError as e:
                self.log(f"  ERROR updating {data['name']}: {str(e)}", error=True)
        
        self.log("Submodule versions applied. Review changes with 'git status'")
    
    def pin_submodules(self):
        """Pin all submodules to current commits"""
        self.log("Pinning submodules...")
        self.set_status("Pinning submodules...")
        
        def run():
            try:
                result = subprocess.run(
                    [str(self.repo_root / "prepare-release.sh"), "--pin-submodules"],
                    cwd=self.repo_root,
                    capture_output=True,
                    text=True
                )
                
                self.log(result.stdout)
                if result.returncode != 0:
                    self.log(result.stderr, error=True)
                    self.set_status("ERROR pinning submodules")
                else:
                    self.log("✓ Submodules pinned successfully")
                    self.set_status("Submodules pinned")
            except Exception as e:
                self.log(f"ERROR: {str(e)}", error=True)
                self.set_status("ERROR")
        
        threading.Thread(target=run, daemon=True).start()
    
    def build_project(self):
        """Build the project using CMake"""
        self.log("Building project...")
        self.set_status("Building project...")
        self.progress.start()
        
        def run():
            try:
                build_dir = Path(self.build_dir_entry.get())
                build_type = self.build_type.get()
                
                # Create build directory
                build_dir.mkdir(parents=True, exist_ok=True)
                
                # Configure
                self.log(f"Configuring CMake (Build Type: {build_type})...")
                result = subprocess.run(
                    ["cmake", str(self.repo_root / "Build" / "CMake"), 
                     f"-DCMAKE_BUILD_TYPE={build_type}"],
                    cwd=build_dir,
                    capture_output=True,
                    text=True
                )
                
                self.log(result.stdout)
                if result.returncode != 0:
                    self.log(result.stderr, error=True)
                    self.set_status("ERROR configuring")
                    self.progress.stop()
                    return
                
                # Build
                self.log("Building...")
                result = subprocess.run(
                    ["cmake", "--build", ".", "--config", build_type],
                    cwd=build_dir,
                    capture_output=True,
                    text=True
                )
                
                self.log(result.stdout)
                if result.returncode != 0:
                    self.log(result.stderr, error=True)
                    self.set_status("ERROR building")
                else:
                    self.log("✓ Build completed successfully")
                    self.set_status("Build complete")
                
            except Exception as e:
                self.log(f"ERROR: {str(e)}", error=True)
                self.set_status("ERROR")
            finally:
                self.progress.stop()
        
        threading.Thread(target=run, daemon=True).start()
    
    def build_innosetup(self):
        """Build installer using InnoSetup"""
        self.log("Building installer with InnoSetup...")
        self.set_status("Building installer...")
        self.progress.start()
        
        def run():
            try:
                build_dir = Path(self.build_dir_entry.get())
                
                if not self.innosetup_script.exists():
                    self.log(f"ERROR: InnoSetup script not found: {self.innosetup_script}", error=True)
                    self.set_status("ERROR: Script not found")
                    self.progress.stop()
                    return
                
                # Set environment variable for build directory
                env = os.environ.copy()
                env['PROLIFE_BUILD_DIR'] = str(build_dir)
                
                # Try to find InnoSetup compiler
                iscc_paths = [
                    r"C:\Program Files (x86)\Inno Setup 6\ISCC.exe",
                    r"C:\Program Files\Inno Setup 6\ISCC.exe",
                    "iscc.exe"  # In PATH
                ]
                
                iscc_exe = None
                for path in iscc_paths:
                    if Path(path).exists() or path == "iscc.exe":
                        iscc_exe = path
                        break
                
                if not iscc_exe:
                    self.log("ERROR: InnoSetup compiler (ISCC.exe) not found", error=True)
                    self.log("Please install InnoSetup from https://jrsoftware.org/isdl.php", error=True)
                    self.set_status("ERROR: ISCC not found")
                    self.progress.stop()
                    return
                
                self.log(f"Running InnoSetup compiler...")
                result = subprocess.run(
                    [iscc_exe, str(self.innosetup_script)],
                    cwd=self.innosetup_script.parent,
                    env=env,
                    capture_output=True,
                    text=True
                )
                
                self.log(result.stdout)
                if result.returncode != 0:
                    self.log(result.stderr, error=True)
                    self.set_status("ERROR building installer")
                else:
                    self.log("✓ Installer created successfully")
                    self.set_status("Installer ready")
                
            except Exception as e:
                self.log(f"ERROR: {str(e)}", error=True)
                self.set_status("ERROR")
            finally:
                self.progress.stop()
        
        threading.Thread(target=run, daemon=True).start()
    
    def complete_release(self):
        """Complete release process - all steps"""
        if not messagebox.askyesno("Complete Release", 
                                   "This will:\n"
                                   "1. Pin submodules\n"
                                   "2. Build project\n"
                                   "3. Create installer\n\n"
                                   "Continue?"):
            return
        
        self.log("="*60)
        self.log("Starting complete release process...")
        self.log("="*60)
        
        # Run steps sequentially
        def run():
            try:
                # Step 1: Pin submodules
                self.pin_submodules()
                import time
                time.sleep(2)  # Wait for pinning
                
                # Step 2: Build project
                self.build_project()
                # Note: Build runs in background, installer will need to wait
                
                self.log("="*60)
                self.log("Release preparation started")
                self.log("Note: Build and installer creation run asynchronously")
                self.log("Monitor the logs for completion status")
                self.log("="*60)
                
            except Exception as e:
                self.log(f"ERROR in release process: {str(e)}", error=True)
        
        threading.Thread(target=run, daemon=True).start()
    
    def set_status(self, text):
        """Update status label"""
        self.status_label.config(text=text)
    
    def log(self, message, error=False):
        """Add message to log"""
        timestamp = __import__('datetime').datetime.now().strftime("%H:%M:%S")
        
        if error:
            line = f"[{timestamp}] ERROR: {message}\n"
            tag = "error"
        else:
            line = f"[{timestamp}] {message}\n"
            tag = None
        
        self.log_text.insert(tk.END, line, tag)
        self.log_text.see(tk.END)
        
        # Configure error tag
        self.log_text.tag_config("error", foreground="red")
    
    def clear_logs(self):
        """Clear log display"""
        self.log_text.delete(1.0, tk.END)
    
    def save_logs(self):
        """Save logs to file"""
        from tkinter import filedialog
        
        filename = filedialog.asksaveasfilename(
            defaultextension=".txt",
            filetypes=[("Text files", "*.txt"), ("All files", "*.*")]
        )
        
        if filename:
            with open(filename, 'w') as f:
                f.write(self.log_text.get(1.0, tk.END))
            
            self.log(f"Logs saved to {filename}")


def main():
    """Main entry point"""
    root = tk.Tk()
    
    # Set theme
    style = ttk.Style()
    style.theme_use('clam')
    
    app = ProLifeReleaseApp(root)
    root.mainloop()


if __name__ == "__main__":
    main()
