import sys

def comparer(file1, file2):
    content1 = file1.readlines()
    content2 = file2.readlines()
    if len(content1) != len(content2):
        print("Something went wrong. Length mismatch")
        exit(1)
    output = open("offset_locator.csv", "w")
    # Looks weird, but just doing some CSV formatting
    output.write(str(file1).split("name='")[1].split("'")[0] + ", " +
        str(file2).split("name='")[1].split("'")[0] + "\n")
    for counter1 in range(len(content1)):
        offset1, hexvalue1 = content1[counter1][0:8], content1[counter1][10:49]
        offset2, hexvalue2 = content2[counter1][0:8], content2[counter1][10:49]
        if hexvalue1 == "0000 0000 0000 0000 0000 0000 0000 0000":
            output.write(offset1+" is 0's, \"\"\n")
        elif hexvalue1 == "FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF":
            output.write(offset2+" is F's, \"\"")
        elif hexvalue2 != hexvalue1:
            for counter2 in range(len(content2)):
                offset2, hexvalue2 = content2[counter2][0:8], content2[counter2][10:49]
                if hexvalue1 == hexvalue2:
                    output.write("\"=\"\""+offset1+"\"\"\""+", \"=\"\""+offset2+"\"\"\"\n")
                if counter2 == len(content2)-1 and hexvalue1 != hexvalue2:
                    output.write("\"=\"\""+offset1+"\"\"\""+", No Match\n")


if len(sys.argv) == 3 and sys.argv[1][-3:] != "bin" and sys.argv[2][-3:] != "bin":
    with open(sys.argv[1],"r") as file1, open(sys.argv[2],"r") as file2:
        comparer(file1,file2)
else:
    print("This program will compare two hex dumps and will output matching possible offsets")
    print("Run 'xxd <file> > <file>.hex' on both files first.")
    print("Usage: python offset_diff.py hexfile1 hexfile2")
    sys.exit(1)
