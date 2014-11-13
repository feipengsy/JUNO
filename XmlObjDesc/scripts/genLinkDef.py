import xparser,os

class genLinkDef:

  def __init__( self, godRoot ):
    self.godRoot = godRoot

  def getExternClasses( self, godClass ):
    classList = []
    for att in godClass['attribute']:
      if att['attrs']['extern'] == 'TRUE':
        classList.append( att['attrs']['type'] )
    return classList

  def genExternTemplates( self, godClass ):
    templateList = []
    templateDict = {}
    for att in godClass['template']:
      if att['attrs']['extern'] == 'TRUE':
        templateList.append( att['attrs']['type'] )
        templateDict[att['attrs']['type']] = self.findTempParameter( att['typename'] )
      attTypeList = att['typename']
      while True:
        if attTypeList[0].has_key( 'typename' ):
          if attTypeList[0]['attrs']['extern'] == 'TRUE':
            templateType = attTypeList[0]['attrs']['type']
            templateList.append( templateType )
            if not templateDict.has_key( templateType ):
              templateDict[templateType] = self.findTempParameter( attTypeList[0]['typename'] )
          attTypeList = attTypeList[0]['typename'] + attTypeList[1:]
        else:
          if attTypeList[0]['attrs']['extern'] == 'TRUE':
            templateType = attTypeList[0]['attrs']['type']
            templateList.append( templateType )
            templateDict[templateType] = ''
          attTypeList = attTypeList[1:]
        if not attTypeList:
          break
    return ( templateList, templateDict )

  def findTempParameter( self, attTypeList ):
    tempParameter = '<'
    depth = 0
    depthLength = { 0 : len( attTypeList ) }
    while True:
      if attTypeList[0].has_key( 'typename' ):
        tempParameter = tempParameter + attTypeList[0]['attrs']['type'] + '<'
        depthLength[depth] = depthLength[depth] - 1
        depth = depth + 1
        depthLength[depth] = len( attTypeList[0]['typename'] )
        attTypeList = attTypeList[0]['typename'] + attTypeList[1:]
      else:
        tempParameter = tempParameter + attTypeList[0]['attrs']['type'] + ','
        depthLength[depth] = depthLength[depth] - 1
        attTypeList = attTypeList[1:]
      if depthLength[depth] == 0:
        if tempParameter.endswith( ',' ):
          tempParameter = tempParameter[:-1]
        tempParameter = tempParameter + '>,'
        depth = depth - 1
      if not attTypeList:
        if tempParameter.endswith( ',' ):
          tempParameter = tempParameter[:-1]
        while depth >= 0:
          tempParameter = tempParameter + '>'
          depth = depth - 1
        break
    return tempParameter

  def parseXld( self ):
    x = xparser.xparser()
    xld = x.parseSource( self.godRoot + 'xml_files/xld.xml' )
    return xld

  def findClass( self, classname, xldList ):
    i = 1
    for attrDict in xldList:
      ct = ''
      if attrDict['attrs'].has_key( 'ns' ):
        ct = attrDict['attrs']['ns'] + '::' + attrDict['attrs']['name']
      else:
        ct = attrDict['attrs']['name']
      if ct == classname:
        return { 'OK' : True, 'Value' : attrDict }
      if attrDict.has_key('attr'):
        return { 'OK' : False, 'Value' : attrDict['attr'] + xldList[i:] }
      i = i + 1
    return { 'OK' : False, 'Value' : '' }

  def getClassTree( self, xld, classList ):
    classTree = {}
    for classname in classList:
      result = { 'OK' : False, 'Value' : xld }
      while True:
        result = self.findClass( classname, result['Value'] )
        if not result['Value']:
          print 'Cannot find extern class %s, please report to us' % classname #FIXME think what should we do here
          classTree[classname] = ''
          break
        if result['OK']:
          classTree[classname] = result['Value']
          break
    return classTree

  def getLinkDefTemps( self, tempTuple, xldList ):
    finalDict = {}
    for temp in tempTuple[0]:
      tempAttrDict = {}
      for tempDict in xldList:
        if tempDict['attrs'].has_key('ns'):
          tempName = tempDict['attrs']['ns'] + '::' + tempDict['attrs']['name']
        else:
          tempName = tempDict['attrs']['name']
        if tempName == temp:
          tempAttrDict = { 'nestedclass' : False, 'operators' : [] , 'reliedTemplate' : [], 'off' : 'NO', 'type' : {} }
          if tempDict['attrs']['type']:
            ftypeList = tempDict['attrs']['type'].split('/')
            rtypeList = self.getTypeName( temp, tempTuple )
            for i in range(len(ftypeList)):
              tempAttrDict['type'][ftypeList[i]] = rtypeList[i]
          if tempDict['attrs']['nestedclass'] == 'TRUE':
            tempAttrDict['nestedclass'] = True
          tempAttrDict['off'] = tempDict['attrs']['off']
          if tempDict.has_key('operators'):
            for opDict in tempDict['operators']:
              tempAttrDict['operators'].append( opDict['attrs']['name'] )
          if tempDict.has_key('template'):
            for tempTempDict in tempDict['template']:
              reliedTemplateDict = { 'nestedclass' : False, 'name' : '' }
              if tempTempDict['attrs'].has_key('ns'):
                rtn = tempTempDict['attrs']['ns'] + '::' + tempTempDict['attrs']['name']
              else:
                rtn = tempTempDict['attrs']['name']
              for k,v in tempAttrDict['type'].items():
                if k in rtn:
                  rtn = rtn.replace(k, v)
              reliedTemplateDict['name'] = rtn
              if tempTempDict['attrs']['nestedclass'] == 'TRUE':
                reliedTemplateDict['nestedclass'] = True
              tempAttrDict['reliedTemplate'].append( reliedTemplateDict )
      if not finalDict.has_key(temp):
        finalDict[temp] = tempAttrDict
    return finalDict

  def getTypeName( self, temp, tempTuple ):
    typepa = tempTuple[1][temp]
    typeList = typepa[1:-1].split(',')
    return [tn.strip() for tn in typeList]

  def getLinkDefClasses( self, classTree ):
    finalList = []
    for k,v in classTree.items():
      if v:
        finalList.append( k + '#class' )
        if v.has_key( 'attr' ):
          vList = v['attr']
          while True:
            ct = ''
            if vList[0]['attrs'].has_key( 'ns' ):
              ct = vList[0]['attrs']['ns'] + '::' + vList[0]['attrs']['name'] + '#' + vList[0]['attrs']['type']
            else:
              ct = vList[0]['attrs']['name'] + '#' + vList[0]['attrs']['type']
            finalList.append( ct )
            if vList[0].has_key( 'attr' ):
              vList = vList[0]['attr'] + vList[1:]
            else:
              vList = vList[1:]
              if not vList:
                break
    finalList = finalList[::-1]
    finalList1 = []
    for ele in finalList:
      if ele not in finalList1:
        finalList1.append( ele )
    return finalList1

  def doit( self, package, godClasses, ns, outputDir ):
    for godClass in godClasses:
      s = ''
      s = s + '#ifdef __CINT__\n\n\n'
      classList = []
      tempTuple = ([],{})
      if godClass.has_key( 'attribute' ):
        classList = self.getExternClasses( godClass )
      if godClass.has_key( 'template' ):
        tempTuple = self.genExternTemplates( godClass )
      finalClassList = []
      if classList:
        xldList = self.parseXld()['attr']
        classTree = self.getClassTree( xldList, classList )
        finalClassList = self.getLinkDefClasses( classTree )
      if tempTuple[0]:
        xldList = self.parseXld()['template']
        tempDict = self.getLinkDefTemps( tempTuple, xldList )
        for temp in tempTuple[0]:
          if not tempDict.has_key(temp):
            s = s + '#pragma link off all_function ' + temp + tempTuple[1][temp] + ';\n'
            s = s + '#pragma link C++ class ' + temp + tempTuple[1][temp] + '+;\n'
          else:
            if not tempDict[temp]:
              temp1 = temp
              if not tempTuple[1][temp]:
                temp1 = temp.strip('*').strip('&')
              s = s + '#pragma link off all_function ' + temp1 + tempTuple[1][temp] + ';\n'
              s = s + '#pragma link C++ class ' + temp1 + tempTuple[1][temp] + '+;\n'
            else:
              if tempDict[temp]['off'] != 'ALL':
                if tempDict[temp]['reliedTemplate']:
                  for tempTempDict in tempDict[temp]['reliedTemplate']:
                    s = s + '#pragma link C++ class ' + tempTempDict['name'] + '+;\n'
                    if tempTempDict['nestedclass']:
                      s = s + '#pragma link C++ class ' + tempTempDict['name'] + '::*+;\n'
                if tempDict[temp]['off'] == 'FUNCTION':
                  s = s + '#pragma link off all_function ' + temp + tempTuple[1][temp] + ';\n'
                s = s + '#pragma link C++ class ' + temp + tempTuple[1][temp] + '+;\n'
                if tempDict[temp]['nestedclass']:
                  s = s + '#pragma link C++ class ' + temp + tempTuple[1][temp] + '::*+;\n'
                if tempDict[temp]['operators']:
                  for operators in tempDict[temp]['operators']:
                    s = s + '#pragma link C++ operators ' + temp + tempTuple[1][temp] + '::' + operators + ';\n'
      for ct in finalClassList:
        print finalClassList
        if ct.endswith( '#class' ):
          if ct[:-6] in tempTuple[0]:
            s = s + '#pragma link off all_function ' + ct[:-6] + tempTuple[1][ct[:-6]] + ';\n'
          else:
            s = s + '#pragma link off all_function ' + ct[:-6] + ';\n'
            s = s + '#pragma link C++ function;\n'
      for ct in finalClassList:
        ctList = ct.split('#')
        if ctList[1] == 'class':
          if ctList[0] in tempTuple[0]:
            s = s + '#pragma link C++ class %s%s+;\n' %  ( ctList[0], tempTuple[1][ctList[0]] )
          else:
            s = s + '#pragma link C++ class %s+;\n' %  ctList[0]
        else:
          s = s + '#pragma link C++ %s %s;\n' %  ( ctList[1], ctList[0] )
      s = s + '#pragma link C++ class %s::%s+;\n' % ( ns, godClass['attrs']['name'] )
      s = s + '\n\n#endif'
      fileName = '%sLinkDef.h' % godClass['attrs']['name']
      ldFile = open( outputDir+os.sep+fileName,'w' )
      ldFile.write( s )
      ldFile.close()

