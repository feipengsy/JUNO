#!/usr/bin/env python
# -*- coding:utf-8 -*-

import Sniper

#create task
task = Sniper.Task("task")
task.asTop()
task.setLogLevel(3)

#I/O related
#create DataRegistritionSvc
import DataRegistritionSvc
task.createSvc("DataRegistritionSvc")

#create RootOutputSvc and configure output path and files
import RootIOSvc
task.createSvc("RootOutputSvc/OutputSvc")
ro = task.find("OutputSvc")
ro.property("OutputStreams").set({"/Event/PhyEvent": "tut_sample0.root"})

#create BufferMemMgr
import BufferMemMgr
bufMgr = task.createSvc("BufferMemMgr")
bufMgr.property("TimeWindow").set([-0.01, 0.01]);

#create algorithm
import RootIOTest
task.property("algs").append("RootOutputTestAlg/oalg")

task.setEvtMax(10000)
task.show()
task.run()

