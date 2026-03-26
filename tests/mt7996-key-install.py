#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""Exercise the real MT7996 set_key -> MCU TLV path with fault injection."""
import argparse
import os
from pathlib import Path
import re
import shlex
import subprocess
import tempfile

base = Path(__file__).resolve().parent
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--source', type=Path, default=base.parent)
args = parser.parse_args()


def function(file, name):
    text = (args.source / file).read_text()
    match = re.search(r'^(?:static (?:inline )?)?(?:int|bool|void)\s+\n?' +
                      name + r'\(', text, re.M)
    if not match:
        raise ValueError('function not found: ' + name)
    return text[match.start():text.index('\n}', match.end()) + 2]


header = (args.source / 'mt7996/mcu.h').read_text()
structs = '\n'.join(re.search(r'struct ' + name + r' \{.*?\} __packed;',
                             header, re.S).group()
                    for name in ('sec_key_uni', 'sta_rec_sec_uni'))
functions = [function(file, name) for file, name in (
    ('mt76.h', 'mt76_key_has_rx_pn'),
    ('mt76.h', 'mt76_key_is_bip'),
    ('mt76.h', 'mt76_wcid_key_link_setup'),
    ('mac80211.c', 'mt76_wcid_key_setup'),
    ('mt7996/mcu.c', 'mt7996_mcu_sta_key_tlv'),
    ('mt7996/mcu.c', 'mt7996_mcu_add_key'),
    ('mt7996/main.c', 'mt7996_set_hw_key'),
    ('mt7996/main.c', 'mt7996_set_key'),
)]
mock = (base / 'mt7996-key-install-mock.c').read_text()
with tempfile.TemporaryDirectory(prefix='mt76-key-install-') as tmp:
    source = Path(tmp) / 'test.c'
    source.write_text(mock.replace('/* STRUCTS */', structs)
                      .replace('/* PRODUCTION */', '\n\n'.join(functions)))
    exe = Path(tmp) / 'test'
    subprocess.run(shlex.split(os.environ.get('CC', 'cc')) + [
        '-std=gnu11', '-Wall', '-Wextra', '-Werror', '-Wno-unused-parameter',
        '-Wno-unused-function', '-g', '-O1', '-fsanitize=address,undefined',
        '-fno-omit-frame-pointer', str(source), '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
