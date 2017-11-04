import struct
import numpy as np

with open('train-images-idx3-ubyte', 'rb') as input:
    with open('dataset.dat', 'wb') as output:
        (k,) = struct.unpack('>i', input.read(4))
        if k != 2051:
            raise Exception('invalid magic number')
        (n,) = struct.unpack('>i', input.read(4))
        (r,) = struct.unpack('>i', input.read(4))
        (c,) = struct.unpack('>i', input.read(4))
        dataset = []
        for i in range(n):
            row = list(struct.unpack('%dB' % (r * c), input.read(r * c)))
            dataset.append(row)
            if (i + 1) % 1000 == 0:
                print i + 1
        dataset = np.array(dataset, dtype=np.float32)
        dataset /= np.linalg.norm(dataset, axis=1).reshape(-1, 1)
        output.write(struct.pack('Q', n))
        for i in range(n):
            output.write(struct.pack('Q', r * c))
            output.write(struct.pack('%df' % (r * c), *list(dataset[i])))
with open('t10k-images-idx3-ubyte', 'rb') as input:
    with open('queries.dat', 'wb') as output:
        (k,) = struct.unpack('>i', input.read(4))
        if k != 2051:
            raise Exception('invalid magic number')
        (n,) = struct.unpack('>i', input.read(4))
        (r,) = struct.unpack('>i', input.read(4))
        (c,) = struct.unpack('>i', input.read(4))
        dataset = []
        for i in range(n):
            row = list(struct.unpack('%dB' % (r * c), input.read(r * c)))
            dataset.append(row)
            if (i + 1) % 1000 == 0:
                print i + 1
        dataset = np.array(dataset, dtype=np.float32)
        dataset /= np.linalg.norm(dataset, axis=1).reshape(-1, 1)
        output.write(struct.pack('Q', n))
        for i in range(n):
            output.write(struct.pack('Q', r * c))
            output.write(struct.pack('%df' % (r * c), *list(dataset[i])))
