

def main():
    import csv
    # import sys

    # for pos, arg in enumerate(sys.argv):
    #     print('Argument %d: %s' % (pos, arg))

    import csv

    x0 = 0
    xPrev = 0
    xN = 0
    XMax = 0
    deltaLiftOff = 1
    liftOf = False
    apogeeThreshhold = 5
    xApogee = 0

    with open('log.csv', newline='') as csvfile:
        spamreader = csv.reader(csvfile, delimiter=',', quotechar='|')


        for row in spamreader:
            x = int (row[0]) 
            if x >= x0 + deltaLiftOff and not liftOf:
                print("Lift Off " + str(x))
                liftOf = True

            if x >= XMax:
                XMax = x
            else:
                if apogeeThreshhold == 0 and not xApogee:
                    print("Apogee at: " + str(x))
                    xApogee = x
                else:
                    if x != xPrev:
                        apogeeThreshhold -= 1
            
            xPrev = x

if __name__ == '__main__':
    main()