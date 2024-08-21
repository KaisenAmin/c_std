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

def compile_to_shared_library(directory, build_dir, openssl_include_path, openssl_lib_path, postgres_include_path, postgres_lib_path):
    source_files = find_c_files(directory)
    if not source_files:
        return None
    
    library_name = os.path.basename(directory)
    if platform.system() == "Windows":
        output = os.path.join(build_dir, f"{library_name}.dll")
    else:
        output = os.path.join(build_dir, f"lib{library_name}.so")
    
    flags = "-std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -Wno-deprecated-declarations -s"
    flags += f" -I{openssl_include_path} -L{openssl_lib_path} -I{postgres_include_path} -L{postgres_lib_path}"
    
    dependencies = []
    if directory in ["fmt", "file_io"]:
        dependencies.append("encoding")
        dependencies.append("string")
    elif directory in ["array", "priority_queue", "queue", "stack", "network"]:
        dependencies.append("vector")
        dependencies.append("fmt")
    elif directory in ["vector", "bitset", "deque", "forward_list", "list", "map", "span", "time", "tuple", "matrix"]:
        dependencies.append("fmt")
    elif directory in ["config", "csv", "log"]:
        dependencies.append("string")
        dependencies.append("fmt")
        dependencies.append("file_io")
    elif directory in ["json"]:
        dependencies.append("string")
        dependencies.append("fmt")
        dependencies.append("file_io")
        dependencies.append("vector")
        dependencies.append("map")
    elif directory in ["concurrent"]:
        dependencies.append("fmt")
    elif directory in ["date"]:
        dependencies.append("fmt")
        dependencies.append("time")
    elif directory in ["cli", "database"]:
        dependencies.append("fmt")
        dependencies.append("string")
   
    command = f"gcc {flags} -shared -o {output} " + " ".join(source_files)
    command += " -lssl -lcrypto -lpq -lraylib -lopengl32 -lgdi32 -lwinmm"
    
    for dep in dependencies:
        if platform.system() == "Windows":
            command += f" -L{build_dir} -l{dep}"
        else:
            command += f" -L{build_dir} -l{dep}"

    if platform.system() == "Windows":
        command += " -lole32 -lbthprops"

    result = subprocess.run(command, shell=True)
    if result.returncode != 0:
        print(f"Failed to compile {directory} to shared library.")
        return None
    
    print(f"Successfully compiled {directory} to {output}.")
    return output

def compile_project(run_after_compile=False, compile_to_shared_only=False, program_args=[]):
    source_directories = [
        # "numeric",
        # "algorithm",
        # "array",
        # "bitset",
        # "config",
        # "csv",
        "deque",
        "encoding",
        # "forward_list",
        # "list",
        "fmt",
        "map",
        "json",
        # "file_io",
        # "log",
        # "priority_queue",
        # "queue",
        # "span",
        # "stack",
        "string",
        # "network",
        "vector",
        "time",
        # "concurrent",
        # "date",
        # "dir",
        # "cli",
        # "crypto",
        # "tuple",
        # "matrix",
        "database",
        "random",
        # "statistics",
        # "sysinfo",
        "turtle"
    ]

    build_dir = "./build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    if platform.system() == "Linux":
        openssl_include_path = "/usr/include/openssl"
        openssl_lib_path = "/usr/lib"
        postgres_include_path = "/usr/include/postgresql"
        postgres_lib_path = "/usr/lib"
    else:
        openssl_include_path = "./dependency/include"
        openssl_lib_path = "./dependency/lib"
        postgres_include_path = "./dependency/include"
        postgres_lib_path = "./dependency/lib"

    shared_libs = []
    for directory in source_directories:
        shared_lib = compile_to_shared_library(directory, build_dir, openssl_include_path, openssl_lib_path, postgres_include_path, postgres_lib_path)
        if shared_lib:
            shared_libs.append(shared_lib)

    if compile_to_shared_only:
        return

    source_files = ["./main.c"]
    flags = "-std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -Wno-deprecated-declarations -s"
    flags += f" -I{openssl_include_path} -L{openssl_lib_path} -I{postgres_include_path} -L{postgres_lib_path}"
    flags += " -Ipath_to_raylib/include"  # Ensure to replace with the actual path
    ldflags = " -Lpath_to_raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm"  # Ensure to replace with the actual path
    if platform.system() == "Windows":
        output = os.path.join(build_dir, "main.exe")
    else:
        output = os.path.join(build_dir, "main")
    
    if os.path.exists(output):
        os.remove(output)
    
    command = f"gcc {flags} -o {output} " + " ".join(source_files)
    for shared_lib in shared_libs:
        lib_name = os.path.basename(shared_lib)
        if platform.system() == "Windows":
            lib_name = lib_name[:-4]  # Remove '.dll' suffix
        else:
            lib_name = lib_name[3:-3]  # Remove 'lib' prefix and '.so' suffix
        command += f" -L{build_dir} -l{lib_name}"
    
    command += ldflags

    if platform.system() == "Windows":
        command += " -lole32 -lbthprops"
    else:
        command += f" -Wl,-rpath,{build_dir}"

    result = subprocess.run(command, shell=True)
    if result.returncode != 0:
        print("Compilation of main project failed.")
    else:
        print(f"Compilation of main project successful. Output: {output}")
        if run_after_compile:
            print("Running the program...\n" + '*' * 25)
            subprocess.run([output] + program_args)

def main():
    if len(sys.argv) < 2:
        print("Usage: python compile.py [b|r|l]")
        sys.exit(1)

    program_args = sys.argv[2:]

    if sys.argv[1] == 'b':
        compile_project()
    elif sys.argv[1] == 'r':
        compile_project(run_after_compile=True, program_args=program_args)
    elif sys.argv[1] == 'l':
        compile_project(compile_to_shared_only=True)
    else:
        print("Invalid argument. Use 'b' to build, 'r' to build and run, or 'l' to compile to shared libraries only.")
        sys.exit(1)

if __name__ == "__main__":
    main()
