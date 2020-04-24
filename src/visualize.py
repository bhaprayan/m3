from graphics import *
import random, time


def draw_polygon(win, edges, h, median):

    if len(edges) > 0:
        r = 0
        g = 0
        b = 0

        if(h < median):
            b = int((h/2 + 0.5) * 255)

        else:
            g = int((h/2 + 0.5) * 255)

        polygon = Polygon(edges)
        color = color_rgb(r,g,b)
        polygon.setFill(color)
        polygon.setWidth(0)
        polygon.draw(win)


def main():


    wth = 1000
    hgt = 1000

    win = GraphWin("Random Circles", wth, hgt)

    f = open("./test.txt","r")
    lines = f.readlines() 
    median = 0.5
    for i, line in enumerate(lines):

        val = line.split()

        if i == 0:
            median = float(val[0])

        else:
        
            h = float(val.pop(0))

            edges = []
            nEdges = int(len(val)/2)
            for i in range(nEdges):
                edges.append( Point(float(val[2*i]),float(val[2*i+1])) )

            draw_polygon(win, edges, h, median)

    
    win.getMouse() # Pause to view result
    win.close()
        

main()