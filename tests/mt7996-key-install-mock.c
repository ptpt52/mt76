/* SPDX-License-Identifier: GPL-2.0-only */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t __le16;
#define __packed __attribute__((packed))
#define BIT(n) (1UL << (n))
#define READ_ONCE(x) (x)
#define WRITE_ONCE(x, v) ((x) = (v))
#define cpu_to_le16(x) (x)
#define fallthrough __attribute__((fallthrough))
#define IEEE80211_NUM_TIDS 16
#define IEEE80211_MLD_MAX_NUM_LINKS 4
#define IEEE80211_KEY_FLAG_PAIRWISE BIT(0)
#define IEEE80211_KEY_FLAG_GENERATE_MMIE BIT(1)
#define MCU_WMWA_UNI_CMD(x) 0
#define STA_REC_KEY_V2 2
#define MT7996_STA_UPDATE_MAX_SIZE 256
#define BP_HW_MODE 1
#define BP_SW_MODE 0
#define IS_ERR(p) ((uintptr_t)(p) >= (uintptr_t)-4095)
#define PTR_ERR(p) ((int)(intptr_t)(p))
#define for_each_set_bit(i, addr, size) \
	for ((i) = 0; (i) < (size); (i)++) if (*(addr) & BIT(i))

enum set_key_cmd { SET_KEY, DISABLE_KEY };
enum { NL80211_IFTYPE_STATION, NL80211_IFTYPE_AP, NL80211_IFTYPE_ADHOC,
       NL80211_IFTYPE_MESH_POINT };
enum { WLAN_CIPHER_SUITE_TKIP = 1, WLAN_CIPHER_SUITE_CCMP,
       WLAN_CIPHER_SUITE_CCMP_256, WLAN_CIPHER_SUITE_GCMP,
       WLAN_CIPHER_SUITE_GCMP_256, WLAN_CIPHER_SUITE_SMS4,
       WLAN_CIPHER_SUITE_AES_CMAC, WLAN_CIPHER_SUITE_BIP_CMAC_256,
       WLAN_CIPHER_SUITE_BIP_GMAC_128, WLAN_CIPHER_SUITE_BIP_GMAC_256,
       WLAN_CIPHER_SUITE_WEP40, WLAN_CIPHER_SUITE_WEP104 };
enum { MCU_CIPHER_NONE, MCU_CIPHER_TKIP, MCU_CIPHER_AES_CCMP,
       MCU_CIPHER_BIP_CMAC_128, MCU_CIPHER_BCN_PROT_CMAC_128,
       MCU_CIPHER_BCN_PROT_GMAC_128, MCU_CIPHER_BCN_PROT_GMAC_256,
       MCU_CIPHER_BCN_PROT_CMAC_256, MCU_CIPHER_OTHER };

/* STRUCTS */
struct mt76_connac_sta_key_conf { s8 keyidx; u8 key[16]; };
struct ieee80211_key_conf {
	int keyidx, link_id, keylen;
	unsigned int cipher, flags;
	u8 key[32], seq;
};
struct ieee80211_key_seq { struct { u8 pn[6]; } ccmp, gcmp; };
struct mt76_wcid {
	int idx;
	u8 hw_key_idx, hw_key_idx2;
	bool sta, rx_check_pn;
	u8 rx_key_pn[IEEE80211_NUM_TIDS + 1][6];
};
struct mt7996_sta_link {
	struct mt76_wcid wcid;
	struct mt76_connac_sta_key_conf bip;
	bool bip_valid;
};
struct mt7996_sta {
	struct mt7996_sta_link deflink, *link[IEEE80211_MLD_MAX_NUM_LINKS];
	bool npu_offload, npu_key_blocked;
};
struct mt7996_vif { bool npu_key_blocked; };
struct mt76_vif_link { int cipher; };
struct mt7996_vif_link {
	struct mt76_vif_link mt76;
	struct mt7996_sta_link msta_link;
	bool active;
};
struct mt7996_phy { int unused; };
struct mt76_dev { int mutex; };
struct mt7996_dev {
	struct mt76_dev mt76;
	struct mt7996_vif_link link[IEEE80211_MLD_MAX_NUM_LINKS];
};
struct ieee80211_hw { struct mt7996_dev *priv; };
struct ieee80211_bss_conf { bool enable_beacon; };
struct ieee80211_vif {
	int type;
	unsigned long valid_links;
	struct ieee80211_bss_conf bss_conf;
	u8 drv_priv[sizeof(struct mt7996_vif)] __attribute__((aligned(8)));
};
struct ieee80211_sta {
	unsigned long valid_links;
	u8 drv_priv[sizeof(struct mt7996_sta)] __attribute__((aligned(8)));
};
struct sk_buff { struct sta_rec_sec_uni sec; };
struct tlv { __le16 tag, len; } __packed;
static struct sta_rec_sec_uni sent;
static int send_error, alloc_error, sends, beacons;
static bool chip7990;
static struct mt7996_dev *mt7996_hw_dev(struct ieee80211_hw *hw) { return hw->priv; }
static struct mt7996_vif_link *mt7996_vif_link(struct mt7996_dev *dev,
		struct ieee80211_vif *vif, unsigned int id) { return &dev->link[id]; }
static struct mt7996_sta_link *mt7996_sta_link_protected(struct mt7996_dev *dev,
		struct mt7996_sta *sta, unsigned int id) { return sta->link[id]; }
static struct mt7996_phy *mt7996_vif_link_phy(struct mt7996_vif_link *link)
{ static struct mt7996_phy phy; return link->active ? &phy : NULL; }
static struct ieee80211_bss_conf *link_conf_dereference_protected(
		struct ieee80211_vif *vif, unsigned int id) { return &vif->bss_conf; }
static bool is_mt7990(struct mt76_dev *dev) { return chip7990; }
static int mt76_npu_offload_flush(struct mt76_dev *dev) { return 0; }
static void mutex_lock(int *lock) { assert(!(*lock)++); }
static void mutex_unlock(int *lock) { assert((*lock)-- == 1); }
static void ieee80211_get_key_rx_seq(struct ieee80211_key_conf *key,
		int tid, struct ieee80211_key_seq *seq) { memset(seq, key->seq, sizeof(*seq)); }
static u8 mt76_connac_mcu_get_cipher(unsigned int cipher)
{
	switch (cipher) {
	case WLAN_CIPHER_SUITE_CCMP: return MCU_CIPHER_AES_CCMP;
	case WLAN_CIPHER_SUITE_AES_CMAC: return MCU_CIPHER_BIP_CMAC_128;
	case WLAN_CIPHER_SUITE_TKIP: return MCU_CIPHER_TKIP;
	case WLAN_CIPHER_SUITE_WEP40: return MCU_CIPHER_NONE;
	default: return MCU_CIPHER_OTHER;
	}
}
static struct sk_buff *__mt76_connac_mcu_alloc_sta_req(struct mt76_dev *dev,
		struct mt76_vif_link *link, struct mt76_wcid *wcid, int len)
{ return alloc_error ? (void *)(intptr_t)alloc_error : calloc(1, sizeof(struct sk_buff)); }
static struct tlv *mt76_connac_mcu_add_tlv(struct sk_buff *skb, int tag, int len)
{ assert(len == sizeof(skb->sec)); return (void *)&skb->sec; }
static void dev_kfree_skb(struct sk_buff *skb) { free(skb); }
static int mt76_mcu_skb_send_msg(struct mt76_dev *dev, struct sk_buff *skb,
		int cmd, bool wait)
{ sent = skb->sec; sends++; free(skb); return send_error; }
static int mt7996_mcu_add_bss_info(struct mt7996_phy *phy,
		struct ieee80211_vif *vif, struct ieee80211_bss_conf *conf,
		struct mt76_vif_link *link, struct mt7996_sta_link *sta, bool enable) { return 0; }
static int mt7996_mcu_add_beacon(struct ieee80211_hw *hw,
		struct ieee80211_vif *vif, struct ieee80211_bss_conf *conf, bool enable)
{ beacons++; return 0; }

/* PRODUCTION */

static struct ieee80211_key_conf key(unsigned int cipher, int index, u8 value)
{
	struct ieee80211_key_conf k = {
		.cipher = cipher, .keyidx = index, .link_id = -1,
		.keylen = 16, .seq = value,
	};
	memset(k.key, value, sizeof(k.key));
	return k;
}

static void check_batch(struct mt7996_sta_link *link,
		struct ieee80211_key_conf *igtk)
{
	assert(sent.n_cipher == 2 && sent.add == SET_KEY);
	assert(sent.key[0].cipher_id == MCU_CIPHER_AES_CCMP);
	assert(sent.key[0].key_id == link->bip.keyidx && !sent.key[0].mgmt_prot);
	assert(sent.key[0].key_len == 16 && sent.key[1].key_len == 16);
	assert(!memcmp(sent.key[0].key, link->bip.key, 16));
	assert(sent.key[1].cipher_id == MCU_CIPHER_BIP_CMAC_128);
	assert(sent.key[1].key_id == igtk->keyidx && sent.key[1].mgmt_prot);
	assert(sent.key[0].wlan_idx == link->wcid.idx);
	assert(sent.key[1].wlan_idx == link->wcid.idx);
	assert(!memcmp(sent.key[1].key, igtk->key, 16));
	assert(igtk->flags & IEEE80211_KEY_FLAG_GENERATE_MMIE);
}

