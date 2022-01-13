"""
GenomeGrapher

Graphs a .genome file and stores the resulting graph as a .svg image
"""

import igraph
import os
from binaryreader import BinaryReader

NODE_GENE_BIAS_DIVISOR = 8192.0
CONN_GENE_WEIGHT_DIVISOR = 128.0

#iterates through the genes found in the file open in filestream
def _iterate_genes(path):
    #open the file
    br = BinaryReader(path)

    #guard clause: make sure we don't continue with an invalid stream
    if not br.open():
        return

    #read until the file is empty
    while br.open():
        #determine the type of the gene
        bit = br.read(1)
        gene_type = bit == 1
        if gene_type: # node gene
            #get the bias
            bias = br.read(15) / NODE_GENE_BIAS_DIVISOR

            #yield the gene
            if br.open():
                yield gene_type, (bias,)
            else:
                break
        else: #connection gene
            #separate the gene into it's component parts
            source_type = br.read(1)
            target_type = br.read(1)
            source_id = br.read(10)
            target_id = br.read(10)
            weight = ((-1 if (br.read(1) == 1) else 1) * br.read(8)) / CONN_GENE_WEIGHT_DIVISOR

            #yield the gene
            if br.open():
                yield gene_type, (source_type, target_type, source_id, target_id, weight)
            else:
                break
    #we're finished with this file. close it and return
    br.close()
    return

#returns the normalised weight of weight x
def _norm_weight(x):
    return x / 2

#class for reading and constructing a network from a genome
class GenomeReader:
    #read the genome file at path and extract the genes
    def __init__(self, path, filename, inputs, outputs):
        #start the graph
        g = igraph.Graph().as_directed()

        #add vertices for the inputs
        for i in range(inputs):
            g.add_vertex(color=(0, 0, 1))

        #iterate through the genes in the file
        connection_genes = []
        for gene_type, gene in _iterate_genes(path):
            #the gene type determines our next step
            if gene_type:
                #node gene. add a vertex to the graph
                color = (0, (gene[0] / 4), 0) if gene[0] > 0 else ((gene[0] / 4), 0, 0)
                g.add_vertex(color=color)
            else:
                #connection gene. we need to handle these later, so for now we just store the gene
                connection_genes.append(gene)

        #add our output vertices
        for i in range(outputs):
            g.add_vertex(color=(1, 0, 0))

        #now we can add our connections
        for gene in connection_genes:
            #determine the source and target indices depending on the source and target types
            source_index = -1
            if gene[0]:
                source_index = inputs + (gene[2] % (len(g.vs) - inputs))
            else:
                source_index = gene[2] % inputs

            target_index = -1
            if gene[1]:
                target_index = (len(g.vs) - outputs) + (gene[3] % outputs)
            else:
                target_index = inputs + (gene[3] % (len(g.vs) - inputs))
            #add an edge between the appropriate vertices with the correct weight
            g.add_edge(
                source_index,
                target_index,
                weight=gene[4],
                arrow_width=abs(gene[4]) + 0.001,
                color=((1 - _norm_weight(gene[4])) / 2, (_norm_weight(gene[4]) + 1) / 2, 0))

        #plot the graph and save it as a file
        igraph.plot(g, f"Renders/{filename}.svg", edge_curved=True, bbox=(500,500), margin=64, layout="fruchterman_reingold")


if __name__ == "__main__":
    #directory where all of the genomes are stored
    GENOME_DIR = input("Genome Directory: ")

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

    reader = GenomeReader(GENOME_DIR + genome_paths[selection], genome_paths[selection], 2, 1)
