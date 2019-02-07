import pypy_sha256
import sys
import textwrap

if len(sys.argv) != 2:
    raise Exception('Need exactly 1 argument (0x-prefixed 64 bytes hex byte string)')

hex_input = sys.argv[1]

if len(hex_input) != 130:
    raise Exception('Argument must be of length 130 characters ("0x" + 64 bytes, each as 2 hex digits')

if hex_input[:2] != '0x':
    raise Exception('Argument must start with 0x')

input = [int(x, 16) for x in textwrap.wrap(hex_input[2:], 2)]

state = pypy_sha256.sha_init()
state['data'] = input
pypy_sha256.sha_transform(state)
digest = state['digest']
print('0x' + str.join('', [hex(word)[2:].zfill(8) for word in digest]))
