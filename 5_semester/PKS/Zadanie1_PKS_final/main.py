 #!/usr/bin/env python
import scapy.all as scapy
import os
import sys


class udp:
    def __init__(self, frame_number, source_ip, destination_ip, source_port, destination_port):
        self.frames_number = []
        self.frames_number.append(frame_number)
        self.source_ip = source_ip
        self.destination_ip = destination_ip
        self.source_port = source_port
        self.destination_port = destination_port




class arp:
    def __init__(self, optional, frame_number, sender_ip, sender_mac, target_ip, status):
        self.optional = optional
        self.frames_number = []
        self.frames_number.append(frame_number)
        self.sender_ip = sender_ip
        self.sender_mac = sender_mac
        self.target_ip = target_ip
        self.status = status
        

#=====================
#=====================
#====nacitanie dat====
#=====================
#=====================

#nacitanie dat zo suboru
def load_data():
    filename = input("Zadaj cestu k súboru alebo \"exit\" pre ukončenie programu: ")

    while not os.path.exists(filename) or not filename.endswith('.pcap'):
        if filename == 'exit':
            exit()
        print("Zlá cesta alebo zly typ súboru.")
        filename = input("Zadaj cestu k súboru alebo \"exit\" pre ukončenie programu: ")
        os.path.exists(filename)

    return scapy.rdpcap(filename)


#funkcia na nacitanie slovnika zo suboru
def load_dictionary(dictionary, filename):
    file = open(filename,"r")
    for line in file:
        to_dictionary = line.split(' ',1)
        key = to_dictionary[0]
        value = to_dictionary[1]
        dictionary[int(key, 16)] = value.replace("\n", "")
    file.close()


#funkcia na nacitanie slovnika, ktory obsahuje protokoly a porty ako slovniky
def load_nested_dictionary():
    nested_dictionary ={}
    ether_types = {}
    llc_saps = {}
    ip_protocols = {}
    tcp_ports = {}
    udp_ports = {}
    icmp_types = {}

    load_dictionary(ether_types, "ether_types.txt")
    load_dictionary(llc_saps, "llc_saps.txt")
    load_dictionary(ip_protocols, "ip_protocols.txt")
    load_dictionary(tcp_ports, "tcp_ports.txt")
    load_dictionary(udp_ports, "udp_ports.txt")
    load_dictionary(icmp_types, "icmp_types.txt")

    nested_dictionary['ether_types'] = ether_types
    nested_dictionary['llc_saps'] = llc_saps
    nested_dictionary['ip_protocols'] = ip_protocols
    nested_dictionary['tcp_ports'] = tcp_ports
    nested_dictionary['udp_ports'] = udp_ports
    nested_dictionary['icmp_types'] = icmp_types

    return nested_dictionary






#vypis dlzky ramca
def frame_len_print(frame_len):
    print("Dĺžka rámca poskytnutá pcap API -", frame_len, "B")
    if frame_len + 4 > 64:
        print("Dĺžka rámca prenasaneho po mediu -", frame_len + 4, "B")
    else:
        print("Dĺžka rámca prenasaneho po mediu - 64 B")


#vypis typu ramca
def frame_type_print(type_or_lenght,first_two_bytes_data):
    if type_or_lenght > 1500: # 0x05DC v hexa
        print("Ethernet II")
    elif first_two_bytes_data == 0xFFFF: # 65535 v desiatkovej
        print("Novell 802.3 RAW")
    elif first_two_bytes_data == 0xAAAA: # 43690 v desiatkovej
        print("IEEE 802.3 LLC + SNAP")
    else:
        print("IEEE 802.3 LLC")


#naformatovanie retazca mac adresy
def format_mac_addr(mac_adr):
    result = ':'.join(mac_adr[i:i+2] for i in range(0,12,2)) 
    return result.upper()


