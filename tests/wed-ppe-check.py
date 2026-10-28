#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""Compile and exercise the real WED receive callbacks against supported APIs."""
import argparse
import os
from pathlib import Path
import shlex
import subprocess
import tempfile

base = Path(__file__).resolve().parent
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--source', type=Path, default=base.parent,
                    help='mt76 source tree (defaults to this checkout)')
args = parser.parse_args()
functions = []
for driver in ('mt7915', 'mt7996'):
    text = (args.source / driver / 'mac.c').read_text()
    start = text.index('static bool\n' + driver + '_wed_check_ppe(')
    end = text.index('\n}', start) + 2
    functions.append(text[start:end])
mock = (base / 'wed-ppe-check-mock.c').read_text()
modes = ('bool-wcid', 'bool-legacy', 'void-wcid',
         'disabled-bool-wcid', 'disabled-bool-legacy', 'disabled-void-wcid')
with tempfile.TemporaryDirectory(prefix='mt76-wed-ppe-') as tmp:
    source = Path(tmp) / 'test.c'
    source.write_text(mock.replace('/* PRODUCTION */', '\n'.join(functions)))
    for mode, name in enumerate(modes):
        exe = Path(tmp) / name
        subprocess.run(shlex.split(os.environ.get('CC', 'cc')) + [
            '-std=gnu11', '-Wall', '-Wextra', '-Werror',
            '-Wno-unused-parameter', '-Wno-unused-function', '-O2',
            '-DTEST_MODE=' + str(mode), str(source), '-o', str(exe)], check=True)
        result = subprocess.run([str(exe)], check=True, text=True,
                                stdout=subprocess.PIPE)
        print(name + ': ' + result.stdout.strip())
