"""
BinaryReader

Reads a file and extracts sections of bits
at a time.
"""
import math

class BinaryReader:
    #open the file at path and prepare to extract bits
    def __init__(self, path):
        #open and store the file at path
        self.file = open(path, "rb")

        #initialize variables
        self.current_bits = []
        self.bits_left = 0
        
        #determine if the file is even open
        self._open = True if self.file else False

    #reads n_bite bits from the open file
    def read(self, n_bits=1):
        #calculate how many more bits we need to read in
        bytes_required = math.ceil((n_bits - self.bits_left) / 8)
        
        #read the bits of the bytes into the current_bytes list
        for i in range(bytes_required):
            as_byte = self.file.read(1) #read in a byte
            if as_byte == b"": #check if the byte is empty
                #yep, it's empty. close the file and exit
                print("LN32")
                self._open = False
                return 0

            as_int = int.from_bytes(as_byte, byteorder="big") #cast the byte to an int
            for i in range(8):
                self.current_bits.append((as_int >> (7 - i)) & 0b1) #append each bit to the bit array
            self.bits_left += 8

        #reassemble and remove the bits requested, leaving the excess to be reused for the next call
        final_result = 0
        for i in range(n_bits):
            #bit shift by 1 to make room for the next bit
            final_result <<= 1
            #add the current bit at the 1's place
            final_result += self.current_bits.pop(0)
        self.bits_left -= n_bits

        #return the final value
        return final_result

    #close the file for this reader
    def close(self):
        #close the file
        self.file.close()

    #returns true if the file is closed
    def open(self):
        return self._open

if __name__ == "__main__":
    br = BinaryReader("../Genomes/test_genome_smallMOD.genome")

    br.close()
