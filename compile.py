import os
import platform
import subprocess
import sys

def find_c_files(directory):
    c_files = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(".c"):
                c_files.append(os.path.join(root, file))
    return c_files

def compile_project(run_after_compile=False):
    source_directories = [
        "algorithm",
        "array",
        "bitset",
        "config",
        "csv",
        "deque",
        "encoding",
        "forward_list",
        "list",
        "map",
        "priority_queue",
        "queue",
        "span",
        "stack",
        "string",
        "vector",
        # Add other directories containing your .c files
    ]

    source_files = ["./main.c"]
    for directory in source_directories:
        source_files.extend(find_c_files(directory))

    flags = "-std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s"
    build_dir = "./build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    output = os.path.join(build_dir, "main.exe" if platform.system() == "Windows" else "main")

    # Clean up old build files
    if os.path.exists(output):
        os.remove(output)

    # Compile the project
    command = f"gcc {flags} -o {output} " + " ".join(source_files)
    result = subprocess.run(command, shell=True)

    if result.returncode != 0:
        print("Compilation failed.")
    else:
        print(f"Compilation successful. Output: {output}")
        if run_after_compile:
            print("Running the program...\n" + '*' * 25)
            subprocess.run(output)

def main():
    if len(sys.argv) < 2:
        print("Usage: python compile_project.py [b|r]")
        sys.exit(1)

    if sys.argv[1] == 'b':
        compile_project()
    elif sys.argv[1] == 'r':
        compile_project(run_after_compile=True)
    else:
        print("Invalid argument. Use 'b' to build or 'r' to build and run.")
        sys.exit(1)

if __name__ == "__main__":
    main()
