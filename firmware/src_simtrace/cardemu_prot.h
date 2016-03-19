#pragma once

/* Smart Card Emulation USB protocol */

/* (C) 2015 by Harald Welte <hwelte@hmw-consulting.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdint.h>

/* DT = Device Terminated. DO = Device Originated */
enum cardemu_usb_msg_type {
	/* Bulk out pipe */
	CEMU_USB_MSGT_DT_TX_DATA,	/* TPDU Date */
	CEMU_USB_MSGT_DT_SET_ATR,	/* Set the ATR stored in simulator */
	CEMU_USB_MSGT_DT_GET_STATS,	/* request DO_STATS */
	CEMU_USB_MSGT_DT_GET_STATUS,	/* request DO_STATUS */
	CEMU_USB_MSGT_DT_CARDINSERT,	/* insert/remove card */

	/* Bulk in pipe */
	CEMU_USB_MSGT_DO_RX_DATA,	/* TPDU data */
	CEMU_USB_MSGT_DO_STATUS,	/* Status information */
	CEMU_USB_MSGT_DO_STATS,		/* Statistics */
	CEMU_USB_MSGT_DO_PTS,		/* Information about PTS */
	CEMU_USB_MSGT_DO_ERROR,		/* Error message */
};

/* generic header, shared by all messages */
struct cardemu_usb_msg_hdr {
	uint8_t msg_type;	/* enum cardemu_usb_msg_type */
	uint8_t seq_nr;		/* sequence number */
	uint16_t msg_len;	/* length of message including hdr */
	uint8_t data[0];
} __attribute__ ((packed));

/* indicates a TPDU header is present in this message */
#define CEMU_DATA_F_TPDU_HDR	0x00000001
/* indicates last part of transmission in this direction */
#define CEMU_DATA_F_FINAL	0x00000002
/* incdicates a PB is present and we should continue with TX */
#define CEMU_DATA_F_PB_AND_TX	0x00000004
/* incdicates a PB is present and we should continue with RX */
#define CEMU_DATA_F_PB_AND_RX	0x00000008

/* CEMU_USB_MSGT_DT_CARDINSERT */
struct cardemu_usb_msg_cardinsert {
	struct cardemu_usb_msg_hdr hdr;
	uint8_t card_insert;
} __attribute__ ((packed));

/* CEMU_USB_MSGT_DT_SET_ATR */
struct cardemu_usb_msg_set_atr {
	struct cardemu_usb_msg_hdr hdr;
	uint8_t atr_len;
	/* variable-length ATR data */
	uint8_t atr[0];
} __attribute__ ((packed));

/* CEMU_USB_MSGT_DT_TX_DATA */
struct cardemu_usb_msg_tx_data {
	struct cardemu_usb_msg_hdr hdr;
	uint32_t flags;
	uint16_t data_len;
	/* variable-length TPDU data */
	uint8_t data[0];
} __attribute__ ((packed));

/* CEMU_USB_MSGT_DO_RX_DATA */
struct cardemu_usb_msg_rx_data {
	struct cardemu_usb_msg_hdr hdr;
	uint32_t flags;
	uint16_t data_len;
	/* variable-length TPDU data */
	uint8_t data[0];
} __attribute__ ((packed));

#define CEMU_STATUS_F_VCC_PRESENT	0x00000001
#define CEMU_STATUS_F_CLK_ACTIVE	0x00000002
#define CEMU_STATUS_F_RCEMU_ACTIVE	0x00000004
#define CEMU_STATUS_F_CARD_INSERT	0x00000008
#define CEMU_STATUS_F_RESET_ACTIVE	0x00000010

/* CEMU_USB_MSGT_DO_STATUS */
struct cardemu_usb_msg_status {
	struct cardemu_usb_msg_hdr hdr;
	uint32_t flags;
	/* phone-applied target voltage in mV */
	uint16_t voltage_mv;
	/* Fi/Di related information */
	uint8_t fi;
	uint8_t di;
	uint8_t wi;
	uint32_t waiting_time;
} __attribute__ ((packed));

/* CEMU_USB_MSGT_DO_PTS */
struct cardemu_usb_msg_pts_info {
	struct cardemu_usb_msg_hdr hdr;
	uint8_t pts_len;
	/* PTS request as sent from reader */
	uint8_t req[6];
	/* PTS response as sent by card */
	uint8_t resp[6];
} __attribute__ ((packed));

/* CEMU_USB_MSGT_DO_ERROR */
struct cardemu_usb_msg_error {
	struct cardemu_usb_msg_hdr hdr;
	uint8_t severity;
	uint8_t subsystem;
	uint16_t code;
	uint8_t msg_len;
	/* human-readable error message */
	uint8_t msg[0];
} __attribute__ ((packed));

static inline void cardemu_hdr_set(struct cardemu_usb_msg_hdr *hdr, uint16_t msgt)
{
	memset(hdr, 0, sizeof(*hdr));
	hdr->msg_type = msgt;
}
