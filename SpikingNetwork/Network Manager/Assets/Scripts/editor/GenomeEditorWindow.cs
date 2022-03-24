using UnityEngine;
using UnityEditor;
using UnityEditorInternal;
using System;
using System.IO;
using System.Collections.Generic;

public class GenomeEditorWindow : EditorWindow
{
    //the Chromosomes currently being edited
    [HideInInspector]
    public NeuronGene[] CurrentNeuronChromosome;
    [HideInInspector]
    public ConnectionGene[] CurrentConnectionChromosome;
    [HideInInspector]
    public uint[] LobeSizes;

    private static Rect NeuronListRect = new Rect(5, 0, 400, 500);
    private static Rect ConnectionListRect = new Rect(5, 0, 400, 500);

    private Vector2 NeuronScrollPosition = Vector2.zero;
    private Vector2 ConnectionScrollPosition = Vector2.zero;

    SerializedObject _objectSO = null;
    ReorderableList _neuronListRE = null;
    ReorderableList _connectionListRE = null;

    string GenomePath = "No Genome Loaded";

    [MenuItem("Neural Networks/Genome Editor")]
    public static void ShowWindow() 
    {
        GetWindow<GenomeEditorWindow>("Genome Editor");
    }

    private void InitNeuronListRE()
    { 
        //create the list
        _neuronListRE = new ReorderableList(_objectSO, _objectSO.FindProperty("CurrentNeuronChromosome"), true,
                true, true, true);

        //handle drawing
        _neuronListRE.drawHeaderCallback = (rect) => EditorGUI.LabelField(rect, "Neuron Genes");
        _neuronListRE.drawElementCallback = (Rect rect, int index, bool isActive, bool isFocused) =>
        {
            rect.y += 2f;
            rect.height = EditorGUIUtility.singleLineHeight;
            GUIContent objectLabel = new GUIContent($"Gene {index}");
            //the index will help numerate the serialized fields
            EditorGUI.PropertyField(rect, _neuronListRE.serializedProperty.GetArrayElementAtIndex(index), objectLabel);
        };

        //change the element height
        _neuronListRE.elementHeight = EditorGUIUtility.singleLineHeight * 4 + 18;
    }
    private void InitConnectionListRE() 
    {
        //create the list
        _connectionListRE = new ReorderableList(_objectSO, _objectSO.FindProperty("CurrentConnectionChromosome"), true,
            true, true, true);

        //handle drawing
        _connectionListRE.drawHeaderCallback = (rect) => EditorGUI.LabelField(rect, "Connection Genes");
        _connectionListRE.drawElementCallback = (Rect rect, int index, bool isActive, bool isFocused) =>
        {
            rect.y += 2f;
            rect.height = EditorGUIUtility.singleLineHeight;
            GUIContent objectLabel = new GUIContent($"Gene {index}");
            //the index will help numerate the serialized fields
            EditorGUI.PropertyField(rect, _connectionListRE.serializedProperty.GetArrayElementAtIndex(index), objectLabel);
        };

        //change the element height
        _connectionListRE.elementHeight = EditorGUIUtility.singleLineHeight * 5 + 20;
    }

    private void OnEnable()
    {
        //init serialized object
        _objectSO = new SerializedObject(this);

        InitNeuronListRE();
        InitConnectionListRE();

        minSize = new Vector2(840, 700);
    }

