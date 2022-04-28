 #!/usr/bin/env python
import socket
import struct
import os
import math
import time
import threading

#autor funkcie crc16 oysstu
#link: https://gist.github.com/oysstu/68072c44c02879a2abf94ef350d1c7c6
#last access: 9.12. 2021
#modifikovany poly z 0x8408 na 0x11021
def crc16(data: bytes, poly=0x11021):
    '''
    CRC-16-CCITT Algorithm
    '''
    data = bytearray(data)
    crc = 0xFFFF
    for b in data:
        cur_byte = 0xFF & b
        for _ in range(0, 8):
            if (crc & 0x0001) ^ (cur_byte & 0x0001):
                crc = (crc >> 1) ^ poly
            else:
                crc >>= 1
            cur_byte >>= 1
    crc = (~crc & 0xFFFF)
    crc = (crc << 8) | ((crc >> 8) & 0xFF)
    
    return crc & 0xFFFF


alive = False

def keep_alive(c_socket, s_address, timer):
    global alive
    time.sleep(timer)
    while True:
        #if not alive:
         #   break
        print("Posielam keep alive")
        c_socket.sendto(set_information_packet(0),s_address)

        try:
            c_socket.settimeout(5)
            data, address = c_socket.recvfrom(1500)
            p_type, packet_count, fragmentation_size, file_name = decode_information_packet(data)
        except(ConnectionResetError, socket.timeout):
                print("Neprisla odozva na keep alive")
                print("Pripojenie ukoncene...")
                if not alive:
                    return
                alive = False
                return
        else:
            if p_type == 0:
                print("Keep alive potvrdeny")
            else:
                alive = False
                print("Pripojenie ukoncene...")
                c_socket.close()
                return
        for i in range(timer):
            time.sleep(1)
            if not alive:
                return

#zadanie ip
def get_ip(who):
    ip = input("Zadaj IP {0}: ".format(who))
    return ip

#zadanie portu
def get_port(who):
    while True:
        try:
            port = int(input("Zadaj port {0}: ".format(who)))
        except ValueError:
            print("Chyba pri zadavani portu.")
            continue
        else:
            if port < 0:
                print("Zadaj kladne cislo pre port.")
                continue
            return port


#1500 - 20(IP H) - 8 (UDP H) - 6 (Moja H)
def get_fragmentation_size():
    while True:
        try:
            frag_size = int(input("Zadaj velkost fragmetov v rozsahu (1 - 1466): "))
        except ValueError:
            print("Chyba pri zadavani.")
            continue
        else:
            if frag_size < 1 or frag_size > 1466:
                print("Zla velkost")
                continue
            return frag_size

#volba zleho paketu
def bad_packet_choice(packet_count):
    while True:
        try:
            packet_id = int(input("Zadaj cislo paketu, ktory sa posle s chybou alebo \"0\" ak nechces poslat zly paket "))
        except ValueError:
            print("Chyba pri zadavani.")
            continue
        
        if packet_id >= 0 and packet_id <= packet_count:
            return packet_id -1

#vypocet poctu paketov na odoslanie
def calculate_packet_count(frag_size, data):
        return math.ceil(len(data) / frag_size)
    
#na zozkladanie informacneho paketu
def set_information_packet(p_type, packet_count=0, fragmentation_size=0, file_name=''):
    #0 - keep alive; 1 - nadviazanie spojenia; 5 - data OK; 6 - data BAD; 7 - ukoncenie spojenia
    if p_type == 0 or p_type == 1 or p_type == 5 or p_type == 6 or p_type == 7:
        return p_type.to_bytes(1, byteorder='big')

    #2 - bude poslana sprava
    if p_type == 2:
        return p_type.to_bytes(1, byteorder='big') + \
            packet_count.to_bytes(3, byteorder='big') + \
            fragmentation_size.to_bytes(2, byteorder='big')

    #3 - bude poslany subor
    if p_type == 3:
        return p_type.to_bytes(1, byteorder='big') + \
            packet_count.to_bytes(3, byteorder='big') + \
            fragmentation_size.to_bytes(2, byteorder='big') + \
            file_name.encode()

#na zozkladanie datoveho paketu
def set_data_packet(p_type, packet_id, crc, data):
    return p_type.to_bytes(1, byteorder='big') + \
        packet_id.to_bytes(3, byteorder='big') + \
        crc.to_bytes(2 , byteorder='big') + \
        data
        
