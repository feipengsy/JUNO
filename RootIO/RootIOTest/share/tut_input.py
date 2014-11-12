#!/usr/bin/env python
# -*- coding:utf-8 -*-

import Sniper

#create task
task = Sniper.Task("task")
task.asTop()
task.setLogLevel(0)

#I/O related
#create DataRegistritionSvc
import DataRegistritionSvc
task.createSvc("DataRegistritionSvc")

#create RootInputSvc and configure input files
import RootIOSvc
task.createSvc("RootInputSvc/InputSvc")
ri = task.find("InputSvc")
ri.property("InputFile").set(["tut_sample.root"])

#create Data Buffer Manager
import BufferMemMgr
bufMgr = task.createSvc("BufferMemMgr")
bufMgr.property("TimeWindow").set([0, 0]);

#create algorithm
import RootIOTest
task.property("algs").append("RootInputTestAlg/ialg")

task.setEvtMax(10)
task.show()
task.run()



