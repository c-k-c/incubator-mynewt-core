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

#ifndef H_HOST_HCI_
#define H_HOST_HCI_

#include "nimble/hci_common.h"
struct ble_hs_conn;
struct os_mbuf;

int host_hci_os_event_proc(struct os_event *ev);
int host_hci_event_rx(uint8_t *data);
int host_hci_cmd_send(uint8_t ogf, uint8_t ocf, uint8_t len, void *cmddata);
int host_hci_cmd_set_event_mask(uint64_t event_mask);
int host_hci_cmd_disconnect(uint16_t handle, uint8_t reason);
int host_hci_cmd_rd_rem_version(uint16_t handle);
int host_hci_cmd_rd_local_version(void);
int host_hci_cmd_read_rssi(uint16_t handle);
int host_hci_cmd_le_set_host_chan_class(uint8_t *new_chan_map);
int host_hci_cmd_le_rd_chanmap(uint16_t handle);
int host_hci_cmd_le_set_scan_rsp_data(uint8_t *data, uint8_t len);
int host_hci_cmd_le_set_adv_data(uint8_t *data, uint8_t len);
int host_hci_cmd_le_set_adv_params(struct hci_adv_params *adv);
int host_hci_cmd_le_set_rand_addr(uint8_t *addr);
int host_hci_cmd_le_set_event_mask(uint64_t event_mask);
int host_hci_cmd_le_read_buffer_size(void);
int host_hci_cmd_le_read_loc_supp_feat(void);
int host_hci_cmd_le_read_rem_used_feat(uint16_t handle);
int host_hci_cmd_le_set_adv_enable(uint8_t enable);
int host_hci_cmd_le_set_scan_params(uint8_t scan_type, uint16_t scan_itvl, 
                                    uint16_t scan_window,
                                    uint8_t own_addr_type,
                                    uint8_t filter_policy);
int host_hci_cmd_le_set_scan_enable(uint8_t enable, uint8_t filter_dups);
int host_hci_cmd_le_create_connection(struct hci_create_conn *hcc);
int host_hci_cmd_le_clear_whitelist(void);
int host_hci_cmd_le_read_whitelist(void);
int host_hci_cmd_le_add_to_whitelist(uint8_t *addr, uint8_t addr_type);
int host_hci_cmd_le_rmv_from_whitelist(uint8_t *addr, uint8_t addr_type);
int host_hci_cmd_reset(void);
int host_hci_cmd_read_adv_pwr(void);
int host_hci_cmd_le_create_conn_cancel(void);
int host_hci_cmd_le_conn_update(struct hci_conn_update *hcu);
int host_hci_cmd_le_conn_param_reply(struct hci_conn_param_reply *hcr);
int host_hci_cmd_le_conn_param_neg_reply(struct hci_conn_param_neg_reply *hcn);

int host_hci_set_buf_size(uint16_t pktlen, uint8_t max_pkts);

uint16_t host_hci_handle_pb_bc_join(uint16_t handle, uint8_t pb, uint8_t bc);

int host_hci_data_rx(struct os_mbuf *om);
int host_hci_data_tx(struct ble_hs_conn *connection, struct os_mbuf *om);

void host_hci_timer_set(void);
void host_hci_init(void);

extern uint16_t host_hci_outstanding_opcode;

#endif /* H_HOST_HCI_ */
