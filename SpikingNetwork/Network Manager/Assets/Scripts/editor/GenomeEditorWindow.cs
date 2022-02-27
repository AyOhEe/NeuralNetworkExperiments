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

    private static Rect NeuronListRect = new Rect(5, 50, 500, 500);
    private static Rect ConnectionListRect = new Rect(510, 50, 500, 500);

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
            string path = EditorUtility.OpenFolderPanel("Select Genome Folder", "", "");

            //get the input and output counts for the network
            int inputs = 2;
            int outputs = 2;

            //load the genome
            LoadGenomeFromPath(path, inputs, outputs);
        }

        GUILayout.Label(string.Format("Current Genome: {0}", GenomePath));

        _objectSO.Update();
        _neuronListRE.DoList(NeuronListRect);
        _connectionListRE.DoList(ConnectionListRect);
        _objectSO.ApplyModifiedProperties();
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
        Bytes = new byte[9];
        List<NeuronGene> _CurrentNeuronChromosome = new List<NeuronGene>();
        while (NeuronChromosome.Read(Bytes, 0, 9) == 9)
        {
            _CurrentNeuronChromosome.Add(new NeuronGene(Bytes));
        }

        //begin reading in the Connections chromosome until we read an incomplete gene
        Bytes = new byte[13];
        List<ConnectionGene> _CurrentConnectionChromosome = new List<ConnectionGene>();
        while (ConnectionChromosome.Read(Bytes, 0, 13) == 13)
        {
            _CurrentConnectionChromosome.Add(new ConnectionGene(Bytes));
        }

        //store the chromosomes
        CurrentNeuronChromosome = _CurrentNeuronChromosome.ToArray();
        CurrentConnectionChromosome = _CurrentConnectionChromosome.ToArray();

        //close the filestreams
        NeuronChromosome.Close();
        ConnectionChromosome.Close();
    }
}
