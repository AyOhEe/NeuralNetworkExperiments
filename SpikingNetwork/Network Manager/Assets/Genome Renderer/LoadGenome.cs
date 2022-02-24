using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LoadGenome : MonoBehaviour
{
    //loads the genome at path as a 3d structure
    public void LoadGenomeFromPath(string path, int inputs, int outputs)
    {
        Debug.Log(string.Format("{0}, {1}, {2}", path, inputs, outputs));
    }
}
