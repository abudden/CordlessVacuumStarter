#!/usr/bin/python3

# This file is part of the Cordless Power Tool Vacuum Start distribution
# (https://github.com/abudden/cordlessvacuumstart).
# Copyright (c) 2022 A. S. Budden
# 
# This program is free software: you can redistribute it and/or modify  
# it under the terms of the GNU General Public License as published by  
# the Free Software Foundation, version 3.
#
# This program is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License 
# along with this program. If not, see <http://www.gnu.org/licenses/>.

import subprocess
import glob
import sys
import os
import shutil
import re
import zipfile
import argparse
import datetime

if sys.hexversion < 0x03050000:
    raise Exception("This script requires Python 3.5+")

# Make sure we're running in the project root directory
os.chdir(os.path.abspath(os.path.dirname(__file__)))

from config import config

targets = {
        'BlackPill': 'WEACT_BLACKPILL_F411CE',
        'Nucleo': 'ST_NUCLEO_F411RE',
        }

# Allow command-line options to build only one variant
parser = argparse.ArgumentParser(description="Build Cordless Vacuum Starter Microcontroller Code")
parser.add_argument('--target', '-t',
        choices=targets.keys(),
        help='Target for which to build code',
        default=None)
manufacturers = [i['Manufacturer'] for i in config]
parser.add_argument('--manufacturer', '-m',
        choices=manufacturers,
        help='Manufacturer of socket for which to build code',
        default=None)

manufacturer_units = {}
for spec in config:
    if spec['Manufacturer'] not in manufacturer_units:
        manufacturer_units[spec['Manufacturer']] = []
    manufacturer_units[spec['Manufacturer']].append(spec['Name'])
unit_names_help = ", ".join(['%s: %r' % (k, v) for k, v in manufacturer_units.items()])

parser.add_argument('--name', '-n',
        help='Unit name for which to build code - options depend on manufacturer: (%s)' % unit_names_help,
        default=None)
parser.add_argument('--unit', '-u',
        type=int,
        help='Unit Number for which to build code',
        default=None)
parser.add_argument('--all', '-a',
        action="store_true",
        help="Build all variants (takes long time!)",
        default=False)
parser.add_argument('--clean', '-c',
        action="store_true",
        help='Clean before building',
        default=False)
parser.add_argument('--define', '-D',
        action='append',
        help='Additional defines to pass to compiler, e.g. -D PERIOD_DEBUGGING -D MYVALUE=3',
        default=[])
parser.add_argument('--publish', '-p',
        action='store_true',
        help='Build everything and make a zip file for publishing (implies --all)',
        default=False)
parser.add_argument('--version', '-v',
        action='store',
        default='unknown',
        help='Version number of release')
args = parser.parse_args()

if args.all or args.publish:
    args.all = True # in case --publish was specified without --all
    if args.manufacturer is not None or args.unit is not None or args.target is not None or args.name is not None:
        print("\nERROR: You must specify either --all OR --manufacturer\n", file=sys.stderr)
        parser.print_help(sys.stderr)
        sys.exit(1)
else:
    if args.manufacturer is None:
        print("\nERROR: You must specify either --all OR --manufacturer\n", file=sys.stderr)
        parser.print_help(sys.stderr)
        sys.exit(1)
    if args.target is None:
        args.target = 'BlackPill'

try:
    args.clean = True
    r = subprocess.run(['hg', 'id', '-nit'], capture_output=True, check=True, encoding='utf8')
    hg_info = dict(zip(['changeset', 'rev', 'branch'], r.stdout.strip().split(' ')))
except:
    hg_info = None

if args.publish:
    # Force a clean build
    if os.path.exists('build'):
        shutil.rmtree('build')
    if hg_info is None or hg_info['changeset'].endswith('+'):
        print("\nERROR: Cannot publish with non-committed source code\n", file=sys.stderr)
        sys.exit(1)

build_dirs = []
for spec in config:
    if args.manufacturer is not None and spec['Manufacturer'] != args.manufacturer:
        continue
    if args.name is not None and spec['Name'] != args.name:
        continue
    for unit, unitcode in spec['UnitCodes'].items():
        if args.unit is not None and args.unit != int(unit):
            continue
        if os.path.exists('_SocketInfo.h'):
            os.remove('_SocketInfo.h')
        if hg_info is None:
            changeset = 'UNKNOWN'
        else:
            changeset = hg_info['changeset']
        definitions = {
                'SOCKET_NAME': '"%s #%s"' % (spec['Manufacturer'], unit),
                'SOCKET_BASE_PATTERN': '0x%08XU' % spec['BasePattern'],
                'SOCKET_UNIT_CODE': '0x%08XU' % unitcode,
                'SOCKET_ON_PATTERN': '0x%08XU' % spec['OnPattern'],
                'SOCKET_OFF_PATTERN': '0x%08XU' % spec['OffPattern'],
                'SOCKET_PATTERN_LENGTH': '%dU' % spec['NumberOfBitsInPattern'],
                'SOCKET_BIT_PERIOD': '%dU' % spec['BitPeriodMicroseconds'],
                'CHANGESET': '"%s"' % changeset,
                'BUILD_DATE': '"%s"' % datetime.datetime.utcnow().strftime('%d/%m/%Y'),
                'VERSION': '"%s"' % args.version,
            }
        if 'OnTimeOverrides' in spec:
            definitions['SOCKET_ON_TIME_OVERRIDE']: ''
            definitions['SOCKET_BIT0_ON_TIME'] = "%dU" % spec['OnTimeOverrides']['0']
            definitions['SOCKET_BIT1_ON_TIME'] = "%dU" % spec['OnTimeOverrides']['1']
        with open('_SocketInfo.h', 'w', encoding='utf8') as fh:
            fh.write('#ifndef _SOCKETINFO_H\n')
            fh.write('#define _SOCKETINFO_H\n')
            fh.write('\n')

            for name, value in definitions.items():
                fh.write('#define %s %s\n' % (name, value))

            fh.write('\n')
            fh.write('#endif\n')

        unit_name = '%s-%s-%s' % (spec['Manufacturer'], spec['Name'], unit)

        for target_name, target in targets.items():

            build_dir = './build/' + unit_name + '/' + target

            if args.target is not None and args.target != target_name:
                continue

            build_dirs.append(build_dir)

            extra_args = []
            if args.clean:
                extra_args.append('--clean')
            for i in args.define:
                extra_args += ['-D', i]
            subprocess.run(['mbed-cli', 'compile',
                '--toolchain', 'GCC_ARM',
                '--build', build_dir,
                '--artifact-name', target_name + "-" + unit_name,
                '--target', target,
                ] + extra_args,
                check=True,
                encoding='utf8')

if args.publish:
    print("")
    version_string = '_' + args.version + '_' + datetime.datetime.utcnow().strftime('%Y-%m-%d') + '_%(changeset)s' % hg_info
    ext_matcher = re.compile(r'(?P<prefix>.*?)(?P<extension>[_.]map\.[a-z]+|\.[a-z]+)')
    build_files = []
    for d in build_dirs:
        build_files += glob.glob(d + '/*.bin')
        build_files += glob.glob(d + '/*.elf')
        build_files += glob.glob(d + '/*map.*')

    zipfilename = 'release%s.zip' % version_string
    print("Generating %s with all variants..." % zipfilename)
    with zipfile.ZipFile(zipfilename, 'w') as zf:
        for f in build_files:
            m = ext_matcher.match(os.path.basename(f))
            newname = m.group('prefix') + '/' + m.group('prefix') + version_string + m.group('extension').replace('_', '.')
            zf.write(f, arcname=newname)
    print("Done")
