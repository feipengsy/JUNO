#!/usr/bin/env python
# -*- coding:utf-8 -*-

import Sniper

task = Sniper.Task("task")
task.asTop()
task.setLogLevel(0)

import DataRegistritionSvc
task.createSvc("DataRegistritionSvc")

import RootIOSvc
task.createSvc("RootOutputSvc/OutputSvc")
ro = task.find("OutputSvc")
ro.property("OutputStreams").set({"/Event/SimEvent": "sample_simevt.root"})

import BufferMemMgr
bufMgr = task.createSvc("BufferMemMgr")
bufMgr.property("TimeWindow").set([-0.01, 0.01]);

import RootIOTest
task.property("algs").append("RootSimEventOutputTestAlg/oalg")

task.setEvtMax(10000)
task.run()

