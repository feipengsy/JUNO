import time, os, sys, string
import genSrcUtils, gparser, genLinkDef

#================================================================================
class genClasses(genSrcUtils.genSrcUtils):
#--------------------------------------------------------------------------------
  def __init__(self, godRoot):
    genSrcUtils.genSrcUtils.__init__(self)
    self.godRoot = godRoot
    self.plurialExceptions = {'Vertex':'Vertices'}
    self.bitfieldEnums = {'public':'', 'protected':'', 'private':''}
    self.gKeyedContainerTypedef = 0
    self.gContainedObjectTypedef = 0
    self.genOStream = 0
    self.genFillStream = 0
    self.isEventClass = 0
#--------------------------------------------------------------------------------
  def reset(self, package, godClass):
    genSrcUtils.genSrcUtils.reset(self,godClass)
    self.excludes    = list(package.excludes)
    self.include     = list(package.include)
    self.verbatimLHCb = []
    self.stdIncludes = list(package.stdIncludes)
    self.forwardDeclLHCb = list(package.forwardDeclLHCb)
    self.forwardDeclGlob = package.forwardDeclGlob
    self.forwardIncl = list(package.forwardIncl)
    self.bitfieldEnums = {'public':'', 'protected':'', 'private':''}
    self.gKeyedContainerTypedef = 0
    self.gContainedObjectTypedef = 0
    self.genOStream = 0
    self.genFillStream = 0
    if godClass.has_key('id') : self.isEventClass = 1
    else                      : self.isEventClass = 0
#--------------------------------------------------------------------------------
  def genClassnamePlurial(self, name):
    for singular in self.plurialExceptions.keys():
      pos = name.find(singular)
      if pos != -1 and pos+len(singular) == len(name):
        return name[:pos] + self.plurialExceptions[singular]
    return name + 's'
#--------------------------------------------------------------------------------
  def parseClassImport(self, dict):
    self.parseImport(dict, self.include, self.stdIncludes, self.forwardDeclLHCb, self.forwardDeclGlob, self.forwardIncl)
#--------------------------------------------------------------------------------
  def genEDMBook(self, godClass, scopeName=''):
    s = ''
    classAtt = godClass['attrs']
    if not classAtt.has_key('path') or not classAtt.has_key('priority'):
      return s
    if not godClass.has_key('SmartRelation'):
      return s
    s += 'JUNO_BOOK_EDM(' + scopeName + ', '
    for sr in godClass['SmartRelation']:
      s += sr['attrs']['type'] + '&'
    s += ', ' + classAtt['priority'] + ', ' + classAtt['path'] + ');\n' 
    return s
#--------------------------------------------------------------------------------
  def genClassVersion(self, godClass):
    s = ''
    classAtt = godClass['attrs']
    if classAtt.has_key('version'):
      s += '\n// Class Version definition\n'
      s += 'static const unsigned int Version_%s = %s;\n' % (classAtt['name'], classAtt['version'])
    return s
#--------------------------------------------------------------------------------
  def genInheritance(self, godClass):
    s = ''
    if godClass.has_key('base'):
      s += ': '
      for base in godClass['base']:
        if (len(': ') != len(s)) :
          s += ', '
        baseAtt = base['attrs']
        if baseAtt['virtual'] == 'TRUE': s += 'virtual '
        s += '%s %s' % ( baseAtt['access'].lower(), baseAtt['name'] )
    return s
#--------------------------------------------------------------------------------
  def genConstructor(self, godClass, const, scopeName=''):
    s = ''
    indent = 0
    if (scopeName and
        ((not const.has_key('code')) or
         (not const['code'][0].has_key('cont')) or
         (not const['code'][0]['cont'].strip()))): return s                         # if outside class body and no implementation
    constAtt = const['attrs']                                                    # return immediately
    if ( not scopeName ) :
      s += self.comment(constAtt['desc'])   # feature bv@bnl.gov
      if constAtt['explicit'] == 'TRUE' : s += 'explicit '
      indent += 2
    else :
      s += 'inline '
      scopeName += '::'
      indent += len(scopeName) + 7
    s += scopeName + godClass['attrs']['name'] + '('
    indent += len(godClass['attrs']['name'])
    pList = []
    if constAtt.has_key('argList') : pList = self.tools.genParamsFromStrg(constAtt['argList'])
    if const.has_key('arg') :        pList = self.tools.genParamsFromElem(const['arg'])
    pIndent = 0
    if len(pList) :
      if scopeName : s += pList[0].split('=')[0]                                # in implementation strip off default arguments
      else         : s += pList[0]
      pIndent = len(pList[0])
      if len(pList[1:]) :
        for p in pList[1:]:
          if scopeName : s += ',\n%s %s' % (indent*' ', p.split('=')[0])        # in implementation strip off default arguments
          else : s += ',\n%s %s' % (indent*' ', p)
          pIndent = max(pIndent,len(p))
    s += ')'
    indent += pIndent+1
    if ( not scopeName ) :                                                      
      if (constAtt.has_key('initList') and not const.has_key('code')) or \
           (const.has_key('code') and \
             ((not const['code'][0].has_key('cont')) or \
              (not const['code'][0]['cont'].strip()))):
        if constAtt.has_key('initList') :
          initList = constAtt['initList'].split(',')
          s += ' : %s' % initList[0]                                             # print initialisation list if there is one
          indent += 3
          if len(initList) > 1 :
            for init in initList[1:]:
              s += ',\n%s%s' % (indent*' ', init)
        s += ' {}\n\n'                                                           # and the empty implementation
      else : s += ';\n\n'                                                        # implementation must be in cpp file
    else :                                                                       # we are outside the class body 
      if constAtt.has_key('initList') :
        initList = constAtt['initList'].split(',')
        s += ' : %s' % initList[0]                                               # print initlist if available
        indent += 3
        if len(initList) > 1:
          for init in initList[1:]:
            s += ',\n%s%s' % (indent*' ', init)
      s += ' \n{\n%s\n}\n\n' % const['code'][0]['cont']                          # print code
    return s
