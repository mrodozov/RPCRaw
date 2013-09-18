#!/user/bin/python

conn_string = "oracle://CMS_RPC_CONSTR:RPCConDB_2012@cmsr"

from sqlalchemy.engine import create_engine
engine = create_engine(conn_string,echo=True)
connection = engine.connect()
  
#FROM = str(" from CMS_RPC_CONSTR.RE4_COSMIC_TABLE c, CMS_RPC_CONSTR.RE4_CHAMBER r")
#WHERE = str(" where c.CHAMBER_ID=r.CHAMBER_ID and r.CHAMBER_NAME not like 'TEST%'")
#ORDERBY = str(" order by r.CHAMBER_NAME, c.DATE_TIME, c.RUN")

#id = [" c.SITE"," c.DATE_TIME"," c.COSMIC_ID"," c.RUN"," r.CHAMBER_NAME"," c.ETA_PARTITION"," r.CHAMBER_TYPE"," c.NUM_EVENTS"," c.HV"]
#cs = [" c.MEAN_CLUSTER_SIZE_IN"]
#hv = [" c.HVMON_TN"," c.HVMON_TW"," c.HVMON_B"]
#ef = [" c.EFFICIENCY"," c.ERR_EFFICIENCY"]


#WHAT = ""
#for l in id:
#    print l
#    WHAT += str(l)+","

         
#for k in hv:
#    if (k <> ef[len(hv)-1]):
#for k in cs:
#    if (k <> ef[len(cs)-1]):
#for k in ef:
#    if (k <> ef[len(ef)-1]):
#        WHAT += str(k)+","
#    else:
#        WHAT += str(k)+""

#QUERY = "select " + WHAT + FROM + WHERE + ORDERBY

#print QUERY

#result = connection.execute(QUERY)

result = connection.execute("select c.SITE, c.DATE_TIME, c.COSMIC_ID, c.RUN, r.CHAMBER_NAME, r.CHAMBER_TYPE, c.ETA_PARTITION, c.FLAG, c.LAYER, c.HV, c.EFFICIENCY, c.ERR_EFFICIENCY, c.NUM_EVENTS, c.HVMON_TN, c.HVMON_TW, c.HVMON_B, c.MEAN_CLUSTER_SIZE_IN from CMS_RPC_CONSTR.RE4_COSMIC_TABLE c, CMS_RPC_CONSTR.RE4_CHAMBER r where c.CHAMBER_ID=r.CHAMBER_ID and r.CHAMBER_NAME not like 'TEST%' order by r.CHAMBER_NAME, c.DATE_TIME, c.RUN" )

resultD = connection.execute("select c.SITE, c.DATE_TIME, c.COSMIC_ID, c.RUN, r.CHAMBER_NAME, r.CHAMBER_TYPE, c.ETA_PARTITION, c.FLAG, c.LAYER, c.HV, c.EFFICIENCY, c.ERR_EFFICIENCY, c.NUM_EVENTS, c.HVMON_TN, c.HVMON_TW, c.HVMON_B, c.MEAN_CLUSTER_SIZE_IN from CMS_RPC_CONSTR.RE4_COSMIC_TABLE c, CMS_RPC_CONSTR.RE4_CHAMBER r where c.CHAMBER_ID=r.CHAMBER_ID and r.CHAMBER_NAME not like 'TEST%' and c.LAYER='DOUBLE' order by r.CHAMBER_NAME, c.DATE_TIME, c.RUN" )

resultT = connection.execute("select c.SITE, c.DATE_TIME, c.COSMIC_ID, c.RUN, r.CHAMBER_NAME, r.CHAMBER_TYPE, c.ETA_PARTITION, c.FLAG, c.LAYER, c.HV, c.EFFICIENCY, c.ERR_EFFICIENCY, c.NUM_EVENTS, c.HVMON_TN, c.HVMON_TW, c.HVMON_B, c.MEAN_CLUSTER_SIZE_IN from CMS_RPC_CONSTR.RE4_COSMIC_TABLE c, CMS_RPC_CONSTR.RE4_CHAMBER r where c.CHAMBER_ID=r.CHAMBER_ID and r.CHAMBER_NAME not like 'TEST%' and c.LAYER='TOP' order by r.CHAMBER_NAME, c.DATE_TIME, c.RUN" )

