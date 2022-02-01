import matplotlib.pyplot as plt
import json

#reads in data from a json file and graphs it using matplotlib
def plot_data(data, plot):
    #if there is a title for the graph, use it
    if "Title" in data:
        plt.title(data["Title"])

    #if there are labels, use them
    if "Labels" in data:
        if len(data["Labels"]) == 2:
            plt.xlabel(data["Labels"][0])
            plt.xlabel(data["Labels"][1])

    #for each of the lines in the data, plot the line
    if "Lines" in data:
        for line in data["Lines"]:
            # plot the points
            plt.plot(
                data["Lines"]["DataY"], 
                data["Lines"]["DataY"], 
                color='green', 
                linestyle='dashed', 
                linewidth = 3, 
                marker='o', 
                markerfacecolor='blue', 
                markersize=12)

if __name__ == "__main__":
    data = json.loads(open(input("File path: ")).read())
    plot_data(data)
    plt.show()