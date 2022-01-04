import network

if __name__ == "__main__":
    #-----TEST 1-----
    print("-----TEST 1-----")
    #create the network
    handle = ctypes.c_uint64(__create_network(
        ctypes.c_char_p(b"../NeuralNetwork/Genomes/test_genome.genome"), #genome path
        ctypes.c_int(2), #inputs
        ctypes.c_int(1), #outputs
        ctypes.c_int(3), #activation function index
        ctypes.c_bool(False) #verbose?
    ))
    
    #get network information
    input_count = __get_network_input_count(handle)
    node_count = __get_network_node_count(handle)
    output_count = __get_network_output_count(handle)

    print(f"\n{input_count = }\n{node_count = }\n{output_count = }\n")

    for i in range(node_count):
        print(f"node{i}_bias = {__get_node_bias(handle, ctypes.c_int(i))}")
        for j in range(__get_node_connection_count(handle, ctypes.c_int(i))):
            print(f"node{i}_conn{j}_weight = {__get_connection_weight(handle, ctypes.c_int(i), ctypes.c_int(j))}")
    
    for i in range(node_count, node_count + output_count):
        for j in range(__get_node_connection_count(handle, ctypes.c_int(i))):
            print(f"node{i}_conn{j}_weight = {__get_connection_weight(handle, ctypes.c_int(i), ctypes.c_int(j))}")

    inputs = [
        (ctypes.c_float * 2)(0, 0),
        (ctypes.c_float * 2)(0, 1),
        (ctypes.c_float * 2)(1, 0),
        (ctypes.c_float * 2)(1, 1)
    ]

    #test input/output
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
        print(f"{outputs[0] = }\n")

    #alter network structure
    __add_node_between_connection(
        handle, #network handle
        ctypes.c_int(0), #node id
        ctypes.c_int(0), #connection id
        ctypes.c_float(-1.5) #bias
    )

    __add_connection_between_nodes(
        handle, #network handle
        ctypes.c_int(0), #source node id
        ctypes.c_int(node_count+1), #target node id
        ctypes.c_float(1.875) #weight
    )

    __set_node_bias(handle, ctypes.c_int(0), ctypes.c_float(0.5))
    __set_connection_weight(handle, ctypes.c_int(1), ctypes.c_int(0), ctypes.c_float(-0.875))
    
    #get new network information
    input_count = __get_network_input_count(handle)
    node_count = __get_network_node_count(handle)
    output_count = __get_network_output_count(handle)

    print(f"\n{input_count = }\n{node_count = }\n{output_count = }\n")
    
    for i in range(node_count):
        print(f"node{i}_bias = {__get_node_bias(handle, ctypes.c_int(i))}")
        for j in range(__get_node_connection_count(handle, ctypes.c_int(i))):
            print(f"node{i}_conn{j}_weight = {__get_connection_weight(handle, ctypes.c_int(i), ctypes.c_int(j))}")
    
    for i in range(node_count, node_count + output_count):
        for j in range(__get_node_connection_count(handle, ctypes.c_int(i))):
            print(f"node{i}_conn{j}_weight = {__get_connection_weight(handle, ctypes.c_int(i), ctypes.c_int(j))}")

    #test input/output again with different structure
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
        print(f"{outputs[0] = }\n")

    #save the network
    __save_network(handle, ctypes.c_char_p(b"../NeuralNetwork/Genomes/altered_genome.genome"))

    #destroy the network now that we're done with it
    __destroy_network(handle)




    #-----TEST 2-----
    print("-----TEST 2-----")
    #recreate the network again
    handle = ctypes.c_uint64(__create_network(
        ctypes.c_char_p(b"../NeuralNetwork/Genomes/altered_genome.genome"), #genome path
        ctypes.c_int(2), #inputs
        ctypes.c_int(1), #outputs
        ctypes.c_int(3), #activation function index
        ctypes.c_bool(False) #verbose?
    ))

    #get network information
    input_count = __get_network_input_count(handle)
    node_count = __get_network_node_count(handle)
    output_count = __get_network_output_count(handle)

    print(f"\n{input_count = }\n{node_count = }\n{output_count = }\n")

    for i in range(node_count):
        print(f"node{i}_bias = {__get_node_bias(handle, ctypes.c_int(i))}")
        for j in range(__get_node_connection_count(handle, ctypes.c_int(i))):
            print(f"node{i}_conn{j}_weight = {__get_connection_weight(handle, ctypes.c_int(i), ctypes.c_int(j))}")
    
    for i in range(node_count, node_count + output_count):
        for j in range(__get_node_connection_count(handle, ctypes.c_int(i))):
            print(f"node{i}_conn{j}_weight = {__get_connection_weight(handle, ctypes.c_int(i), ctypes.c_int(j))}")

    #test input/output
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
        print(f"{outputs[0] = }\n")

    #alter network structure
    __remove_node(
        handle, #network handle
        ctypes.c_int(0), #node id
    )

    __remove_connection(
        handle, #network handle
        ctypes.c_int(0), #source node id
        ctypes.c_int(0)  #connection id
    )
    
    #get new network information
    input_count = __get_network_input_count(handle)
    node_count = __get_network_node_count(handle)
    output_count = __get_network_output_count(handle)

    print(f"\n{input_count = }\n{node_count = }\n{output_count = }\n")
    
    for i in range(node_count):
        print(f"node{i}_bias = {__get_node_bias(handle, ctypes.c_int(i))}")
        for j in range(__get_node_connection_count(handle, ctypes.c_int(i))):
            print(f"node{i}_conn{j}_weight = {__get_connection_weight(handle, ctypes.c_int(i), ctypes.c_int(j))}")
    
    for i in range(node_count, node_count + output_count):
        for j in range(__get_node_connection_count(handle, ctypes.c_int(i))):
            print(f"node{i}_conn{j}_weight = {__get_connection_weight(handle, ctypes.c_int(i), ctypes.c_int(j))}")

    #test input/output again with different structure
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
        print(f"{outputs[0] = }\n")

    #save the network
    __save_network(handle, ctypes.c_char_p(b"../NeuralNetwork/Genomes/further_altered_genome.genome"))

    #destroy the network now that we're done with it
    __destroy_network(handle)