#--------------------------------------------------------------------------------
  def genConstructors(self,godClass,clname=''):
    s = ''
    cname = godClass['attrs']['name']
    hasDefaultConstructor = 0
    hasCopyConstructor = 0
    if godClass.has_key('constructor'):                                         # are there any constrs defined
      for const in godClass['constructor']:
        if (not const['attrs'].has_key('argList')) and (not const.has_key('arg')):
          hasDefaultConstructor = 1
        s += self.genConstructor(godClass,const,clname)
    if not (hasDefaultConstructor or clname):                                   # no constructors defined lets
      s += '  /// Default Constructor\n'                                        # generate a default ctr
      s2 = '  %s()' % cname
      indent = ' ' * (len(s2) + 3)
      s += s2
      if godClass.has_key('attribute') :                                        # if there are attributes
        for att in godClass['attribute'] :                                      # loop over them
          attAtt = att['attrs']
          if ( s[-1] != ',' ) : s += ' : '                                     # this is the first item
          else : s += '\n' + indent
          s += 'm_%s' % attAtt['name'] 
          if attAtt.has_key('init')                        : s += '(%s),' % attAtt['init'] 
          elif self.tools.isIntegerT(attAtt['type']) or \
               self.tools.isBitfieldT(attAtt['type']) or \
               attAtt['type'].strip()[-1] == '*'           : s += '(0),'
          elif self.tools.isFloatingPointT(attAtt['type']) : s += '(0.0),'
          else                                             : s += '(),'
        if s[-1] == ',' : s = s[:-1]                                             # strip off the last ','
      s += ' {}\n\n'
    if godClass.has_key('copyconstructor'):
      if not clname :
        s += '  /// Copy Constructor\n'
        s += '  %s(const %s & rh);\n\n' % ( cname, cname )
      else:
        s += 'inline %s::%s(const %s & rh) : \n' % (clname, cname, clname)
        if godClass.has_key('base'):
          for b in godClass['base']:
            bname = b['attrs']['name']
            if bname.find('KeyedObject') != -1 : s += '   %s(),\n' % bname
            else                               : s += '   %s(rh),\n' % bname
        if godClass.has_key('attribute'):
          for a in godClass['attribute']:
            if a['attrs']['storage'] == 'TRUE':
              aname = a['attrs']['name']
              s += '   m_%s( rh.m_%s ),\n' % ( aname , aname )
        if godClass.has_key('relation') :
          for r in godClass['relation']:
            rname = r['attrs']['name']
            s += '   m_%s( rh.m_%s ),\n' % ( rname, rname )
        if godClass.has_key('SmartRelation'):
          for sr in godClass['SmartRelation']:
            srname = sr['attrs']['name']
            s += '   m_%s( rh.m_%s ),\n' % ( srname, srname )
        s = s[:-2] + '\n   {}\n\n'
    if godClass.has_key('assignmentoperator'):
      if not clname:
        s += '   /// Assignment operator\n'
        s += '   %s & operator=(const %s & rh);\n\n' % (cname, cname)
      else:
        s += 'inline %s & %s::operator=(const %s & rh) {\n' % (clname, clname, clname)
        s += '  if ( this != &rh ) {\n'
        maxlen = 0;
        mlist = []
        if godClass.has_key('attribute'):
          for a in godClass['attribute']:
            if a['attrs']['storage'] == 'TRUE':
              aname = 'm_'+a['attrs']['name']
              maxlen = max(maxlen,len(aname))
              mlist.append(aname)
        if godClass.has_key('relation'):
          for r in godClass['relation']:
            rname = 'm_'+r['attrs']['name']
            maxlen = max(maxlen,len(rname))
            mlist.append(rname)
        if godClass.has_key('SmartRelation'):
          for sr in godClass['SmartRelation']:
            srname = 'm_'+sr['attrs']['name']
            maxlen = max(maxlen,len(srname))
            mlist.append(srname)
        for m in mlist:
          s += '    %s = rh.%s;\n' % ( m.ljust(maxlen), m )
        s += '  }\n  return *this;\n}\n\n'
    return s[:-1]
