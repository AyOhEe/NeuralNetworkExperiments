"""
BinaryReader

Reads a file and extracts sections of bits
at a time.
"""

class BinaryReader:
    #open the file at path and prepare to extract bits
    def __init__(self, path):
        #open and store the file at path
        self.file = open(path)

        #initialize variables
        self.current_byte = self.file.read(1)
        self.bits_left = 

    #reads n_bite bits from the open file
    def read(self, n_bits=1):
        pass