#na ziskanie jednotlivych casti z informacneho paketu
def decode_information_packet(packet_data):
    if len(packet_data) == 1:
        return int.from_bytes(packet_data, byteorder='big'), None, None, None;
    
    #ide o spravu
    if len(packet_data) == 6:
        p_type, packet_count, fragmentation_size = struct.unpack('!B3sH',packet_data)

        packet_count = int.from_bytes(packet_count, byteorder='big')

        return p_type, packet_count, fragmentation_size, None;

    #s nazvom suboru
    if len(packet_data) > 6:
        p_type, packet_count, fragmentation_size, file_name = struct.unpack("!B3sH{0}s".format(len(packet_data) - 6), packet_data)

        packet_count = int.from_bytes(packet_count, byteorder='big')

        return p_type, packet_count, fragmentation_size, file_name.decode();

#na ziskanie jednotlivych casti z datoveho paketu
def decode_data_packet(packet_data):
    p_type, packet_id, crc, data = struct.unpack("!B3sH{0}s".format(len(packet_data) - 6),packet_data)
    return p_type, packet_id, crc, data;


#============Client=============

def client_initialization():
    try:
        s_ip = get_ip("pre server")
        s_port = get_port("pre server")
        s_address = (s_ip, s_port)

        c_socket =socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
        #poslanie inicializacneho paketu na server
        c_socket.sendto(set_information_packet(1), s_address)
        print("prvy inic od klienta")
        c_socket.settimeout(30)
        data, s_address = c_socket.recvfrom(1500)
        p_type, packet_count, fragmentation_size, file_name = decode_information_packet(data)

    except(socket.timeout, socket.gaierror, ConnectionResetError) as err:
        print(err)
        print("Nadviazanie spojenia so serverom sa nepodarilo")
        print()
        print("Navrat do menu...")
        c_socket.close()

    else:
        if p_type == 1:
            print("Nadviazanie spojenia so serverom", s_address[0], s_address[1])
            print()
            client(c_socket, s_address)


def client(c_socket, s_address):
    global alive
    first_run = True
    alive_thread = None
    print("IP adresa servera", s_address[0], "port", s_address[1])

    while True:

        option = client_menu()

        #ukoncenie role a upovedomenie servera
        if option == 0:
            print("Ukoncujem komunikaciu")
            c_socket.sendto(set_information_packet(7), s_address)
            if alive:
                alive = False
                alive_thread.join()
            c_socket.close()
            return

        #zmena role
        if option == 3: 
            if alive:
                alive = False
                alive_thread.join()
            os.system('cls')
            print("Rola zmenena")
            server(c_socket, s_address)
            return

        #poslanie spravy
        if option == 1:

            #nacitanie spravy
            message = input("Zadaj spravu: ")

            frag_size = get_fragmentation_size()

            message = message.encode(encoding='UTF-8',errors='strict')

            packet_count = calculate_packet_count(frag_size, message)

            print("Bude poslanych", packet_count,"paketov, velkost fragmentacie",frag_size)
            
            bad_packet = bad_packet_choice(packet_count)

            c_socket.sendto(set_information_packet(2, packet_count, frag_size), s_address)

            print("Posielam spravu:",message)
            #cakanie na odpoved
            try:
                c_socket.settimeout(2)
                c_socket.recvfrom(1500)
            except(socket.timeout):
                print("Server neodpoveda")
                continue
            else:
                #vypnutie keep alive ak bola zapnuta a nastavenie ze sa potom zapne po dokonceni odosielania dat
                refresh = False
                if alive:
                    refresh = True
                    alive = False
                    alive_thread.join()

                #posielanie dat
                data_sending(2,c_socket, s_address, message, frag_size, packet_count, bad_packet)
                
                #zapnutie keep alive po prvom odosielani
                if first_run:
                    print("Zapnutie keep alive")
                    alive = True
                    alive_thread = threading.Thread(target=keep_alive, args=(c_socket, s_address,10))
                    alive_thread.start()
                    first_run = False
                #zapnutie keep alive ak bolo pozastavene
                if refresh:
                    print("Zapnutie keep alive")
                    alive = True
                    alive_thread = threading.Thread(target=keep_alive, args=(c_socket, s_address,10))
                    alive_thread.start()


        #poslanie suboru
        if option == 2:

            #nacitanie suboru
            filename = input("Zadaj cestu k súboru: ")
            #ziskanie nazvu suboru
            while not os.path.exists(filename) :
                print("Zlá cesta.")
                filename = input("Zadaj cestu k súboru: ")
                os.path.exists(filename)
            #otvorenie suboru
            with open(filename, "rb") as f:
                    file_data = f.read()

            frag_size = get_fragmentation_size()
            
            packet_count = calculate_packet_count(frag_size, file_data)

            print("Bude poslanych", packet_count,"paketov")

            bad_packet = bad_packet_choice(packet_count)

            print("Posielam subor", filename,"base",os.path.basename(filename))

            c_socket.sendto(set_information_packet(3, packet_count, frag_size, os.path.basename(filename)), s_address)
            #cakanie na odpoved
            try:
                c_socket.settimeout(20)
                c_socket.recvfrom(1500)
            except(socket.timeout):
                print("Server neodpoveda")
                continue
            else:
                #vypnutie keep alive ak bola zapnuta a nastavenie ze sa potom zapne po dokonceni odosielania dat
                refresh = False
                if alive:
                    refresh = True
                    alive = False
                    alive_thread.join()

                #posielanie dat
                data_sending(3, c_socket, s_address, file_data, frag_size, packet_count, bad_packet)
                
                #zapnutie keep alive po prvom odosielani
                if first_run:
                    print("Zapnutie keep alive")
                    alive = True
                    alive_thread = threading.Thread(target=keep_alive, args=(c_socket, s_address,10))
                    alive_thread.start()
                    first_run = False
                #zapnutie keep alive ak bolo pozastavene
                if refresh:
                    print("Zapnutie keep alive")
                    alive = True
                    alive_thread = threading.Thread(target=keep_alive, args=(c_socket, s_address,10))
                    alive_thread.start()
                
                