#--------------------------------------------------------------------------------
  def genDestructor(self,godClass,dest,scopeName=''):
    s = ''
    if (scopeName and not dest.has_key('code')) : return s
    if ( not scopeName ) :
      s += self.comment(dest['attrs']['desc'])   # feature bv@bnl.gov
      if dest['attrs']['virtual'] == 'TRUE' : s += 'virtual '
    else :
      s += 'inline '
      scopeName += '::'
    s += '%s~%s()' % (scopeName, godClass['attrs']['name'])
    if ( not scopeName ) :
      s += ';\n\n'
    else :
      src = dest['code'][0].get('cont')
      if src == None : src = ''
      s += ' \n{\n%s\n}\n\n' % src
    return s
#--------------------------------------------------------------------------------
  def genDestructors(self,godClass,clname=''):
    s = ''
    if godClass.has_key('destructor'):                                          # there is a destructor defined
      dest = godClass['destructor'][0]
      s += self.genDestructor(godClass, dest,clname)
    elif not clname:                                                             # no destructor defined let's
      virt = 'virtual'
      if godClass['attrs']['virtual'] == 'FALSE' : virt = ''
      s += '  /// Default Destructor\n'
      s += '  %s ~%s() {}\n\n' % (virt, godClass['attrs']['name'])
    return s[:-1]
#--------------------------------------------------------------------------------
  def genGetSetAttMethod(self,att,what,scopeName=''):
    desc = {'get':'Retrieve ', 'get_c':'Retrieve const ', 'set':'Update '}
    s = ''
    if ( what == 'get' and self.tools.isFundamentalT(att['type'])) : return s     # for attributes with fundamental types we 
    if ( not scopeName ) :
      s += '  /// %s \n  ' % desc[what]    # ... only generate the const getter 
      s += self.comment(att['desc'])   # feature
    else : 
      s += 'inline '
      scopeName += '::'
    metName = self.tools.lowerGetterName(att['name'])
    if what == 'set' : metName = att['name'][0].upper() + att['name'][1:]
    ret = ''
    param = ''
    constF = ''
    if what == 'get_c' : 
      constF = ' const' 
      if not self.tools.isFundamentalT(att['type']) and att['type'].strip()[:6] != 'const ' : ret = 'const '
    if what == 'set'  : 
      ret = 'void '
      param = self.tools.genParamFromStrg(att['type']) + ' value'
    elif what in ['get','get_c'] : 
      ret = self.tools.genReturnFromStrg(ret + att['type'], self.generatedTypes, scopeName)
      if att['virtualaccessor'] in ['TRUE', 'PURE'] and not scopeName : ret = '  virtual ' + ret
      if not self.tools.isFundamentalT(att['type']) and not self.tools.isPointerT(att['type']): ret += '&'
      ret += ' '
      what = ''
    s +=  '  ' + ret + scopeName + what + metName + '(' + param + ')' + constF
    if ( not scopeName ) : # this is a declaration
      if not what and att['virtualaccessor'] == 'PURE' : s += ' = 0'
      s += ';\n\n'
    else :
      if not what and att['virtualaccessor'] == 'PURE' : s = ''
      elif not what        : s += ' \n{\n  return m_%s;\n}\n\n' % att['name']        # what was set to '' if getter
      elif what == 'set' : s += ' \n{\n  m_%s = value;\n}\n\n' % att['name'] 
    return s
#--------------------------------------------------------------------------------
  def genSmartRefFunctionalMethod(self, srs, scopeName=''):
    s = ''
    if not scopeName:
      #declaration
      s += '  /// Set entry number of events\n'
      s += '  void setEventEntry(const std::string& eventName, Long64_t& value);\n\n'
      s += '  /// Get event\n'
      s += '  JM::EventObject* event(const std::string& eventName);\n\n'
    else:
      #definition
      s += 'inline void ' + scopeName + '::setEventEntry(const std::string& eventName, Long64_t& value)\n{\n'
      for sr in srs:
        s += '  if ("eventName == "' + sr['attrs']['type'] + '") { \n'
        s += '    m_' + sr['attrs']['name'] + '.setEntry(value);\n'
        s += '  }\n'
      s += '}\n\n'
      s += 'inline JM::EventObject* ' + scopeName + '::event(const std::string& eventName)\n{\n'
      for sr in srs:
        s += '  if (eventName == "' + sr['attrs']['type'] + '") { \n'
        s += '    m_' + sr['attrs']['name'] + '.SetBranchID(0);\n'
        s += '    return m_' + sr['attrs']['name'] + '.GetObject();\n'
        s += '  }\n'
      s += '  return 0; \n}\n\n'
    return s
