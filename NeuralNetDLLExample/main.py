from network import Network

if __name__ == "__main__":
    #test settings
    network_count = 100
    episode_length = 200

    #import openai-gym https://gym.openai.com/docs/
    import gym

    #create the networks
    networks = [Network(b"Genomes/Default.genome", 4, 1, Network.ActivationFunction.Sigmoid) for i in range(network_count)]

    #start the test environment
    env = gym.make('CartPole-v0')
    env.reset()

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
                print(f"Episode for network {net_i} finished after {t+1} timesteps with {total_reward} reward")
                break
        scores.append((net_i, total_reward))
        env.reset()

    #sort the networks based on their scores
    scores = sorted(scores, key=lambda x: x[1], reverse=True)
    print(scores)

    env.close()