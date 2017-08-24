import os
import sys

#
# (C) Copytight 2017 Marek Bielawski
# addClass
#

newLine = str('\n')

def sexyComment(lines) :
    out = '/*' + newLine
    for line in lines:
        out += ' * ' + line + newLine
    out += ' */' + newLine
    return out

if len( sys.argv ) < 2:
    print( "too few arguments" )
    quit()
paramName = sys.argv[1];
paramArr = paramName.split('::')

hPath = 'Code/' + '/'.join(paramArr) + '.h'
cppPath = 'Code/' + '/'.join(paramArr) + '.cpp'
if os.path.isfile( hPath ) or os.path.isfile(cppPath) :
    print ( "class {} allready eists".format(paramName) )
    quit()
className = paramArr[-1] 
namespaceArr = paramArr[:-1]
namespaceStart = ''
namespaceEnd = newLine
guardName = '_'.join(paramArr).upper() + "_DEF"
for namespace in namespaceArr:
    namespaceStart += 'namespace ' + namespace + ' {' + newLine
    namespaceEnd += '}' + newLine
classStart = 'class ' + className + newLine + '{' + newLine
classEnd = '};' + newLine
comment = sexyComment([
    "(C) Copytight 2017 Marek Bielawski",
    paramName
    ])
include = "#include <" + '/'.join(paramArr) + '.h>' + newLine
headerContent = "#ifndef " + guardName + newLine + "#define " + guardName + newLine
headerContent += comment + namespaceStart + newLine
headerContent += classStart + classEnd + namespaceEnd + "#endif" + newLine
    
cppContent = include + comment + namespaceStart + newLine + namespaceEnd
hFile = open(hPath, "w")
cppFile = open(cppPath, "w")
hFile.write(headerContent)
cppFile.write(cppContent)
hFile.close()
cppFile.close()