#--------------------------------------------------------------------------------
  def genGetSetRelMethod(self,rel,what,scopeName=''):
    desc = {'gett':'Retrieve ', 'gett_c':'Retrieve (const) ', 'sett':'Update ', \
            'gettr':'Retrieve referenced ', 'gettr_c':'Retrieve referenced (const) ', \
            'gets':'Retrieve ', 'gets_c':'Retrieve (const) ', 'sets':'Update ', \
            'getsr':'Retrieve referenced ', 'getse_c':'Retrieve', 'setse':'Update', \
            'settr':'Update referenced ', 'setsr':'Update referenced ', 'addTo':'Add to ', \
            'addTo_p':'Att to (pointer) ', 'removeFrom':'Remove from ', \
            'removeFrom_p':'Remove from (pointer) ', 'clear':'Clear '}
    s = ''
    if ( not scopeName ) :
      s += '  /// %s\n  ' % desc[what]
      s += self.comment(rel['desc'])    # feature bv@bnl.gov
    else : 
      s += 'inline '
      scopeName += '::'
    ret = 'void '
    if what in ['gett', 'gett_c']:
      if rel['multiplicity'] != '1' : 
        ret = self.tools.genReturnFromStrg('TRefArray&',self.generatedTypes,scopeName) + ' '
      else : 
        ret = self.tools.genReturnFromStrg('TRef&',self.generatedTypes,scopeName) + ' '
    if what in ['gettr', 'gettr_c', 'getsr']:
      ret = self.tools.genReturnFromStrg(rel['type'] + '*',self.generatedTypes,scopeName) + ' '
    if what in ['gets', 'gets_c']:
      ret = self.tools.genReturnFromStrg('JM::SmartRef&',self.generatedTypes,scopeName) + ' '
    if what == 'getse_c':
      ret = self.tools.genReturnFromStrg('Long64_t',self.generatedTypes,scopeName) + ' '
    metName = ''
    if what in ['gett', 'gett_c', 'gets', 'gets_c'] :
      metName = self.tools.lowerGetterName(rel['name']) + 'Ref'
    if what in ['gettr', 'gettr_c', 'getsr']:
      metName = self.tools.lowerGetterName(rel['name'])
    if what in ['sett', 'settr', 'sets', 'setse', 'getse_c', 'setsr', 'addTo', 'addTo_p', 'removeFrom', 'removeFrom_p', 'clear'] :
      metName = rel['name'][0].upper() + rel['name'][1:]
    param = ''
    if what == 'sett' and rel['multiplicity'] != '1': 
      param = self.tools.genParamFromStrg('TRefArray') + ' value'
    elif what in ['sett','addTo', 'removeFrom'] : 
      param = self.tools.genParamFromStrg('TRef') + ' value'
    elif what == 'sets':
      param = self.tools.genParamFromStrg('JM::SmartRef') + ' value'
    elif what == 'setse':
      param = self.tools.genParamFromStrg('Long64_t') + ' value'
    elif what in ['settr', 'setsr', 'addTo_p', 'removeFrom_p']:
      param = self.tools.genParamFromStrg( rel['type'] + '*') + ' value'
    constF = ''
    if what in ['gett_c', 'getr_c', 'gets_c', 'getse_c'] : 
        constF = ' const'
        ret = 'const ' + ret
    if what[-2] == '_' : what = what[:-2]
    if what in ['gett', 'gets']:
      what = ''
      rflag = 0
    if what in ['gettr', 'getsr']:
      what = ''
      rflag = 1
    if what in ['sett', 'sets', 'settr', 'setsr']:
      what = 'set'
    if ( not scopeName ):
      s += '  '
    if what == 'getse':
      s += ret + scopeName + 'get' + metName + 'Entry(' + param + ')' + ' const'
    elif what == 'setse':
      s += ret + scopeName + 'set' + metName + 'Entry(' + param + ')'
    elif what in ['','set','addTo','clear','removeFrom']:
      s += ret + scopeName + what + metName + '(' + param + ')' + constF
    if ( not scopeName )       : s += ';\n\n'                                       # this is a declaration
    else : 
      if not what:                                                                  # what was set to '' if getter
        if not rflag:
          s += ' \n{\n  return m_%s;\n}\n\n' % rel['name']
        else:
          s += ' \n{\n  m_%s.SetBranchID(0); \n  return (%s*)m_%s.GetObject();\n}\n\n' % (rel['name'],rel['type'],rel['name'])
      elif what == 'set'       : s += ' \n{\n  m_%s = value;\n}\n\n' % rel['name']
      elif what == 'setse'     : s += ' \n{\n  m_%s.setEntry(value);\n}\n\n' % rel['name']
      elif what == 'getse'     : s += ' \n{\n  return m_%s.entry();\n}\n\n' % rel['name']
      elif what == 'addTo'     : s += ' \n{\n  m_%s.Add(value);\n}\n\n' % rel['name']
      elif what == 'clear'     : s += ' \n{\n  m_%s.Clear();\n}\n\n' % rel['name']
      elif what == 'removeFrom': s += ' \n{\n  m_%s.Remove(value);\n}\n\n' % rel['name']
    return s
