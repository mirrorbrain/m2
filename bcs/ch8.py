inputfile = open('ch8')
output = ""
for line in inputfile:
    pixel = line.split('+')
    for couple in pixel:
        n = couple.split('x')
        if n[0] == '0':
            for i in range(0, int(n[1])):
                output += " "
        else:
            for i in range(0, int(n[1])):
                output += "."
    output += "\n"
outputfile = open("output_ch8", 'w')
print (output, file = outputfile)
