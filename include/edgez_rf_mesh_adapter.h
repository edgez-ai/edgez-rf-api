/*
 * Copyright 2026 EdgeZeron AI AB
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EDGEZ_RF_MESH_BACKEND_MAC_LEN 6U

enum edgez_rf_mesh_backend_peer_event_type {
    EDGEZ_RF_MESH_BACKEND_PEER_DISCOVERED = 0,
    EDGEZ_RF_MESH_BACKEND_SAE_ACCEPTED,
    EDGEZ_RF_MESH_BACKEND_SAE_REJECTED,
    EDGEZ_RF_MESH_BACKEND_MPM_OPEN_ACCEPTED,
    EDGEZ_RF_MESH_BACKEND_MPM_CONFIRM_ACCEPTED,
    EDGEZ_RF_MESH_BACKEND_MPM_CLOSE_ACCEPTED,
    EDGEZ_RF_MESH_BACKEND_PEERING_REJECTED,
    EDGEZ_RF_MESH_BACKEND_PEERING_ESTABLISHED,
    EDGEZ_RF_MESH_BACKEND_PEER_REMOVED,
};

enum edgez_rf_mesh_backend_peer_operation {
    EDGEZ_RF_MESH_BACKEND_START_SAE = 0,
    EDGEZ_RF_MESH_BACKEND_SEND_OPEN,
    EDGEZ_RF_MESH_BACKEND_SEND_CONFIRM,
    EDGEZ_RF_MESH_BACKEND_AUTHORIZE,
    EDGEZ_RF_MESH_BACKEND_DEAUTHORIZE,
    EDGEZ_RF_MESH_BACKEND_SEND_CLOSE,
    EDGEZ_RF_MESH_BACKEND_REMOVE,
};

enum edgez_rf_mesh_backend_mgmt_disposition {
    EDGEZ_RF_MESH_BACKEND_MGMT_PASS = 0,
    EDGEZ_RF_MESH_BACKEND_MGMT_CONSUMED,
    EDGEZ_RF_MESH_BACKEND_MGMT_DROP,
};

struct edgez_rf_mesh_backend_peer_operation_request {
    enum edgez_rf_mesh_backend_peer_operation operation;
    uint8_t peer_addr[EDGEZ_RF_MESH_BACKEND_MAC_LEN];
    uint16_t local_link_id;
    uint16_t peer_link_id;
    uint16_t reason;
};

typedef int (*edgez_rf_mesh_backend_peer_operation_executor)(
    void *context,
    const struct edgez_rf_mesh_backend_peer_operation_request *request);
typedef void (*edgez_rf_mesh_backend_timer_handler)(
    void *backend_context, void *handler_context);
typedef int (*edgez_rf_mesh_backend_timer_scheduler)(
    void *backend_context, uint32_t delay_ms,
    edgez_rf_mesh_backend_timer_handler handler, void *handler_context);
typedef void (*edgez_rf_mesh_backend_timer_canceler)(
    void *backend_context, edgez_rf_mesh_backend_timer_handler handler,
    void *handler_context);

struct edgez_rf_mesh_backend_peer_event {
    enum edgez_rf_mesh_backend_peer_event_type type;
    uint8_t peer_addr[EDGEZ_RF_MESH_BACKEND_MAC_LEN];
    uint16_t local_link_id;
    uint16_t peer_link_id;
    uint16_t reason;
    edgez_rf_mesh_backend_peer_operation_executor operation_executor;
    void *operation_context;
    edgez_rf_mesh_backend_timer_scheduler timer_scheduler;
    edgez_rf_mesh_backend_timer_canceler timer_canceler;
};

int edgez_rf_mesh_backend_execute_peer_operation(
    void *context,
    const struct edgez_rf_mesh_backend_peer_operation_request *request);
int edgez_rf_mesh_backend_schedule_timer(
    void *backend_context, uint32_t delay_ms,
    edgez_rf_mesh_backend_timer_handler handler, void *handler_context);
void edgez_rf_mesh_backend_cancel_timer(
    void *backend_context, edgez_rf_mesh_backend_timer_handler handler,
    void *handler_context);
void edgez_rf_mesh_backend_emit_peer_event(
    enum edgez_rf_mesh_backend_peer_event_type type,
    const uint8_t peer_addr[EDGEZ_RF_MESH_BACKEND_MAC_LEN],
    uint16_t local_link_id, uint16_t peer_link_id, uint16_t reason,
    void *operation_context);
void edgez_rf_mesh_backend_emit_peer_operation(
    enum edgez_rf_mesh_backend_peer_operation operation,
    const uint8_t peer_addr[EDGEZ_RF_MESH_BACKEND_MAC_LEN]);
enum edgez_rf_mesh_backend_mgmt_disposition edgez_rf_mesh_backend_route_mgmt_rx(
    const uint8_t *frame, size_t length);
bool edgez_rf_mesh_backend_is_authoritative(void);

#ifdef __cplusplus
}
#endif