#--------------------------------------------------------------------------------
  def genGetSetBitfieldMethod(self, bf, what, attAtt, scopeName):
    desc = {'get':'Retrieve', 'set':'Update', 'check':'Check'}
    s = ''
    if not scopeName :
      s += '  /// %s\n  ' % desc[what]
      s += self.comment(bf['desc'])      # feature bv@bnl.gov
    else :
      s += 'inline '
      scopeName += '::'
    metName = bf['name']
    bfType = 'bool'
    if bf.has_key('type')    : bfType = bf['type']
    elif bf['length'] != '1' : bfType = attAtt['type']
    ret = 'void '
    param = ''
    constF = ''
    if what == 'set' :
      metName = bf['name'][0].upper() + bf['name'][1:]
      param = self.tools.genParamFromStrg(bfType) + ' value'
    if what == 'get':
      metName = self.tools.lowerGetterName(metName)
      what = ''
      ret = self.tools.genReturnFromStrg(bfType,self.generatedTypes,scopeName) + ' '
      constF = ' const'
    if what == 'check':
      ret = 'bool '
      metName = bf['name'][0].upper() + bf['name'][1:]
      param = self.tools.genParamFromStrg(bfType) + ' value'
      constF = ' const'
    s += ret + scopeName + what + metName + '(' + param + ')' + constF
    if not scopeName : s += ';\n\n'
    else:
      if bf['length'].isdigit() : bits = bf['name']
      else : bits = bf['length'].split(',')[0]
      if not what        :
        retF = '0 != '                            
        if (ret[:-1]) != 'bool' : retF = '(%s)'%ret[:-1]
        s += '\n{\n  return %s((m_%s & %sMask) >> %sBits);\n}\n\n' % (retF, attAtt['name'],bf['name'],bits)
      elif what == 'set' :
        s += '\n{\n  unsigned int val = (unsigned int)value;\n' 
        if bf['exclusive'] == 'TRUE' : s += '  m_%s &= ~%sMask;\n' % (attAtt['name'], bf['name']) 
        s += '  m_%s |= ((((unsigned int)val) << %sBits) & %sMask);\n}\n\n' % (attAtt['name'],bits,bf['name'])
      elif what == 'check':
        s += '\n{\n  unsigned int val = (unsigned int)value;\n'
        s += '  return 0 != ( m_%s & %sMask & ( val << %sBits ));\n}\n\n' % ( attAtt['name'], bf['name'], bf['name'] )
    return s
#--------------------------------------------------------------------------------
  def genGetSetMethods(self,godClass,clname=''):
    s = ''
    if godClass.has_key('attribute'):
      for att in godClass['attribute']:
        attAtt = att['attrs']
        if attAtt['getMeth'] == 'TRUE': 
          s += self.genGetSetAttMethod(attAtt,'get_c',clname)
	  if attAtt['nonconstaccessor'] == 'TRUE' :
	    s += self.genGetSetAttMethod(attAtt,'get',clname)
        if attAtt['setMeth'] == 'TRUE':
          s += self.genGetSetAttMethod(attAtt,'set',clname)
        if att.has_key('bitfield'):
          for bf in att['bitfield']:
            bfAtt = bf['attrs']
            if bfAtt['getMeth'] == 'TRUE':
              s += self.genGetSetBitfieldMethod(bfAtt,'get',attAtt,clname)
            if bfAtt['setMeth'] == 'TRUE':
              s += self.genGetSetBitfieldMethod(bfAtt,'set',attAtt,clname)
            if bfAtt['checkMeth'] == 'TRUE':
              s += self.genGetSetBitfieldMethod(bfAtt,'check',attAtt,clname)
    if godClass.has_key('template'):
      for att in godClass['template']:
        attAtt = att['attrs']
        tempType = att['attrs']['type']
        attAtt['type'] = attAtt['type'] + genLinkDef.genLinkDef( self.godRoot ).findTempParameter( att['typename'] )
        if attAtt['getMeth'] == 'TRUE':
          s += self.genGetSetAttMethod(attAtt,'get_c',clname)
	  if attAtt['nonconstaccessor'] == 'TRUE' :
	    s += self.genGetSetAttMethod(attAtt,'get',clname)
        if attAtt['setMeth'] == 'TRUE':
          s += self.genGetSetAttMethod(attAtt,'set',clname)
        att['attrs']['type'] = tempType
    if godClass.has_key('relation'):
      for rel in godClass['relation']:
        relAtt = rel['attrs']
        if relAtt['multiplicity'] == '1':
          if relAtt['getMeth'] == 'TRUE': 
            s += self.genGetSetRelMethod(relAtt,'gett_c',clname)
	    if relAtt['nonconstaccessor'] == 'TRUE' :
	      s += self.genGetSetRelMethod(relAtt,'gett',clname)
          if relAtt['getRMeth'] == 'TRUE':
            s += self.genGetSetRelMethod(relAtt,'gettr_c',clname)
            if relAtt['nonconstaccessor'] == 'TRUE' :
              s += self.genGetSetRelMethod(relAtt,'gettr',clname)
          if relAtt['setMeth'] == 'TRUE':
            s += self.genGetSetRelMethod(relAtt,'sett',clname)
          if relAtt['setRMeth'] == 'TRUE':
            s += self.genGetSetRelMethod(relAtt,'settr',clname)
        else:
          if relAtt['getMeth'] == 'TRUE':
            s += self.genGetSetRelMethod(relAtt,'gett_c',clname)
            if relAtt['nonconstaccessor'] == 'TRUE' :
              s += self.genGetSetRelMethod(relAtt,'gett',clname)
          if relAtt['setMeth'] == 'TRUE':
            s += self.genGetSetRelMethod(relAtt,'sett',clname)
          if relAtt['addMeth'] == 'TRUE':
            s += self.genGetSetRelMethod(relAtt,'addTo_p',clname) 
          if relAtt['remMeth'] == 'TRUE':
            s += self.genGetSetRelMethod(relAtt,'removeFrom_p',clname)
          if relAtt['clrMeth'] == 'TRUE':
            s += self.genGetSetRelMethod(relAtt,'clear',clname)
    if godClass.has_key('SmartRelation'):
      for rel in godClass['SmartRelation']:
        relAtt = rel['attrs']
        if relAtt['getMeth'] == 'TRUE':
          s += self.genGetSetRelMethod(relAtt,'gets_c',clname)
          if relAtt['nonconstaccessor'] == 'TRUE' :
            s += self.genGetSetRelMethod(relAtt,'gets',clname)
        s += self.genGetSetRelMethod(relAtt,'getsr',clname)
        if relAtt['setMeth'] == 'TRUE':
          s += self.genGetSetRelMethod(relAtt,'sets',clname)
        s += self.genGetSetRelMethod(relAtt,'setsr',clname)
      s += self.genSmartRefFunctionalMethod(godClass['SmartRelation'], clname)
    return s[:-1]
