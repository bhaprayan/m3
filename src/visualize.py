from graphics import *
import random, time


def draw_polygon(win, edges, h):

    if len(edges) > 0:
        r = 0
        g = 0
        b = 0

        if(h < 0.5):
            b = int((h + 0.5) * 255)

        else:
            g = int((h) * 255)

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
    edges = []
    h = 0
    for line in lines:
        val = line.split()
        
        if(val[0] == "v"): #vcenter
            draw_polygon(win, edges, h)

            edges = []
            h = float(val[3])


        elif(val[0] == "e"): #edge
            edges.append( Point(float(val[1]),float(val[2])) )

    draw_polygon(win, edges, h)

    win.getMouse() # Pause to view result
    win.close()
        

main()