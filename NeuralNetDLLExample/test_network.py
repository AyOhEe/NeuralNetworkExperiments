from network import Network
import gym

if __name__ == "__main__":
    net = Network(f"Networks/{input('Select Network: ')}", 2, 2, Network.ActivationFunction.Sigmoid)

    #start the test environment
    env = gym.make('MountainCarContinuous-v0')
    env.reset()

    #test the networks on the environment
    total_reward = 0
    observation, reward, done, info = (0, 0, 0, 0), 0, False, {}
    for t in range(1000):
        env.render()
        act = net.CalculateOutputs(observation)
        observation, reward, done, info = env.step(act) #do an action
        total_reward += reward #store the reward for this iteration
