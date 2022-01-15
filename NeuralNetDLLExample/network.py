import ctypes
import pathlib
import enum
import platform

#load the library
nnd__neuralnetdllname = ""
if platform.system() == "Windows":
    nnd__neuralnetdllname = str(pathlib.Path().absolute()) + "\\NeuralNetDLL.dll"
elif platform.system() == "Linux":
    nnd__neuralnetdllname = str(pathlib.Path().absolute()) + "/NeuralNetDLL.so"
nnd__neuralnetdll = ctypes.CDLL(nnd__neuralnetdllname)

#set up bindings
nnd__neuralnetdll.CREATE_NETWORK.restype = ctypes.c_uint64
nnd__neuralnetdll.DESTROY_NETWORK.restype = ctypes.c_bool
nnd__create_network = nnd__neuralnetdll.CREATE_NETWORK
nnd__destroy_network = nnd__neuralnetdll.DESTROY_NETWORK

nnd__neuralnetdll.GET_NETWORK_OUTPUTS.restype = ctypes.c_bool
nnd__neuralnetdll.SET_NETWORK_INPUTS.restype = ctypes.c_bool
nnd__get_network_outputs = nnd__neuralnetdll.GET_NETWORK_OUTPUTS
nnd__set_network_inputs = nnd__neuralnetdll.SET_NETWORK_INPUTS

nnd__neuralnetdll.GET_NETWORK_INPUT_COUNT.restype = ctypes.c_int
nnd__neuralnetdll.GET_NETWORK_NODE_COUNT.restype = ctypes.c_int
nnd__neuralnetdll.GET_NETWORK_OUTPUT_COUNT.restype = ctypes.c_int
nnd__get_network_input_count = nnd__neuralnetdll.GET_NETWORK_INPUT_COUNT
nnd__get_network_node_count = nnd__neuralnetdll.GET_NETWORK_NODE_COUNT
nnd__get_network_output_count = nnd__neuralnetdll.GET_NETWORK_OUTPUT_COUNT

nnd__neuralnetdll.ADD_NODE_BETWEEN_CONNECTION.restype = ctypes.c_bool
nnd__neuralnetdll.ADD_CONNECTION_BETWEEN_NODES.restype = ctypes.c_bool
nnd__add_node_between_connection = nnd__neuralnetdll.ADD_NODE_BETWEEN_CONNECTION
nnd__add_connection_between_nodes = nnd__neuralnetdll.ADD_CONNECTION_BETWEEN_NODES

nnd__neuralnetdll.REMOVE_NODE.restype = ctypes.c_bool
nnd__neuralnetdll.REMOVE_CONNECTION.restype = ctypes.c_bool
nnd__remove_node = nnd__neuralnetdll.REMOVE_NODE
nnd__remove_connection = nnd__neuralnetdll.REMOVE_CONNECTION

nnd__neuralnetdll.GET_NODE_BIAS.restype = ctypes.c_float
nnd__neuralnetdll.SET_NODE_BIAS.restype = ctypes.c_bool
nnd__get_node_bias = nnd__neuralnetdll.GET_NODE_BIAS
nnd__set_node_bias = nnd__neuralnetdll.SET_NODE_BIAS

nnd__neuralnetdll.GET_NODE_CONNECTION_COUNT.restype = ctypes.c_int
nnd__get_node_connection_count = nnd__neuralnetdll.GET_NODE_CONNECTION_COUNT

nnd__neuralnetdll.GET_CONNECTION_WEIGHT.restype = ctypes.c_float
nnd__neuralnetdll.SET_CONNECTION_WEIGHT.restype = ctypes.c_bool
nnd__get_connection_weight = nnd__neuralnetdll.GET_CONNECTION_WEIGHT
nnd__set_connection_weight = nnd__neuralnetdll.SET_CONNECTION_WEIGHT

nnd__neuralnetdll.SAVE_NETWORK.restype = ctypes.c_bool
nnd__save_network = nnd__neuralnetdll.SAVE_NETWORK

