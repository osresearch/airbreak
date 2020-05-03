import sys

def comparer(file1, file2):
    content1 = file1.readlines()
    content2 = file2.readlines()
    if len(content1) != len(content2):
        print("Something went wrong. Length mismatch")
        exit(1)
    output = open("offset_differences.csv", "w")
    # Looks weird, but just doing some CSV formatting
    output.write("offset, " + str(file1).split("name='")[1].split("'")[0] + ", " +
        str(file2).split("name='")[1].split("'")[0] + "\n")
    for counter in range(len(content1)):
        offset1, hexvalue1 = content1[counter].split(":")[0], content1[counter].split(" ")[1]
        offset2, hexvalue2 = content2[counter].split(":")[0], content2[counter].split(" ")[1]
        if offset1 != offset2:
            print("Offset mismatch. Please run 'xxd -c 1 <file> > <file>.hex on both files again")
            exit(1)
        if hexvalue1 != hexvalue2:
            output.write(""+offset1+", "+hexvalue1+", "+hexvalue2+"\n")


if len(sys.argv) == 3 and sys.argv[1][-3:] != "bin" and sys.argv[2][-3:] != "bin":

    with open(sys.argv[1],"r") as file1, open(sys.argv[2],"r") as file2:
        comparer(file1,file2)
else:
    print("This program will compare two hex dumps and will output non-matching offsets")
    print("Run 'xxd -c 1 <file> > <file>.hex' on both files first.")
    print("Usage: python offset_diff.py hexfile1 hexfile2")
    sys.exit(1)