#vypis mac adries
def mac_addr_src_dst_print(mac_addresses):
    print("Zdrojova MAC adresa:",format_mac_addr(mac_addresses[12:24]))
    print("Cielova MAC adresa:",format_mac_addr(mac_addresses[0:12]))


#formatovany vypis celeho ramca v hexa
def frame_print(frame):
    count = 0
    frame_len = len(frame)
    for i in frame.upper():
        print(i, end='')
        count += 1
        if count % 32 == 0:
            print()
            continue
        if count % 16 == 0:
            print('   ', end='')
            continue
        if count % 2 == 0:
            print(' ', end='')

    print()


#transformacia ip z hex do desiatkovej
def ipv4_address_hex_traslation(ip_address):    
    result = '.'.join(str(int(ip_address[i:i+2],16))  for i in range(0,8,2)) 
    return result


#ziskanie ipcky, ktora poslala najviac ipv4 paketov
def get_max_ipv4_add_sender(dictionary: dict):
    count = list(dictionary.values())
    ip_address = list(dictionary.keys())
    return ip_address[count.index(max(count))]



def inside_protocol_print(frame, type_or_lenght, first_two_bytes_data, nested_dictionary):
    ether_types = dict(nested_dictionary['ether_types'])
    llc_saps = dict(nested_dictionary['llc_saps'])
    ip_protocols = dict(nested_dictionary['ip_protocols'])
    tcp_ports = dict(nested_dictionary['tcp_ports'])
    udp_ports = dict(nested_dictionary['udp_ports'])
    icmp_types = dict(nested_dictionary['icmp_types'])

    #print(ether_types, "\n\n\n",llc_saps, "\n\n\n",ip_protocols, "\n\n\n", tcp_ports, "\n\n\n",udp_ports, "\n\n\n",icmp_types)


    if type_or_lenght > 1500:
        if type_or_lenght in ether_types:
            print(ether_types.get(type_or_lenght))
        else:
            print("Neznámy EtherType")


        # ARP
        if type_or_lenght == 0x0806:
            
            
            if int(frame[40:44],16) == 1:
                print("         ARP-Request")

                print("     Zdrojova MAC adresa: ",format_mac_addr(frame[44:56]))
                print("     Zdrojova IP adresa: ", ipv4_address_hex_traslation(frame[56:64]))
                print("     Cielova MAC adresa: ",format_mac_addr(frame[64:76]))
                print("     Cielova IP adresa ku ktorej sa hlada MAC: ", ipv4_address_hex_traslation(frame[76:84]))
            else:
                print("         ARP-Reply")

                print("     Zdrojova MAC adresa (Hladana): ",format_mac_addr(frame[44:56]))
                print("     Zdrojova IP adresa: ", ipv4_address_hex_traslation(frame[56:64]))
                print("     Cielova MAC adresa: ",format_mac_addr(frame[64:76]))
                print("     Cielova IP adresa: ", ipv4_address_hex_traslation(frame[76:84]))



        # IPv4
        if type_or_lenght == 0x0800:
            print("     Zdrojova IP adresa: ", ipv4_address_hex_traslation(frame[52:60]))
            print("     Cielova IP adresa: ", ipv4_address_hex_traslation(frame[60:68]))
            ihl = frame[29:30]
            ipv4_header_len = int(ihl,16) * 4
            #print(ihl,"   ", ipv4_header_len)
            ipv4_protocol = int(frame[46:48], 16)
            #print(ipv4_protocol)
            if ipv4_protocol in ip_protocols:
                print("     ",ip_protocols.get(ipv4_protocol))
            else:
                print("     IPv4 protokol nenajdeny")

            # ICMP
            if ipv4_protocol == 1:
                if int(frame[28 + ipv4_header_len * 2: 28 + ipv4_header_len * 2 + 2],16) in icmp_types:
                    print("     Info: ",icmp_types.get(int(frame[28 + ipv4_header_len * 2: 28 + ipv4_header_len * 2 + 2],16)))
                #print()
            elif ipv4_protocol == 6:
                source_port = int(frame[28 + ipv4_header_len * 2: 28 + ipv4_header_len * 2 + 4],16)
                destination_port = int(frame[28 + ipv4_header_len * 2 + 4: 28 + ipv4_header_len * 2 + 8],16)

                if source_port in tcp_ports:
                    print("     ", tcp_ports.get(source_port))
                elif destination_port in tcp_ports:
                    print("     ", tcp_ports.get(destination_port))
                else:
                    print("     Neznáme porty")

                print("     Source port: ", source_port)
                print("     Destination port: ", destination_port)

            elif ipv4_protocol == 17:
                source_port = int(frame[28 + ipv4_header_len * 2: 28 + ipv4_header_len * 2 + 4],16)
                destination_port = int(frame[28 + ipv4_header_len * 2 + 4: 28 + ipv4_header_len * 2 + 8],16)

                if source_port in udp_ports:
                    print("     ", udp_ports.get(source_port))
                elif destination_port in udp_ports:
                    print("     ", udp_ports.get(destination_port))
                else:
                    print("     Neznáme porty")

                print("     Source port: ", source_port)
                print("     Destination port: ", destination_port)

    #(RAW)
    elif first_two_bytes_data == 0xFFFF: # 65535 v desiatkovej
        print("     IPX")

    #(LLC + SNAP)
    elif first_two_bytes_data == 0xAAAA: # 43690 v desiatkovej 
        snap_ethertype = int(frame[40:44], 16)
        if snap_ethertype in ether_types:
            print("     ",ether_types.get(snap_ethertype))
        else:
            print("     Neznámy EtherType v LLC + SNAP")
            #print(frame[40:44])

    #(LLC)
    else:  
        dsap = int(frame[28:30],16)
        if dsap in llc_saps:
            print("     ",llc_saps.get(dsap))
        else:
            print("     Neznámy DSAP v LLC")

    