#posielanie dat
def data_sending(data_type, c_socket, s_address, data, frag_size, packet_count, bad_packet):
    max_attempts = 3
    print()
    for i in range(packet_count):
        print("Paket c.:",i + 1)
        attempts = 0
        try:
            data_frag = data[i * frag_size:(i + 1) * frag_size]
        except IndexError:
            data_frag = data[i * frag_size:]

        #crc = 250
        crc = crc16(data_frag)
        #print("crc ",crc16(data))
        if bad_packet == i:
            print("paket bude zly.")
            crc = crc // 2
            #data_frag[0] = (1).to_bytes(1, byteorder='big')
            bad_packet = -1
        

        #print("     Obsah:",data_frag)

        c_socket.sendto(set_data_packet(4, i, crc, data_frag), s_address)

        #print("     Obsah:",set_data_packet(4, i, crc, data_frag))
        #print()
        c_socket.settimeout(20)
        try:
            response, address = c_socket.recvfrom(1500)
        except(socket.timeout):
            print("server neodpoveda")
            return

        p_type, packet_count, fragmentation_size, file_name = decode_information_packet(response)
        #print("             ",decode_information_packet(response))
        print("     Odpoved:",p_type)
        print()
        
        if p_type == 6:
            while attempts < max_attempts:
                print("     Paket c.:", i + 1, "Chybne dorucene data, odosielam nanovo.")
                print("         Pokus c.:",attempts + 1)
                #crc = 250
                crc = crc16(data_frag)
                c_socket.sendto(set_data_packet(4, i, crc, data_frag), s_address)
                response, address = c_socket.recvfrom(1500)
                p_type, packet_count, fragmentation_size, file_name = decode_information_packet(response)
                print("     Odpoved:",p_type)
                print() 
                #print("             ",decode_information_packet(response))
                #data OK
                if p_type == 5:
                    break

                attempts += 1 
            #paket nebol doruceny spravne niekolko krat, ukoncujem spojenie
            if attempts == max_attempts:
                c_socket.sendto(set_information_packet(7), s_address)
                break
    
                
            
def client_menu():
    while True:
        print()
        print("Dostupné možnosti:")
        print("     1 - Poslanie spravy")
        print("     2 - Poslanie suboru")
        print("     3 - Zmenit rolu")
        print("     0 - Pre ukončenie")

        try:
            menu_option = int(input('Zadaj možnosť: '))
        except ValueError:
            print("Chyba pri zadavani.")
            continue

        if menu_option == 1:
             return menu_option
        elif menu_option == 2:
            return menu_option 
        elif menu_option == 3:
            return menu_option 
        elif menu_option == 0:
            exit()
        else:
            print("Zadal si zlú možnosť.")
    
#============server=============

def server_initialization():
    try:
        s_ip = ""
        s_port = get_port("serveru")
        s_address = (s_ip, s_port)

        s_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
        s_socket.bind(s_address)
        print("Pocuvam na porte",s_socket.getsockname()[1])

        s_socket.settimeout(30)
        data,c_address = s_socket.recvfrom(1500)
        #print(data, c_address)
        s_socket.sendto(set_information_packet(1), c_address)
        print("prvy inic zo serveru",set_information_packet(1))

    except(socket.timeout) as err:
        print(err)
        print("Neprislo pripojenie na port {0}".format(s_port))
        print()
        print("Navrat do menu...")
    
    else:
        print("Nadviazané spojenie z adresy", c_address[0], c_address[1])
        print()
        server(s_socket, c_address)