#--------------------------------------------------------------------------------
  def genBitfield(self,att):
    s = ''
    attName = att['attrs']['name']
    if att.has_key('bitfield'):
      maxLenName = 0
      for bf in att['bitfield'] : maxLenName = max(maxLenName,len(bf['attrs']['name']))
      bf0Att = att['bitfield'][0]['attrs']
      indent = (len(attName) + 11) * ' '
      s += '  /// Offsets of bitfield %s\n' % attName
      s += '  enum %sBits{%s = 0' % (attName, (bf0Att['name']+'Bits').ljust(maxLenName+4))
      offset = string.atoi(bf0Att['length'])
      for bf in att['bitfield'][1:]:
        bfAtt = bf['attrs']
        if bfAtt['length'].isdigit():
          s += ',\n%s %s = %d' % (indent, (bfAtt['name']+'Bits').ljust(maxLenName+4), offset)
          offset += string.atoi(bfAtt['length'])
      s += '};\n\n'
      s += '  /// Bitmasks for bitfield %s\n' % attName
      offset = 0
      indent += '  '
      s += '  enum %sMasks{' % attName
      for bf in att['bitfield']:
        if s[-1] != '{' : s += ',\n%s' % indent
        bfAtt = bf['attrs']
        if bfAtt['length'].isdigit():
          lInt = string.atoi(bfAtt['length'])
          mask = 0
          for j in range(offset,offset+lInt) : mask += (1L<<j)
          offset += lInt
          s += '%s = %s' % ((bfAtt['name']+'Mask').ljust(maxLenName+4), hex(mask))
        else:
          masks = bfAtt['length'].split(',')
          mask = ''
          for m in masks : mask += m.strip() + 'Mask + '
          s += '%s = %s' % ((bfAtt['name']+'Mask').ljust(maxLenName+4), mask[:-3])
      s += '\n%s};\n\n' % indent[1:]
    return s
#-------------------------------------------------------------------------------- added by Li
  def genClassDefine( self, godClass, className ):
    s = ''
    classVersion = '1'
    if godClass['attrs'].has_key('version'):
      classVersion = godClass['attrs']['version']
    if godClass['attrs']['classdef'] == 'TRUE':
      s += '  ClassDef(%s,%s);\n' % (className, classVersion)
    return s
