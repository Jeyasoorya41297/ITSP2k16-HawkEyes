import matplotlib.pyplot as plt
def plotFn(fileName):
    x = []
    y = []
    z = []
    t = []
    f = open(fileName,'r')
    data = f.readlines()
    for line in data:
        lineData = line.split()
        x.append(lineData[0])
        y.append(lineData[1])
        z.append(lineData[2])
        t.append(lineData[3])
    plt.plot(t,x,'bo')
    plt.plot(t,y,'ro')
    plt.plot(t,z,'go')
    plt.show()

if __name__=="__main__":
    plotFn("../data/pts.txt")
