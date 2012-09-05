from serial import Serial

PortName = 'COM11'
PortSpeed = 9600
PortTimeout = 0.5

#StrQuestion = '?'
StrQuestion = 'd'
#StrQuestion = 's0\n'
#StrQuestion = 's20\n'
#StrQuestion = 's3F\n'
#StrQuestion = 's64\n'

with Serial(PortName,PortSpeed,timeout=PortTimeout) as port:

    print('Send question \"{}\"'.format(StrQuestion))
    port.write(StrQuestion.encode('ascii'))
    answ = port.readlines()
    if len(answ)>0:
        print('Receive answer \"{}\"'.format(answ[-1].strip().decode('ascii')))
    else:
        print('No answer');
    port.close()
