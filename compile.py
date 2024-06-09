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

def compile_to_dll(directory, build_dir, openssl_include_path, openssl_lib_path, postgres_include_path, postgres_lib_path):
    source_files = find_c_files(directory)
    if not source_files:
        return None
    
    dll_name = os.path.basename(directory)
    output = os.path.join(build_dir, f"{dll_name}.dll" if platform.system() == "Windows" else f"lib{dll_name}.so")
    
    flags = "-std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -Wno-deprecated-declarations -s"
    flags += f" -I{openssl_include_path} -L{openssl_lib_path} -I{postgres_include_path} -L{postgres_lib_path}"
    
    # dependencies based on the directory
    dependencies = []
    if directory in ["fmt"] or directory in ["file_io"]:
        dependencies.append("encoding")
        dependencies.append("string")
    elif directory in ["array"] or directory in ["priority_queue"] or directory in ["queue"] or directory in ["stack"] \
        or directory in ["network"]:
        dependencies.append("vector")
        dependencies.append("fmt")
    elif directory in ["vector"] or directory in ["bitset"] or directory in ["deque"] or directory in ["forward_list"] \
        or directory in ["list"] or directory in ["map"] or directory in ["span"] or directory in ["time"] or \
            directory in ["tuple"] or directory in ["matrix"]:
        dependencies.append("fmt")
    elif directory in ["config"] or directory in ["csv"] or directory in ["log"]:
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
    elif directory in ["cli"] or directory in ["database"]:
        dependencies.append("fmt")
        dependencies.append("string")
   

    command = f"gcc {flags} -shared -o {output} " + " ".join(source_files)
    command += " -lssl -lcrypto -lpq"
    
    for dep in dependencies:
        dep_path = os.path.join(build_dir, f"{dep}.dll" if platform.system() == "Windows" else f"lib{dep}.so")
        command += f" -L{build_dir} -l{os.path.splitext(os.path.basename(dep_path))[0]}"
    
    if platform.system() == "Windows":
        command += " -lWs2_32 -lAdvapi32"
        if "dir" in directory:
            command += " -lshlwapi"

    result = subprocess.run(command, shell=True)
    if result.returncode != 0:
        print(f"Failed to compile {directory} to DLL.")
        return None
    
    print(f"Successfully compiled {directory} to {output}.")
    return output

def compile_project(run_after_compile=False, compile_to_dll_only=False, program_args=[]):#
    source_directories = [
        # "numeric",
        # "algorithm",
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
        # "file_io",
        # "log",
        # "priority_queue",
        # "queue",
        # "span",
        # "stack",
        "string",
        # "network",
        # "vector",
        # "time",
        # "concurrent",
        # "date",
        # "dir",
        # "cli",
        # "crypto",
        # "tuple",
        # "matrix",
        "database",
    ]

    build_dir = "./build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    openssl_include_path = "./dependency/include"
    openssl_lib_path = "./dependency/lib"
    postgres_include_path = "./dependency/include"
    postgres_lib_path = "./dependency/lib"

    dlls = []
    for directory in source_directories:
        dll = compile_to_dll(directory, build_dir, openssl_include_path, openssl_lib_path, postgres_include_path, postgres_lib_path)
        if dll:
            dlls.append(dll)

    if compile_to_dll_only:
        return

    # Compile the main project
    source_files = ["./main.c"]
    flags = "-std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -Wno-deprecated-declarations -s"
    flags += f" -I{openssl_include_path} -L{openssl_lib_path} -I{postgres_include_path} -L{postgres_lib_path}"
    output = os.path.join(build_dir, "main.exe" if platform.system() == "Windows" else "main")
    
    if os.path.exists(output):
        os.remove(output)
    
    command = f"gcc {flags} -o {output} " + " ".join(source_files)
    for dll in dlls:
        command += f" -L{build_dir} -l{os.path.splitext(os.path.basename(dll))[0]}"

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
        print("Usage: python compile_project.py [b|r|l]")
        sys.exit(1)

    program_args = sys.argv[2:]

    if sys.argv[1] == 'b':
        compile_project()
    elif sys.argv[1] == 'r':
        compile_project(run_after_compile=True, program_args=program_args)
    elif sys.argv[1] == 'l':
        compile_project(compile_to_dll_only=True)
    else:
        print("Invalid argument. Use 'b' to build, 'r' to build and run, or 'l' to compile to DLLs only.")
        sys.exit(1)

if __name__ == "__main__":
    main()
