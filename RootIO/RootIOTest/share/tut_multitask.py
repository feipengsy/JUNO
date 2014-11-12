#!/usr/bin/env python
# -*- coding:utf-8 -*-

import Sniper

#------------configure top-task------------
#create top-task
top_task = Sniper.Task("top_task")
top_task.asTop()
top_task.setLogLevel(0)

#I/O related
#create DataRegistritionSvc
import DataRegistritionSvc
top_task.createSvc("DataRegistritionSvc")

#create RootOutputSvc and configure output path and files
import RootIOSvc
ro = top_task.createSvc("RootOutputSvc/OutputSvc")
ro.property("OutputStreams").set({"/Event/PhyEvent": "tut_multi_sample.root"})

#create BufferMemMgr
import BufferMemMgr
bufMgr = top_task.createSvc("BufferMemMgr")
bufMgr.property("TimeWindow").set([-0.1, 0.1])

#create algorithm
import RootIOTest
top_task.property("algs").append("RootMultiTaskTestAlg/malg")

#------------configure top-task end------------

#------------configure sub-tasks------------
for i in range(3):
  sub_task = top_task.createTask("Task/subTask"+str(i))
  sub_task.setLogLevel(0)
  sub_task.createSvc("DataRegistritionSvc")
  ri = sub_task.createSvc("RootInputSvc/InputSvc")
  ri.property("InputFile").set(["tut_sample" + str(i) + ".root"])
  bufMgr = sub_task.createSvc("BufferMemMgr")
  bufMgr.property("TimeWindow").set([0,0])
#------------configure sub-tasks end------------

top_task.setEvtMax(10000)
top_task.show()
top_task.run()
