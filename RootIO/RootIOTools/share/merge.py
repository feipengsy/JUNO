#!/usr/bin/env python
# -*- coding:utf-8 -*-

import sys

if __name__ == '__main__':
  if len(sys.argv) < 3:
      print "too few arguments"
      sys.exit(0)

  import Sniper
  task = Sniper.Task("task")
  task.asTop()
  task.setLogLevel(0)

  import RootIOTools
  import glob

  mfalg = task.createAlg("MergeRootFilesAlg/mfalg")
  fileList = []
  for fn in sys.argv[1:-1]:
    fileList += glob.glob(fn)
  fileList = list(set(fileList))
  mfalg.property("InputFiles").set(fileList)
  mfalg.property("OutputFile").set(sys.argv[-1])

  task.setEvtMax(-1)
  task.show()
  task.run()
