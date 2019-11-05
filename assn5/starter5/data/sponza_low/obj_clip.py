""" Simple throwaway script.
"""
import sys
import os

def main():
    if len(sys.argv) != 3:
        print("Usage: %s <infile> <outfile>")
        exit(-1)

    ifname = sys.argv[1]
    ofname = sys.argv[2]

    fh = open(ifname, "r")
    fw = open(ofname, "w")

    positions = []
    for line in fh:
        if line.strip().startswith("f"):
            indices = [int(x) for x in line.split()[1:]]
            p1y = positions[indices[0] - 1][1]
            p2y = positions[indices[1] - 1][1]
            p3y = positions[indices[2] - 1][1]
            
            trunc = 5.85
            if p1y < trunc and p2y < trunc and p3y < trunc:
              fw.write("f %d %d %d\n" % tuple(indices))
        elif line.strip().startswith("v "):
            pos = [float (x) for x in line.split()[1:]]
            positions.append(tuple(pos))
            fw.write("v %f %f %f\n" % tuple(pos))
        else:
            fw.write("%s" % line)

    fh.close()
    fw.close()

if __name__ == "__main__":
    main()
