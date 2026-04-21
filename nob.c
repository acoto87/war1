#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#define NOB_IMPLEMENTATION
#define NOB_EXPERIMENTAL_DELETE_OLD
#include "nob.h"

typedef enum {
    TOOLCHAIN_GCC,
    TOOLCHAIN_CLANG,
    TOOLCHAIN_MSVC,
} Toolchain;

typedef enum {
    TARGET_LINUX64,
    TARGET_ARM32,
    TARGET_WIN32,
    TARGET_WIN64,
} Target;

typedef struct {
    Toolchain toolchain;
    Target target;
    bool debug;
    bool check_only;
} Build_Options;

static const char *toolchain_name(Toolchain toolchain)
{
    switch (toolchain) {
        case TOOLCHAIN_GCC:   return "gcc";
        case TOOLCHAIN_CLANG: return "clang";
        case TOOLCHAIN_MSVC:  return "msvc";
        default:              return "unknown";
    }
}

static const char *toolchain_program(Toolchain toolchain)
{
    switch (toolchain) {
        case TOOLCHAIN_GCC:   return "gcc";
        case TOOLCHAIN_CLANG: return "clang";
        case TOOLCHAIN_MSVC:  return "cl";
        default:              return NULL;
    }
}

static const char *target_name(Target target)
{
    switch (target) {
        case TARGET_LINUX64: return "linux64";
        case TARGET_ARM32:   return "arm32";
        case TARGET_WIN32:   return "win32";
        case TARGET_WIN64:   return "win64";
        default:             return "unknown";
    }
}

static const char *target_output_dir(Target target)
{
    switch (target) {
        case TARGET_LINUX64: return "build/linux64";
        case TARGET_ARM32:   return "build/arm32";
        case TARGET_WIN32:   return "build/win32";
        case TARGET_WIN64:   return "build/win64";
        default:             return NULL;
    }
}

static const char *target_library_dir(Target target)
{
    switch (target) {
        case TARGET_LINUX64: return "deps/lib/linux64";
        case TARGET_ARM32:   return "deps/lib/arm32";
        case TARGET_WIN32:   return "deps/lib/win32";
        case TARGET_WIN64:   return "deps/lib/win64";
        default:             return NULL;
    }
}

static const char *target_binary_path(Target target)
{
    const char *output_dir = target_output_dir(target);
    switch (target) {
        case TARGET_WIN32:
        case TARGET_WIN64:
            return nob_temp_sprintf("%s/war1.exe", output_dir);

        case TARGET_LINUX64:
        case TARGET_ARM32:
            return nob_temp_sprintf("%s/war1", output_dir);

        default:
            return NULL;
    }
}

static bool target_is_windows(Target target)
{
    return target == TARGET_WIN32 || target == TARGET_WIN64;
}

static bool host_is_windows(void)
{
#ifdef _WIN32
    return true;
#else
    return false;
#endif
}

static Target default_target(void)
{
#ifdef _WIN32
#  if defined(_WIN64)
    return TARGET_WIN64;
#  else
    return TARGET_WIN32;
#  endif
#else
    return TARGET_LINUX64;
#endif
}

static Toolchain default_toolchain(void)
{
#if defined(_MSC_VER) && !defined(__clang__)
    return TOOLCHAIN_MSVC;
#elif defined(__clang__)
    return TOOLCHAIN_CLANG;
#else
    return TOOLCHAIN_GCC;
#endif
}

static void usage(const char *program)
{
    printf("Usage: %s [build] [--cc <gcc|clang|msvc>] [--target <linux64|arm32|win32|win64>] [--debug|--release] [--check]\n", program);
    printf("\n");
    printf("Examples:\n");
    printf("  %s build --cc gcc --target linux64\n", program);
    printf("  %s build --cc clang --target linux64\n", program);
    printf("  %s build --cc msvc --target win64 --check\n", program);
}

static bool parse_toolchain(const char *value, Toolchain *toolchain)
{
    if (strcmp(value, "gcc") == 0) {
        *toolchain = TOOLCHAIN_GCC;
        return true;
    }

    if (strcmp(value, "clang") == 0) {
        *toolchain = TOOLCHAIN_CLANG;
        return true;
    }

    if (strcmp(value, "msvc") == 0) {
        *toolchain = TOOLCHAIN_MSVC;
        return true;
    }

    return false;
}

static bool parse_target(const char *value, Target *target)
{
    if (strcmp(value, "linux64") == 0) {
        *target = TARGET_LINUX64;
        return true;
    }

    if (strcmp(value, "arm32") == 0 || strcmp(value, "rpi") == 0) {
        *target = TARGET_ARM32;
        return true;
    }

    if (strcmp(value, "win32") == 0) {
        *target = TARGET_WIN32;
        return true;
    }

    if (strcmp(value, "win64") == 0) {
        *target = TARGET_WIN64;
        return true;
    }

    return false;
}

