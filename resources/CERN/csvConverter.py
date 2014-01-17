import sys,json,copy

file = ''; #empty 

if sys.argv and sys.argv >= 1:
  try:
    file = open(sys.argv[1],'r')
  except IOError:
    print 'Cannot open file !'
    
else:
  print 'At least one argument is expected or the argument is not a file !'

content = file.readlines()

firstPart = True
secondPart = False
thirdpart = False

globalDetails = {'date':'','run_start':'','run_end':'','references':'','events':0};
chamberDetails = {'chambers':''}
runsList = []

linesCounter = 0;
listOfAllChambers = []

for fileLine in content:
  
  listOfLineDetails = fileLine.split()
  
  if 'Shelf' in fileLine:
    firstPart = False
    secondPart = True
    continue
  
  if 'TRG' in fileLine:
    secondPart = False
    thirdpart = True
    continue
  
  if firstPart:
    
    if 'DATE' in listOfLineDetails[0]:
      globalDetails['date'] = listOfLineDetails[1]
    
    if 'RUN START' in fileLine:
      if 'RUN_FINISH' in fileLine:
	fileLine.replace('RUN_FINISH','RUN FINISH')
      globalDetails['run_start']=listOfLineDetails[2]
      globalDetails['run_end']=listOfLineDetails[5]
    
    if 'REF' in listOfLineDetails[0]:
      listOfreferenceChambers = []
      i = 1
      
      for ii in listOfLineDetails:
	if i%3 == 0:
	  listOfreferenceChambers.append(ii)
	i += 1
      
      globalDetails['references']=listOfreferenceChambers
      
    if 'Events' in fileLine:
      globalDetails['events'] = listOfLineDetails[1]
      
  if secondPart:
    
    singleChamber = {}
    singleChamber['HVcable'] = listOfLineDetails[0]
    singleChamber['shelf'] = listOfLineDetails[1]
    singleChamber['TDCstat'] = listOfLineDetails[2]
    singleChamber['TDCcable'] = listOfLineDetails[3]
    singleChamber['name'] = listOfLineDetails[4]
    singleChamber['LV'] = listOfLineDetails[5]
    singleChamber['threshold'] = listOfLineDetails[6]
    listOfAllChambers.append(singleChamber)
    
  if thirdpart:
    counter = 0
    singleRun = {'run':0,'HVapplied':0,'TriggerLayer':''}
    singleRun2 = {'run':0,'HVapplied':0,'TriggerLayer':''}
    singleRun3 = {'run':0,'HVapplied':0,'TriggerLayer':''}
    
    singleRun['run'] =  listOfLineDetails[0]
    singleRun['HVapplied'] = listOfLineDetails[1]
    singleRun['TriggerLayer'] = listOfLineDetails[2]
    #print singleRun
    runsList.append(singleRun)
    singleRun2['run'] =  listOfLineDetails[3]
    singleRun2['HVapplied'] = listOfLineDetails[4]
    singleRun2['TriggerLayer'] = listOfLineDetails[5]
    #print singleRun2
    runsList.append(singleRun2)
    singleRun3['run'] =  listOfLineDetails[6]
    singleRun3['HVapplied'] = listOfLineDetails[7]
    singleRun3['TriggerLayer'] = listOfLineDetails[8]
    #print singleRun3
    buffer3 = singleRun
    runsList.append(singleRun3)
    
  
    
