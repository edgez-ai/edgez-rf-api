/*
 * Copyright 2026 EdgeZeron AI AB
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum edgez_rf_status {
    EDGEZ_RF_SUCCESS = 0,
    EDGEZ_RF_ERROR,
    EDGEZ_RF_INVALID_ARGUMENT,
    EDGEZ_RF_UNAVAILABLE,
};

uint8_t edgez_rf_mesh_authorized_peer_count(void);
enum edgez_rf_status edgez_rf_mesh_authorized_peer(
    uint8_t peer_index, uint8_t peer_addr[6], int16_t *rssi_dbm,
    bool *rssi_valid);
enum edgez_rf_status edgez_rf_mesh_request_scan(void);

/* Hostap owns the concrete context; callers outside the adapter treat it as
 * opaque. */
void edgez_rf_hostap_set_mesh_context(void *context);

#ifdef __cplusplus
}
#endif