    private void OnGUI()
    {
        //make sure we've been initialised
        if(_objectSO == null)
        {
            //init serialized object
            _objectSO = new SerializedObject(this);
        }
        if(_neuronListRE == null) 
        {
            InitNeuronListRE();
        }
        if (_connectionListRE == null)
        {
            InitConnectionListRE();
        }

        //load genome button
        if (GUILayout.Button("Load Genome"))
        {
            //get the path to load from
            GenomePath = EditorUtility.OpenFolderPanel("Select Genome Folder", "", "");

            //get the input and output counts for the network
            int inputs = 2;
            int outputs = 2;

            //load the genome
            LoadGenomeFromPath(GenomePath, out LobeSizes, out CurrentNeuronChromosome, out CurrentConnectionChromosome);
        }
        //save genome button
        if (GUILayout.Button("Save Genome")) 
        {
            //get the path to save to
            string path = EditorUtility.SaveFolderPanel("Select Genome Folder", "", "");

            //save the genome
            SaveGenomeToPath(path, LobeSizes, CurrentNeuronChromosome, CurrentConnectionChromosome);
        }

        GUILayout.Label(string.Format("Current Genome: {0}", GenomePath));

        //render the gene lists
        _objectSO.Update();
        GUILayout.BeginArea(new Rect(0, EditorGUIUtility.singleLineHeight * 3 + 8, 820, 500));
        EditorGUILayout.BeginHorizontal();
        {
            NeuronScrollPosition = EditorGUILayout.BeginScrollView(
                NeuronScrollPosition,
                GUIStyle.none,
                GUI.skin.verticalScrollbar
            );
            {
                _neuronListRE.DoList(NeuronListRect);
                GUILayout.Space(_neuronListRE.GetHeight());
            }
            EditorGUILayout.EndScrollView();

            ConnectionScrollPosition = EditorGUILayout.BeginScrollView(
                ConnectionScrollPosition,
                GUIStyle.none,
                GUI.skin.verticalScrollbar
            );
            {
                _connectionListRE.DoList(ConnectionListRect);
                GUILayout.Space(_connectionListRE.GetHeight());
            }
            EditorGUILayout.EndScrollView();
        }
        EditorGUILayout.EndHorizontal();
        GUILayout.EndArea();
        _objectSO.ApplyModifiedProperties();
    }


    //loads the genome at path
    public static void LoadGenomeFromPath(string path, out uint[] LobeSizes, out NeuronGene[] NeuronGenes, out ConnectionGene[] ConnectionGenes)
    {
        //input validation
        if (!Directory.Exists(path))  //path
        {
            Debug.LogError(string.Format("Invalid path for LoadGenomeFromPath! \"{0}\"", path));
            throw new ArgumentException(string.Format("Invalid path for LoadGenomeFromPath \"{0}\"", path), "path");
        }

        //load the chromosomes
        LoadNeuronChromosomeFromPath(path, out LobeSizes, out NeuronGenes);
        LoadConnectionChromosomeFromPath(path, out ConnectionGenes);
    }

    //loads a neuron chromosome at path
    private static void LoadNeuronChromosomeFromPath(string path, out uint[] LobeSizes, out NeuronGene[] NeuronGenes) 
    {
        //try to create a filestream for the chromosome
        FileStream NeuronChromosome = null;
        try
        {
            NeuronChromosome = new FileStream(path + "/Neurons.chr2", FileMode.Open, FileAccess.Read);
        }
        catch (IOException e)
        {
            //we couldn't open the files for some reason. 
            Debug.LogError(string.Format("Could not Load Neuron Chromosome at path {0}! {1}", path, e.ToString()));

            //make sure we close the filestream if we opened it
            if (NeuronChromosome != null)
                NeuronChromosome.Close();

            throw new Exception(string.Format("Could not Load Neuron Chromosome at path {0}!", path), e);
        }

        //read in the lobe sizes from the Neuron gene
        byte[] Bytes = new byte[4];
        List<uint> _LobeSizes = new List<uint>();
        if (NeuronChromosome.Read(Bytes, 0, 4) == 4)
        {
            uint LobeCount = ((uint)Bytes[0] << 24) + ((uint)Bytes[1] << 16) + ((uint)Bytes[2] << 8) + (uint)Bytes[3];
            Debug.Log(string.Format("{0} Lobes. Reading Lobe Sizes", LobeCount));
            for (uint i = 0; i < LobeCount; i++)
            {
                if (NeuronChromosome.Read(Bytes, 0, 4) == 4)
                {
                    _LobeSizes.Add(((uint)Bytes[0] << 24) + ((uint)Bytes[1] << 16) + ((uint)Bytes[2] << 8) + (uint)Bytes[3]);
                }
                else
                {
                    //oops, we couldn't load in the bytes
                    //TODO: this
                    throw new Exception("Oops need to fix this search through \"//TODO: this\" instances");
                }
            }
            LobeSizes = _LobeSizes.ToArray();
            Debug.Log("Lobe Sizes read");
        }
        else
        {
            //oops, we couldn't load in the bytes
            //TODO: this
            throw new Exception("Oops need to fix this search through \"//TODO: this\" instances");
        }

        //begin reading in the Neuron chromosome until we read an incomplete gene
        Bytes = new byte[9];
        List<NeuronGene> _NeuronGenesList = new List<NeuronGene>();
        while (NeuronChromosome.Read(Bytes, 0, 9) == 9)
        {
            //reverse the bytes array due to endianness
            Array.Reverse(Bytes);
            _NeuronGenesList.Add(new NeuronGene(Bytes));
        }

        //store the genes
        NeuronGenes = _NeuronGenesList.ToArray();

        //close the file
        NeuronChromosome.Close();
    }

