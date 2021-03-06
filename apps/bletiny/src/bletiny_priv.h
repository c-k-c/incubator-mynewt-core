/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef H_BLETINY_PRIV_
#define H_BLETINY_PRIV_

#include <inttypes.h>
#include "nimble/nimble_opt.h"
#include "log/log.h"
#include "os/queue.h"

#include "host/ble_gatt.h"
struct ble_gap_white_entry;
struct ble_hs_adv_fields;
struct ble_gap_upd_params;
struct ble_gap_crt_params;
struct hci_adv_params;
struct ble_l2cap_sig_update_req;
struct ble_l2cap_sig_update_params;

typedef int cmd_fn(int argc, char **argv);
struct cmd_entry {
    char *name;
    cmd_fn *cb;
};

struct kv_pair {
    char *key;
    int val;
};

struct bletiny_dsc {
    SLIST_ENTRY(bletiny_dsc) next;
    struct ble_gatt_dsc dsc;
};
SLIST_HEAD(bletiny_dsc_list, bletiny_dsc);

struct bletiny_chr {
    SLIST_ENTRY(bletiny_chr) next;
    struct ble_gatt_chr chr;

    struct bletiny_dsc_list dscs;
};
SLIST_HEAD(bletiny_chr_list, bletiny_chr);

struct bletiny_svc {
    SLIST_ENTRY(bletiny_svc) next;
    struct ble_gatt_service svc;

    struct bletiny_chr_list chrs;
};

SLIST_HEAD(bletiny_svc_list, bletiny_svc);

struct bletiny_conn {
    uint16_t handle;
    uint8_t addr_type;
    uint8_t addr[6];

    struct bletiny_svc_list svcs;
};

extern struct bletiny_conn bletiny_conns[NIMBLE_OPT_MAX_CONNECTIONS];
extern int bletiny_num_conns;

extern const char *bletiny_device_name;
extern const uint16_t bletiny_appearance;
extern const uint8_t bletiny_privacy_flag;
extern uint8_t bletiny_reconnect_addr[6];
extern uint8_t bletiny_pref_conn_params[8];
extern uint8_t bletiny_gatt_service_changed[4];

extern struct nmgr_transport nm_ble_transport;
extern uint16_t nm_attr_val_handle;

extern struct log bletiny_log;

void print_addr(void *addr);
void print_uuid(void *uuid128);
const struct cmd_entry *parse_cmd_find(const struct cmd_entry *cmds,
                                       char *name);
struct kv_pair *parse_kv_find(struct kv_pair *kvs, char *name);
char *parse_arg_find(char *key);
long parse_arg_long_bounds(char *name, long min, long max, int *out_status);
long parse_arg_long(char *name, int *staus);
uint16_t parse_arg_uint16(char *name, int *status);
uint16_t parse_arg_uint16_dflt(char *name, uint16_t dflt, int *out_status);
uint32_t parse_arg_uint32(char *name, int *out_status);
int parse_arg_kv(char *name, struct kv_pair *kvs);
int parse_arg_byte_stream(char *name, int max_len, uint8_t *dst, int *out_len);
int parse_arg_byte_stream_exact_length(char *name, uint8_t *dst, int len);
int parse_arg_mac(char *name, uint8_t *dst);
int parse_arg_uuid(char *name, uint8_t *dst_uuid128);
int parse_err_too_few_args(char *cmd_name);
int parse_arg_all(int argc, char **argv);
int cmd_init(void);
void periph_init(void);
int nm_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                  uint8_t op, union ble_gatt_access_ctxt *ctxt,
                  void *arg);
int nm_rx_rsp(uint8_t *attr_val, uint16_t attr_len);
void nm_init(void);
void bletiny_lock(void);
void bletiny_unlock(void);
int bletiny_exchange_mtu(uint16_t conn_handle);
int bletiny_disc_svcs(uint16_t conn_handle);
int bletiny_disc_svc_by_uuid(uint16_t conn_handle, uint8_t *uuid128);
int bletiny_disc_all_chrs(uint16_t conn_handle, uint16_t start_handle,
                           uint16_t end_handle);
int bletiny_disc_chrs_by_uuid(uint16_t conn_handle, uint16_t start_handle,
                               uint16_t end_handle, uint8_t *uuid128);
int bletiny_disc_all_dscs(uint16_t conn_handle, uint16_t chr_val_handle,
                           uint16_t chr_end_handle);
int bletiny_find_inc_svcs(uint16_t conn_handle, uint16_t start_handle,
                           uint16_t end_handle);
int bletiny_read(uint16_t conn_handle, uint16_t attr_handle);
int bletiny_read_long(uint16_t conn_handle, uint16_t attr_handle);
int bletiny_read_by_uuid(uint16_t conn_handle, uint16_t start_handle,
                          uint16_t end_handle, uint8_t *uuid128);
int bletiny_read_mult(uint16_t conn_handle, uint16_t *attr_handles,
                       int num_attr_handles);
int bletiny_write(uint16_t conn_handle, uint16_t attr_handle, void *value,
                   uint16_t value_len);
int bletiny_write_no_rsp(uint16_t conn_handle, uint16_t attr_handle,
                          void *value, uint16_t value_len);
int bletiny_write_long(uint16_t conn_handle, uint16_t attr_handle,
                        void *value, uint16_t value_len);
int bletiny_write_reliable(uint16_t conn_handle, struct ble_gatt_attr *attrs,
                            int num_attrs);
int bletiny_adv_start(int disc, int conn, uint8_t *peer_addr, int addr_type,
                       struct hci_adv_params *params);
int bletiny_adv_stop(void);
int bletiny_conn_initiate(int addr_type, uint8_t *peer_addr,
                           struct ble_gap_crt_params *params);
int bletiny_conn_cancel(void);
int bletiny_term_conn(uint16_t conn_handle);
int bletiny_wl_set(struct ble_gap_white_entry *white_list,
                    int white_list_count);
int bletiny_scan(uint32_t dur_ms, uint8_t disc_mode, uint8_t scan_type,
                  uint8_t filter_policy);
int bletiny_set_adv_data(struct ble_hs_adv_fields *adv_fields);
int bletiny_update_conn(uint16_t conn_handle,
                         struct ble_gap_upd_params *params);
void bletiny_chrup(uint16_t attr_handle);
int bletiny_l2cap_update(uint16_t conn_handle,
                          struct ble_l2cap_sig_update_params *params);
int bletiny_show_rssi(uint16_t conn_handle);

#define BLETINY_LOG_MODULE  (LOG_MODULE_PERUSER + 0)
#define BLETINY_LOG(lvl, ...) \
    LOG_ ## lvl(&bletiny_log, BLETINY_LOG_MODULE, __VA_ARGS__)

#endif