static bool parse_args(int argc, char **argv, Build_Options *options)
{
    const char *program = nob_shift_args(&argc, &argv);
    bool command_consumed = false;

    while (argc > 0) {
        const char *arg = nob_shift_args(&argc, &argv);

        if (!command_consumed && strcmp(arg, "build") == 0) {
            command_consumed = true;
            continue;
        }

        command_consumed = true;

        if (strcmp(arg, "--cc") == 0) {
            if (argc <= 0) {
                nob_log(NOB_ERROR, "missing value after --cc");
                usage(program);
                return false;
            }

            if (!parse_toolchain(nob_shift_args(&argc, &argv), &options->toolchain)) {
                nob_log(NOB_ERROR, "unsupported toolchain");
                usage(program);
                return false;
            }

            continue;
        }

        if (strcmp(arg, "--target") == 0) {
            if (argc <= 0) {
                nob_log(NOB_ERROR, "missing value after --target");
                usage(program);
                return false;
            }

            if (!parse_target(nob_shift_args(&argc, &argv), &options->target)) {
                nob_log(NOB_ERROR, "unsupported target");
                usage(program);
                return false;
            }

            continue;
        }

        if (strcmp(arg, "--debug") == 0) {
            options->debug = true;
            continue;
        }

        if (strcmp(arg, "--release") == 0) {
            options->debug = false;
            continue;
        }

        if (strcmp(arg, "--check") == 0) {
            options->check_only = true;
            continue;
        }

        if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            usage(program);
            return false;
        }

        nob_log(NOB_ERROR, "unknown argument: %s", arg);
        usage(program);
        return false;
    }

    return true;
}

static bool nob_read_entire_dir_recursively(const char* parent, Nob_File_Paths* file_paths, int idx)
{
    // NOTE: This library to deal with files and directories seems nice: https://github.com/cxong/tinydir

    if (!nob_read_entire_dir(parent, file_paths)) {
        return false;
    }

    int count = file_paths->count;

    for (int i = idx; i < count; i++) {
        if (strcmp(file_paths->items[i], ".") == 0) continue;
        if (strcmp(file_paths->items[i], "..") == 0) continue;

        // NOTE: Here I'm leaking the allocation for each file name on the nob_read_entire_dir call
        // but I'm don't care that much because that alloc is done on the temp static arena
        file_paths->items[i] = nob_temp_sprintf("%s/%s", parent, file_paths->items[i]);

        Nob_File_Type file_type = nob_get_file_type(file_paths->items[i]);
        if (file_type == NOB_FILE_DIRECTORY) {
            if (!nob_read_entire_dir_recursively(file_paths->items[i], file_paths, file_paths->count)) {
                return false;
            }
        }
    }

    return true;
}

static bool clear_folder(const char* parent)
{
    Nob_File_Paths file_paths = {0};

    if (!nob_read_entire_dir_recursively(parent, &file_paths, 0)) {
        return false;
    }

    for (int i = 0; i < file_paths.count; i++) {
        Nob_File_Type file_type = nob_get_file_type(file_paths.items[i]);
        if (file_type == NOB_FILE_REGULAR) {
            if (!nob_delete_file(file_paths.items[i])) {
                return false;
            }
        }
    }

    file_paths.count = 0;

    return true;
}

static bool ensure_output_dirs(Target target)
{
    return nob_mkdir_if_not_exists("build") &&
           nob_mkdir_if_not_exists(target_output_dir(target));
}

static bool append_gnu_common_flags(Nob_Cmd *cmd, const Build_Options *options)
{
    nob_cmd_append(cmd,
                   toolchain_program(options->toolchain),
                   "-std=c11",
                   "-Wall",
                   "-Wno-misleading-indentation",
                   "-Wpedantic",
                   "-x",
                   "c",
                   "-Ideps/include");

    if (options->debug) {
        nob_cmd_append(cmd, "-g", "-D__DEBUG__=1");
    } else {
        nob_cmd_append(cmd, "-O2");
    }

    return true;
}

static bool append_msvc_common_flags(Nob_Cmd *cmd, const Build_Options *options)
{
    nob_cmd_append(cmd,
                   toolchain_program(options->toolchain),
                   "/nologo",
                   "/TC",
                   "/std:c11",
                   "/W4",
                   "/D_CRT_SECURE_NO_WARNINGS",
                   "/Ideps/include");

    if (options->debug) {
        nob_cmd_append(cmd, "/Zi", "/D__DEBUG__=1");
    } else {
        nob_cmd_append(cmd, "/O2", "/DNDEBUG");
    }

    return true;
}

static bool compile_gnu_source(const Build_Options *options, const char *source_path, const char *object_path)
{
    Nob_Cmd cmd = {0};
    append_gnu_common_flags(&cmd, options);
    nob_cmd_append(&cmd, "-c", source_path, "-o", object_path);
    return nob_cmd_run(&cmd);
}