    //loads a connection chromosome at path
    private static void LoadConnectionChromosomeFromPath(string path, out ConnectionGene[] ConnectionGenes) 
    {
        //try to create a filestream for the connection and neuron chromosomes
        FileStream ConnectionChromosome = null;
        try
        {
            ConnectionChromosome = new FileStream(path + "/Connections.chr2", FileMode.Open, FileAccess.Read);
        }
        catch (IOException e)
        {
            //we couldn't open the files for some reason. 
            Debug.LogError(string.Format("Could not Load Connection Chromosome at path {0}! {1}", path, e.ToString()));

            //make sure we close the filestreams if we opened any
            if (ConnectionChromosome != null)
                ConnectionChromosome.Close();

            throw new Exception(string.Format("Could not Load Connection Chromosome at path {0}!", path), e);
        }

        //begin reading in the Connections chromosome until we read an incomplete gene
        byte[] Bytes = new byte[13];
        List<ConnectionGene> _ConnectionGenesList = new List<ConnectionGene>();
        while (ConnectionChromosome.Read(Bytes, 0, 13) == 13)
        {
            //reverse the bytes array due to endianness
            Array.Reverse(Bytes);
            _ConnectionGenesList.Add(new ConnectionGene(Bytes));
        }

        //store the genes
        ConnectionGenes = _ConnectionGenesList.ToArray();

        //close the file
        ConnectionChromosome.Close();
    }


    //saves the genome to path
    public static void SaveGenomeToPath(string path, uint[] Lobes, NeuronGene[] NeuronGenes, ConnectionGene[] ConnectionGenes) 
    {
        //create the genome folder if it doesn't exist
        if (!Directory.Exists(path))
        {
            //oops, the directory was invalid
            //TODO: this
            throw new Exception("Oops need to fix this search through \"//TODO: this\" instances");
        }

        //save the neuron chromosome
        SaveNeuronChromosomeToPath(path, Lobes, NeuronGenes);
        //save the connection chromosome
        SaveConnectionChromosomeToPath(path, ConnectionGenes);
    }
    
    //saves a neuron chromosome to a path
    private static void SaveNeuronChromosomeToPath(string path, uint[] LobeSizes, NeuronGene[] NeuronGenes) 
    {
        //create the neuron chromosome file
        FileStream NeuronChromosome = new FileStream(
            path + Path.DirectorySeparatorChar + "Neurons.chr2",
            FileMode.Create, FileAccess.Write
        );

        //append the lobe count
        byte[] LobeCountBytes = BitConverter.GetBytes(LobeSizes.Length);
        Array.Reverse(LobeCountBytes);
        NeuronChromosome.Write(LobeCountBytes, 0, 4);
        Debug.Log(String.Format("{0} {1} {2} {3}", LobeCountBytes[0], LobeCountBytes[1], LobeCountBytes[2], LobeCountBytes[3]));
        //append the lobe sizes
        byte[] LobeSizeBytes;
        for (int i = 0; i < LobeSizes.Length; i++)
        {
            LobeSizeBytes = BitConverter.GetBytes(LobeSizes[i]);
            Array.Reverse(LobeSizeBytes);
            NeuronChromosome.Write(LobeSizeBytes, 0, 4);
            Debug.Log(String.Format("{0} {1} {2} {3}", LobeSizeBytes[0], LobeSizeBytes[1], LobeSizeBytes[2], LobeSizeBytes[3]));
        }

        //append the neuron genes
        for (int i = 0; i < NeuronGenes.Length; i++)
        {
            NeuronChromosome.Write(NeuronGenes[i].ToBytes(), 0, 9);
        }

        //close the file
        NeuronChromosome.Close();
    }
    
    //saves a connection chromosome to a path
    private static void SaveConnectionChromosomeToPath(string path, ConnectionGene[] ConnectionGenes) 
    {
        //create the connection chromosome file
        FileStream ConnectionChromosome = new FileStream(
            path + Path.DirectorySeparatorChar + "Connections.chr2",
            FileMode.Create, FileAccess.Write
        );

        //append the connection genes
        for(int i = 0; i < ConnectionGenes.Length; i++) 
        {
            ConnectionChromosome.Write(ConnectionGenes[i].ToBytes(), 0, 13);
        }

        //close the file
        ConnectionChromosome.Close();
    }
}
