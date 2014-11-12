#!/usr/bin/env python
# -*- coding:utf-8 -*-
# author: lintao

import Sniper

task = Sniper.Task("task")
task.asTop()
task.setLogLevel(0)

import DataRegistritionSvc
task.createSvc("DataRegistritionSvc")

import RootIOSvc
inputsvc = task.createSvc("RootInputSvc/InputSvc")
inputsvc.property("InputFile").set(["sample_calib.root"])

import BufferMemMgr
bufMgr = task.createSvc("BufferMemMgr")
#bufMgr.property("TimeWindow").set([-0.01, 0.01]);

import RootIOTest
task.property("algs").append("RootCalibEventInputTestAlg/ialg")

task.setEvtMax(-1)
task.run()
