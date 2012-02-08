#!/usr/bin/python

import argparse
import os
import glob


#--- arguments, just to read every need parameter
parser = argparse.ArgumentParser(description="Give me a directory where I should look for files.")

parser.add_argument('-d','--destination', default=".", dest='destination', action='store',
                    help="Destination where to move the processed files, default is '.'")
parser.add_argument('-r','--remove', default=False, dest='remove_rar', action='store_true',
                    help="Remove rar after processing")
parser.add_argument('path', nargs='?', default=".",
                    help="The source directory where the rar files are located")

parse_res = parser.parse_args()

src_path    = parse_res.path
remove_rar  = parse_res.remove_rar
mv_destionation = parse_res.destination
#-----

#--- processing source directory
listing = glob.glob( os.path.join(src_path, '*.rar') )
print("found ", len(listing), " elements in path: ", src_path)

for infile in listing:
    print("current file is: ", infile)
 
