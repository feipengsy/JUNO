#!/usr/bin/env python
import sys,os

import argparse
parser = argparse.ArgumentParser(description="generate impl files in src")
parser.add_argument('-n', '--namespace', 
                    default="JM",
                    help='namespace for the code')
parser.add_argument('headers', nargs='+', help="header files")
args = parser.parse_args()
# set the namespace
ns = args.namespace
headers = args.headers

def genSrc( fullName , namespace="JM"):
  # TODO:
  # It's really a limit, user must create one class per file.
  fileName = fullName.split('/')[-1]
  srcFile = open( fullName + '.cc', 'w' )
  src = '#include "Event/%s"\n' % (fileName + '.h')
  src += 'ClassImp(%s::%s);\n' % (namespace, fileName)
  srcFile.write( src )
  srcFile.close()

def check( cwd, headers ):
  srcSufList = [ 'cc', 'CC', 'cpp', 'CPP', 'cxx', 'CXX', 'c++', 'C++', 'c', 'C' ]
  genList = []
  for headerFile in headers:
    fileName = headerFile[:-2]
    obj2doth = fileName + '.obj2doth'
    if os.path.exists( obj2doth ):
      fileList = fileName.split( '/' )
      genName = cwd + '/' + fileList[-1]
      genList.append( genName )
      for suf in srcSufList:
        if os.path.exists(  genName + '.' + suf ):
          genList.remove( genName )
  return genList  


if __name__ == '__main__' :
  genList = check( os.getcwd(), headers )
  for fullName in genList:
    genSrc( fullName , namespace=ns)
