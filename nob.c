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
    TARGET_ARM64,  // Raspberry Pi 4/5 (64-bit OS)
    TARGET_WIN32,
    TARGET_WIN64,
} Target;

typedef enum {
    COMMAND_BUILD,
    COMMAND_RUN,
    COMMAND_EDITOR,
    COMMAND_RUN_EDITOR,
} Command;

typedef struct {
    Command command;
    Toolchain toolchain;
    Target target;
    bool debug;
    bool check_only;
    bool profile;           // enable Tracy profiler instrumentation
    bool build_before_run;  // set when build flags are passed alongside 'run'
    int app_argc;           // extra args forwarded to the war1 executable (after --)
    char **app_argv;
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
        case TARGET_ARM64:   return "arm64";
        case TARGET_WIN32:   return "win32";
        case TARGET_WIN64:   return "win64";
        default:             return "unknown";
    }
}

static const char *target_output_dir(Target target)
{
    switch (target) {
        case TARGET_LINUX64: return "build/linux64";
        case TARGET_ARM64:   return "build/arm64";
        case TARGET_WIN32:   return "build/win32";
        case TARGET_WIN64:   return "build/win64";
        default:             return NULL;
    }
}

static const char *target_library_dir(Target target)
{
    switch (target) {
        case TARGET_LINUX64: return "deps/lib/linux64";
        case TARGET_ARM64:   return "deps/lib/arm64";
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
        case TARGET_ARM64:
            return nob_temp_sprintf("%s/war1", output_dir);

        default:
            return NULL;
    }
}

