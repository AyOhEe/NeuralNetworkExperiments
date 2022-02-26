using UnityEngine;
using UnityEditor;
using System;
using System.IO;
using System.Collections.Generic;

public class GenomeEditorWindow : EditorWindow
{
    enum ChromosomeTypes 
    {
        Neuron,
        Chromosome
    }

    //the Chromosomes currently being edited
    List<NeuronGene> CurrentNeuronChromosome;
    List<ConnectionGene> CurrentConnectionChromosome;

    //the type of the chromosome currently being edited
    ChromosomeTypes CurrentChromosomeType;

    [MenuItem("Neural Networks/Genome Editor")]
    public static void ShowWindow() 
    {
        GetWindow<GenomeEditorWindow>("Genome Editor");
    }

    private void OnGUI()
    {
        //get the current chromosome type
        CurrentChromosomeType = (ChromosomeTypes)EditorGUILayout.EnumPopup("Chromosome Type: ", CurrentChromosomeType);

        //load genome button
        if (GUILayout.Button("Load Genome"))
        {
            //get the path to load from
            string path = EditorUtility.OpenFolderPanel("Select Genome Folder", "", "");

            //get the input and output counts for the network
            int inputs = 2;
            int outputs = 2;

            //load the genome
            LoadGenomeFromPath(path, inputs, outputs);
        }
    }

    //loads the genome at path as a 3d structure
    public void LoadGenomeFromPath(string path, int inputs, int outputs)
    {
        //input validation
        if (string.IsNullOrEmpty(path))  //path
        {
            Debug.LogError(string.Format("Invalid path for LoadGenomeFromPath! \"{0}\"", path));
            throw new ArgumentException(string.Format("Invalid path for LoadGenomeFromPath \"{0}\"", path), "path");
        }
        if (inputs < 1)  //inputs
        {
            //invalid inputs. throw
            Debug.LogError("Invalid inputs for LoadGenomeFromPath!");
            throw new ArgumentException("Invalid inputs for LoadGenomeFromPath: expected greater than zero", "inputs");
        }
        if (outputs < 1)  //outputs
        {
            //invalid outputs. throw
            Debug.LogError("Invalid outputs for LoadGenomeFromPath!");
            throw new ArgumentException("Invalid outputs for LoadGenomeFromPath: expected greater than zero", "outputs");
        }

        //try to create a filestream for the connection and neuron chromosomes
        FileStream ConnectionChromosome = null;
        FileStream NeuronChromosome = null;
        try
        {
            ConnectionChromosome = new FileStream(path + "/Connections.chr2", FileMode.Open, FileAccess.Read);
            NeuronChromosome = new FileStream(path + "/Neurons.chr2", FileMode.Open, FileAccess.Read);
        }
        catch (IOException e)
        {
            //we couldn't open the files for some reason. 
            Debug.LogError(string.Format("Could not Load Chromosomes at path {0}! {1}", path, e.ToString()));

            //make sure we close the filestreams if we opened any
            if (ConnectionChromosome != null)
                ConnectionChromosome.Close();
            if (NeuronChromosome != null)
                NeuronChromosome.Close();

            throw new Exception(string.Format("Could not Load Chromosomes at path {0}!", path), e);
        }

        //read in the lobe sizes from the Neuron gene
        byte[] Bytes = new byte[4];
        List<uint> LobeSizes = new List<uint>();
        if (NeuronChromosome.Read(Bytes, 0, 4) == 4)
        {
            uint LobeCount = ((uint)Bytes[0] << 24) + ((uint)Bytes[1] << 16) + ((uint)Bytes[2] << 8) + (uint)Bytes[3];
            Debug.Log(string.Format("{0} Lobes. Reading Lobe Sizes", LobeCount));
            for (uint i = 0; i < LobeCount; i++)
            {
                if (NeuronChromosome.Read(Bytes, 0, 4) == 4)
                {
                    LobeSizes.Add(((uint)Bytes[0] << 24) + ((uint)Bytes[1] << 16) + ((uint)Bytes[2] << 8) + (uint)Bytes[3]);
                }
                else
                {
                    //oops, we couldn't load in the bytes
                    //TODO: this
                    throw new Exception("Oops need to fix this search through \"//TODO: this\" instances");
                }
            }
            Debug.Log("Lobe Sizes read");
        }
        else
        {
            //oops, we couldn't load in the bytes
            //TODO: this
            throw new Exception("Oops need to fix this search through \"//TODO: this\" instances");
        }

        //begin reading in the Neuron chromosome until we read an incomplete gene
        Bytes = new byte[5];
        while (NeuronChromosome.Read(Bytes, 0, 5) == 5)
        {
            Debug.Log(string.Join(", ", Bytes));
        }

        //begin reading in the Connections chromosome until we read an incomplete gene
        Bytes = new byte[11];
        while (ConnectionChromosome.Read(Bytes, 0, 11) == 11)
        {
            Debug.Log(string.Join(", ", Bytes));
        }

        //close the filestreams
        NeuronChromosome.Close();
        ConnectionChromosome.Close();
    }
}
