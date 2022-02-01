import matplotlib.pyplot as plt
import json

#reads in data from a json file and graphs it using matplotlib
def plot_data(data):
    #if there is a title for the graph, use it
    if "Title" in data:
        plt.title(data["Title"])

    #if there are labels, use them
    if "Labels" in data:
        if len(data["Labels"]) == 2:
            plt.xlabel(data["Labels"][0])
            plt.ylabel(data["Labels"][1])

    #if there is valid limit data, use it
    if "xlim start" in data and "xlim end" in data and "ylim start" in data and "ylim end" in data:
        plt.xlim(data["xlim start"], data["xlim end"])
        plt.ylim(data["ylim start"], data["ylim end"])

    #for each of the lines in the data, plot the line
    if "Lines" in data:
        for line in data["Lines"]:
            # plot the points
            plt.plot(
                line["DataX"], 
                line["DataY"], 
                label=line["Label"],
                **line["kwargs"])

if __name__ == "__main__":
    data = json.loads(open(input("File path: ")).read())
    plot_data(data)
    plt.show()