#vypis informacii o ramci do terminalu
def to_terminal(frame,frame_number, nested_dictionary):
    print("Ramec",frame_number,":")

    type_or_lenght = int(frame[24:28],16)
    first_two_bytes_data = int(frame[28:32],16)

    frame_len_print(int(len(frame)/2))

    frame_type_print(type_or_lenght, first_two_bytes_data)
    
    mac_addr_src_dst_print(frame[0:24])
    
    inside_protocol_print(frame, type_or_lenght, first_two_bytes_data, nested_dictionary)

    frame_print(frame)
    print()
    print()


#funkcia pre vypis ip adries, ktore posielali pakety ipv4 a ip adresu s najviac poslanymi paketmi
def ipv4_addresses_print(ipv4_addresses: dict):
    if len(ipv4_addresses) != 0:
        print("Zoznam IP adries všetkých odosielajúcich uzlov:")
        for key in ipv4_addresses.keys():    
            print(key)
        print()
        print("IP adresa uzla s najväčší počtom odoslaných IPv4 paketov:")
        
        print(get_max_ipv4_add_sender(ipv4_addresses), max(ipv4_addresses.values()), " paketov")


# ukladanie ip adries a naratavanie poctu odoslanych ipv4 paketov
def ipv4_add_to_dict_counter(frame, ipv4_addresses):
    if int(frame[24:28], 16) == 0x0800:
        ipv4_source_address = ipv4_address_hex_traslation(frame[52:60])
        
        if ipv4_source_address in ipv4_addresses:
            ipv4_addresses[ipv4_source_address] += 1
        else:
            ipv4_addresses[ipv4_source_address] = 1


#funkcia na vypis vsetkych ramcov z pcap suboru
def all_frames(data, nested_dictionary):
    ipv4_addresses = {}

    #print(len(data),type(data))
    for frame_number in range(0,len(data)):
        raw = scapy.raw(data[frame_number])
        #print(raw.hex(),"\n")
        frame = raw.hex()
        to_terminal(frame, frame_number + 1, nested_dictionary)
        
        ipv4_add_to_dict_counter(frame, ipv4_addresses)
        

    ipv4_addresses_print(ipv4_addresses)
    

