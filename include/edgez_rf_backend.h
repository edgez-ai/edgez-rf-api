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

#define EDGEZ_RF_BACKEND_ABI_VERSION 4U
#define EDGEZ_UMAC_MAC_LEN 6U
#define EDGEZ_UMAC_KEY_MAX_LEN 32U
#define EDGEZ_UMAC_INVALID_VIF_ID UINT16_MAX

enum edgez_umac_peer_state
{
    EDGEZ_UMAC_PEER_NOT_PRESENT = 0,
    EDGEZ_UMAC_PEER_CREATED,
    EDGEZ_UMAC_PEER_AUTHENTICATED,
    EDGEZ_UMAC_PEER_ASSOCIATED,
    EDGEZ_UMAC_PEER_AUTHORIZED,
};

struct edgez_umac_channel
{
    uint32_t operating_frequency_hz;
    uint8_t operating_bw_mhz;
    uint8_t primary_bw_mhz;
    uint8_t primary_1mhz_channel_index;
};

struct edgez_umac_radio_info
{
    uint8_t mac_addr[EDGEZ_UMAC_MAC_LEN];
    uint32_t chip_id;
    uint8_t firmware_major;
    uint8_t firmware_minor;
    uint8_t firmware_patch;
};

struct edgez_umac_key
{
    uint8_t key[EDGEZ_UMAC_KEY_MAX_LEN];
    size_t key_len;
    uint64_t tx_packet_number;
    uint8_t key_index;
    bool pairwise;
};

enum edgez_umac_rx_format
{
    EDGEZ_UMAC_RX_RAW_80211 = 0,
    EDGEZ_UMAC_RX_PROTECTED_MESH_DATA,
};

/** Vendor-normalized header information available before CCMP decryption. */
struct edgez_umac_rx_mesh_data
{
    uint8_t receiver[EDGEZ_UMAC_MAC_LEN];
    uint8_t transmitter[EDGEZ_UMAC_MAC_LEN];
    /** Canonical Ethernet destination/source after vendor RX normalization. */
    uint8_t destination[EDGEZ_UMAC_MAC_LEN];
    uint8_t source[EDGEZ_UMAC_MAC_LEN];
    uint8_t address3[EDGEZ_UMAC_MAC_LEN];
    uint8_t address4[EDGEZ_UMAC_MAC_LEN];
    uint16_t sequence_number;
    uint8_t tid;
    bool has_address4;
    bool mesh_control_present;
    size_t mac_header_length;
    size_t protected_body_offset;
};

struct edgez_umac_rx_frame
{
    const uint8_t *data;
    size_t length;
    uint32_t frequency_hz;
    uint32_t timestamp_ms;
    int16_t rssi_dbm;
    int8_t noise_dbm;
    uint16_t driver_channel;
    uint8_t bandwidth_mhz;
    uint8_t vif_id;
    bool decrypted;
    enum edgez_umac_rx_format format;
    struct edgez_umac_rx_mesh_data mesh_data;
};

struct edgez_umac_tx_status
{
    uintptr_t token;
    const uint8_t *frame;
    size_t frame_length;
    uint8_t vif_id;
    uint8_t attempts;
    uint8_t status_flags;
};

enum edgez_rf_tx_status_flags
{
    EDGEZ_RF_TX_STATUS_FAILED = 1U << 0,
    EDGEZ_RF_TX_STATUS_AGGREGATED = 1U << 1,
};

enum edgez_umac_tx_frame_type
{
    EDGEZ_UMAC_TX_DATA = 0,
    EDGEZ_UMAC_TX_MANAGEMENT,
};

struct edgez_umac_tx_request
{
    uintptr_t token;
    const uint8_t *frame;
    size_t frame_length;
    enum edgez_umac_tx_frame_type frame_type;
    uint16_t aid;
    uint8_t tid;
    uint8_t key_index;
    uint8_t mcs;
    uint8_t attempts;
    uint8_t bandwidth_mhz;
    bool short_guard;
    bool rts_cts;
    bool hardware_encrypt;
    bool no_ack;
    bool immediate_status;
    /** Permit A-MPDU transmission after an originator BA agreement. */
    bool ampdu_enabled;
    /** Recipient BA window advertised to the radio, zero if not established. */
    uint8_t reorder_buffer_size;
};

struct edgez_rf_beacon_template
{
    const uint8_t *frame;
    size_t frame_length;
    uint8_t mcs;
    uint8_t bandwidth_mhz;
    bool short_guard;
};

/**
 * Vendor-neutral UMAC-to-radio contract.
 *
 * A backend owns all vendor SDK translation. The UMAC sees only immutable raw
 * frames, scalar metadata, and EdgeZ-owned types. Callbacks return zero on
 * success and a backend-local status value on failure.
 */
struct edgez_rf_backend_ops
{
    uint16_t abi_version;
    uint16_t size;
    const char *name;
    int (*prepare)(void *backend_ctx);
    void (*release)(void *backend_ctx);
    int (*radio_start)(void *backend_ctx, const char country_code[3],
                       struct edgez_umac_radio_info *info);
    void (*radio_stop)(void *backend_ctx);
    int (*add_mesh_interface)(void *backend_ctx,
                              const uint8_t mac_addr[EDGEZ_UMAC_MAC_LEN],
                              uint16_t *vif_id);
    int (*remove_interface)(void *backend_ctx, uint16_t vif_id);
    int (*set_channel)(void *backend_ctx,
                       const struct edgez_umac_channel *channel);
    int (*configure_bss)(void *backend_ctx, uint16_t vif_id,
                         uint16_t beacon_interval_tu, uint8_t dtim_period,
                         uint32_t compressed_ssid);
    /** Copy or clear the template; frame storage is borrowed for this call. */
    int (*set_beacon_template)(
        void *backend_ctx, uint16_t vif_id,
        const struct edgez_rf_beacon_template *beacon_template);
    int (*set_beaconing)(void *backend_ctx, uint16_t vif_id, bool enabled);
    int (*set_peer_state)(void *backend_ctx, uint16_t vif_id, uint16_t aid,
                          const uint8_t peer_addr[EDGEZ_UMAC_MAC_LEN],
                          enum edgez_umac_peer_state state);
    int (*install_key)(void *backend_ctx, uint16_t vif_id, uint16_t aid,
                       const struct edgez_umac_key *key,
                       uint8_t *hardware_key_index);
    int (*disable_key)(void *backend_ctx, uint16_t vif_id, uint16_t aid,
                       uint8_t hardware_key_index, bool pairwise);
    int (*transmit)(void *backend_ctx, uint16_t vif_id,
                    const struct edgez_umac_tx_request *request);
};

#define EDGEZ_RF_BACKEND_OPS_INIT(backend_name)          \
    {                                                     \
        .abi_version = EDGEZ_RF_BACKEND_ABI_VERSION,     \
        .size = sizeof(struct edgez_rf_backend_ops),     \
        .name = (backend_name),                          \
    }

#ifdef __cplusplus
}
#endif