def server(s_socket, c_address):
    first_run = True
    #contin_run = False
    option = -1

    print("Pocuvam na porte",s_socket.getsockname()[1])

    while True:
        if not first_run:
            option = server_menu()
            

        if option == 2:
            os.system('cls')
            print("Rola zmenena")
            client(s_socket, c_address)


        if option == 0:
            print("Ukoncujem")
            s_socket.close()
            return
        
        try:
            s_socket.settimeout(20)
            while True:
                
                data, c_address = s_socket.recvfrom(1500)
                
                print("info paket na servery...", data)
                p_type, packet_count, fragmentation_size, file_name = decode_information_packet(data)

                if p_type == 0:
                    print("Prisiel keep alive")
                    s_socket.sendto(set_information_packet(0),c_address)
                
                #pridu data
                if p_type == 2 or p_type == 3:
                    s_socket.sendto(set_information_packet(0),c_address)
                    print("pridu data... pocet paketov",packet_count,"fragmentacia",fragmentation_size)
                    
                    data_obtaining(s_socket, c_address, p_type, packet_count, fragmentation_size, file_name)
                    first_run = False
                    break
                    


                if p_type == 7:
                    print("Klient ukoncil komunikaciu")
                    s_socket.close()
                    return
                    
        except(socket.timeout):
            print("Client neodpoveda, vypinam pocuvanie...")
            s_socket.close()
            return
        
        

def data_obtaining(s_socket, c_address, data_type, packet_count, fragmentation_size, file_name):
    if data_type == 2:
        print("Pride sprava")
    if data_type == 3:
        print("Pride subor")
        direct_name = input("Zadaj cestu, kde sa ma subor ulozit: ")
        while not os.path.exists(direct_name):
            direct_name = input("Zadaj cestu, kde sa ma subor ulozit: ")

        if direct_name != '.':
            file_name = os.path.join(direct_name, file_name)

    obtained = 0
    whole_data = []

    while True:
        if obtained == packet_count:
            break
        
        print("Paket c.:",obtained + 1)

        data_undecoded, address = s_socket.recvfrom(1500)
        p_type, packet_id, c_crc, data = decode_data_packet(data_undecoded)

        #s_crc = 250
        s_crc = crc16(data)
        #print("crc", crc16(data))

        if s_crc != c_crc:
            print("     Je zly, posielam spravu na znovu obdrzanie")
            s_socket.sendto(set_information_packet(6), c_address)
            continue

        #sprava
        if data_type == 2:
            whole_data.append(data.decode())
        #subor
        if data_type == 3:
            whole_data.append(data)


        print("     Je v poriadku")
        s_socket.sendto(set_information_packet(5), c_address)
        obtained += 1
    
    #sprava
    if data_type == 2:
        print("Prijata sprava:",''.join(whole_data))
    #subor
    if data_type == 3:
        #for data_frag in whole_data:
        file_data = b''.join(whole_data)
        with open(file_name, "wb") as f:
            f.write(file_data)
            
        print("Subor sa ulozil ",file_name)





def server_menu():
    while True:
        print()
        print("Dostupné možnosti:")
        print("     1 - Pokracovanie")
        print("     2 - Zmenit rolu")
        print("     0 - Pre ukončenie")

        try:
            menu_option = int(input('Zadaj možnosť: '))
        except ValueError:
            print("Chyba pri zadavani.")
            continue

        if menu_option == 1:
             return menu_option
        elif menu_option == 2:
            return menu_option 
        elif menu_option == 0:
            exit()
        else:
            print("Zadal si zlú možnosť.")


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




def menu():
    while True:
        print()
        print("Dostupné možnosti:")
        print("     1 - Odosielatel")
        print("     2 - Prijemca")
        print("     0 - Pre ukončenie")

        try:
            menu_option = int(input('Zadaj možnosť: '))
        except ValueError:
            print("Chyba pri zadavani.")
            continue

        if menu_option == 1:
            client_initialization()   
        elif menu_option == 2:
            server_initialization() 
        elif menu_option == 0:
            exit()
        else:
            print("Zadal si zlú možnosť.")

def main():
    name_print()
    menu()
    

if __name__ == '__main__':
    main()