#funkcia na vypis konkretneho ramca z pcap suboru
def one_frame(data, frame_number, nested_dictionary):
    if frame_number in range(1,len(data) + 1):
        print()
        raw = scapy.raw(data[frame_number - 1])
        to_terminal(raw.hex(),frame_number, nested_dictionary)
    else:
        print("Zadal si zle číslo rámca.")



#=====================
#=====================
#========TCP==========
#=====================
#=====================

def tcp_analization(data, nested_dictionary, selected_protocol):
    tcp_ports = dict(nested_dictionary['tcp_ports'])

    for frame_number in range(0,len(data)):
        raw = scapy.raw(data[frame_number])
        #print(raw.hex(),"\n")
        frame = raw.hex()
        type_or_lenght = int(frame[24:28],16)
        
         # IPv4
        if type_or_lenght == 0x0800:
            ihl = frame[29:30]
            ipv4_header_len = int(ihl,16) * 4
            ipv4_protocol = int(frame[46:48], 16)

            #print(ipv4_protocol)

            
            if ipv4_protocol == 6:
                source_port = int(frame[28 + ipv4_header_len * 2: 28 + ipv4_header_len * 2 + 4],16)
                destination_port = int(frame[28 + ipv4_header_len * 2 + 4: 28 + ipv4_header_len * 2 + 8],16)

                if source_port in tcp_ports:
                    tcp_protocol = tcp_ports.get(source_port)
                    if  tcp_protocol != selected_protocol:
                        continue
                elif destination_port in tcp_ports:
                    tcp_protocol = tcp_ports.get(destination_port)
                    if  tcp_protocol != selected_protocol:
                        continue
                else:
                    continue

                #teda vloz
                to_terminal(frame,frame_number + 1, nested_dictionary)
                




#=====================
#=====================
#========TFTP=========
#=====================
#=====================

#vrati index objektu s danymi parametrami, ak neexistuje vrati -1
def already_in_udp_array(udp_array, source_ip, destination_ip, source_port, destination_port):
    for object in udp_array:
        if object.source_ip == source_ip and object.destination_ip == destination_ip and object.source_port == source_port and object.destination_port == destination_port:            
            return udp_array.index(object)
        if source_port == -1:
            if object.source_ip == source_ip and object.destination_ip == destination_ip and object.destination_port == destination_port:            
                return udp_array.index(object)
    return -1


def add_to_udp_array(udp_array, frame_number, source_ip, destination_ip, source_port, destination_port):
    already_exist_index = already_in_udp_array(udp_array, source_ip, destination_ip, source_port, destination_port)
    if already_exist_index > -1:
        udp_array[already_exist_index].frames_number.append(frame_number)
    else:
        udp_array.append(udp(frame_number, source_ip, destination_ip, source_port, destination_port))



