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

nnd__neuralnetdll.COPY_NETWORK.restype = ctypes.c_bool
nnd__copy_network = nnd__neuralnetdll.COPY_NETWORK

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

nnd__neuralnetdll.BREED_NETWORKS.restype = ctypes.c_uint64
nnd__breed_networks = nnd__neuralnetdll.BREED_NETWORKS

#store the error codes
class ErrorCodes(enum.Enum):
    def __int__(self):
        return self.value

    #general codes
    SUCCESS = 0                             #the operation succeeded
    UNKNOWN_FAILURE = 1                     #the operation failed for an unknown reason,
                                            #the error was logged in console

    #connection error codes
    CONNECTION_MISSING_SOURCE = 2           #a connection attempted to calculate it's value, but found it's source invalid

    #node error codes
    NODE_INVALID_CONNECTION_DISCARDED = 3   #a connection was found to be invalid, and was removed

    #network error codes
    NETWORK_INVALID_GENOME = 4              #the network was given an invalid genome
    NETWORK_INVALID_INPUTS = 5              #the network was passed invalid inputs
    NETWORK_INVALID_NODE_INDEX = 6          #the network was given an invalid node index
    NETWORK_INVALID_CONN_INDEX = 7          #the network was given an invalid connection index
    NETWORK_INVALID_ARGUMENT = 8            #the network was given an invalid argument
    NETWORK_INVALID_SAVE_PATH = 9           #the network was given an invalid save path
    NETWORK_INVALID_BREED_GENOME_A = 12     #the network was given an invalid genome A path to breed
    NETWORK_INVALID_BREED_GENOME_B = 13     #the network was given an invalid genome B path to breed

    #API Error codes
    INVALID_HANDLE = 14                     #the api was passed an invalid network handle

def nnd__assert_errcode_success(error_code):
    if(error_code != 0) and (error_code != ErrorCodes.SUCCESS):
        raise Exception(f"Error thrown from Network Library! Error: {str(error_code)}")

