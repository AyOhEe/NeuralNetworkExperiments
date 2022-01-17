from network import Network

if __name__ == "__main__":
    #test settings
    network_count = 100
    episode_length = 200
    generation_length = 1000

    node_add_chance = 50
    connection_add_chance = 50
    node_remove_chance = 50
    connection_remove_chance = 50

    bias_alter_chance = 50
    weight_alter_chance = 50

    #import openai-gym https://gym.openai.com/docs/
    import gym
    import random
    import ctypes

    #create the networks
    networks = [Network(b"Genomes/Default", 4, 1, Network.ActivationFunction.Sigmoid) for i in range(network_count)]

    #start the test environment
    env = gym.make('CartPole-v0')
    env.reset()
    
    for gen_i in range(generation_length):
        print(f"\n\n================================\n\tGENERATION {gen_i}\n================================\n")
        #test the networks on the environment
        scores = []
        for net_i in range(len(networks)):
            total_reward = 0
            observation, reward, done, info = (0, 0, 0, 0), 0, False, {}
            for t in range(episode_length):
                observation, reward, done, info = env.step(int(round(networks[net_i].CalculateOutputs(observation)[0]))) #do an action
                total_reward += reward #store the reward for this iteration

                if done: #exit if we've finished the episode
                    print(f"Episode for network {net_i} in {gen_i} finished after {t+1} timesteps with {total_reward} reward")
                    break
            scores.append((net_i, total_reward))
            env.reset()

        #sort the networks based on their scores
        scores = sorted(scores, key=lambda x: x[1], reverse=True)

        #destroy the bottom half
        for net_tuple in scores[len(scores)//2:]:
            networks[net_tuple[0]].destroy()

        #and create a new network list with the successful networks
        good_networks = []
        for net_tuple in scores[:len(scores)//2]:
            good_networks.append(networks[net_tuple[0]])

        #randomly breed these networks to make up for the deficit
        bred_networks = []
        for i in range(network_count - len(good_networks)):
            parentA = random.randrange(0, len(good_networks)) #get the first parent index
            parentB = random.randrange(0, len(good_networks)) #get the second parent index
            while parentB == parentA: #ensure the parent indices aren't the same
                parentB = random.randrange(0, len(good_networks))

            bred_networks.append(
                good_networks[parentA].BreedNetwork(good_networks[parentB], (ctypes.c_float * 4)(0.2, 0.4, 0.6, 0.8), 50, True)
            )

            #decide if we should add to the network's structure
            if random.randint(1, 1000) < connection_add_chance:
                try:
                    source_index = random.randrange(0, bred_networks[-1].GetInputCount() + bred_networks[-1].GetNodeCount())
                    target_index = random.randrange(0, bred_networks[-1].GetNodeCount() + bred_networks[-1].GetOutputCount())
                    bred_networks[-1].AddConnection(source_index, target_index, (random.random()-0.5)*3.9)
                except ValueError:
                    pass

            if random.randint(1, 1000) < node_add_chance:
                try:
                    node_index = random.randrange(0, bred_networks[-1].GetNodeCount() + bred_networks[-1].GetOutputCount())
                    connection_index = random.randrange(0, bred_networks[-1].GetNodeConnectionCount(node_index))
                    bred_networks[-1].AddNode(node_index, connection_index, (random.random()-0.5)*3.9)
                except ValueError:
                    pass

            #decide if we should remove from the network's structure
            if random.randint(1, 1000) < connection_remove_chance:
                try:
                    node_index = random.randrange(0, bred_networks[-1].GetNodeCount() + bred_networks[-1].GetOutputCount())
                    connection_index = random.randrange(0, bred_networks[-1].GetNodeConnectionCount(node_index))
                    bred_networks[-1].RemoveConnection(node_index, connection__index)
                except ValueError:
                    pass

            if random.randint(1, 1000) < node_remove_chance:
                try:
                    node_index = random.randrange(0, bred_networks[-1].GetNodeCount() + bred_networks[-1].GetOutputCount())
                    bred_networks[-1].RemoveNode(node_index)
                except ValueError:
                    pass

            #decide if we should alter the network's properties
            if random.randint(1, 1000) < weight_alter_chance:
                try:
                    target_index = random.randrange(0, bred_networks[-1].GetNodeCount() + bred_networks[-1].GetOutputCount())
                    connection_index = random.randrange(0, bred_networks[-1].GetNodeConnectionCount(target_index))
                    bred_networks[-1].SetConnectionWeight(
                        source_index, 
                        connectionIndex, 
                        bred_networks[-1].GetConnectionWeight(source_index, connectionIndex) + (random.random() - 0.5) * 0.05
                    )
                except ValueError:
                    pass

            if random.randint(1, 1000) < bias_alter_chance:
                try:
                    node_index = random.randrange(0, bred_networks[-1].GetNodeCount())
                    bred_networks[-1].SetNodeBias(
                        node_index, 
                        bred_networks[-1].GetNodeBias(node_index) + (random.random() - 0.5) * 0.05
                    )
                except ValueError:
                    pass

        #store the next generation
        networks = good_networks + bred_networks

    #test the networks on the environment
    scores = []
    for net_i in range(len(networks)):
        total_reward = 0
        observation, reward, done, info = (0, 0, 0, 0), 0, False, {}
        for t in range(episode_length):
            env.render() #render the environment

            observation, reward, done, info = env.step(int(round(networks[net_i].CalculateOutputs(observation)[0]))) #do an action
            total_reward += reward #store the reward for this iteration

            if done: #exit if we've finished the episode
                print(f"Episode for network {net_i}finished after {t+1} timesteps with {total_reward} reward")
                break
        scores.append((net_i, total_reward))
        env.reset()

    env.close()