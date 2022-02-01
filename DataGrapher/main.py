import matplotlib.pyplot as plt
from graph import Graph
from line import Line

if __name__ == "__main__":
    #test line class
    print("Testing Line class")
    l = Line("", color="blue", linestyle="-.", linewidth=2, marker="^", markerfacecolor="red", markersize=18)
    l.add_point(0, 1)
    l.add_points([(1, 2), (2, 4), (3, 3), (4, 2), (5, 5), (6, 2)])

    #test graph class
    print("Testing Graph Class")
    g = Graph(filename="test/example_data.json")
    g.display_graph()
    g.save_graph("test/example_render.png")
    g.add_line(l)
    g.save_graph_data("test/saved_graph.json")

    #test graph static functions
    print("Testing static functions")
    Graph.show_plot_file("test/saved_graph.json")
    Graph.save_plot_file("test/saved_graph.json", "test/example_render_2.png")