#interface class for the bindings in NeuralNetDLL.dll
class Network:
    #enum for activation function indexes
    class ActivationFunction(enum.Enum):
        Sigmoid = 0
        Tanh = 1
        ReLu = 2
        LeakyReLu = 3
        
        def __int__(self):
            return self.value

    #constructor
    def __init__(self, genome_path, inputs, outputs, activation_function, verbose=False):
        #create a network and store the handle
        self.__handle = ctypes.c_uint64(nnd__create_network(
            ctypes.c_char_p(genome_path),
            ctypes.c_int(inputs),
            ctypes.c_int(outputs),
            ctypes.c_int(int(activation_function)),
            ctypes.c_bool(verbose)
        ))

        #store information about the network
        self._inputs = inputs
        self._outputs = outputs

    #interface for the destructor
    def destroy(self):
        #destroy the network on the c++ end
        nnd__destroy_network(self.__handle)
       
    #looks at and returns the outputs from the last call of CalculateOutputs
    def PeekOutputs(self):
        #create a ctypes array for the outputs to be copied to
        last_outputs_array = (ctypes.c_float * self._outputs)()

        #get the last outputs
        nnd__get_network_outputs(
            self.__handle,
            last_outputs_array,
            ctypes.c_int(self._outputs)
        )

        #return the last outputs
        return [v for v in last_outputs_array]

    #calculates and returns the outputs of the network given inputs
    def CalculateOutputs(self, inputs):
        #convert the inputs list into a ctypes array
        inputs_array = (ctypes.c_float * len(inputs))(*inputs)
        #set the inputs
        nnd__set_network_inputs(
            self.__handle,
            inputs_array,
            ctypes.c_int(len(inputs))
        )
        #return the new outputs
        return self.PeekOutputs()

    #returns relevant information about the network
    def GetInputCount(self):
        return nnd__get_network_input_count(self.__handle)

    def GetNodeCount(self):
        return nnd__get_network_node_count(self.__handle)

    def GetOutputCount(self):
        return nnd__get_network_output_count(self.__handle)

    def GetNetworkStructure(self):
        return (self.GetInputCount(), self.GetNodeCount(), self.GetOutputCount())

    #adds a node between a connection to the network
    def AddNode(self, NodeIndex, ConnectionIndex, bias):
        nnd__add_node_between_connection(
            self.__handle, 
            ctypes.c_int(NodeIndex),
            ctypes.c_int(ConnectionIndex), 
            ctypes.c_float(bias)
        )

    #adds a connection to the network
    def AddConnection(self, SourceNodeIndex, TargetNodeIndex, weight):
        nnd__add_connection_between_nodes(
            self.__handle,
            ctypes.c_int(SourceNodeIndex),
            ctypes.c_int(TargetNodeIndex),
            ctypes.c_float(weight)
        )

    #removes a node from the network
    def RemoveNode(self, NodeIndex):
        nnd__remove_node(
            self.__handle,
            ctypes.c_int(NodeIndex)
        )

    #removes a connection from the network
    def RemoveConnection(self, NodeIndex, ConnectionIndex):
        nnd__remove_connection(
            self.__handle,
            ctypes.c_int(NodeIndex),
            ctypes.c_int(ConnectionIndex)
        )

    #returns the bias of a node
    def GetNodeBias(self, NodeIndex):
        return nnd__get_node_bias(
            self.__handle,
            ctypes.c_int(NodeIndex)
        )

    #sets the bias of a node
    def SetNodeBias(self, NodeIndex, bias):
        nnd__set_node_bias(
            self.__handle, 
            ctypes.c_int(NodeIndex), 
            ctypes.c_float(bias)
        )

    #returns the number of connections targeting node NodeIndex
    def GetNodeConnectionCount(self, NodeIndex):
        return nnd__get_node_connection_count(
            self.__handle, 
            ctypes.c_int(NodeIndex)
        )

    #returns the weight of a connection
    def GetConnectionWeight(self, NodeIndex, ConnectionIndex):
        return nnd__get_connection_weight(
            self.__handle, 
            ctypes.c_int(NodeIndex), 
            ctypes.c_int(ConnectionIndex)
        )

    #sets the weight of a connection
    def SetConnectionWeight(self, NodeIndex, ConnectionIndex, weight):
        nnd__set_connection_weight(
            self.__handle, 
            ctypes.c_int(NodeIndex), 
            ctypes.c_int(ConnectionIndex), 
            ctypes.c_float(weight)
        )

    #saves the network in it's current state to a file
    def SaveNetwork(self, GenomePath):
        nnd__save_network(
            self.__handle, 
            ctypes.c_char_p(bytes(GenomePath))
        )

