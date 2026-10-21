// LD_PRELOAD shim for Steam's ARM64 streaming_client (Remote Play, #285).
//
// With V4L2 hardware decode, streaming_client creates its NV12 textures with
// SDL.texture.create.colorspace set to 0 (SDL_COLORSPACE_UNKNOWN). SDL3's
// Vulkan renderer rejects that with "Unsupported YUV colorspace", so Steam
// drops hardware decoding and resets the stream. Replace it with BT.709 full
// range, which is what the host stream signals.
//
// Only touches explicit-unknown YUV textures in streaming_client; everything
// else passes straight through. SDL types and constants are spelled out so
// the shim needs no SDL headers.
#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef uint32_t SDL_PropertiesID;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;

#define PROP_FORMAT     "SDL.texture.create.format"
#define PROP_COLORSPACE "SDL.texture.create.colorspace"

// SDL_DEFINE_PIXELFOURCC('N','V','1','2') and ('N','V','2','1')
#define SDL_PIXELFORMAT_NV12      0x3231564Eu
#define SDL_PIXELFORMAT_NV21      0x3132564Eu
#define SDL_COLORSPACE_BT709_FULL 0x22100421u

typedef SDL_Texture *(*create_fn)(SDL_Renderer *, SDL_PropertiesID);
typedef int64_t (*get_number_fn)(SDL_PropertiesID, const char *, int64_t);
typedef bool (*set_number_fn)(SDL_PropertiesID, const char *, int64_t);

static bool enabled;
static bool logged;

__attribute__((constructor)) static void init(void)
{
    enabled = strcmp(program_invocation_short_name, "streaming_client") == 0;
}

static void fix_colorspace(SDL_PropertiesID props)
{
    get_number_fn get = (get_number_fn)dlsym(RTLD_NEXT, "SDL_GetNumberProperty");
    set_number_fn set = (set_number_fn)dlsym(RTLD_NEXT, "SDL_SetNumberProperty");
    if (!get || !set)
        return;

    int64_t format = get(props, PROP_FORMAT, 0);
    if (format != SDL_PIXELFORMAT_NV12 && format != SDL_PIXELFORMAT_NV21)
        return;
    // Unset lets SDL choose a default; only an explicit 0 is rejected.
    if (get(props, PROP_COLORSPACE, -1) != 0)
        return;

    if (set(props, PROP_COLORSPACE, SDL_COLORSPACE_BT709_FULL) && !logged) {
        logged = true;
        fprintf(stderr, "steam-stream-colorspace: YUV texture had an unknown "
                        "colorspace, using BT.709 full range\n");
    }
}

SDL_Texture *SDL_CreateTextureWithProperties(SDL_Renderer *renderer, SDL_PropertiesID props)
{
    static create_fn real;
    if (!real)
        real = (create_fn)dlsym(RTLD_NEXT, "SDL_CreateTextureWithProperties");
    if (!real)
        return NULL;

    if (enabled)
        fix_colorspace(props);
    return real(renderer, props);
}