#--------------------------------------------------------------------------------
  def genStreamer(self, godClass, className=''):
    s = ''
    if not className and godClass['attrs']['serializers'] == 'TRUE':
      if godClass.has_key('method'):
        for met in godClass['method']:
          metAtt = met['attrs']
          metName = metAtt['name']
          if metName in ('operator<<','fillStream'):
            ret = ''
            if metAtt.has_key('type')  :
              ret = self.tools.genReturnFromStrg(metAtt['type'],self.generatedTypes,godClass['attrs']['name'])
            elif met.has_key('return') :
              ret = self.tools.genReturnFromElem(met['return'],self.generatedTypes,godClass['attrs']['name'])
            if metName == 'operator<<' and ret == 'std::ostream&' : self.genOStream = 0
            if metName == 'fillStream' and ret == 'std::ostream&' : self.genFillStream = 0
    else :
      className += '::'
    if self.genFillStream:
      self.addInclude('ostream',1)
      virt = 'virtual '
      if godClass['attrs']['virtual'] == 'FALSE' : virt = ''
      if className : s += 'inline '
      else : s += '  /// Fill the ASCII output stream\n %s' % virt
      s += 'std::ostream& %sfillStream(std::ostream& s) const' % className
      if not className : s += ';\n'
      else:
        s += '\n{\n'
        if godClass.has_key('attribute'):
          for att in godClass['attribute']:
            attAtt = att['attrs']
            if attAtt['type'] == 'bool' and attAtt['storage'] == 'TRUE':
              s += "  char l_%s = (m_%s) ? 'T' : 'F';\n" % (attAtt['name'], attAtt['name'])
        if godClass.has_key('base'):
          for b in godClass['base']:
            if b['attrs']['name'].split('<')[0] not in ('ContainedObject', 'DataObject', 'KeyedObject'):
              s += '  %s::fillStream(s);\n' % b['attrs']['name']
        if godClass.has_key('attribute'):
          for att in godClass['attribute']:
            attAtt = att['attrs']
            type = attAtt['type'].lstrip()
            if type[:5] == 'std::' and attAtt['storage'] == 'TRUE':
              if 'GaudiKernel/SerializeSTL' not in self.include:
                self.include.append('GaudiKernel/SerializeSTL')
                 # Trick for namespace problems (Savannah bug 29887)
                self.verbatimLHCb.append('using GaudiUtils::operator<<;')
                break;
          s += '  s << "{ "'
          for att in godClass['attribute']:
            attAtt = att['attrs']
            if attAtt['storage'] == 'TRUE':
              type = attAtt['type'].lstrip()
              if s[-1] == '"' : s += ' << "%s :\t" ' % attAtt['name']
              else            : s += '\n            << "%s :\t" ' % attAtt['name']
              if   type == 'bool'   : s += '<< l_'
              elif type == 'double' : s += '<< (float)m_'
              else                  : s += '<< m_'
              s += attAtt['name'] + ' << std::endl'
          s += ' << " }";\n'
        s += '  return s;\n'
        s += '}\n\n'
    return s
#--------------------------------------------------------------------------------
  def genClassOstreamOverload(self, godClass):
    s = ''
    if not self.isEventClass and self.genOStream:
      self.addInclude('ostream',1)
      s += 'inline std::ostream& operator<< (std::ostream& str, const %s& obj)' % ( godClass['attrs']['name'])
      s += '\n{\n  return obj.fillStream(str);\n}\n'
    return s
#--------------------------------------------------------------------------------
  def genAllocatorOperators(self, godClass,allocatorType):
    s = ''
    if allocatorType == "FROMXML":
      allocatorType = godClass['attrs']['allocator']

    if allocatorType == 'DEFAULT' :
      # set the default allocator type
      allocatorType = 'BOOST'
        
    data = {}
    data['classname'] = godClass['attrs']['name']

    if allocatorType == 'BOOST': # Boost allocator with check on delete
      s ="""
#ifndef GOD_NOALLOC
  /// operator new
  static void* operator new ( size_t size )
  {
    return ( sizeof(%(classname)s) == size ? 
             boost::singleton_pool<%(classname)s, sizeof(%(classname)s)>::malloc() :
             ::operator new(size) );
  }

  /// placement operator new
  /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
  /// it is not needed in libstdc++ >= 3.4
  static void* operator new ( size_t size, void* pObj )
  {
    return ::operator new (size,pObj);
  }

  /// operator delete
  static void operator delete ( void* p )
  {
    boost::singleton_pool<%(classname)s, sizeof(%(classname)s)>::is_from(p) ?
    boost::singleton_pool<%(classname)s, sizeof(%(classname)s)>::free(p) :
    ::operator delete(p);
  }

  /// placement operator delete
  /// not sure if really needed, but it does not harm
  static void operator delete ( void* p, void* pObj )
  {
    ::operator delete (p, pObj);
  }
#endif"""%data
      self.include.append("XmlObjDesc/boost_allocator.h")
      
    elif allocatorType == 'BOOST2': # Boost allocator without check on delete
      s ="""
#ifndef GOD_NOALLOC
  /// operator new
  static void* operator new ( size_t size )
  {
    return ( sizeof(%(classname)s) == size ? 
             boost::singleton_pool<%(classname)s, sizeof(%(classname)s)>::malloc() :
             ::operator new(size) );
  }

  /// placement operator new
  /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
  /// it is not needed in libstdc++ >= 3.4
  static void* operator new ( size_t size, void* pObj )
  {
    return ::operator new (size,pObj);
  }

  /// operator delete
  static void operator delete ( void* p )
  {
    boost::singleton_pool<%(classname)s, sizeof(%(classname)s)>::free(p);
  }

  /// placement operator delete
  /// not sure if really needed, but it does not harm
  static void operator delete ( void* p, void* pObj )
  {
    ::operator delete (p, pObj);
  }
#endif"""%data
      self.include.append("XmlObjDesc/boost_allocator.h")
      
    elif allocatorType == 'DEBUG': # Boost allocator with check on delete and debug print-out
      s = """
#ifndef GOD_NOALLOC
  /// operator new
  static void* operator new ( size_t size )
  {
    void *ptr = sizeof(%(classname)s) == size ? 
      boost::singleton_pool<%(classname)s, sizeof(%(classname)s)>::malloc() :
      ::operator new(size);
    std::cout << "%(classname)s::new() -> " << ptr << std::endl;
    return ( ptr );
  }

  /// placement operator new
  /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
  /// it is not needed in libstdc++ >= 3.4
  static void* operator new ( size_t size, void* pObj )
  {
    std::cout << "%(classname)s::new(" << pObj << ")" << std::endl;
    return ::operator new (size,pObj);
  }

  /// operator delete
  static void operator delete ( void* p )
  {
    std::cout << "%(classname)s::delete(" << p << ") "
              << boost::singleton_pool<%(classname)s, sizeof(%(classname)s)>::is_from(p)
              << std::endl;
    boost::singleton_pool<%(classname)s, sizeof(%(classname)s)>::is_from(p) ?
    boost::singleton_pool<%(classname)s, sizeof(%(classname)s)>::free(p) :
    ::operator delete(p);
  }

  /// placement operator delete
  /// not sure if really needed, but it does not harm
  static void operator delete ( void* p, void* pObj )
  {
    std::cout << "%(classname)s::delete(" << p << "," << pObj << ") " << std::endl;
    ::operator delete (p, pObj);
  }
#endif"""%data
      self.include.append("XmlObjDesc/boost_allocator.h")
      self.addInclude('iostream',1)
      
    return s
