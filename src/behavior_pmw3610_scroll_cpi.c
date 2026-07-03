/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT pixart_pmw3610_scroll_cpi

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <drivers/behavior.h>
#include <zmk/behavior.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(pmw3610, CONFIG_INPUT_LOG_LEVEL);

/* See behavior_pmw3610_cpi.c for the cross-split rationale (shared dtsi node,
 * no-op on the half without the trackball, guarded pmw3610.h include). */
#if IS_ENABLED(CONFIG_PMW3610)
#include "pmw3610.h"
#endif

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
#if IS_ENABLED(CONFIG_PMW3610)
    const struct device *sensor = DEVICE_DT_GET_ONE(pixart_pmw3610);
    if (!device_is_ready(sensor)) {
        LOG_ERR("PMW3610 device not ready");
        return -ENODEV;
    }
    int ret = pmw3610_set_scroll_cpi(sensor, (uint32_t)binding->param1);
    return ret < 0 ? ret : ZMK_BEHAVIOR_OPAQUE;
#else
    /* No trackball on this half; nothing to do. */
    return ZMK_BEHAVIOR_OPAQUE;
#endif
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_pmw3610_scroll_cpi_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
    .locality = BEHAVIOR_LOCALITY_CENTRAL,
};

#define PMW3610_SCROLL_CPI_INST(n)                                                                  \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL, POST_KERNEL,                                \
                            CONFIG_APPLICATION_INIT_PRIORITY,                                      \
                            &behavior_pmw3610_scroll_cpi_driver_api);

DT_INST_FOREACH_STATUS_OKAY(PMW3610_SCROLL_CPI_INST)

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */
