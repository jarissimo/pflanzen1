class Decoded_msg:
    pass

H2O_MAGIC = 0xAC
H2O_VERSION = 0x01
H2O_MSG_MAP=[
    ["DATA_MASK" , 0x10],
    ["RQST_MASK" , 0x20],
    ["WARN_MASK" , 0x90],
    ["INFO_MASK" , 0xA0]
	]

def h2o_perform_checksum(array):
    return 1

def h2o_get_msg_type(m_type):
    msg_type = m_type + 0	#Convert to int 
    i=0
    while( msg_type >= H2O_MSG_MAP[i][1]):
        i=i+1
    return i-1


def h2o_convert1Bhex(hex_array):
    hex = ''.join([str(hex_array[1]).zfill(2), str(hex_array[0]).zfill(2) ])
    return(int(hex, 16))
       
def h2o_decoder(msg_array):
    
    decoded_msg = Decoded_msg()
	
    assert msg_array[0] == H2O_MAGIC, ("Protocol is not defined: %r") % msg_array[0]
    print("Decoding H2O protocol.", end="")
     
    assert msg_array[1] == H2O_VERSION, ("Protocol version is not defined\n")
    print(" Version:", msg_array[1])
    decoded_msg.version = msg_array[1]
	
    assert msg_array[2] == len(msg_array), ("Error in packet length. defined len: %i, arr len: %i\n" % (msg_array[2], len(msg_array)))		            
    decoded_msg.msg_len = msg_array[2]
    
    assert msg_array[3] < H2O_MSG_MAP[-1][1] or msg_array[3] > H2O_MSG_MAP[0][1] , ("Undefined message type, %i" % msg_array[3])	#Security here when we check only message type and not the subtype also (e.g eimer voll, pflanze durst). We leave checking sub-type for later.
    
    msg_type = h2o_get_msg_type(msg_array[3])
    print("Message type mask:", H2O_MSG_MAP[-1][msg_type])
    decoded_msg.msg_mask = H2O_MSG_MAP[-1][msg_type]
    
    decoded_msg.node_id = h2o_convert1Bhex(msg_array[4:6])
    print("Node ID:", decoded_msg.node_id)

    print("Performing checksum:")
    #assert h2o_perform_checksum(msg_array) == checksum , ("Invalid checksum value, received: %", checksum)
    decoded_msg.checksum = h2o_convert1Bhex(msg_array[6:8])
    
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

