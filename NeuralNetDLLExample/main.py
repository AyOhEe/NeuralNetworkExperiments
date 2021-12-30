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

if __name__ == "__main__":
    handle = ctypes.c_uint64(__create_network(
        ctypes.c_char_p(b"../NeuralNetwork/Genomes/test_genome.genome"), #genome path
        ctypes.c_int(2), #inputs
        ctypes.c_int(2), #outputs
        ctypes.c_int(0), #activation function index
        ctypes.c_bool(False) #verbose?
    ))

    input_count = __get_network_input_count(handle)
    node_count = __get_network_node_count(handle)
    output_count = __get_network_output_count(handle)
    
    node0_conn_count = __get_node_connection_count(handle, ctypes.c_int(0))
    node1_conn_count = __get_node_connection_count(handle, ctypes.c_int(1))

    print(f"\n{input_count = }\n{node_count = }\n{output_count = }\n")
    print(f"{node0_conn_count = }\n{node1_conn_count = }\n")

    inputs = [
        (ctypes.c_float * 2)(0, 0),
        (ctypes.c_float * 2)(1, 1),
        (ctypes.c_float * 2)(2, 0),
        (ctypes.c_float * 2)(3, 1)
    ]

    outputs = (ctypes.c_float*2)()
    for input_set in inputs:
        __set_network_inputs(
            handle, #network handle
            input_set, #inputs
            ctypes.c_int(2) #number of inputs
        )

        __get_network_outputs(
            handle, #network handle
            outputs, #output array
            ctypes.c_int(2) #number of outputs
        )
        print(f"{outputs[0] = }\n{outputs[1] = }\n")

    __destroy_network(handle)
