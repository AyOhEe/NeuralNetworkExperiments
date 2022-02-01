import matplotlib.pyplot as plt

class Graph:
    #creates a graph
    def __init__(self, title="Untitled Graph", x_label="X Axis", y_label="Y Axis"):
        self.title = title
        self.lines = []
        self.x_label = x_label
        self.y_label = y_label

    #adds a line to this graph
    def add_line(self, line):
        self.lines.append(line)

    #displays this graph as a window on screen
    def display_graph(self):
        #clear the plot
        plt.clf()

        #calculate our x and y limits
        min_x = min([min(line.x_data) for line in self.lines])
        min_y = min([min(line.y_data) for line in self.lines])
        max_x = max([max(line.x_data) for line in self.lines])
        max_y = max([max(line.y_data) for line in self.lines])
       
        #set up our plot
        plt.title(self.title)
        plt.xlabel(self.x_label)
        plt.ylabel(self.y_label)
        plt.xlim(min_x, max_x)
        plt.ylim(min_y, max_y)

        #plot our lines
        for line in self.lines:
            plt.plot(label.x_data, label.y_data, label=line.label, line.properties)

        #show the plot
        plt.show()
        
        #leave the plot clean when we leave
        plt.clf()
    
    #saves this graph as a .png file
    def save_graph(self, filename):
        #clear the plot
        plt.clf()
        
        #calculate our x and y limits
        min_x = min([min(line.x_data) for line in self.lines])
        min_y = min([min(line.y_data) for line in self.lines])
        max_x = max([max(line.x_data) for line in self.lines])
        max_y = max([max(line.y_data) for line in self.lines])

        #set up our plot
        plt.title(self.title)
        plt.xlabel(self.x_label)
        plt.ylabel(self.y_label)
        plt.xlim(min_x, max_x)
        plt.ylim(min_y, max_y)

        #plot our lines
        for line in self.lines:
            plt.plot(label.x_data, label.y_data, label=line.label, line.properties)
        
        #save the graph
        plt.savefig(filename)

        #leave the plot clean when we leave
        plt.clf()

    #saves this graph's data as a .json file
    def save_graph_data(self):
        #calculate our x and y limits
        min_x = min([min(line.x_data) for line in self.lines])
        min_y = min([min(line.y_data) for line in self.lines])
        max_x = max([max(line.x_data) for line in self.lines])
        max_y = max([max(line.y_data) for line in self.lines])

        #store this graph's data in a dictionary
        graph_data = {
            "Title": self.title, 
            "xlim start": min_x, 
            "xlim end": max_x, 
            "ylim start": min_y, 
            "ylim end": max_y,
            "Labels": [self.x_label, self.y_label]
            "Lines": []
        }

        #add the lines to the dictionary
        for line in self.lines:
            graph_data["Lines"].append(line.as_dict())

        #return the dictionary
        return graph_data