def udp_analization(data, nested_dictionary):
    udp_array = []

    #nacitavanie do listu
    for frame_number in range(0,len(data)):
        raw = scapy.raw(data[frame_number])
        frame = raw.hex()
        type_or_lenght = int(frame[24:28],16)

        if type_or_lenght == 0x0800:
            
                ihl = frame[29:30]
                ipv4_header_len = int(ihl,16) * 4
                ipv4_protocol = int(frame[46:48], 16)

                if ipv4_protocol == 17:
                    source_port = int(frame[28 + ipv4_header_len * 2: 28 + ipv4_header_len * 2 + 4],16)
                    destination_port = int(frame[28 + ipv4_header_len * 2 + 4: 28 + ipv4_header_len * 2 + 8],16)
                    source_ip = ipv4_address_hex_traslation(frame[52:60])
                    destination_ip = ipv4_address_hex_traslation(frame[60:68])
                    add_to_udp_array(udp_array, frame_number + 1, source_ip, destination_ip, source_port, destination_port)

    #for i in udp_array:
    #    print(i.source_port, i.destination_port, len(i.frames_number),i.frames_number)
    
    #vypis komunikcii
    count = 1
    temp_src_port = -1
    for object in udp_array:
        if len(object.frames_number) == 1:

            if object.destination_port == 69 or object.source_port == 69:
                print("________________________________________________________________")
                print("________________________________________________________________")
                print()
                print("Komunikácia č. ",count)
                print()

                raw = scapy.raw(data[(object.frames_number[0]) - 1])
                frame = raw.hex()
                to_terminal(frame, (object.frames_number.pop(0)), nested_dictionary)
                

                if object.destination_port == 69:
                    temp_dst_port = object.source_port
                else:
                    temp_dst_port = object.destination_port


                temp_dest_ip = object.destination_ip
                temp_src_ip = object.source_ip

                while (udp_frame_id := already_in_udp_array(udp_array, temp_dest_ip, temp_src_ip, temp_src_port ,temp_dst_port)) != -1:
                    raw = scapy.raw(data[(udp_array[udp_frame_id].frames_number[0]) - 1])
                    frame = raw.hex()
                    to_terminal(frame, udp_array[udp_frame_id].frames_number[0], nested_dictionary)
                    swap = True
                    #print("totok",udp_array[udp_frame_id].frames_number)
                    udp_array[udp_frame_id].frames_number.pop(0)
                    
                    #print("totok",udp_array[udp_frame_id].frames_number)
                    if len(udp_array[udp_frame_id].frames_number) > 1 and udp_array[udp_frame_id].frames_number[0] + 1 == udp_array[udp_frame_id].frames_number[1]:
                        swap = False

                    if swap:
                        #vymena
                        temp_dest_ip, temp_src_ip = temp_src_ip, temp_dest_ip
                        temp_src_port = temp_dst_port
                        temp_dst_port = udp_array[udp_frame_id].source_port

                    if  len(udp_array[udp_frame_id].frames_number) == 0:
                        udp_array.pop(udp_frame_id)
                temp_src_port = -1
                count += 1




#=====================
#=====================
#========ICMP=========
#=====================
#=====================


def icmp_analization(data, nested_dictionary):
    for frame_number in range(0,len(data)):
        raw = scapy.raw(data[frame_number])
        #print(raw.hex(),"\n")
        frame = raw.hex()
        type_or_lenght = int(frame[24:28],16)
        if type_or_lenght == 0x0800:
            ipv4_protocol = int(frame[46:48], 16)
            # ICMP
            if ipv4_protocol == 1:
                to_terminal(frame, frame_number + 1, nested_dictionary)


#=====================
#=====================
#========ARP==========
#=====================
#=====================

def already_in_arp_array(arp_array : list, sender_ip, target_mac ,target_ip):
    for object in arp_array:
        if object.status:
            continue
        if  (object.sender_ip == sender_ip and object.target_ip == target_ip) or ((object.sender_ip == target_ip and object.target_ip == sender_ip)):
            return arp_array.index(object)
    return -1



def add_to_arp_array(arp_array : list, optional, frame_number, sender_ip, target_mac ,target_ip):
    already_exist_index = already_in_arp_array(arp_array, sender_ip, target_mac ,target_ip)
    if already_exist_index > -1:
        arp_array[already_exist_index].frames_number.append(frame_number)
        if optional == 2:
            arp_array[already_exist_index].status = True
    else:
        if optional == 2:
            arp_array.append(arp(optional, frame_number, sender_ip, target_mac ,target_ip, True))
        else:
            arp_array.append(arp(optional, frame_number, sender_ip, target_mac ,target_ip, False))