resultB = connection.execute("select c.SITE, c.DATE_TIME, c.COSMIC_ID, c.RUN, r.CHAMBER_NAME, r.CHAMBER_TYPE, c.ETA_PARTITION, c.FLAG, c.LAYER, c.HV, c.EFFICIENCY, c.ERR_EFFICIENCY, c.NUM_EVENTS, c.HVMON_TN, c.HVMON_TW, c.HVMON_B, c.MEAN_CLUSTER_SIZE_IN from CMS_RPC_CONSTR.RE4_COSMIC_TABLE c, CMS_RPC_CONSTR.RE4_CHAMBER r where c.CHAMBER_ID=r.CHAMBER_ID and r.CHAMBER_NAME not like 'TEST%' and c.LAYER='BOTTOM' order by r.CHAMBER_NAME, c.DATE_TIME, c.RUN" )

f = open ("cosmictestRE4.txt","w")
fD = open ("DOUBLE_RE4.txt","w")
fT = open ("TOP_RE4.txt","w")
fB = open ("BOTTOM_RE4.txt","w")

#    print row['SITE'], row['DATE_TIME'], row['COSMIC_ID'], row['RUN'], row['CHAMBER_NAME'], row['ETA_PARTITION'], row['CHAMBER_TYPE'], row ['FLAG'], row['LAYER'], row['HV'], row['EFFICIENCY'], row['ERR_EFFICIENCY'], row['NUM_EVENTS'] 

for row in result: 
    f.write (row["SITE"]+" "+str(row["DATE_TIME"])+" "+str(row["COSMIC_ID"])+" "+str(row["RUN"])+" "+str(row["CHAMBER_NAME"])+" "+str(row["CHAMBER_TYPE"])+" "+str(row["ETA_PARTITION"])+" "+str(row["LAYER"])+" "+str(row["NUM_EVENTS"])+" "+str(row["HV"])+" "+str(row["HVMON_TN"])+" "+str(row["HVMON_TW"])+" "+str(row["HVMON_B"])+" "+str(row["EFFICIENCY"])+" "+str(row["ERR_EFFICIENCY"])+" "+str(row["MEAN_CLUSTER_SIZE_IN"]) + "\n")

for row in resultD: 
    fD.write (row["SITE"]+" "+str(row["DATE_TIME"])+" "+str(row["COSMIC_ID"])+" "+str(row["RUN"])+" "+str(row["CHAMBER_NAME"])+" "+str(row["CHAMBER_TYPE"])+" "+str(row["ETA_PARTITION"])+" "+str(row["LAYER"])+" "+str(row["NUM_EVENTS"])+" "+str(row["HV"])+" "+str(row["HVMON_TN"])+" "+str(row["HVMON_TW"])+" "+str(row["HVMON_B"])+" "+str(row["EFFICIENCY"])+" "+str(row["ERR_EFFICIENCY"])+" "+str(row["MEAN_CLUSTER_SIZE_IN"]) + "\n")

for row in resultT:
    fT.write (row["SITE"]+" "+str(row["DATE_TIME"])+" "+str(row["COSMIC_ID"])+" "+str(row["RUN"])+" "+str(row["CHAMBER_NAME"])+" "+str(row["CHAMBER_TYPE"])+" "+str(row["ETA_PARTITION"])+" "+str(row["LAYER"])+" "+str(row["NUM_EVENTS"])+" "+str(row["HV"])+" "+str(row["HVMON_TN"])+" "+str(row["HVMON_TW"])+" "+str(row["HVMON_B"])+" "+str(row["EFFICIENCY"])+" "+str(row["ERR_EFFICIENCY"])+" "+str(row["MEAN_CLUSTER_SIZE_IN"]) + "\n")

for row in resultB:
    fB.write (row["SITE"]+" "+str(row["DATE_TIME"])+" "+str(row["COSMIC_ID"])+" "+str(row["RUN"])+" "+str(row["CHAMBER_NAME"])+" "+str(row["CHAMBER_TYPE"])+" "+str(row["ETA_PARTITION"])+" "+str(row["LAYER"])+" "+str(row["NUM_EVENTS"])+" "+str(row["HV"])+" "+str(row["HVMON_TN"])+" "+str(row["HVMON_TW"])+" "+str(row["HVMON_B"])+" "+str(row["EFFICIENCY"])+" "+str(row["ERR_EFFICIENCY"])+" "+str(row["MEAN_CLUSTER_SIZE_IN"]) + "\n")

resultD.close()
resultT.close()
resultB.close()
result.close()

f.close()
fD.close()
fT.close()
fB.close()


#print WHAT

#SOWHAT = WHAT [WHAT.find(".")+1:len(WHAT)]

#SOWHAT = ""
#for m in id:
#    n = m [m.find(".")+1:len(m)]
#    SOWHAT += str(n)+","
        
#for i in ef:
#    j = i [i.find(".")+1:len(i)]
#     if (i <> ef[len(ef)-1]):
#       SOWHAT += str(j)+","
#    else:
#        SOWHAT += str(j)+""

#print SOWHAT
  
