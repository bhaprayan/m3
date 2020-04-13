from graphics import *
import random, time

def main():


    wth = 1000
    hgt = 1000

    win = GraphWin("Random Circles", wth, hgt)


    f = open("./test.txt","r")
    lines = f.readlines() 
    for line in lines:
        val = line.split()
        x = float(val[0])
        y = float(val[1])
        h = float(val[2])

        x *= 4
        y *= 4

        x += wth/2
        y += hgt/2


        r = 0
        g = 0
        b = 0
        if(h < 0.5):
            b = int((h + 0.5) * 255)

        else:
            g = int((h) * 255)


        color = color_rgb(r, g, b)
        
        circle = Circle(Point(x,y), 10)
        circle.setFill(color)
        circle.setWidth(0)
        circle.draw(win)


    win.getMouse() # Pause to view result
    win.close()
        

main()