def getSingleRunDetailsFromFile(filename,folder):
  runDetailsJSONdict = {}
  runDetailsJSONdict['chambersByShelfNumber'] = {}
  filename = folder+filename
  f = open(filename)
  f.close()
  try:
    f = open(filename,'r')
  except IOError:
    print 'Cannot open file', filename
    
  fileContent = f.readlines()
  fisrtFilePart = True
  secondFilePart = False
  
  chamberLineData = 0
  chamberLineCounter = 0
  stationAsString = ''
  
  singleChamberDetails = {'name':'','FEBS':'','topwide':'','topnarrow':'','bottom':''}
  
  for Line in fileContent:
    
    lineDetails = Line.split()
    
    if 'Environment:' in Line:
      runDetailsJSONdict['Temperature'] = lineDetails[1]
      runDetailsJSONdict['Humidity'] = lineDetails[2]
      runDetailsJSONdict['Pressure'] = lineDetails[3]
      
    if 'Trigger_layer' in Line:
      runDetailsJSONdict['TriggerLayer'] = lineDetails[3]
    if 'Stack' in Line:
      runDetailsJSONdict['StackID'] = lineDetails[2]
    
    if 'CMS' in Line:
      firstPart = False
      secondFilePart = True
      
    if 'CMS' in Line and secondFilePart:
      chamberLineCounter += 1
      
      gapName = ''
      
      if chamberLineCounter == 1:
	gapName = 'topnarrow'
      elif chamberLineCounter == 2:
	gapName = 'topwide'
      else:
	gapName = 'bottom'
      gapDetailsDict = {'current':lineDetails[1],'vset':lineDetails[2],'vmon':lineDetails[3]}
      
      stationAsString = lineDetails[4]
      
      singleChamberDetails[gapName] = gapDetailsDict
      nameOnly = lineDetails[0][:lineDetails[0].rfind('_')-len(lineDetails[0])]
      singleChamberDetails['name']=nameOnly
      
    if 'FEBs' in Line:
      chamberLineCounter = 0
      del lineDetails[0]
      singleChamberDetails['FEBS']=lineDetails
      runDetailsJSONdict['chambersByShelfNumber'][stationAsString] = copy.copy(singleChamberDetails)
  
  f.close()
  return runDetailsJSONdict
  
  
chamberDetails['chambers'] = listOfAllChambers

  
gDetails = json.dumps(globalDetails)
cDetails = json.dumps(chamberDetails)
rList = json.dumps(runsList)

'''

print gDetails
print cDetails
print rList

print '-------------Single file details----------------------'

dictFromFile = getSingleRunDetailsFromFile('1940.info','')

for key in dictFromFile['chambersByShelfNumber']:
  chamberAsJSON = json.dumps(dictFromFile['chambersByShelfNumber'][key])
  print chamberAsJSON

'''

# change the names of chambers in cDetails dictionary only once

firstRunDetailsDict = getSingleRunDetailsFromFile(runsList[0]['run']+'.info',sys.argv[2])
firstRunChambersDetails = firstRunDetailsDict['chambersByShelfNumber']
for chamber in chamberDetails['chambers']:
  chamber['name'] = firstRunChambersDetails[chamber['shelf']]['name']
  
# bring the logic for the output using all dictionaries

inputFileName = sys.argv[1]
fileNameToWrite = inputFileName[:inputFileName.rfind('.csv')-len(inputFileName)] + '.json'
fileToWrite = open(fileNameToWrite,'w')

for run in runsList:
  singleRunDict = {}
  fileToRead = run['run']+'.info'
  singleRunDetailsDict = getSingleRunDetailsFromFile(fileToRead,sys.argv[2])
  singleRunDict['referenceChambers'] = globalDetails['references']
  singleRunDict['events'] = globalDetails['events']
  # applied HV , run number , gaps operation mode, stack id
  #singleRunDict['run'] = run['run']
  singleRunDict['HVApplied'] = run['HVapplied']
  singleRunDict['gapsMode'] = singleRunDetailsDict['TriggerLayer']
  singleRunDict['stackID'] = singleRunDetailsDict['StackID']
  # environmental vars 
  singleRunDict['temperature'] = singleRunDetailsDict['Temperature']
  singleRunDict['humidity'] =  singleRunDetailsDict['Humidity'] 
  singleRunDict['preasure'] =  singleRunDetailsDict['Pressure'] 
  # chamber details 
  singleRunDict['chambers'] = chamberDetails['chambers']
  
  for chamber in singleRunDict['chambers']:
    shelf = chamber['shelf']
    chamber['topnarrow'] = singleRunDetailsDict['chambersByShelfNumber'][shelf]['topnarrow']
    chamber['topwide'] = singleRunDetailsDict['chambersByShelfNumber'][shelf]['topwide']
    chamber['bottom'] = singleRunDetailsDict['chambersByShelfNumber'][shelf]['bottom']
    chamber['FEBS'] = singleRunDetailsDict['chambersByShelfNumber'][shelf]['FEBS']
  
  runToWrite = {run['run']:singleRunDict}
  fileToWrite.write(json.dumps(runToWrite))
  fileToWrite.write('\n')
  
fileToWrite.close()  
  
  