##--------------------------------------------------------------------------------
  def doit(self,package,godClasses,outputDir,lname,allocatorType = 'FROMXML'):

    for godClass in godClasses:

      self.reset(package,godClass)
      self.parseClassImport(godClass)

      classDict = package.dict
      classname = godClass['attrs']['name']

      # Get/set class namespace to package namespace if not set in class
      try:
        namespace = godClass['attrs']['namespace']
      except KeyError:
        namespace = package.dict['packagenamespace']
        godClass['attrs']['namespace'] = namespace

      scoped_classname = namespace+'::'+classname

      fileName = '%s.h' % classname

      print '    File %s' % fileName.ljust(lname),

      classDict['classname']                    = classname
      classDict['uclassname']                   = classname.upper()  #added by Li
      #classDict['EDMBook']                      = self.genEDMBook(godClass,scoped_classname)
      #classDict['classVersion']                = self.genClassVersion(godClass)
      classDict['desc']                         = self.genDesc(godClass)
      classDict['author']                       = godClass['attrs']['author']
      classDict['today']                        = time.ctime()
      classDict['classNamespace']               = namespace
      classDict['inheritance']                  = self.genInheritance(godClass)
      classDict['constructorDecls']             = self.genConstructors(godClass)
      classDict['destructorDecl']               = self.genDestructors(godClass)
      for modifier in ['public','protected','private']:
        classDict[modifier+'Typedefs']          = self.genTypedefs(modifier,godClass)
        classDict[modifier+'Attributes']        = self.genAttributes(modifier,godClass)
        classDict[modifier+'TemplateAttr']      = self.genTemplateAttr(modifier,godClass)
        classDict[modifier+'BitfieldEnums']     = self.bitfieldEnums[modifier]
        classDict[modifier+'Enums']             = self.genEnums(modifier,godClass)
        classDict[modifier+'MethodDecls']       = self.genMethods(modifier,godClass)
        classDict[modifier+'MethodDefs']        = self.genMethods(modifier,godClass,scoped_classname)
      classDict['getSetMethodDecls']            = self.genGetSetMethods(godClass)
      classDict['constructorDefs']              = self.genConstructors(godClass,scoped_classname)
      classDict['destructorDef']                = self.genDestructors(godClass,scoped_classname)
      classDict['getSetMethodDefs']             = self.genGetSetMethods(godClass,scoped_classname)
      defs,maps,dcls = self.genEnumConversions(godClass, scoped_classname)
      classDict['enumConversionDefs']  = defs
      classDict['enumConversionMaps']  = maps
      classDict['enumConversionDecls'] = dcls
      classDict['includes']                     = self.genIncludes()
      classDict['forwardDeclsGlob']             = self.genForwardDeclsGlob()
      classDict['forwardDeclsLHCb']             = self.genForwardDeclsLHCb()
      classDict['forwardIncludes']              = self.genForwardIncludes(classname)
      classDict['classDefine']                  = self.genClassDefine(godClass,classname) #added by Li
      
      g = gparser.gparser()
      g.parse(self.godRoot+'templates/header.tpl',classDict)

      file = open(outputDir+os.sep+fileName,'w')
      file.write(g.stream)
      file.close()

      print ' - Done'
      
#================================================================================
#import xparser
#if __name__ == '__main__':
#  x = xparser.xparser('xml_files/Event.xml','xml_files/GODsClassDB.xml')
#  x.parse()
#  g = genClasses(x.gdd)
#  g.doit()

