#!/apps/python/PRO/bin/python
from subprocess import call
import sys,os,time

def main():
    
    gunX = 0.
    gunY = 0.
    gunZ = 0.
    gunE=1000#MeV

    nrDet = 30
    materials = ["tungsten","lead","copper"]

    printInterval = 10000
    email="ciprian@jlab.org"
    inpDir="/lustre/expphy/work/hallc/qweak/ciprian/simCodeG410/radDamage"
    outDir="/lustre/expphy/volatile/hallc/qweak/ciprian/farmoutput/radDmg"

    nEv=20000
    nrStop=3
    nrStart=0
    submit=0

    for mat in materials:
        for nr in range(nrStart,nrStop): # repeat for nr jobs
            idN= mat + '_%04d'% (nr) 
            print idN

            createMacFile(outDir,idN,nrDet,mat,gunX,gunY,gunZ,gunE,nEv,nr,printInterval)
            call(["cp",inpDir+"/build/radTest",outDir+"/radTest/"+idN+"/radTest"]))

        createXMLfile(inpDir,outDir,mat,nrStart,nrStop,email)


        if submit==1:
            print "submitting position sampled with id",_idN," between ",_nrStart,_nrStop
            call(["jsub","-xml",_source+"/scripts/jobs/"+idRoot+".xml"])
        else:
            print "NOT submitting position sampled with id",_idN," between ",_nrStart,_nrStop
        
    print "I am all done"


def createMacFile(directory,idname,
                  nrDet,material,xPos,yPos,zPos,
                  beamE,nEv,nr,printInterval):
    if not os.path.exists(directory+"/radTest/"+idname+"/log"):
        os.makedirs(directory+"/radTest/"+idname+"/log")
   
    f=open(directory+"/radTest/"+idname+"/myRun.mac",'w')
    f.write("/rad/det/setNrDetectors "+str(nrDet)+"\n")
    f.write("/rad/det/setTargetMaterial "+material+" \n")
    f.write("/rad/run/setGunPosX "+str(xPos)+" cm\n")
    f.write("/rad/run/setGunPosY "+str(yPos)+" cm\n")
    f.write("/rad/run/setGunPosZ "+str(zPos)+" cm\n")
    f.write("/rad/run/setGunEnergy "+str(beamE)+" MeV\n")
    f.write("/rad/det/updateGeometry\n")
    seedA=int(time.time())+   100*nr+nr
    seedB=int(time.time())*100+10*nr+nr
    f.write("/random/setSeeds "+str(seedA)+" "+str(seedB)+"\n")
    f.write("/rad/event/setPrintModulo "+str(printInterval+"\n")
    f.write("/run/beamOn "+str(nEv)+"\n")
    f.close()
    return 0

def createXMLfile(source,writeDir,idRoot,nStart,nStop,email):
    
    if not os.path.exists(source+"/scripts/jobs"):
        os.makedirs(source+"/scripts/jobs")

    f=open(source+"/scripts/jobs/"+idRoot+"_"+str(nStart)+"_"+str(nStop)+".xml","w")
    f.write("<Request>\n")
    f.write("  <Email email=\""+email+"\" request=\"false\" job=\"true\"/>\n")
    f.write("  <Project name=\"qweak\"/>\n")
#    f.write("  <Track name=\"debug\"/>\n")
    f.write("  <Track name=\"simulation\"/>\n")
    f.write("  <Name name=\""+idRoot+"\"/>\n")
    f.write("  <OS name=\"centos7\"/>\n")
    f.write("  <Command><![CDATA[\n")
    f.write("./radTest -m myRun\n")
    f.write("  ]]></Command>\n")
    f.write("  <Memory space=\"2000\" unit=\"MB\"/>\n")

    for nr in range(nStart,nStop): # repeat for nr jobs
        f.write("  <Job>\n")
        idName= writeDir+"/radTest/"+idRoot+'_%04d'%(nr)
        f.write("    <Input src=\""+idName+"/radTest\" dest=\"radTest\"/>\n")
        f.write("    <Input src=\""+idName+"/myRun.mac\" dest=\"myRun.mac\"/>\n")
        f.write("    <Output src=\"o_radTree.root\" dest=\""+idName+"/QwSim_0.root\"/>\n")
        f.write("    <Stdout dest=\""+idName+"/log/log.out\"/>\n")
        f.write("    <Stderr dest=\""+idName+"/log/log.err\"/>\n")
        f.write("  </Job>\n\n")

    f.write("</Request>\n")
    f.close()
    return 0

                    
if __name__ == '__main__':
    main()
                            
