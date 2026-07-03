/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT pixart_pmw3610_cpi

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <drivers/behavior.h>
#include <zmk/behavior.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(pmw3610, CONFIG_INPUT_LOG_LEVEL);

/* pmw3610.h pulls in register/timing macros guarded by CONFIG_PMW3610_* and
 * will #error if the driver isn't configured. Only the half with the trackball
 * needs the pmw3610_set_move_cpi() declaration, so include it there only. */
#if IS_ENABLED(CONFIG_PMW3610)
#include "pmw3610.h"
#endif

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

/* The behavior node lives in the shared dtsi so the &tb_cpi label resolves on
 * both halves of the split. Only the half that actually has the trackball
 * (CONFIG_PMW3610) talks to the sensor; on the other half this is a no-op so
 * the shared keymap still links. The sensor is resolved via DEVICE_DT_GET_ONE
 * instead of a DT phandle, which keeps the behavior free of a devicetree
 * dependency on the trackball (avoids init-priority ordering constraints). */

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
#if IS_ENABLED(CONFIG_PMW3610)
    const struct device *sensor = DEVICE_DT_GET_ONE(pixart_pmw3610);
    if (!device_is_ready(sensor)) {
        LOG_ERR("PMW3610 device not ready");
        return -ENODEV;
    }
    int ret = pmw3610_set_move_cpi(sensor, (uint32_t)binding->param1);
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

static const struct behavior_driver_api behavior_pmw3610_cpi_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
    .locality = BEHAVIOR_LOCALITY_CENTRAL,
};

#define PMW3610_CPI_INST(n)                                                                         \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL, POST_KERNEL,                                \
                            CONFIG_APPLICATION_INIT_PRIORITY,                                      \
                            &behavior_pmw3610_cpi_driver_api);

DT_INST_FOREACH_STATUS_OKAY(PMW3610_CPI_INST)

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */
