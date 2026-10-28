// SPDX-License-Identifier: GPL-2.0-only
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef uint32_t u32;
typedef uint16_t u16;
#define BIT(n) (1U << (n))
#define MT_DMA_INFO_PPE_VLD BIT(31)
#define MT_DMA_PPE_CPU_REASON (255U << 8)
#define MT_DMA_PPE_ENTRY 255U
#define FIELD_GET(mask, value) (((value) & (mask)) >> __builtin_ctz(mask))
#define container_of(ptr, type, field) ((type *)((char *)(ptr) - offsetof(type, field)))
struct net_device { int id; };
struct wireless_dev { struct net_device *netdev; };
struct ieee80211_vif { struct wireless_dev *wdev; char drv_priv[16]; };
struct sk_buff { struct net_device *dev; };
struct mt76_queue { bool wed_rx; };
struct mtk_wed_device { int id; };
struct mt76_dev { struct { struct mtk_wed_device wed; } mmio; };
struct wcid { u16 idx; };
struct mt7915_dev { struct mt76_dev mt76; };
struct mt7996_dev { struct mt76_dev mt76; };
struct mt7915_sta { void *vif; struct wcid wcid; };
struct mt7996_sta_link { struct wcid wcid; };
struct mt7996_sta { void *vif; unsigned int deflink_id; struct mt7996_sta_link *link; };
static bool mt76_queue_is_wed_rx(struct mt76_queue *q) { return q->wed_rx; }
static struct wireless_dev *ieee80211_vif_to_wdev(struct ieee80211_vif *vif)
{
	return vif->wdev;
}
static struct mt7996_sta_link *mt7996_sta_link(struct mt7996_sta *sta, unsigned int id)
{
	assert(id == sta->deflink_id);
	return sta->link;
}
static int checks, calls;
static bool drop;
static struct mtk_wed_device *expected_wed;
static struct sk_buff *expected_skb;
static u16 expected_wcid;
#define CHECK(value) do { assert(value); checks++; } while (0)
static bool check_ppe(struct mtk_wed_device *wed, struct sk_buff *skb,
		      u32 reason, u32 hash, u16 wcid)
{
	CHECK(wed == expected_wed && skb == expected_skb);
	CHECK(reason == 17 && hash == 23);
	CHECK(wcid == (TEST_MODE == 1 ? UINT16_MAX : expected_wcid));
	calls++;
	return drop;
}
#if TEST_MODE == 0 || TEST_MODE == 3
#define MTK_WED_PPE_CHECK_RETURNS_BOOL
#define MTK_WED_PPE_CHECK_HAS_WCID
#elif TEST_MODE == 1 || TEST_MODE == 4
#define MTK_WED_PPE_CHECK_RETURNS_BOOL
#endif
#if TEST_MODE == 0
#define mtk_wed_device_ppe_check(dev, skb, reason, hash, wcid) \
	check_ppe(dev, skb, reason, hash, wcid)
#elif TEST_MODE == 1
#define mtk_wed_device_ppe_check(dev, skb, reason, hash) \
	check_ppe(dev, skb, reason, hash, UINT16_MAX)
#elif TEST_MODE == 2
#define mtk_wed_device_ppe_check(dev, skb, reason, hash, wcid) \
	do { (void)check_ppe(dev, skb, reason, hash, wcid); } while (0)
#elif TEST_MODE == 3
#define mtk_wed_device_ppe_check(dev, skb, reason, hash, wcid) false
#elif TEST_MODE == 4
#define mtk_wed_device_ppe_check(dev, skb, reason, hash) false
#else
#define mtk_wed_device_ppe_check(dev, skb, reason, hash, wcid) do {} while (0)
#endif

/* PRODUCTION */

#define RUN(driver, device, sta) do { \
	int previous = calls; \
	expected_wed = &(device).mt76.mmio.wed; \
	expected_skb = &skb; \
	CHECK(!driver(&(device), &q, NULL, &skb, info)); \
	(sta).vif = NULL; \
	CHECK(!driver(&(device), &q, &(sta), &skb, info)); \
	(sta).vif = vif.drv_priv; \
	q.wed_rx = false; \
	CHECK(!driver(&(device), &q, &(sta), &skb, info)); \
	q.wed_rx = true; \
	CHECK(!driver(&(device), &q, &(sta), &skb, info & ~MT_DMA_INFO_PPE_VLD)); \
	CHECK(calls == previous); \
	drop = false; \
	CHECK(!driver(&(device), &q, &(sta), &skb, info)); \
	CHECK(skb.dev == &netdev); \
	drop = true; \
	CHECK(driver(&(device), &q, &(sta), &skb, info) == (TEST_MODE < 2)); \
	CHECK(calls == previous + (TEST_MODE < 3 ? 2 : 0)); \
} while (0)

int main(void)
{
	struct net_device netdev = {};
	struct wireless_dev wdev = { .netdev = &netdev };
	struct ieee80211_vif vif = { .wdev = &wdev };
	struct sk_buff skb = {};
	struct mt76_queue q = { .wed_rx = true };
	struct mt7915_dev dev7915 = {};
	struct mt7996_dev dev7996 = {};
	struct mt7915_sta sta7915 = { .vif = vif.drv_priv, .wcid.idx = 71 };
	struct mt7996_sta_link link = { .wcid.idx = 233 };
	struct mt7996_sta sta7996 = { .vif = vif.drv_priv, .deflink_id = 2, .link = &link };
	u32 info = MT_DMA_INFO_PPE_VLD | (17U << 8) | 23;

	expected_wcid = sta7915.wcid.idx;
	RUN(mt7915_wed_check_ppe, dev7915, sta7915);
	expected_wcid = link.wcid.idx;
	RUN(mt7996_wed_check_ppe, dev7996, sta7996);
	sta7996.link = NULL;
	CHECK(!mt7996_wed_check_ppe(&dev7996, &q, &sta7996, &skb, info));
	printf("%d checks passed\n", checks);
	return 0;
}
