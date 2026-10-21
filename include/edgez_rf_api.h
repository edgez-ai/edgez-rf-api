/*
 * Copyright 2026 EdgeZeron AI AB
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "edgez_rf_backend.h"

#ifdef __cplusplus
extern "C" {
#endif

struct edgez_umac;

struct edgez_rf_callback_metrics
{
    uint32_t rx_frames;
    uint32_t tx_status;
    uint32_t tx_pause_changes;
    uint32_t hardware_restarts;
    uint32_t scan_completions;
};

struct edgez_rf_regulatory_channel
{
    uint16_t duty_cycle_centi_percent;
    bool duty_cycle_omit_control_response;
    int8_t max_eirp_dbm;
};

/** Initialize platform services required by the selected RF backend. */
int edgez_rf_platform_init(void);

/** Return the selected platform radio backend. */
const struct edgez_rf_backend_ops *edgez_rf_driver_ops(void);

/** Bind or unbind the platform radio callbacks to an EdgeZ UMAC instance. */
int edgez_rf_bind_umac(struct edgez_umac *umac);
void edgez_rf_unbind_umac(void);

/** Optional observer bridge for integrations that retain a reference UMAC. */
int edgez_rf_callback_bridge_start(void);
void edgez_rf_callback_bridge_stop(void);
void edgez_rf_callback_bridge_get_metrics(
    struct edgez_rf_callback_metrics *metrics);

/** Forward native driver notifications through the optional observer bridge. */
bool edgez_rf_host_rx(void *packet, uint16_t driver_channel);
bool edgez_rf_host_tx_status(void *packet);
bool edgez_rf_host_tx_pause(uint16_t source_mask, bool paused);
bool edgez_rf_host_hardware_restart(void);
bool edgez_rf_host_scan_complete(int driver_scan_status);

/** Resolve an IEEE 802.11ah channel using the selected radio's regulatory DB. */
bool edgez_rf_resolve_s1g_channel(const char country_code[3], uint8_t channel,
                                  uint32_t *frequency_khz,
                                  uint8_t *bandwidth_mhz);

/** Resolve regulatory limits for an exact center frequency and bandwidth. */
bool edgez_rf_resolve_regulatory_channel(
    const char country_code[3], uint32_t frequency_hz, uint8_t bandwidth_mhz,
    struct edgez_rf_regulatory_channel *regulatory);

#ifdef __cplusplus
}
#endif
