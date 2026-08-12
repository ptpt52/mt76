#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""Check MT7603 MCU priming order and initialization failure propagation."""
import os
from pathlib import Path
import re
import shlex
import subprocess
import tempfile

root = Path(__file__).resolve().parent.parent


def function(file, name):
    text = (root / file).read_text()
    match = re.search(r'^(?:static )?(?:int|void)\s+\n?' + name + r'\(', text, re.M)
    return text[match.start():text.index('\n}', match.end()) + 2]


mock = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#define FIELD_PREP(mask, value) (value)
#define MT_INT_SOURCE_CSR 0
#define MT_WPDMA_GLO_CFG 0
#define MT_WF_RFCR 0
#define MT76_STATE_INITIALIZED 0
#define MT7603_WTBL_SIZE 2
#define MT_PSE_RTA 0
#define MT_PSE_RTA_BUSY 0
#define MT_PSE_RTA_WRITE 0
#define MT_PSE_RTA_TAG_ID 0
struct mt7603_dev {
    unsigned int rxfilter;
    struct { unsigned long state; } mphy;
    struct { int napi[2], pre_tbtt_tasklet, dev; struct { int res_q; } mcu; } mt76;
};
#define mt76_for_each_q_rx(dev, i) for ((i) = 0; (i) < 2; (i)++)
static bool mt7628;
static int prime_error, eeprom_error;
static char calls[32];
static unsigned int n_calls;
static void record(char event) { assert(n_calls < sizeof(calls) - 1); calls[n_calls++] = event; }
static bool is_mt7628(struct mt7603_dev *dev) { return mt7628; }
static int mt7603_mcu_upload_eeprom(struct mt7603_dev *dev, bool blocking)
{ record(blocking ? 'E' : 'P'); return blocking ? eeprom_error : prime_error; }
static void mt76_wr(struct mt7603_dev *dev, int reg, unsigned int value) {}
static unsigned int mt76_rr(struct mt7603_dev *dev, int reg) { return 0; }
static bool mt76_poll(struct mt7603_dev *dev, int reg, int mask, int value, int timeout) { return true; }
static void set_bit(int bit, unsigned long *value) { *value |= 1UL << bit; }
static int mt7603_eeprom_init(struct mt7603_dev *dev) { return 0; }
static int mt7603_dma_init(struct mt7603_dev *dev) { return 0; }
static void mt7603_mac_dma_start(struct mt7603_dev *dev) {}
static int mt7603_mcu_init(struct mt7603_dev *dev) { return 0; }
static void mt7603_dma_sched_init(struct mt7603_dev *dev) { record('S'); }
static void mt7603_phy_init(struct mt7603_dev *dev) { record('Y'); }
static void mt7603_mac_init(struct mt7603_dev *dev) { record('M'); }
static int test_and_clear_bit(int bit, unsigned long *value)
{ int set = *value & (1UL << bit); *value &= ~(1UL << bit); return set; }
static void mt7603_irq_disable(struct mt7603_dev *dev, unsigned int mask)
{ assert(mask == ~0U); record('I'); }
static void synchronize_irq(int irq) { assert(irq == 5); record('S'); }
static void tasklet_kill(int *tasklet) { record('K'); }
static void napi_disable(int *napi) { record('N'); }
static void mt7603_mac_stop(struct mt7603_dev *dev) { record('T'); }
static void mt7603_dma_cleanup(struct mt7603_dev *dev) { record('D'); }
static void skb_queue_purge(int *queue) { record('Q'); }
static void devm_free_irq(int device, int irq, struct mt7603_dev *dev)
{ assert(irq == 5); record('F'); }
/* PRODUCTION */
static void run(bool soc, int prime, int eeprom, int expected, const char *order)
{
    struct mt7603_dev dev = {};
    mt7628 = soc; prime_error = prime; eeprom_error = eeprom;
    n_calls = 0; memset(calls, 0, sizeof(calls));
    assert(mt7603_init_hardware(&dev) == expected);
    assert(!strcmp(calls, order));
    if (expected) {
        n_calls = 0; memset(calls, 0, sizeof(calls));
        mt7603_cleanup_failed_probe(&dev, 5);
        assert(!dev.mphy.state && !strcmp(calls, "ISKNNTDQIF"));
    }
}
int main(void)
{
    struct mt7603_dev early = {};
    mt7603_cleanup_failed_probe(&early, 5);
    assert(!strcmp(calls, "IF"));
    run(false, -ENOMEM, 0, -ENOMEM, "P");
    run(false, -EBUSY, 0, -EBUSY, "P");
    run(false, 0, -ETIMEDOUT, -ETIMEDOUT, "PSE");
    run(false, 0, 0, 0, "PSEYM");
    run(true, -ENOMEM, 0, 0, "SEYM");
    run(true, -ENOMEM, -EIO, -EIO, "SE");
    puts("MT7603 MCU priming: 6 order/error scenarios and staged probe cleanup passed");
    return 0;
}
'''
functions = [function(file, name) for file, name in (
    ('mt7603/mcu.c', 'mt7603_mcu_set_eeprom'),
    ('mt7603/mcu.c', 'mt7603_mcu_prime_eeprom'),
    ('mt7603/init.c', 'mt7603_mcu_prime'),
    ('mt7603/init.c', 'mt7603_init_hardware'),
    ('mt7603/init.c', 'mt7603_cleanup_failed_probe'),
)]
with tempfile.TemporaryDirectory(prefix='mt76-mcu-prime-') as tmp:
    source = Path(tmp) / 'test.c'
    source.write_text(mock.replace('/* PRODUCTION */', '\n\n'.join(functions)))
    exe = Path(tmp) / 'test'
    subprocess.run(shlex.split(os.environ.get('CC', 'cc')) + [
        '-std=gnu11', '-Wall', '-Wextra', '-Werror', '-Wno-unused-parameter',
        '-g', '-O1', '-fsanitize=address,undefined', '-fno-omit-frame-pointer',
        str(source), '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
