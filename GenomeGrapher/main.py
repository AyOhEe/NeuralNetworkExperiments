"""
GenomeGrapher

Graphs a .genome file and stores the resulting graph as a .svg image
"""

import igraph
import os

NODE_GENE_BIAS_DIVISOR = 8192.0
CONN_GENE_WEIGHT_DIVISOR = 128.0

#iterates through the genes found in the file open in filestream
def _iterate_genes(path):
    #open the file
    file_stream = open(path, "rb")

    #guard clause: make sure we don't continue with an invalid stream
    if not file_stream:
        return

    #read until the file is empty
    byte = b""
    try:
        while (byte := file_stream.read(1)) != b"":
            #we're only interested in the first(only) byte we read
            byte = byte[0]

            #determine the type of the gene
            gene_type = (byte & 0x80) == 0x80
            if gene_type: # node gene
                #get all the bytes for this gene
                gene_bytes = [byte] #store the single byte we already have
                additional_bytes = file_stream.read(1) #read in the rest of the bytes
                if additional_bytes != b"": #append them, only if we actually read something
                    gene_bytes += additional_bytes
                else: #invalid gene, we've reached the end of the file
                    return

                #separate the gene into its component parts
                bias = ((int(gene_bytes[0] & 0x7F) << 8) + int(gene_bytes[1])) / NODE_GENE_BIAS_DIVISOR

                #yield the gene
                yield gene_type, (bias)
            else: #connection gene
                #get all the bytes for this gene
                gene_bytes = [byte] #store the single byte we already have
                additional_bytes = file_stream.read(3) #read in the rest of the bytes
                if additional_bytes != b"": #append them, only if we actually read something
                    gene_bytes += additional_bytes
                else: #invalid gene, we've reached the end of the file
                    return

                #separate the gene into it's component parts
                source_type = (gene_bytes[0] & 0x40) == 0x40
                target_type = (gene_bytes[0] & 0x20) == 0x20
                source_id = ((gene_bytes[0] & 0x1F) << 5) + ((gene_bytes[1] & 0xF8) >> 3)
                target_id = ((gene_bytes[1] & 0x07) << 7) + ((gene_bytes[2] & 0xFE) >> 1)
                weight = ((-1 if (gene_bytes[2] & 0x01) else 1) * gene_bytes[3]) / CONN_GENE_WEIGHT_DIVISOR

                #yield the gene
                yield gene_type, (source_type, target_type, source_id, target_id, weight)
    except IndexError:
        #end of file, we couldn't read another byte.
        #we can just silently ignore this and continue without finishing that gene.
        pass
    #we're finished with this file. close it and return
    file_stream.close()
    return


#class for reading and constructing a network from a genome
class GenomeReader:
    #read the genome file at path and extract the genes
    def __init__(self, path):
        #iterate through the genes in the file
        for gene_type, gene in _iterate_genes(path):
            print(gene_type, gene)

if __name__ == "__main__":
    #directory where all of the genomes are stored
    GENOME_DIR = "../Genomes/"

    #get all genomes in ../Genomes
    genome_paths = [dir for dir in os.listdir(GENOME_DIR) if dir[-7:] == ".genome"]

    #ask the user to pick a file until they pick a valid file
    print("Pick a genome to graph")
    for i, path in enumerate(genome_paths):
        print(f"\t{i + 1}. {path}")
    selection = int(input("Selection: ")) - 1

    while not len(genome_paths) > selection >= 0:
        print("\nInvalid! Pick a genome to graph")
        for i, path in enumerate(genome_paths):
            print(f"\t{i + 1}. {path}")
        selection = int(input("Selection: ")) - 1

    #once we've escaped the loop we have a valid choice
    print(f"Genome Selected: {genome_paths[selection]}")


    reader = GenomeReader(GENOME_DIR + genome_paths[selection])
