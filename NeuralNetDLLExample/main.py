import ctypes
import pathlib

#load the library
__neuralnetdllname = str(pathlib.Path().absolute()) + "\\NeuralNetDLL.dll"
__neuralnetdll = ctypes.CDLL(__neuralnetdllname)

#set up bindings
__neuralnetdll.CREATE_NETWORK.restype = ctypes.c_uint64
__create_network = __neuralnetdll.CREATE_NETWORK

__neuralnetdll.DESTROY_NETWORK.restype = ctypes.c_bool
__destroy_network = __neuralnetdll.DESTROY_NETWORK

__neuralnetdll.SET_NETWORK_INPUTS.restype = ctypes.c_bool
__set_network_inputs = __neuralnetdll.SET_NETWORK_INPUTS

__neuralnetdll.GET_NETWORK_OUTPUTS.restype = ctypes.c_bool
__get_network_outputs = __neuralnetdll.GET_NETWORK_OUTPUTS

if __name__ == "__main__":
    handle = ctypes.c_uint64(__create_network(
        ctypes.c_char_p(b"../NeuralNetwork/Genomes/test_genome_small.genome"), #genome path
        ctypes.c_int(2), #inputs
        ctypes.c_int(1), #outputs
        ctypes.c_int(0) #activation function index
    ))

    inputs = [
        (ctypes.c_float * 2)(0, 0),
        (ctypes.c_float * 2)(0, 1),
        (ctypes.c_float * 2)(1, 0),
        (ctypes.c_float * 2)(1, 1)
    ]
    outputs = (ctypes.c_float*1)()
    for input_set in inputs:
        __set_network_inputs(
            handle, #network handle
            input_set, #inputs
            ctypes.c_int(2) #number of inputs
        )

        __get_network_outputs(
            handle, #network handle
            outputs, #output array
            ctypes.c_int(1) #number of outputs
        )
        print(outputs[0])

    __destroy_network(handle)
