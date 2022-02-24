using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(LoadGenome))]
public class LoadGenomeEditor : Editor
{ 
    public override void OnInspectorGUI() 
    {
        //we'd like all of the original gui
        base.OnInspectorGUI();

        //get the target LoadGenome monobehaviour
        LoadGenome loadGenome = (LoadGenome)target;

        //load genome button
        if (GUILayout.Button("Load Genome as GameObject")) 
        {
            //get the path to load from
            string path = EditorUtility.OpenFolderPanel("Select Genome Folder", "", "");
            //get the input and output counts for the network
            int inputs = 0;
            int outputs = 0;

            //load the genome
            loadGenome.LoadGenomeFromPath(path, inputs, outputs);
        }
    }
}
