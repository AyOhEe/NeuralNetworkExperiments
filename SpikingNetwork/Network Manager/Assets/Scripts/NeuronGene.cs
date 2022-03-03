using System;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

[Serializable]
public struct NeuronGene
{
    //gene values
    public bool Type;
    public bool OutputType;
    public float MembraneResistance;
    public float ThresholdOffset;

    //creates a Neuron gene from a byte list
    public NeuronGene(byte[] bytes)
    {
        //get our values
        Type = (bytes[8] & 0b10000000) == 0b10000000;
        OutputType = (bytes[8] & 0b01000000) == 0b01000000;
        MembraneResistance = BitConverter.ToSingle(bytes, 4);
        ThresholdOffset = BitConverter.ToSingle(bytes, 0);
    }
    
    //returns this gene as a byte sequence
    public byte[] ToBytes() 
    {
        //the byte array
        List<byte> bytes = new List<byte>();

        //get the membrane resistance and threshold offset as bytes
        byte[] MembraneBytes = BitConverter.GetBytes(MembraneResistance);
        byte[] ThresholdBytes = BitConverter.GetBytes(ThresholdOffset);
        Array.Reverse(MembraneBytes);
        Array.Reverse(ThresholdBytes);

        //store our values
        bytes.Add((byte)((Type ? 0b10000000u : 0) + (OutputType ? 0b01000000u : 0)));
        bytes.InsertRange(1, MembraneBytes);
        bytes.InsertRange(5, ThresholdBytes);

        //return our byte array
        return bytes.ToArray();
    }
}

//https://riptutorial.com/unity3d/example/8282/custom-property-drawer
[CustomPropertyDrawer(typeof(NeuronGene))]
public class NeuronGeneDrawer : PropertyDrawer 
{
    public override float GetPropertyHeight(SerializedProperty property, GUIContent label)
    {
        // The 6 comes from extra spacing between the fields (2px each)
        return EditorGUIUtility.singleLineHeight * 4 + 18;
    }

    public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
    {
        EditorGUI.BeginProperty(position, label, property);

        EditorGUI.LabelField(position, label);

        var typeRect = new Rect(position.x, position.y + 18, position.width, 16);
        var outputTypeRect = new Rect(position.x, position.y + 36, position.width, 16);
        var membraneResistanceRect = new Rect(position.x, position.y + 54, position.width, 16);
        var thresholdOffsetRect = new Rect(position.x, position.y + 72, position.width, 16);

        EditorGUI.indentLevel++;

            EditorGUI.PropertyField(typeRect, property.FindPropertyRelative("Type"));
            EditorGUI.PropertyField(outputTypeRect, property.FindPropertyRelative("OutputType"));
            EditorGUI.PropertyField(membraneResistanceRect, property.FindPropertyRelative("MembraneResistance"));
            EditorGUI.PropertyField (thresholdOffsetRect, property.FindPropertyRelative("ThresholdOffset"));

        EditorGUI.indentLevel--;

        EditorGUI.EndProperty();
    }
}