def arp_analization(data, nested_dictionary):
    arp_array = []

    for frame_number in range(0,len(data)):
        raw = scapy.raw(data[frame_number])
        #print(raw.hex(),"\n")
        frame = raw.hex()
        type_or_lenght = int(frame[24:28],16)


        
         # ARP
        if type_or_lenght == 0x0806:            
            sender_ip = ipv4_address_hex_traslation(frame[56:64])
            target_mac = format_mac_addr(frame[64:76])
            target_ip = ipv4_address_hex_traslation(frame[76:84])

            if int(frame[40:44],16) == 1:
                add_to_arp_array(arp_array, 1, frame_number + 1, sender_ip, target_mac ,target_ip)
                #print("ARP-Request")
            else:
                add_to_arp_array(arp_array, 2, frame_number + 1, sender_ip, target_mac ,target_ip)
                #print("ARP-Reply")

    #vypis arp komunikacii
    if len(arp_array) > 0:
        temp_count = 1
        for object in arp_array:
            print("________________________________________________________________")
            print("________________________________________________________________")
            print()
            print("Komunikácia č. ", temp_count)
            print()
            for number in object.frames_number:
                raw = scapy.raw(data[number - 1])
                frame = raw.hex()
                to_terminal(frame, number, nested_dictionary)

            temp_count += 1



#=====================
#=====================
#=======PROGRAM=======
#======interface======
#=====================


#hlavička programu
def name_print():
    print()
    print("------------Analyzátor sieťovej komunikácie------------")
    print()
    print("                Autor: Adrián Vančo                    ")
    print()


#funkcia na zvolenie co sa ma vykonat
def menu(pcap, nested_dictionary):
    while True:
        file_option = int(input('Zapisat do suboru 1 - áno / 0 - nie: '))
        print()
        print("Dostupné možnosti:")
        print("     1 - pre výpis všetkých rámcov")
        print("     2 - pre vypis konkrétneho rámca")
        print("     3 - pre vypis HTTP komunikácie")
        print("     4 - pre vypis HTTPS komunikácie")
        print("     5 - pre vypis TELNET komunikácie")
        print("     6 - pre vypis SSH komunikácie")
        print("     7 - pre vypis FTP riadiace komunikácie")
        print("     8 - pre vypis FTP dátové komunikácie")
        print("     9 - pre vypis TFTP komunikácie")
        print("     10 - pre vypis ICMP komunikácie")
        print("     11 - pre vypis ARP komunikácie")
        print("     0 - pre ukončenie")

        menu_option = int(input('Zadaj možnosť: '))

        if file_option == 1:
            orig_stdout = sys.stdout
            f = open('out.txt', 'w')
            sys.stdout = f


        if menu_option == 1:
            all_frames(pcap, nested_dictionary)
        elif menu_option == 2:
            frame_number = int(input('Zadaj číslo rámca: '))
            one_frame(pcap, frame_number, nested_dictionary)
        elif menu_option == 3:
            tcp_analization(pcap, nested_dictionary, 'HTTP')
        elif menu_option == 4:
            tcp_analization(pcap, nested_dictionary, 'HTTPS')
        elif menu_option == 5:
            tcp_analization(pcap, nested_dictionary, 'TELNET')
        elif menu_option == 6:
            tcp_analization(pcap, nested_dictionary, 'SSH')
        elif menu_option == 7:
            tcp_analization(pcap, nested_dictionary, 'FTP riadiace')
        elif menu_option == 8:
            tcp_analization(pcap, nested_dictionary, 'FTP datove')
        elif menu_option == 9:
            udp_analization(pcap, nested_dictionary)
        elif menu_option == 10:
            icmp_analization(pcap, nested_dictionary)
        elif menu_option == 11:
            arp_analization(pcap, nested_dictionary)
        elif menu_option == 0:
            exit()
        else:
            print("Zadal si zlú možnosť.")

        if file_option == 1:
            sys.stdout = orig_stdout
            f.close()



def main():
    name_print()
    pcap = load_data()
    nested_dictionary = load_nested_dictionary()
    menu(pcap, nested_dictionary)
    

if __name__ == '__main__':
    main()