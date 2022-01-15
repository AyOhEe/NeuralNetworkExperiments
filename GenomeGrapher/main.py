"""
GenomeGrapher

Graphs a .genome file and stores the resulting graph as a .svg image
"""

import igraph
import os
from chromosome import Chromosome

NODE_GENE_BIAS_DIVISOR = 8192.0
CONN_GENE_WEIGHT_DIVISOR = 128.0

#class for reading and constructing a network from a genome
class GenomeReader:
    #read the genome file at path and extract the genes
    def __init__(self, path, filename, inputs, outputs):
        #start the graph
        g = igraph.Graph().as_directed()

        #open the chromosomes
        NodeChromosome = Chromosome(path + "\\Nodes.chr", [1, 1, 14])
        ConnectionChromosome = Chromosome(path + "\\Connections.chr", [1, 1, 1, 10, 10, 1, 8])

        #add vertices for the inputs
        for i in range(inputs):
            g.add_vertex(color=(0.512, 0.742, 0.867), label="Input: " + str(i))

        #add our output vertices
        for i in range(outputs):
            g.add_vertex(color=(1, 0.5, 0.5), label="Output: " + str(i))

        #iterate through the node chromosome
        for gene in NodeChromosome.GeneIter():
            #add a vertex for the node
            colour = (0, 0, 0)
            if gene[1] == 1:
                colour = ((gene[2] / NODE_GENE_BIAS_DIVISOR) * 0.5, 0, 0)
            else:
                colour = (0, (gene[2] / NODE_GENE_BIAS_DIVISOR) * 0.5, 0)
            g.add_vertex(color=colour)
        #close the node chromosome now that we're done with it

        #iterate through the connection chromosome
        for gene in ConnectionChromosome.GeneIter():
            #determine the source and target indices depending on the source and target types
            source_index = -1
            if gene[1] == 1:
                source_index = inputs + outputs + (gene[3] % (len(g.vs) - (inputs + outputs)))
            else:
                source_index = gene[3] % inputs

            target_index = -1
            if gene[2] == 1:
                target_index = inputs + (gene[4] % outputs)
            else:
                target_index = inputs + outputs + (gene[4] % (len(g.vs) - (inputs + outputs)))

            #add an edge between the appropriate vertices with the correct weight
            weight = (-1 if gene[5] == 1 else 1) * gene[6] / CONN_GENE_WEIGHT_DIVISOR
            g.add_edge(
                source_index,
                target_index,
                weight=weight,
                arrow_width=(gene[6] / CONN_GENE_WEIGHT_DIVISOR) + 0.001,
                color=(-weight / 2, 0, 0) if weight < 0 else (0, weight / 2, 0))

        #plot the graph and save it as a file
        igraph.plot(g, f"Renders/{filename}.svg", edge_curved=True, bbox=(500,500), margin=64, layout="fruchterman_reingold")


if __name__ == "__main__":
    #directory where all of the genomes are stored
    GENOME_DIR = input("Genome Directory: ")

    #get all genomes in ../Genomes
    genome_paths = [dir for dir in os.listdir(GENOME_DIR)]

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

    reader = GenomeReader(GENOME_DIR + "\\" + genome_paths[selection], genome_paths[selection], 2, 1)
