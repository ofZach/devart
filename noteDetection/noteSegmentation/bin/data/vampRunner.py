
import subprocess
import sys
from subprocess import call

# please use ' ' around the two args for files that have spaces in their names. 

print "running vamp app"
command = "../../../data/../../../../utils/vampCommandLine/vampTestDebug -s mtg-melodia:melodia:melody %s -o %s " % (sys.argv[1], sys.argv[2])
#print command
call( command, shell=True)  
print "done running vamp app"