int main(void)
{
	struct mt7996_dev dev = {};
	struct ieee80211_hw hw = { .priv = &dev };
	struct ieee80211_vif vif = { .type = NL80211_IFTYPE_AP };
	struct ieee80211_sta sta = {};
	struct mt7996_sta *msta = (void *)sta.drv_priv;
	struct mt7996_sta_link *link = &dev.link[0].msta_link;
	struct ieee80211_key_conf gtk = key(WLAN_CIPHER_SUITE_CCMP, 1, 0xa1);
	struct ieee80211_key_conf igtk = key(WLAN_CIPHER_SUITE_AES_CMAC, 4, 0xb2);
	struct ieee80211_key_conf bigtk = key(WLAN_CIPHER_SUITE_AES_CMAC, 6, 0xc3);
	int before;

	dev.link[0].active = true;
	link->wcid.idx = 8;
	link->wcid.hw_key_idx = link->wcid.hw_key_idx2 = 0xff;
	assert(mt7996_set_key(&hw, SET_KEY, &vif, NULL, &igtk) == -EOPNOTSUPP);
	assert(!sends && !dev.link[0].mt76.cipher);
	assert(!mt7996_set_key(&hw, SET_KEY, &vif, NULL, &gtk));
	assert(link->bip_valid && link->wcid.hw_key_idx == 1);
	assert(!mt7996_set_key(&hw, SET_KEY, &vif, NULL, &igtk));
	check_batch(link, &igtk);
	assert(link->wcid.hw_key_idx == 1 && link->wcid.hw_key_idx2 == 0xff);
	assert(link->wcid.rx_check_pn);

	vif.bss_conf.enable_beacon = true;
	assert(!mt7996_set_key(&hw, SET_KEY, &vif, NULL, &bigtk));
	assert(sent.n_cipher == 1 && sent.key[0].cipher_id == MCU_CIPHER_BCN_PROT_CMAC_128);
	assert(beacons == 2 && link->wcid.hw_key_idx2 == 6);
	igtk.keyidx = 5;
	assert(!mt7996_set_key(&hw, SET_KEY, &vif, NULL, &igtk));
	check_batch(link, &igtk);
	assert(link->wcid.hw_key_idx == 1 && link->wcid.hw_key_idx2 == 6);
	before = sends;
	assert(!mt7996_set_key(&hw, DISABLE_KEY, &vif, NULL, &igtk));
	assert(sends == before && link->bip_valid && link->wcid.hw_key_idx2 == 6);

	/* A failed IGTK update must not change either hardware key index. */
	send_error = -EIO;
	assert(mt7996_set_key(&hw, SET_KEY, &vif, NULL, &igtk) == -EIO);
	assert(link->bip_valid && link->wcid.hw_key_idx == 1 && link->wcid.hw_key_idx2 == 6);
	/* A failed CCMP rekey invalidates the cached data key. */
	gtk.keyidx = 2;
	assert(mt7996_set_key(&hw, SET_KEY, &vif, NULL, &gtk) == -EIO);
	assert(!link->bip_valid);
	before = sends;
	assert(mt7996_set_key(&hw, SET_KEY, &vif, NULL, &igtk) == -EOPNOTSUPP);
	assert(sends == before);
	send_error = 0;
	assert(!mt7996_set_key(&hw, SET_KEY, &vif, NULL, &gtk));
	assert(!mt7996_set_key(&hw, SET_KEY, &vif, NULL, &igtk));
	check_batch(link, &igtk);
	assert(!mt7996_set_key(&hw, DISABLE_KEY, &vif, NULL, &gtk));
	assert(!link->bip_valid);
	assert(mt7996_set_key(&hw, SET_KEY, &vif, NULL, &igtk) == -EOPNOTSUPP);

	assert(!mt7996_set_key(&hw, SET_KEY, &vif, NULL, &gtk));
	alloc_error = -ENOMEM;
	assert(mt7996_set_key(&hw, SET_KEY, &vif, NULL, &gtk) == -ENOMEM);
	assert(!link->bip_valid);
	alloc_error = 0;
	gtk.cipher = WLAN_CIPHER_SUITE_GCMP;
	assert(!mt7996_set_key(&hw, SET_KEY, &vif, NULL, &gtk));
	assert(!link->bip_valid);
	assert(mt7996_set_key(&hw, SET_KEY, &vif, NULL, &igtk) == -EOPNOTSUPP);
	igtk.cipher = WLAN_CIPHER_SUITE_BIP_GMAC_128;
	assert(mt7996_set_key(&hw, SET_KEY, &vif, NULL, &igtk) == -EOPNOTSUPP);

	/* Pairwise PN state must survive IGTK installation and removal. */
	msta->link[0] = &msta->deflink;
	msta->deflink.wcid.sta = true;
	msta->deflink.wcid.idx = 32;
	gtk = key(WLAN_CIPHER_SUITE_CCMP, 0, 0xd4);
	gtk.flags = IEEE80211_KEY_FLAG_PAIRWISE;
	igtk = key(WLAN_CIPHER_SUITE_AES_CMAC, 4, 0xe5);
	assert(!mt7996_set_key(&hw, SET_KEY, &vif, &sta, &gtk));
	assert(!mt7996_set_key(&hw, SET_KEY, &vif, &sta, &igtk));
	check_batch(&msta->deflink, &igtk);
	assert(msta->deflink.wcid.hw_key_idx == 0);
	for (int i = 0; i <= IEEE80211_NUM_TIDS; i++)
		assert(msta->deflink.wcid.rx_key_pn[i][0] == gtk.seq);
	assert(msta->deflink.wcid.rx_check_pn);
	assert(!mt7996_set_key(&hw, DISABLE_KEY, &vif, &sta, &igtk));
	assert(msta->deflink.wcid.rx_check_pn);

	/* Exercise the defensive TLV check independently of the public gate. */
	before = sends;
	assert(mt7996_mcu_add_key(&dev.mt76, &dev.link[0], NULL, &igtk,
				0, &link->wcid, SET_KEY) == -EOPNOTSUPP);
	assert(sends == before);
	puts("MT7996 key installation: IGTK 4/5 batching, BIGTK coexistence, PN preservation, rekey/removal and MCU faults passed");
	return 0;
}