if __name__ == "__main__":
    #-----TEST 1-----
    print("-----TEST 1-----")
    #create a network
    net = Network(
        b"../NeuralNetwork/Genomes/test_genome", #genome path
        2,                                             #inputs
        1,                                             #outputs
        Network.ActivationFunction.LeakyReLu,          #activation function index
        verbose=False                                  #optional: verbosity
    )

    print(f"\n{net.GetNetworkStructure() = }")

    print("\nNodes: ")
    for i in range(net.GetNodeCount()):
        print(f"  node{i}_bias = {net.GetNodeBias(i)}")
        for j in range(net.GetNodeConnectionCount(i)):
            print(f"    node{i}_conn{j}_weight = {net.GetConnectionWeight(i, j)}")
    print("\nOutputNodes:")
    for i in range(net.GetNodeCount(), net.GetNodeCount() + net.GetOutputCount()):
        print(f"  output node{i}")
        for j in range(net.GetNodeConnectionCount(i)):
            print(f"    node{i}_conn{j}_weight = {net.GetConnectionWeight(i, j)}")

    print(f"\n{net.CalculateOutputs([0, 0]) = }")
    print(f"{net.CalculateOutputs([0, 1]) = }")
    print(f"{net.CalculateOutputs([1, 0]) = }")
    print(f"{net.CalculateOutputs([1, 1]) = }")

    net.AddNode(0, 0, -1.5)
    net.AddConnection(0, net.GetNodeCount(), 1.875)
    net.SetNodeBias(0, 0.5)
    net.SetConnectionWeight(1, 0, -0.875)

    print(f"\n{net.GetNetworkStructure() = }")
    
    print("\nNodes: ")
    for i in range(net.GetNodeCount()):
        print(f"  node{i}_bias = {net.GetNodeBias(i)}")
        for j in range(net.GetNodeConnectionCount(i)):
            print(f"    node{i}_conn{j}_weight = {net.GetConnectionWeight(i, j)}")
    print("\nOutputNodes:")
    for i in range(net.GetNodeCount(), net.GetNodeCount() + net.GetOutputCount()):
        print(f"  output node{i}")
        for j in range(net.GetNodeConnectionCount(i)):
            print(f"    node{i}_conn{j}_weight = {net.GetConnectionWeight(i, j)}")

    print(f"\n{net.CalculateOutputs([0, 0]) = }")
    print(f"{net.CalculateOutputs([0, 1]) = }")
    print(f"{net.CalculateOutputs([1, 0]) = }")
    print(f"{net.CalculateOutputs([1, 1]) = }")

    net.SaveNetwork(b"../NeuralNetwork/Genomes/altered_genome")

    #-----TEST 2-----
    print("\n\n-----TEST 2-----")
    #create a network
    net = Network(
        b"../NeuralNetwork/Genomes/altered_genome", #genome path
        2,                                             #inputs
        1,                                             #outputs
        Network.ActivationFunction.LeakyReLu,          #activation function index
        verbose=False                                  #optional: verbosity
    )

    print(f"\n{net.GetNetworkStructure() = }")
    
    print("\nNodes: ")
    for i in range(net.GetNodeCount()):
        print(f"  node{i}_bias = {net.GetNodeBias(i)}")
        for j in range(net.GetNodeConnectionCount(i)):
            print(f"    node{i}_conn{j}_weight = {net.GetConnectionWeight(i, j)}")
    print("\nOutputNodes:")
    for i in range(net.GetNodeCount(), net.GetNodeCount() + net.GetOutputCount()):
        print(f"  output node{i}")
        for j in range(net.GetNodeConnectionCount(i)):
            print(f"    node{i}_conn{j}_weight = {net.GetConnectionWeight(i, j)}")

    print(f"\n{net.CalculateOutputs([0, 0]) = }")
    print(f"{net.CalculateOutputs([0, 1]) = }")
    print(f"{net.CalculateOutputs([1, 0]) = }")
    print(f"{net.CalculateOutputs([1, 1]) = }")

    net.RemoveNode(0)
    net.RemoveConnection(0, 0)

    print(f"\n{net.GetNetworkStructure() = }")
    
    print("\nNodes: ")
    for i in range(net.GetNodeCount()):
        print(f"  node{i}_bias = {net.GetNodeBias(i)}")
        for j in range(net.GetNodeConnectionCount(i)):
            print(f"    node{i}_conn{j}_weight = {net.GetConnectionWeight(i, j)}")
    print("\nOutputNodes:")
    for i in range(net.GetNodeCount(), net.GetNodeCount() + net.GetOutputCount()):
        print(f"  output node{i}")
        for j in range(net.GetNodeConnectionCount(i)):
            print(f"    node{i}_conn{j}_weight = {net.GetConnectionWeight(i, j)}")

    print(f"\n{net.CalculateOutputs([0, 0]) = }")
    print(f"{net.CalculateOutputs([0, 1]) = }")
    print(f"{net.CalculateOutputs([1, 0]) = }")
    print(f"{net.CalculateOutputs([1, 1]) = }")

    net.SaveNetwork(b"../NeuralNetwork/Genomes/further_altered_genome")