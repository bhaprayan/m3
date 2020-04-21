from graphics import *
import random, time
import statistics


def main():


    wth = 1000
    hgt = 1000

    win = GraphWin("Random Circles", wth, hgt)


    f = open("./test.txt","r")
    lines = f.readlines() 

    h_val = []
    for line in lines: 
        val = line.split()
        h = float(val[2])

        h_val.append(h)

    med_h = statistics.median(h_val)



    polygon = Polygon(Point(1,2), Point(30,40), Point(200,600))
    color = color_rgb(0,0,0)
    polygon.setFill(color)
    polygon.setWidth(0)
    polygon.draw(win)



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
        if(h < med_h):
            b = int((h + 0.5) * 255)
            if(b > 255):
                b = 255
            if(b < 0):
                b = 0

        else:
            g = int((h) * 255)
            if(g > 255):
                g = 255
            if(g < 0):
                g = 0


        color = color_rgb(r, g, b)
        
        circle = Circle(Point(x,y), 10)
        circle.setFill(color)
        circle.setWidth(0)
        circle.draw(win)




    win.getMouse() # Pause to view result
    win.close()
        

main()