def nnd__errcode_check(func):
    def wrapper(*args, **kwargs):
            error_code = ctypes.c_uint(0)
            result = func(*args, ErrCode=error_code, **kwargs)
            nnd__assert_errcode_success(ErrorCodes(error_code.value))
            return result
    return wrapper


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
    @nnd__errcode_check
    def __init__(self, genome_path, inputs, outputs, activation_function, verbose=False, __handle=None, ErrCode=ctypes.c_uint(1)):
        #are we creating a new network or just storing an existing one?
        if __handle == None:
            #create a network and store the handle
            self.__handle = ctypes.c_uint64(nnd__create_network(
                ctypes.c_char_p(genome_path),
                ctypes.c_int(inputs),
                ctypes.c_int(outputs),
                ctypes.c_int(int(activation_function)),
                ctypes.c_bool(verbose),
                ctypes.byref(ErrCode)
            ))
        else:
            #store the handle
            self.__handle = ctypes.c_uint64(__handle)

        #store information about the network
        self._inputs = inputs
        self._outputs = outputs
        self._activation_function = activation_function

        #error code integer

    #returns a copy of this network
    @nnd__errcode_check
    def copy(self, verbose=False, ErrCode=ctypes.c_uint(int(ErrorCodes.SUCCESS))):
        #create the copy
        copy_handle = nnd__copy_network(self.__handle, ctypes.byref(ErrCode))
        #return a network object created with that handle
        return Network("", self._inputs, self._outputs, self._activation_function, verbose, copy_handle)

    #interface for the destructor
    @nnd__errcode_check
    def destroy(self, ErrCode=ctypes.c_uint(int(ErrorCodes.SUCCESS))):
        #destroy the network on the c++ end
        nnd__destroy_network(self.__handle, ctypes.byref(ErrCode))
       
    #looks at and returns the outputs from the last call of CalculateOutputs
    @nnd__errcode_check
    def PeekOutputs(self, ErrCode=ctypes.c_uint(int(ErrorCodes.SUCCESS))):
        #create a ctypes array for the outputs to be copied to
        last_outputs_array = (ctypes.c_float * self._outputs)()

        #get the last outputs
        nnd__get_network_outputs(
            self.__handle,
            last_outputs_array,
            ctypes.c_int(self._outputs), 
            ctypes.byref(ErrCode)
        )

        #return the last outputs
        return [float(v) for v in last_outputs_array]

    #calculates and returns the outputs of the network given inputs
    @nnd__errcode_check
    def CalculateOutputs(self, inputs, ErrCode=ctypes.c_uint(int(ErrorCodes.SUCCESS))):
        #convert the inputs list into a ctypes array
        inputs_array = (ctypes.c_float * len(inputs))(*inputs)
        #set the inputs
        nnd__set_network_inputs(
            self.__handle,
            inputs_array,
            ctypes.c_int(len(inputs)), 
            ctypes.byref(ErrCode)
        )
        #return the new outputs
        return self.PeekOutputs()

    #returns relevant information about the network
    @nnd__errcode_check
    def GetInputCount(self, ErrCode=ctypes.c_uint(int(ErrorCodes.SUCCESS))):
        return nnd__get_network_input_count(self.__handle, ctypes.byref(ErrCode))

    @nnd__errcode_check
    def GetNodeCount(self, ErrCode=ctypes.c_uint(int(ErrorCodes.SUCCESS))):
        return nnd__get_network_node_count(self.__handle, ctypes.byref(ErrCode))

    @nnd__errcode_check
    def GetOutputCount(self, ErrCode=ctypes.c_uint(int(ErrorCodes.SUCCESS))):
        return nnd__get_network_output_count(self.__handle, ctypes.byref(ErrCode))

    def GetNetworkStructure(self):
        return (self.GetInputCount(), self.GetNodeCount(), self.GetOutputCount())

    #adds a node between a connection to the network
    @nnd__errcode_check
    def AddNode(self, NodeIndex, ConnectionIndex, bias, ErrCode=ctypes.c_uint(int(ErrorCodes.SUCCESS))):
        nnd__add_node_between_connection(
            self.__handle, 
            ctypes.c_int(NodeIndex),
            ctypes.c_int(ConnectionIndex), 
            ctypes.c_float(bias), 
            ctypes.byref(ErrCode)
        )

    #adds a connection to the network
    @nnd__errcode_check
    def AddConnection(self, SourceNodeIndex, TargetNodeIndex, weight, ErrCode=ctypes.c_uint(int(ErrorCodes.SUCCESS))):
        nnd__add_connection_between_nodes(
            self.__handle,
            ctypes.c_int(SourceNodeIndex),
            ctypes.c_int(TargetNodeIndex),
            ctypes.c_float(weight), 
            ctypes.byref(ErrCode)
        )

    #removes a node from the network
    @nnd__errcode_check
    def RemoveNode(self, NodeIndex, ErrCode=ctypes.c_uint(int(ErrorCodes.SUCCESS))):
        nnd__remove_node(
            self.__handle,
            ctypes.c_int(NodeIndex), 
            ctypes.byref(ErrCode)
        )

    #removes a connection from the network
    @nnd__errcode_check
    def RemoveConnection(self, NodeIndex, ConnectionIndex, ErrCode=ctypes.c_uint(int(ErrorCodes.SUCCESS))):
        nnd__remove_connection(
            self.__handle,
            ctypes.c_int(NodeIndex),
            ctypes.c_int(ConnectionIndex), 
            ctypes.byref(ErrCode)
        )

    #returns the bias of a node
    @nnd__errcode_check
    def GetNodeBias(self, NodeIndex, ErrCode=ctypes.c_uint(int(ErrorCodes.SUCCESS))):
        return nnd__get_node_bias(
            self.__handle,
            ctypes.c_int(NodeIndex), 
            ctypes.byref(ErrCode)
        )

    #sets the bias of a node
    @nnd__errcode_check
    def SetNodeBias(self, NodeIndex, bias, ErrCode=ctypes.c_uint(int(ErrorCodes.SUCCESS))):
        nnd__set_node_bias(
            self.__handle, 
            ctypes.c_int(NodeIndex), 
            ctypes.c_float(bias), 
            ctypes.byref(ErrCode)
        )

    #returns the number of connections targeting node NodeIndex
    @nnd__errcode_check
    def GetNodeConnectionCount(self, NodeIndex, ErrCode=ctypes.c_uint(int(ErrorCodes.SUCCESS))):
        return nnd__get_node_connection_count(
            self.__handle, 
            ctypes.c_int(NodeIndex), 
            ctypes.byref(ErrCode)
        )

    #returns the weight of a connection
    @nnd__errcode_check
    def GetConnectionWeight(self, NodeIndex, ConnectionIndex, ErrCode=ctypes.c_uint(int(ErrorCodes.SUCCESS))):
        return nnd__get_connection_weight(
            self.__handle, 
            ctypes.c_int(NodeIndex), 
            ctypes.c_int(ConnectionIndex), 
            ctypes.byref(ErrCode)
        )

    #sets the weight of a connection
    @nnd__errcode_check
    def SetConnectionWeight(self, NodeIndex, ConnectionIndex, weight, ErrCode=ctypes.c_uint(int(ErrorCodes.SUCCESS))):
        nnd__set_connection_weight(
            self.__handle, 
            ctypes.c_int(NodeIndex), 
            ctypes.c_int(ConnectionIndex), 
            ctypes.c_float(weight), 
            ctypes.byref(ErrCode)
        )

    #saves the network in it's current state to a file
    @nnd__errcode_check
    def SaveNetwork(self, GenomePath, ErrCode=ctypes.c_uint(int(ErrorCodes.SUCCESS))):
        nnd__save_network(
            self.__handle, 
            ctypes.c_char_p(bytes(GenomePath, "utf-8")), 
            ctypes.byref(ErrCode)
        )
    
    #breeds this network with another with Settings and returns the new network
    @nnd__errcode_check
    def BreedNetwork(self, OtherNetwork, CrossoverPoints, MutationChance, verbose=False, ErrCode=ctypes.c_uint(int(ErrorCodes.SUCCESS))):
        NewHandle = nnd__neuralnetdll.BREED_NETWORKS(
            self.__handle, 
            OtherNetwork.__handle, 
            self._inputs, 
            self._outputs, 
            ctypes.c_int(int(self._activation_function)),
            CrossoverPoints, 
            ctypes.c_int(4), 
            ctypes.c_float(MutationChance), 
            ctypes.c_bool(verbose), 
            ctypes.byref(ErrCode)
        )
        #create a network with the new handle
        return Network(
            "",
            self._inputs, 
            self._outputs,
            self._activation_function,
            verbose,
            NewHandle
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