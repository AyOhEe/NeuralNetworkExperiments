using System;
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
        Type = (bytes[0] & 0b10000000) == 0b10000000;
        OutputType = (bytes[0] & 0b01000000) == 0b01000000;
        MembraneResistance = BitConverter.ToSingle(bytes, 1);
        ThresholdOffset = BitConverter.ToSingle(bytes, 5);
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