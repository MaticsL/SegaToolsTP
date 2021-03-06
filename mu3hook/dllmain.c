#include <windows.h>

#include "board/io4.h"
#include "board/sg-reader.h"
#include "board/vfd.h"

#include "hook/process.h"

#include "hooklib/serial.h"
#include "hooklib/spike.h"

#include "mu3hook/config.h"
#include "mu3hook/io4.h"

#include "platform/platform.h"

#include "util/dprintf.h"

static HMODULE mu3_hook_mod;
static process_entry_t mu3_startup;
static struct mu3_hook_config mu3_hook_cfg;

static DWORD CALLBACK mu3_pre_startup(void)
{
    HRESULT hr;

    dprintf("--- Begin mu3_pre_startup ---\n");

    /* Load config */

    mu3_hook_config_load(&mu3_hook_cfg, L".\\segatools.ini");

    /* Hook Win32 APIs */

    serial_hook_init();

    /* Initialize emulation hooks */

    hr = platform_hook_init(
            &mu3_hook_cfg.platform,
            "SDDT",
            "AAV2",
            mu3_hook_mod);

    if (FAILED(hr)) {
        return hr;
    }

    hr = sg_reader_hook_init(&mu3_hook_cfg.aime, 1);

    if (FAILED(hr)) {
        return hr;
    }

    hr = vfd_hook_init(2);

    if (FAILED(hr)) {
        return hr;
    }

    hr = mu3_io4_hook_init();

    if (FAILED(hr)) {
        return hr;
    }

    /* Initialize debug helpers */

    spike_hook_init(L".\\segatools.ini");

    dprintf("---  End  mu3_pre_startup ---\n");

    /* Jump to EXE start address */

    return mu3_startup();
}

BOOL WINAPI DllMain(HMODULE mod, DWORD cause, void *ctx)
{
    HRESULT hr;

    if (cause != DLL_PROCESS_ATTACH) {
        return TRUE;
    }

    mu3_hook_mod = mod;

    hr = process_hijack_startup(mu3_pre_startup, &mu3_startup);

    if (!SUCCEEDED(hr)) {
        dprintf("Failed to hijack process startup: %x\n", (int) hr);
    }

    return SUCCEEDED(hr);
}
