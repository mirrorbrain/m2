#!/bin/python
import socket
rootmePadding = ("challenge01.root-me.org", 51014)
print "Connecting to server..."
sock1=socket.socket()
sock1.connect(rootmePadding)
print "Connected to root-me server on port 51014. Executing the algorithm ..."
cipherText=[["0xBC","0x16","0x54","0x24","0x33","0x10","0x0D","0x95","0x22","0xDC","0x3B","0x64","0x28","0xD4","0xFF","0x5F"],["0x7F","0xC6","0x7B","0x49","0x94","0x32","0x3C","0x47","0xED","0x09","0xF1","0x85","0xC3","0xCE","0x7A","0x2E"]]
result=[]
for k in range (0,16):
    for i in range (0,256):
        prev=[]
        for j in range (0,k):
            prev.append(hex((k+1)^int(result[j],16)^int(cipherText[0][15-j],16)))
        prev.append(hex(i))
        for j in range (k+1,16):
            prev.append('0x00')
        prev.reverse()
        blockPair=[]
        for j in range (0,16):
            blockPair.append(prev[j])
        for j in range (0,16):
            blockPair.append(cipherText[1][j])
        sock1.send(''.join(blockPair).replace("0x","").upper().zfill(64))
        response=sock1.recv(1024)
        if (response.find("Right password")!=-1 or response.find("Wrong password")!=-1):
            print "DEBUG: C'= "+str(hex(i))+"\n"+"DEBUG: C'||C2 = "+str(blockPair)
            result.append(hex((k+1)^int(cipherText[0][15-k],16)^int(blockPair[15-k],16)))
            print "DEBUG: P2= "+str(result)+"\n"
            break
print "Execution completed."
print "Plaintext :",''.join([chr(int(i,16)) for i in result[::-1]])
sock1.close()
