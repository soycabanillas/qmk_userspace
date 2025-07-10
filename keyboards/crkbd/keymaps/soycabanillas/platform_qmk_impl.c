#include "platform_qmk.h"

#ifdef PLATFORM_QMK
#include "_wait.h"
#include "action.h"
#include "action_layer.h"
#include "deferred_exec.h"
#include "timer.h"
#elif defined(PLATFORM_TEST)
// For test environment, we rely on the mock definitions
// All the necessary function definitions will be mocked
#include <stdint.h>
#include <stdbool.h>
#endif

#ifdef PLATFORM_QMK

uint8_t platform_current_layer = 0;

// Timer implementations
platform_time_t platform_timer_read(void) {
    return timer_read();
}

void platform_wait_ms(uint16_t ms) {
    wait_ms(ms);
}

void platform_layer_select(uint8_t layer)
{
    platform_current_layer = layer;
    layer_move(layer);
}

// Key function implementations
void platform_register_code(platform_keycode_t keycode) {
    register_code(keycode);
}

void platform_unregister_code(platform_keycode_t keycode) {
    unregister_code(keycode);
}


// Deferred execution implementations
platform_deferred_token platform_defer_exec(uint32_t delay_ms, platform_deferred_callback callback, void *cb_arg) {
    return defer_exec(delay_ms, callback, cb_arg);
}

void platform_cancel_deferred_exec(platform_deferred_token token) {
    cancel_deferred_exec(token);
}

#endif // PLATFORM_QMK

