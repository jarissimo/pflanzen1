import socket
from struct import *
import logging

class Decoded_msg:
    pass

H2O_MAGIC = 0xAC
H2O_VERSION = 0x01
H2O_MSG = {
    0x10: 'DATA_MASK',
    0x20: 'RQST_MASK',
    0x90: 'WARN_MASK',
    0xA0: 'INFO_MASK',
}

#https://stackoverflow.com/a/25259157
POLYNOMIAL = 0x1021
PRESET = 0x1D0F

def _initial(c):
    crc = 0
    c = c << 8
    for j in range(8):
        if (crc ^ c) & 0x8000:
            crc = (crc << 1) ^ POLYNOMIAL
        else:
            crc = crc << 1
        c = c << 1
    return crc

_tab = [ _initial(i) for i in range(256) ]

def _update_crc(crc, c):
    cc = 0xff & c

    tmp = (crc >> 8) ^ cc
    crc = (crc << 8) ^ _tab[tmp & 0xff]
    crc = crc & 0xffff
    return crc

def crc(str):
    crc = PRESET
    for c in str:
        crc = _update_crc(crc, ord(c))
    return crc

def crcb(*i):
    crc = PRESET
    for c in i:
        crc = _update_crc(crc, c)
    return crc

def h2o_perform_checksum(array):
    #print(*array)
    return crcb(*array)


def h2o_convert1Bhex(hex_array):
    return (hex_array[0] << 8) + hex_array[1]
       
def h2o_decoder(msg_array):
    
    decoded_msg = Decoded_msg()
	
    assert msg_array[0] == H2O_MAGIC, ("Protocol is not defined: %r") % msg_array[0]
    print("Decoding H2O protocol.", end="")
     
    assert msg_array[1] == H2O_VERSION, ("Protocol version is not defined\n")
    print(" Version:", msg_array[1])
    decoded_msg.version = msg_array[1]
	
    assert msg_array[2] == len(msg_array), ("Error in packet length. defined len: %i, arr len: %i\n" % (msg_array[2], len(msg_array)))		            
    decoded_msg.msg_len = msg_array[2]
    
    assert (msg_array[3] & 0xF0) in H2O_MSG.keys(), ("Undefined message type, %i" % msg_array[3])
    
    msg_type = H2O_MSG[msg_array[3] & 0xF0 ]
    print("Message type mask:", msg_type)
    decoded_msg.msg_mask = msg_type
    
    msg_subtype = msg_array[3] & 0x0f
    
    decoded_msg.node_id = h2o_convert1Bhex(msg_array[4:6])
    print("Node ID:", decoded_msg.node_id)

    
    print("Performing checksum:")
    temp_array= msg_array[:6]+ bytes((0,0)) + msg_array[8:]    
    decoded_msg.checksum = h2o_convert1Bhex(msg_array[6:8])
    calc_checksum = h2o_perform_checksum(temp_array)
    assert calc_checksum == decoded_msg.checksum , ("Invalid checksum value, received: %x, calculated: %x" % (decoded_msg.checksum, calc_checksum ) )
    
    
    print("Reading data...")
    decoded_msg.data=[]
    for i in range(8, len(msg_array)):
        print(msg_array[i], end=" ")
        decoded_msg.data.append(msg_array[i])
    print_to_file(decoded_msg)
	
def print_to_file(d_msg):
    with open("msgs.txt", "a") as msgfile:
        msgfile.write("H2O protocol msg.")
        msgfile.write("Version: %i \n" % d_msg.version)
        msgfile.write("Message type mask: %s \n"% d_msg.msg_mask)
        msgfile.write("Node ID: %i\n"% d_msg.node_id)
        msgfile.write("Performed checksum: %i \n"% d_msg.checksum)		#inconsistency might happen.  
        msgfile.write(str(d_msg.data)+"\n")
        msgfile.write("----------------------------------")

 
def udp_server(host='::', port=44555):
    s = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    log.info("Listening on udp %s:%s" % (host, port))
    s.bind((host, port))
    while True:
        (data, addr) = s.recvfrom(128*1024)
        yield data


#FORMAT_CONS = '%(asctime)s %(name)-12s %(levelname)8s\t%(message)s'
#logging.basicConfig(level=logging.DEBUG, format=FORMAT_CONS)
#hexdump.dump("Hello World", sep=":")

log = logging.getLogger('udp_server')
for data in udp_server():
	#log.debug("%r" % (data,))
	print(data, end='\n')
	print(":".join("{:02x}".format(c) for c in data))
	h2o_decoder(data)

TCP_IP = '127.0.0.1'
TCP_PORT = 11111
BUFFER_SIZE = 20  # Normally 1024, but we want fast response

#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.TCPServer.allow_reuse_address = True
#s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
#s.bind((TCP_IP, TCP_PORT))
#s.listen(1)
#conn, addr = s.accept()

#while 1:
#    data = conn.recv(BUFFER_SIZE)
#    if not data: break
#    print(data)
#    h2o_decoder(data)
#	#decode_protocol(unpack('bbbbhh',data))
#    conn.send(data)  # echo
#conn.close()

