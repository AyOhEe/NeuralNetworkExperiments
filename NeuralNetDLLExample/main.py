from network import Network

if __name__ == "__main__":
    #import openai-gym https://gym.openai.com/docs/
    import gym

    #create the network
    net = Network(b"Genomes/Default.genome", 4, 2, Network.ActivationFunction.Sigmoid, verbose=True)

    #start the test environment
    env = gym.make('CartPole-v0')
    env.reset()

    #test the network on the environment
    total_reward = 0
    observation, reward, done, info = (0, 0, 0, 0), 0, False, {}
    for t in range(200):
        env.render() #render the environment

        observation, reward, done, info = env.step(int(net.CalculateOutputs(observation)[0])) #do an action
        total_reward += reward #store the reward for this iteration

        if done: #exit if we've finished the episode
            print("Episode finished after {} timesteps with {} reward".format(t+1, total_reward))
            break

    env.close()