"""
Chromosome

Reads in genes from a chromosome file
"""
import binaryreader

class Chromosome:
    #open a binary reader at path and prepare to extract genes
    def __init__(self, path, bit_pattern):
        #store the bit pattern
        self.bit_pattern = bit_pattern

        #open and store the binary reader
        self.reader = binaryreader.BinaryReader(path)

    #yields all of the genes in the file
    def GeneIter(self):
        #only read while the binary reader can read
        while self.reader.open():
            #get the gene
            Gene = []
            for bits in self.bit_pattern:
                #only read if the file is still open
                if self.reader.open():
                    Gene.append(self.reader.read(bits))
                #otherwise, stop reading the genome
                else:
                    return
            
            #yield the gene
            yield Gene

    #close the reader for this chromosome
    def close(self):
        #close the file
        self.reader.close()

    #returns true if the file is open
    def open(self):
        return True if self.reader.open() else False