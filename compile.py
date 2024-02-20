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

def compile_project(run_after_compile=False, program_args=[]):
    source_directories = [
        # "numeric",
        "algorithm",
        # "array",
        # "bitset",
        # "config",
        # "csv",
        # "deque",
        "encoding",
        # "forward_list",
        # "list",
        "fmt",
        # "map",
        # "json",
        "file_io",
        "log",
        # "priority_queue",
        # "queue",
        # "span",
        # "stack",
        "string",
        "network",
        "vector",
        "time",
        "concurrent",
        # "date",
        "dir",
        # "cli",
        # "crypto",
        # "tuple",
        "matrix",
        "graph",
        # Add other directories containing your .c files
    ]

    source_files = ["./main.c"]
    for directory in source_directories:
        source_files.extend(find_c_files(directory))

    openssl_include_path = "./dependency/include"  # No longer pointing to C:/msys64...
    openssl_lib_path = "./dependency/lib"   

    # Compiler flags
    flags = "-std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -Wno-deprecated-declarations -s"
    flags += f" -I{openssl_include_path}"  # Include path for OpenSSL headers
    flags += f" -L{openssl_lib_path}"      # Library path for OpenSSL libraries

    build_dir = "./build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    output = os.path.join(build_dir, "main.exe" if platform.system() == "Windows" else "main")

    if os.path.exists(output):
        os.remove(output)

    # Compile the project with OpenSSL flags
    command = f"gcc {flags} -o {output} " + " ".join(source_files) + f" -I{openssl_include_path} -L{openssl_lib_path} -lssl -lcrypto"
    if platform.system() == "Windows":
        command += " -lWs2_32 -lAdvapi32"  # Add Windows-specific library

        if "dir" in source_directories:
            command += " -lshlwapi"

    result = subprocess.run(command, shell=True)

    if result.returncode != 0:
        print("Compilation failed.")
    else:
        print(f"Compilation successful. Output: {output}")
        if run_after_compile:
            print("Running the program...\n" + '*' * 25)
            subprocess.run([output] + program_args)

def main():
    if len(sys.argv) < 2:
        print("Usage: python compile_project.py [b|r]")
        sys.exit(1)

    program_args = sys.argv[2:]

    if sys.argv[1] == 'b':
        compile_project()
    elif sys.argv[1] == 'r':
        compile_project(run_after_compile=True, program_args=program_args)
    else:
        print("Invalid argument. Use 'b' to build or 'r' to build and run.")
        sys.exit(1)

if __name__ == "__main__":
    main()
