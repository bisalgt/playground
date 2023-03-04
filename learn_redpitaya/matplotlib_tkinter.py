import tkinter as tk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib as mpl
import weakref

class PlotFrame:
    def __init__(self, parent):
        self.fig = mpl.figure.Figure()
        self.canvas = FigureCanvasTkAgg(self.fig, master=parent)
        self.canvas.get_tk_widget().pack()
        self.ax = self.fig.add_subplot(1, 1, 1)
        self.ax.set_ylim((0, 150))
        self.ax.set_xlim((0, 12))

        self.counter = 0

        timer = self.fig.canvas.new_timer(interval=20)
        timer.add_callback(self._refresh_plot, ())
        timer.start()

    def _refresh_plot(self, arg):
        if True:  # True to plot 1 or more lines, False for no lines at all
            if len(self.ax.lines) > 2:  # Remove the oldest line
                existing_lines = self.ax.get_lines()
                # r = weakref.ref(existing_lines[0])  # weakref to check that the line is being correctly removed
                old_line = existing_lines.pop(0)
                old_line.remove()
                del old_line, existing_lines
                # print('Old line dead? {}'.format(r() is None))  # Prints Old line dead? True

            # Define a line to plot
            x = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
            y = [(v - abs(len(x) - self.counter % (2 * len(x)))) ** 2 for v in x]
            self.counter += 1

            new_line = mpl.lines.Line2D(x, y, linestyle='-', marker='.')
            self.ax.add_line(new_line)
            del new_line, x, y

        self.canvas.draw()

if __name__ == "__main__":
    root = tk.Tk()
    cpp = PlotFrame(root)
    root.mainloop()