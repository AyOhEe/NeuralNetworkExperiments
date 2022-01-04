import ctypes
import pathlib

#load the library
__neuralnetdllname = str(pathlib.Path().absolute()) + "\\NeuralNetDLL.dll"
__neuralnetdll = ctypes.CDLL(__neuralnetdllname)

#set up bindings
__neuralnetdll.CREATE_NETWORK.restype = ctypes.c_uint64
__neuralnetdll.DESTROY_NETWORK.restype = ctypes.c_bool
__create_network = __neuralnetdll.CREATE_NETWORK
__destroy_network = __neuralnetdll.DESTROY_NETWORK

__neuralnetdll.GET_NETWORK_OUTPUTS.restype = ctypes.c_bool
__neuralnetdll.SET_NETWORK_INPUTS.restype = ctypes.c_bool
__get_network_outputs = __neuralnetdll.GET_NETWORK_OUTPUTS
__set_network_inputs = __neuralnetdll.SET_NETWORK_INPUTS

__neuralnetdll.GET_NETWORK_INPUT_COUNT.restype = ctypes.c_int
__neuralnetdll.GET_NETWORK_NODE_COUNT.restype = ctypes.c_int
__neuralnetdll.GET_NETWORK_OUTPUT_COUNT.restype = ctypes.c_int
__get_network_input_count = __neuralnetdll.GET_NETWORK_INPUT_COUNT
__get_network_node_count = __neuralnetdll.GET_NETWORK_NODE_COUNT
__get_network_output_count = __neuralnetdll.GET_NETWORK_OUTPUT_COUNT

__neuralnetdll.ADD_NODE_BETWEEN_CONNECTION.restype = ctypes.c_bool
__neuralnetdll.ADD_CONNECTION_BETWEEN_NODES.restype = ctypes.c_bool
__add_node_between_connection = __neuralnetdll.ADD_NODE_BETWEEN_CONNECTION
__add_connection_between_nodes = __neuralnetdll.ADD_CONNECTION_BETWEEN_NODES

__neuralnetdll.REMOVE_NODE.restype = ctypes.c_bool
__neuralnetdll.REMOVE_CONNECTION.restype = ctypes.c_bool
__remove_node = __neuralnetdll.REMOVE_NODE
__remove_connection = __neuralnetdll.REMOVE_CONNECTION

__neuralnetdll.GET_NODE_BIAS.restype = ctypes.c_float
__neuralnetdll.SET_NODE_BIAS.restype = ctypes.c_bool
__get_node_bias = __neuralnetdll.GET_NODE_BIAS
__set_node_bias = __neuralnetdll.SET_NODE_BIAS

__neuralnetdll.GET_NODE_CONNECTION_COUNT.restype = ctypes.c_int
__get_node_connection_count = __neuralnetdll.GET_NODE_CONNECTION_COUNT

__neuralnetdll.GET_CONNECTION_WEIGHT.restype = ctypes.c_float
__neuralnetdll.SET_CONNECTION_WEIGHT.restype = ctypes.c_bool
__get_connection_weight = __neuralnetdll.GET_CONNECTION_WEIGHT
__set_connection_weight = __neuralnetdll.SET_CONNECTION_WEIGHT

__neuralnetdll.SAVE_NETWORK.restype = ctypes.c_bool
__save_network = __neuralnetdll.SAVE_NETWORK

#interface class for the bindings in NeuralNetDLL.dll
class Network:
    #constructor
    def __init__(self, genome_path, inputs, outputs, activation_function, verbose=False):
        #create a network and store the handle
        self._handle = __create_network(
            ctypes.c_char_p(bytes(genome_path)),
            ctypes.c_int(inputs),
            ctypes.c_int(outputs),
            ctypes.c_int(activation_function),
            ctypes.c_bool(verbose)
        )

        #store information about the network
        self._inputs = inputs
        self._outputs = outputs
      
    #looks at and returns the outputs from the last call of CalculateOutputs
    def PeekOutputs(self):
        #create a ctypes array for the outputs to be copied to
        last_outputs_array = (ctypes.c_float * self._outputs)()

        #get the last outputs
        __get_network_outputs(
            self.handle,
            last_outputs_array,
            ctypes.c_int(self._outputs)
        )

        #return the last outputs
        return last_outputs_array

    #calculates and returns the outputs of the network given inputs
    def CalculateOutputs(self, inputs):
        #convert the inputs list into a ctypes array
        inputs_array = (ctypes.c_float * len(inputs))(*inputs)
        #set the inputs
        __set_network_inputs(
            handle,
            inputs_array,
            ctypes.c_int(len(inputs))
        )

    #destuctor
    def __del__(self):
        #when the network is garbage collected, destroy it on the library end
        __destroy_network(handle)