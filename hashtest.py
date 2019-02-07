import binascii
import hashlib

m = hashlib.sha256()
#m.update(binascii.unhexlify('b0'))

#a_pk = binascii.unhexlify('e0d3cf665fad7012e5ebbe984eb3e44a13b518daa0087dd8e647f97ad43f68c7')
#m.update(a_pk)

#v = binascii.unhexlify('8000000000000000')
#m.update(v)

#rho = binascii.unhexlify('eecf10157fa8138cf3b8328e8fbff50e5284b65941178d51c72cf72dbc015b71')
#m.update(rho)

#r = binascii.unhexlify('b7c5e33bcb0921c94363a7fbd526bc9ef97d896ec6e69b21ae4fce756907ae1c')
#m.update(r)

print(m.hexdigest())