static const char *editor_binary_path(Target target)
{
    const char *output_dir = target_output_dir(target);
    switch (target) {
        case TARGET_WIN32:
        case TARGET_WIN64:
            return nob_temp_sprintf("%s/war1_editor.exe", output_dir);

        case TARGET_LINUX64:
        case TARGET_ARM64:
            return nob_temp_sprintf("%s/war1_editor", output_dir);

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
#elif defined(__aarch64__)
    return TARGET_ARM64;
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
    printf("Usage: %s [build|run|editor] [--cc <gcc|clang|msvc>] [--target <linux64|arm64|win32|win64>] [--debug|--release] [--check] [--profile] [-- <app args>]\n", program);
    printf("\n");
    printf("Examples:\n");
    printf("  %s build --cc gcc --target linux64\n", program);
    printf("  %s build --cc clang --target linux64\n", program);
    printf("  %s build --cc msvc --target win64 --check\n", program);
    printf("  %s build --cc gcc --target arm64\n", program);
    printf("  %s run --target linux64\n", program);
    printf("  %s run --cc gcc --target linux64 --debug\n", program);
    printf("  %s build --cc msvc --target win64 --profile\n", program);
    printf("  %s editor --cc msvc --target win64 --check\n", program);
    printf("  %s editor --cc gcc --target linux64\n", program);
    printf("  %s run editor --target win64\n", program);
    printf("  %s run editor --cc msvc --target win64 --debug\n", program);
    printf("  %s run -- --map 5 --skip-intro\n", program);
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

    if (strcmp(value, "arm64") == 0 || strcmp(value, "rpi4") == 0 || strcmp(value, "rpi5") == 0) {
        *target = TARGET_ARM64;
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
            options->command = COMMAND_BUILD;
            command_consumed = true;
            continue;
        }

        if (!command_consumed && strcmp(arg, "run") == 0) {
            // Check for optional 'editor' sub-command: "run editor"
            if (argc > 0 && strcmp(argv[0], "editor") == 0) {
                nob_shift_args(&argc, &argv); // consume "editor"
                options->command = COMMAND_RUN_EDITOR;
            } else {
                options->command = COMMAND_RUN;
            }
            command_consumed = true;
            continue;
        }

        if (!command_consumed && strcmp(arg, "editor") == 0) {
            options->command = COMMAND_EDITOR;
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

            if (options->command == COMMAND_RUN || options->command == COMMAND_RUN_EDITOR) options->build_before_run = true;
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

            if (options->command == COMMAND_RUN || options->command == COMMAND_RUN_EDITOR) options->build_before_run = true;
            continue;
        }

        if (strcmp(arg, "--debug") == 0) {
            options->debug = true;
            if (options->command == COMMAND_RUN || options->command == COMMAND_RUN_EDITOR) options->build_before_run = true;
            continue;
        }

        if (strcmp(arg, "--release") == 0) {
            options->debug = false;
            if (options->command == COMMAND_RUN || options->command == COMMAND_RUN_EDITOR) options->build_before_run = true;
            continue;
        }

        if (strcmp(arg, "--check") == 0) {
            options->check_only = true;
            continue;
        }

        if (strcmp(arg, "--profile") == 0) {
            options->profile = true;
            if (options->command == COMMAND_RUN || options->command == COMMAND_RUN_EDITOR) options->build_before_run = true;
            continue;
        }

        if (strcmp(arg, "--") == 0) {
            // Everything after -- is forwarded to the war1 executable
            options->app_argc = argc;
            options->app_argv = argv;
            break;
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

static bool delete_build_output_if_exists(const char *path)
{
    if (!nob_file_exists(path)) {
        return true;
    }

    return nob_delete_file(path);
}

static bool clear_build_outputs(const Build_Options *options, const char *base_name)
{
    const char *output_dir = target_output_dir(options->target);

    if (!output_dir) {
        return false;
    }

    if (!delete_build_output_if_exists(nob_temp_sprintf("%s/%s.o", output_dir, base_name))) return false;
    if (!delete_build_output_if_exists(nob_temp_sprintf("%s/%s.obj", output_dir, base_name))) return false;
    if (!delete_build_output_if_exists(nob_temp_sprintf("%s/%s.pdb", output_dir, base_name))) return false;
    if (!delete_build_output_if_exists(nob_temp_sprintf("%s/%s.ilk", output_dir, base_name))) return false;
    if (!delete_build_output_if_exists(nob_temp_sprintf("%s/%s.exp", output_dir, base_name))) return false;
    if (!delete_build_output_if_exists(nob_temp_sprintf("%s/%s.lib", output_dir, base_name))) return false;
    if (!delete_build_output_if_exists(nob_temp_sprintf("%s/%s.res", output_dir, base_name))) return false;
    if (!delete_build_output_if_exists(nob_temp_sprintf("%s/%s.res.o", output_dir, base_name))) return false;

    if (target_is_windows(options->target)) {
        return delete_build_output_if_exists(nob_temp_sprintf("%s/%s.exe", output_dir, base_name));
    }

    return delete_build_output_if_exists(nob_temp_sprintf("%s/%s", output_dir, base_name));
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
                   "-Ideps/include");

    if (options->debug) {
        nob_cmd_append(cmd, "-g", "-D__DEBUG__=1", "-DSHL_MZ_DEBUG");
    } else {
        nob_cmd_append(cmd, "-O2");
    }

    if (options->profile) {
        nob_cmd_append(cmd, "-DTRACY_ENABLE");
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
        nob_cmd_append(cmd, "/Zi", "/D__DEBUG__=1", "/DSHL_MZ_DEBUG");
    } else {
        nob_cmd_append(cmd, "/Zi", "/O2", "/DNDEBUG");
    }

    if (options->profile) {
        nob_cmd_append(cmd, "/DTRACY_ENABLE");
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

static bool compile_windows_resource_gnu_like(const Build_Options *options,
                                              const char *rc_path,
                                              const char *res_object_path)
{
    Nob_Cmd cmd = {0};

    (void)options;

    nob_cmd_append(&cmd, "windres", "-i", rc_path, "-O", "coff", "-o", res_object_path);
    return nob_cmd_run(&cmd);
}

static bool compile_windows_resource_msvc(const Build_Options *options,
                                          const char *rc_path,
                                          const char *res_path)
{
    Nob_Cmd cmd = {0};

    (void)options;

    nob_cmd_append(&cmd, "rc", "/nologo", nob_temp_sprintf("/fo%s", res_path), rc_path);
    return nob_cmd_run(&cmd);
}

static bool copy_runtime_files(const Build_Options *options)
{
    const char *output_dir = target_output_dir(options->target);

    if (!nob_copy_directory_recursively("assets", output_dir)) {
        return false;
    }

    if (!options->check_only) {
        if (target_is_windows(options->target)) {
            const char *dll_path = nob_temp_sprintf("%s/SDL3.dll", target_library_dir(options->target));
            const char *dll_copy_path = nob_temp_sprintf("%s/SDL3.dll", output_dir);

            if (!nob_copy_file(dll_path, dll_copy_path)) {
                return false;
            }

            if (options->profile) {
                const char *tracy_dll_path = nob_temp_sprintf("%s/Tracy.dll", target_library_dir(options->target));
                const char *tracy_dll_copy_path = nob_temp_sprintf("%s/Tracy.dll", output_dir);

                if (!nob_copy_file(tracy_dll_path, tracy_dll_copy_path)) {
                    return false;
                }
            }
        } else {
            // Copy SDL3 shared libraries so the binary can find them via $ORIGIN rpath
            const char *lib_dir = target_library_dir(options->target);
            const char *so_names[] = { "libSDL3.so.0.5.0", "libSDL3.so.0", "libSDL3.so" };
            for (size_t i = 0; i < NOB_ARRAY_LEN(so_names); i++) {
                const char *src = nob_temp_sprintf("%s/%s", lib_dir, so_names[i]);
                const char *dst = nob_temp_sprintf("%s/%s", output_dir, so_names[i]);
                if (!nob_copy_file(src, dst)) {
                    return false;
                }
            }

            if (options->profile) {
                const char *tracy_src = nob_temp_sprintf("%s/libTracy.so", target_library_dir(options->target));
                const char *tracy_dst = nob_temp_sprintf("%s/libTracy.so", output_dir);
                if (!nob_copy_file(tracy_src, tracy_dst)) {
                    return false;
                }
            }
        }
    }

    return true;
}

static bool build_with_gnu_like(const Build_Options *options)
{
    const char *binary_path = target_binary_path(options->target);
    const char *output_dir = target_output_dir(options->target);
    const char *res_object_path = NULL;

    if (!ensure_output_dirs(options->target)) {
        nob_log(NOB_ERROR, "Could not create output directories");
        return false;
    }

    if (!clear_build_outputs(options, "war1")) {
        nob_log(NOB_ERROR, "Could not clear previous game outputs");
        return false;
    }

    if (options->check_only) {
        if (!compile_gnu_source(options, "src/war1.c", nob_temp_sprintf("%s/war1.o", output_dir))) {
            return false;
        }

        return true;
    }

    if (target_is_windows(options->target)) {
        res_object_path = nob_temp_sprintf("%s/war1.res.o", output_dir);
        if (!compile_windows_resource_gnu_like(options, "src/war1.rc", res_object_path)) {
            return false;
        }
    }

    Nob_Cmd cmd = {0};
    append_gnu_common_flags(&cmd, options);
    nob_cmd_append(&cmd,
                   "src/war1.c",
                   "-o",
                   binary_path,
                   nob_temp_sprintf("-L%s", target_library_dir(options->target)));

    if (res_object_path) {
        nob_cmd_append(&cmd, res_object_path);
    }

    if (target_is_windows(options->target)) {
        nob_cmd_append(&cmd, "-lSDL3", "-lws2_32");

        if (options->toolchain == TOOLCHAIN_GCC) {
            nob_cmd_append(&cmd, "-static-libgcc");
        }

        if (options->profile) {
            nob_cmd_append(&cmd, "-lTracy");
        }
    } else {
        nob_cmd_append(&cmd, "-lSDL3", "-lpthread", "-lm", "-ldl", "-Wl,-rpath,$ORIGIN");

        if (options->target == TARGET_LINUX64) {
            nob_cmd_append(&cmd, "-no-pie");
        }

        if (options->profile) {
            nob_cmd_append(&cmd, "-lTracy");
        }
    }

    if (!nob_cmd_run(&cmd)) {
        return false;
    }

    return copy_runtime_files(options);
}

static bool build_with_msvc(const Build_Options *options)
{
    const char *res_path = NULL;

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

    if (!clear_build_outputs(options, "war1")) {
        nob_log(NOB_ERROR, "Could not clear previous game outputs");
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

    res_path = nob_temp_sprintf("%s/war1.res", output_dir);
    if (!compile_windows_resource_msvc(options, "src/war1.rc", res_path)) {
        return false;
    }

    // Full build: compile + link
    Nob_Cmd cmd = {0};
    append_msvc_common_flags(&cmd, options);
    nob_cmd_append(&cmd,
                   "src/war1.c",
                   nob_temp_sprintf("/Fe:%s", target_binary_path(options->target)),
                   nob_temp_sprintf("/Fo:%s/", output_dir),
                   "/link",
                   res_path,
                   nob_temp_sprintf("/LIBPATH:%s", lib_dir),
                   "SDL3.lib",
                   "shell32.lib",
                   "ws2_32.lib");

    if (options->profile) {
        nob_cmd_append(&cmd, "Tracy.lib");
    }

    if (!nob_cmd_run(&cmd)) {
        return false;
    }

    return copy_runtime_files(options);
}

static bool append_editor_defines_gnu(Nob_Cmd *cmd)
{
    nob_cmd_append(cmd,
                   "-Isrc",
                   "-DWAR_EDITOR_BUILD",
                   "-DCIMGUI_DEFINE_ENUMS_AND_STRUCTS",
                   "-DCIMGUI_NO_EXPORT",
                   "-DCIMGUI_USE_SDL3",
                   "-DCIMGUI_USE_SDLRENDERER3",
                   "-Ideps/include/cimgui");
    return true;
}

static bool append_editor_defines_msvc(Nob_Cmd *cmd)
{
    nob_cmd_append(cmd,
                   "/DWAR_EDITOR_BUILD",
                   "/DCIMGUI_DEFINE_ENUMS_AND_STRUCTS",
                   "/DCIMGUI_NO_EXPORT",
                   "/DCIMGUI_USE_SDL3",
                   "/DCIMGUI_USE_SDLRENDERER3",
                   "/Ideps/include/cimgui");
    return true;
}

static bool build_editor_with_gnu_like(const Build_Options *options)
{
    const char *output_dir = target_output_dir(options->target);
    const char *res_object_path = NULL;

    if (!ensure_output_dirs(options->target)) {
        nob_log(NOB_ERROR, "Could not create output directories");
        return false;
    }

    if (!clear_build_outputs(options, "war1_editor")) {
        nob_log(NOB_ERROR, "Could not clear previous editor outputs");
        return false;
    }

    if (options->check_only) {
        Nob_Cmd cmd = {0};
        append_gnu_common_flags(&cmd, options);
        append_editor_defines_gnu(&cmd);
        nob_cmd_append(&cmd,
                       "-c",
                       "src/war1_editor.c",
                       "-o",
                       nob_temp_sprintf("%s/war1_editor.o", output_dir));
        return nob_cmd_run(&cmd);
    }

    if (target_is_windows(options->target)) {
        res_object_path = nob_temp_sprintf("%s/war1_editor.res.o", output_dir);
        if (!compile_windows_resource_gnu_like(options, "src/war1_editor.rc", res_object_path)) {
            return false;
        }
    }

    // Full build: compile + link
    const char *lib_dir = target_library_dir(options->target);

    Nob_Cmd cmd = {0};
    append_gnu_common_flags(&cmd, options);
    append_editor_defines_gnu(&cmd);
    nob_cmd_append(&cmd,
                   "src/war1_editor.c",
                   "-o",
                   editor_binary_path(options->target),
                   nob_temp_sprintf("-L%s", lib_dir));

    if (res_object_path) {
        nob_cmd_append(&cmd, res_object_path);
    }

    if (target_is_windows(options->target)) {
        nob_cmd_append(&cmd, "-lcimgui", "-lSDL3", "-lws2_32", "-lstdc++");
        if (options->toolchain == TOOLCHAIN_GCC) {
            nob_cmd_append(&cmd, "-static-libgcc");
        }
    } else {
        // cimgui is a C++ static lib; must link C++ runtime explicitly from C
        const char *cimgui_lib = nob_temp_sprintf("%s/libcimgui.a", lib_dir);
        nob_cmd_append(&cmd,
                       cimgui_lib,
                       "-lSDL3",
                       "-lpthread",
                       "-lm",
                       "-ldl",
                       "-lstdc++",
                       "-Wl,-rpath,$ORIGIN");
        if (options->target == TARGET_LINUX64) {
            nob_cmd_append(&cmd, "-no-pie");
        }
    }

    if (!nob_cmd_run(&cmd)) {
        return false;
    }

    return copy_runtime_files(options);
}

static bool build_editor_with_msvc(const Build_Options *options)
{
    const char *res_path = NULL;

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

    if (!clear_build_outputs(options, "war1_editor")) {
        nob_log(NOB_ERROR, "Could not clear previous editor outputs");
        return false;
    }

    const char *output_dir = target_output_dir(options->target);
    const char *lib_dir = target_library_dir(options->target);

    if (options->check_only) {
        Nob_Cmd cmd = {0};
        append_msvc_common_flags(&cmd, options);
        append_editor_defines_msvc(&cmd);
        nob_cmd_append(&cmd,
                       "/c",
                       "src/war1_editor.c",
                       nob_temp_sprintf("/Fo:%s/war1_editor.obj", output_dir));
        return nob_cmd_run(&cmd);
    }

    res_path = nob_temp_sprintf("%s/war1_editor.res", output_dir);
    if (!compile_windows_resource_msvc(options, "src/war1_editor.rc", res_path)) {
        return false;
    }

    // Full build: compile + link
    Nob_Cmd cmd = {0};
    append_msvc_common_flags(&cmd, options);
    append_editor_defines_msvc(&cmd);
    nob_cmd_append(&cmd,
                   "src/war1_editor.c",
                   nob_temp_sprintf("/Fe:%s/war1_editor.exe", output_dir),
                   nob_temp_sprintf("/Fo:%s/", output_dir),
                   "/link",
                   res_path,
                   nob_temp_sprintf("/LIBPATH:%s", lib_dir),
                   "SDL3.lib",
                   "cimgui.lib",
                   "shell32.lib",
                   "ws2_32.lib");

    if (!nob_cmd_run(&cmd)) {
        return false;
    }

    return copy_runtime_files(options);
}

static bool build_editor(const Build_Options *options)
{
    nob_log(NOB_INFO,
            "Building editor target=%s toolchain=%s mode=%s build=%s",
            target_name(options->target),
            toolchain_name(options->toolchain),
            options->check_only ? "check" : "build",
            options->debug ? "debug" : "release");

    switch (options->toolchain) {
        case TOOLCHAIN_GCC:
        case TOOLCHAIN_CLANG:
            return build_editor_with_gnu_like(options);

        case TOOLCHAIN_MSVC:
            return build_editor_with_msvc(options);

        default:
            nob_log(NOB_ERROR, "unsupported toolchain");
            return false;
    }
}

static bool build_project(const Build_Options *options)
{
    nob_log(NOB_INFO,
            "Building target=%s toolchain=%s mode=%s%s build=%s",
            target_name(options->target),
            toolchain_name(options->toolchain),
            options->check_only ? "check" : "build",
            options->profile ? " +profile" : "",
            options->debug ? "debug" : "release");

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

static bool run_project(const Build_Options *options)
{
    if (target_is_windows(options->target) && !host_is_windows()) {
        nob_log(NOB_ERROR, "Cannot run a Windows binary on a non-Windows host");
        return false;
    }

    const char *output_dir = target_output_dir(options->target);
    const char *binary = target_is_windows(options->target) ? "war1.exe" : "./war1";

    nob_log(NOB_INFO, "Changing directory to: %s", output_dir);

    if (!nob_set_current_dir(output_dir)) {
        nob_log(NOB_ERROR, "Could not change to directory: %s", output_dir);
        return false;
    }

    nob_log(NOB_INFO, "Running: %s", binary);

    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, binary);
    for (int i = 0; i < options->app_argc; i++) {
        nob_da_append(&cmd, options->app_argv[i]);
    }
    return nob_cmd_run(&cmd);
}

static bool run_editor_project(const Build_Options *options)
{
    if (target_is_windows(options->target) && !host_is_windows()) {
        nob_log(NOB_ERROR, "Cannot run a Windows binary on a non-Windows host");
        return false;
    }

    const char *output_dir = target_output_dir(options->target);
    const char *binary = target_is_windows(options->target) ? "war1_editor.exe" : "./war1_editor";

    nob_log(NOB_INFO, "Changing directory to: %s", output_dir);

    if (!nob_set_current_dir(output_dir)) {
        nob_log(NOB_ERROR, "Could not change to directory: %s", output_dir);
        return false;
    }

    nob_log(NOB_INFO, "Running: %s", binary);

    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, binary);
    for (int i = 0; i < options->app_argc; i++) {
        nob_da_append(&cmd, options->app_argv[i]);
    }
    return nob_cmd_run(&cmd);
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    Build_Options options = {
        .command        = COMMAND_BUILD,
        .toolchain      = default_toolchain(),
        .target         = default_target(),
        .debug          = false,
        .check_only     = false,
        .profile        = false,
        .build_before_run = false,
    };

    if (!parse_args(argc, argv, &options)) {
        return 1;
    }

    switch (options.command) {
        case COMMAND_BUILD:
            if (!build_project(&options)) return 1;
            break;
        case COMMAND_RUN:
            if (options.build_before_run) {
                if (!build_project(&options)) return 1;
            }
            if (!run_project(&options)) return 1;
            break;
        case COMMAND_EDITOR:
            if (!build_editor(&options)) return 1;
            break;
        case COMMAND_RUN_EDITOR:
            if (options.build_before_run) {
                if (!build_editor(&options)) return 1;
            }
            if (!run_editor_project(&options)) return 1;
            break;
        default:
            nob_log(NOB_ERROR, "unknown command");
            return 1;
    }

    return 0;
}