static bool compile_msvc_source(const Build_Options *options, const char *source_path, const char *object_path)
{
    Nob_Cmd cmd = {0};
    append_msvc_common_flags(&cmd, options);
    nob_cmd_append(&cmd, "/c", source_path, nob_temp_sprintf("/Fo:%s", object_path));
    return nob_cmd_run(&cmd);
}

static bool copy_runtime_files(const Build_Options *options)
{
    const char *output_dir = target_output_dir(options->target);

    if (!nob_copy_directory_recursively("assets", output_dir)) {
        return false;
    }

    if (target_is_windows(options->target) && !options->check_only) {
        const char *dll_path = nob_temp_sprintf("%s/SDL3.dll", target_library_dir(options->target));
        const char *dll_copy_path = nob_temp_sprintf("%s/SDL3.dll", output_dir);

        if (!nob_copy_file(dll_path, dll_copy_path)) {
            return false;
        }
    }

    return true;
}

static bool build_with_gnu_like(const Build_Options *options)
{
    const char *binary_path = target_binary_path(options->target);

    if (!ensure_output_dirs(options->target)) {
        nob_log(NOB_ERROR, "Could not create output directories");
        return false;
    }

    if (!clear_folder(target_output_dir(options->target))) {
        nob_log(NOB_ERROR, "Could not clear output directory");
        return false;
    }

    if (options->check_only) {
        if (!compile_gnu_source(options, "src/war1.c", nob_temp_sprintf("%s/war1.o", target_output_dir(options->target)))) {
            return false;
        }

        return true;
    }

    Nob_Cmd cmd = {0};
    append_gnu_common_flags(&cmd, options);
    nob_cmd_append(&cmd,
                   "src/war1.c",
                   "-o",
                   binary_path,
                   nob_temp_sprintf("-L%s", target_library_dir(options->target)));

    if (target_is_windows(options->target)) {
        nob_cmd_append(&cmd, "-lSDL3", "-lws2_32");

        if (options->toolchain == TOOLCHAIN_GCC) {
            nob_cmd_append(&cmd, "-static-libgcc");
        }
    } else {
        nob_cmd_append(&cmd, "-lSDL3", "-lpthread", "-lm", "-ldl");

        if (options->target == TARGET_LINUX64) {
            nob_cmd_append(&cmd, "-no-pie");
        }
    }

    if (!nob_cmd_run(&cmd)) {
        return false;
    }

    return copy_runtime_files(options);
}

static bool build_with_msvc(const Build_Options *options)
{
    if (!host_is_windows()) {
        nob_log(NOB_ERROR, "msvc builds are only supported when running nob on Windows");
        return false;
    }

    if (!target_is_windows(options->target)) {
        nob_log(NOB_ERROR, "msvc builds require a Windows target");
        return false;
    }

    if (!ensure_output_dirs(options->target)) {
        nob_log(NOB_ERROR, "Could not create output directories");
        return false;
    }

    if (!clear_folder(target_output_dir(options->target))) {
        nob_log(NOB_ERROR, "Could not clear output directory");
        return false;
    }

    const char *output_dir = target_output_dir(options->target);
    const char *lib_dir = target_library_dir(options->target);

    if (options->check_only) {
        if (!compile_msvc_source(options, "src/war1.c", nob_temp_sprintf("%s/war1.obj", output_dir))) {
            return false;
        }

        return true;
    }

    // Full build: compile + link
    Nob_Cmd cmd = {0};
    append_msvc_common_flags(&cmd, options);
    nob_cmd_append(&cmd,
                   "src/war1.c",
                   nob_temp_sprintf("/Fe:%s", target_binary_path(options->target)),
                   nob_temp_sprintf("/Fo:%s/", output_dir),
                   "/link",
                   nob_temp_sprintf("/LIBPATH:%s", lib_dir),
                   "SDL3.lib",
                   "shell32.lib",
                   "ws2_32.lib");

    if (!nob_cmd_run(&cmd)) {
        return false;
    }

    return copy_runtime_files(options);
}

static bool build_project(const Build_Options *options)
{
    nob_log(NOB_INFO,
            "Building target=%s toolchain=%s mode=%s",
            target_name(options->target),
            toolchain_name(options->toolchain),
            options->check_only ? "check" : "build");

    switch (options->toolchain) {
        case TOOLCHAIN_GCC:
        case TOOLCHAIN_CLANG:
            return build_with_gnu_like(options);

        case TOOLCHAIN_MSVC:
            return build_with_msvc(options);

        default:
            nob_log(NOB_ERROR, "unsupported toolchain");
            return false;
    }
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    Build_Options options = {
        .toolchain = default_toolchain(),
        .target = default_target(),
        .debug = false,
        .check_only = false,
    };

    if (!parse_args(argc, argv, &options)) {
        return 1;
    }

    if (!build_project(&options)) {
        return 1;
    }

    return 0;
}
