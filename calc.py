#!/usr/bin/python3
key=[1,2,3] 
vol=3
sum=0
for i in key:
	sum=sum+i
 
def calcadrint(adrext):
	temp=adrext
	adrint=adrext // (sum * vol) * sum 
	adrext=adrext % (sum * vol)
	i=0
	vid=0
	vola=[0 for i in range(0,vol)]
	while adrext >= key[i]:
		adrext=adrext-key[i] 
		vola[vid]=vola[vid] + key[i] 
		vid=(vid+1) % vol
		i=(i+1)%len(key)
	adrint=adrint+adrext+vola[vid] 
	return [temp, vid, adrint]

for i in range(0, 5000): # exemple with 2000 first addresses
	a=calcadrint(i)
	print("%d;%d;%d" % (a[0], a[1], a[2]))

