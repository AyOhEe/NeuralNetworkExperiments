using System;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

[Serializable]
public struct ConnectionGene
{
    //gene values
    public bool SourceType;
    public bool TargetType;
    public uint SourceIndex;
    public uint TargetIndex;
    public float Weight;

    public ConnectionGene(byte[] bytes) 
    {
        //get our values
        SourceType = (bytes[12] & 0b10000000) == 0b10000000;
        TargetType = (bytes[12] & 0b01000000) == 0b01000000;
        SourceIndex = BitConverter.ToUInt32(bytes, 8);
        TargetIndex = BitConverter.ToUInt32(bytes, 4);
        Weight = BitConverter.ToSingle(bytes, 0);
    }

    //returns this gene as a byte sequence
    public byte[] ToBytes()
    {
        //the byte array
        List<byte> bytes = new List<byte>();

        //get the source and target index and weight as bytes
        byte[] SourceIndexBytes = BitConverter.GetBytes(SourceIndex);
        byte[] TargetIndexBytes = BitConverter.GetBytes(TargetIndex);
        byte[] WeightBytes = BitConverter.GetBytes(Weight);
        Array.Reverse(SourceIndexBytes);
        Array.Reverse(TargetIndexBytes);
        Array.Reverse(WeightBytes);

        //store our values
        bytes.Add((byte)((SourceType ? 0b10000000 : 0) + (TargetType ? 0b01000000 : 0)));
        bytes.InsertRange(1, SourceIndexBytes);
        bytes.InsertRange(5, TargetIndexBytes);
        bytes.InsertRange(9, WeightBytes);

        //return our byte array
        return bytes.ToArray();
    }
}

//https://riptutorial.com/unity3d/example/8282/custom-property-drawer
[CustomPropertyDrawer(typeof(ConnectionGene))]
public class ConnectionGeneDrawer : PropertyDrawer
{
    public override float GetPropertyHeight(SerializedProperty property, GUIContent label)
    {
        // The 6 comes from extra spacing between the fields (2px each)
        return EditorGUIUtility.singleLineHeight * 5 + 20;
    }

    public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
    {
        EditorGUI.BeginProperty(position, label, property);

        EditorGUI.LabelField(position, label);

        var sourceTypeRect = new Rect(position.x, position.y + 18, position.width, 16);
        var targetTypeRect = new Rect(position.x, position.y + 36, position.width, 16);
        var sourceIndexRect = new Rect(position.x, position.y + 54, position.width, 16);
        var targetIndexRect = new Rect(position.x, position.y + 72, position.width, 16);
        var weightRect = new Rect(position.x, position.y + 90, position.width, 16);

        EditorGUI.indentLevel++;

            EditorGUI.PropertyField(sourceTypeRect, property.FindPropertyRelative("SourceType"));
            EditorGUI.PropertyField(targetTypeRect, property.FindPropertyRelative("TargetType"));
            EditorGUI.PropertyField(sourceIndexRect, property.FindPropertyRelative("SourceIndex"));
            EditorGUI.PropertyField(targetIndexRect, property.FindPropertyRelative("TargetIndex"));
            EditorGUI.PropertyField(weightRect, property.FindPropertyRelative("Weight"));

        EditorGUI.indentLevel--;

        EditorGUI.EndProperty();
    }
}