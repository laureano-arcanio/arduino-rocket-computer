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

    measures = 2
    xApogee = 0

    lastAltitude = 0

    with open('log_eeprom.csv', newline='') as csvfile:
        spamreader = csv.reader(csvfile, delimiter='\t', quotechar='|')


        for row in spamreader:
            currAltitude = int (row[2]) 
            if currAltitude > lastAltitude:
                measures = 2
            else:
                if measures == 0:
                    print("Apogee at: " + str(currAltitude))
                    print(row)
                else:
                    if lastAltitude != currAltitude:
                         measures -= 1
            lastAltitude = currAltitude


#   // //detect Apogee
#   if (status == STATUS_LIFTOFF)
#   {
#     if (currAltitude >= lastAltitude)
#     {
#       lastAltitude = currAltitude;
#       measures = 2;
#     }
#     else
#     {
#       if (measures == 0)
#       {
#         status = STATUS_APOGEE;
#       }
#       else
#       {
#         if (prevAltitude != currAltitude)
#         {
#           measures -= 1;
#         }
#       }
#     }
#   }

if __name__ == '__main__':
    main()