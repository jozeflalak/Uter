#!/bin/sh 

version="v001r001b001"
tarfile="uter_${version}.tar.gz"
binfile="uter_${version}.bin"

rm -f ${tarfile}
tar -zcvf ${tarfile} -C ${PWD} \
			uter/bin \
			uter/lib \
			uter/python \
			uter/document \
			uter/setup.sh
lines=`wc -l ${tarfile} | awk '{print $1}'`
lines=`expr ${lines} + 1`

rm -f ${binfile}
#---------------------
echo '#!/bin/sh' >> ${binfile}
echo 'tail -n '${lines}' $0 | tar -zvx -C ${PWD}' >> ${binfile}
echo 'cd ${PWD}/uter/' >> ${binfile}
echo 'chmod 755 ./setup.sh' >> ${binfile}
echo './setup.sh' >> ${binfile}
echo 'exit 0' >> ${binfile}
#---------------------
cat ${tarfile} >> ${binfile}
chmod 755 ${binfile}






