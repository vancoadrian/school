 #!/usr/bin/env python

import random
import numpy as np
import collections
import queue
import time

class Node:
    def __init__(self, state, nothing, parent = None, move = 0, depth = 0):
        self.state = state  #dostane 1D typ tuple repreyentujuci stav hru 
        self.parent = parent    #referencia na rodica
        self.depth = depth  #hlbka uzla
        self.move = move    #vykonana operacia
        self.nothing = nothing  #pozicia prazdneho policka
            #prevedie tuple na string
        if self.state:  
            self.map =  ''.join(str(c) for c in self.state)
        

    

#hlavička programu
def name_print():
    print()
    print("------------Zadanie 2: 8-hlavolam------------")
    print("-------------------Uloha c)------------------")

    print()
    print("            Autor: Adrián Vančo              ")
    print()


#nacita rozmer hry
def load_rows_and_colums():
    rows = input('Zadaj pocet riadkov: ')
    while (not rows.isdigit()) or (int(rows) < 1):
        print("Zly vstup")
        rows = input('Zadaj pocet riadkov: ')
    rows = int(rows)

    columns = input('Zadaj pocet stlpcov: ')
    while (not columns.isdigit()) or (int(columns) < 1):
        print("Zly vstup")
        columns = input('Zadaj pocet stlpcov: ')
    columns = int(columns)

    return rows, columns


#vracia ci su inverzie parne alebo neparne
def solvable(puzzle, rows, columns):
    inv = 0

    nothing_index = puzzle.index(0)
    #pocitanie inverzov
    for i in range(rows*columns):
        for j in range(i + 1, rows*columns):
            if all((puzzle[i] > 0, puzzle[j] > 0, puzzle[i] > puzzle[j])):
                inv += 1
    #pripocitanie cisla riadku prazdneho policka           
    inv += (nothing_index // columns)
    return inv % 2 == 0


#funkcia na reprezentaciu operatorov DOLE, HORE, VPRAVO, VLAVO (ak je to pre dany operator mozne vytvori tuple s vykonanim operatora)
def move(state : tuple, direction, rows, columns):
        #index_nothing je pozicia prazdneho miesta
    index_nothing = state.index(0)
        #prevedie tuple na pole pre moznost vykonania operatorov
    arr_state = np.asarray(state)
    
    #DOLE
    if direction == 'DOLE':
        arr_state[index_nothing], arr_state[index_nothing - columns] = arr_state[index_nothing - columns], arr_state[index_nothing]
        return tuple(arr_state)
    
    #VLAVO
    elif direction == 'VLAVO':
        arr_state[index_nothing], arr_state[index_nothing + 1] = arr_state[index_nothing + 1], arr_state[index_nothing]
        return tuple(arr_state)
    
    #HORE
    elif direction == 'HORE':
        arr_state[index_nothing], arr_state[index_nothing + columns] = arr_state[index_nothing + columns], arr_state[index_nothing]
        return tuple(arr_state)
    
    #VPRAVO
    else:
        arr_state[index_nothing], arr_state[index_nothing - 1] = arr_state[index_nothing - 1], arr_state[index_nothing]
        return tuple(arr_state) 


#funkcia na vytvorenie potomkov
def child_nodes(actual : Node, rows, columns):

    child_queue = collections.deque()
    index_nothing = actual.nothing

    #DOLE
    if actual.nothing >= columns:
        child_queue.append(Node(move(actual.state, 'DOLE', rows, columns), index_nothing - columns, actual, 4, actual.depth + 1))
    
    #VLAVO 
    if (actual.nothing % columns != columns - 1):
        child_queue.append(Node(move(actual.state, 'VLAVO', rows, columns), index_nothing + 1, actual, 3, actual.depth + 1))
   
    #HORE
    if (actual.nothing < (rows - 1) * columns):
        child_queue.append(Node(move(actual.state, 'HORE', rows, columns), index_nothing + columns, actual, 1, actual.depth + 1))
    
    #VPRAVO
    if (actual.nothing % columns) != 0:
        child_queue.append(Node(move(actual.state, 'VPRAVO', rows, columns), index_nothing - 1, actual, 2, actual.depth + 1))
    
    return child_queue


#vykresli stav hry
def print_game(str,rows,columns):
    for i in range(0,rows):
        for j in range(0,columns):
            print(str[i * columns + j], end = ' ')
        print()
    print()


#funkcia na obojsmerne hladanie
def Bidirectional_BFS(start_state, dest_state,rows,columns):
        #premenne pre zakladne info
    search_depth = 0
    max_queue_size = 0

        #zaciatocne naplnenie queues a nastavenie setov
    start_queue = collections.deque([Node(start_state, start_state.index(0))])
    start_visited = set()

    dest_queue = collections.deque([Node(dest_state, dest_state.index(0))])
    dest_visited = set()

        #pomocna pre zistenie ci sa nasiel prienik
    intersec = 0

        #pociatocne vlozenie do setov
    start_visited.add(start_queue[0].map)
    dest_visited.add(dest_queue[0].map)

        #cyklus na hladanie riesenia
    while start_queue and dest_queue and intersec == 0:
            #popnutie stavu z queues
        actual_start = start_queue.popleft()
        actual_dest = dest_queue.popleft()

        #rozvatvenie
        posible_nodes_start = child_nodes(actual_start, rows, columns)
        posible_nodes_dest = child_nodes(actual_dest, rows, columns)

        #rozvoj od startovacej pozicie
        for node in posible_nodes_start:
            if node.map not in start_visited:   #ak uz taky stav hracej plochy bol tak ho preskoci, uklada len nove
                start_queue.append(node)        #pridanie do queue
                start_visited.add(node.map)
                if node.depth > search_depth:   #ukladanie maximalnej hlbky aka bola zatial dosiahnuta
                    search_depth += 1 
        if len(start_queue) > max_queue_size:   #ukladanie maximalneho poctu stavov, ktory bol dosiahnuty
            max_queue_size = len(start_queue)

        #rozvoj od cielovej pozicie
        for node in posible_nodes_dest:
            if node.map not in dest_visited:    #ak uz taky stav hracej plochy bol tak ho preskoci, uklada len nove
                dest_queue.append(node)         #pridanie do queue
                dest_visited.add(node.map)      
                if node.depth > search_depth:
                    search_depth += 1 
        if len(dest_queue) > max_queue_size:
            max_queue_size = len(dest_queue)

        intersec = len(set(start_visited) & set(dest_visited))
        if intersec >= 1:
                #hladanie prieniku a nasledne vypisanie operatorov
            for map_s in start_visited:
                for map_d in dest_visited:
                    if map_s == map_d:      #ak sa nasli dva rovnake stavy tak sa ide hladat objekty k tymto stavom 
                        for node_s in start_queue:
                            if node_s.map == map_s:
                                temp_s = node_s

                        for node_d in dest_queue:
                            if node_d.map == map_d:
                                temp_d = node_d

                            #ziskanie krokov pre startovaciu vetvu (lifo lebo potrebuje sa vypisat opacne postup krokov)
                        q_s_lifo = queue.LifoQueue()
                        while temp_s.parent != None:
                            q_s_lifo.put(temp_s.move)
                            temp_s = temp_s.parent

                            #ziskanie krokov pre cielovu vetvu
                        q_d_fifo = queue.Queue()
                        while temp_d != None:
                            q_d_fifo.put(temp_d.move)
                            temp_d = temp_d.parent
                        
                        #vypisovanie krokov pre startovaciu vetvu
                        while not q_s_lifo.empty():
                            temp = q_s_lifo.get()
                            if temp == 1:
                                print("Hore ",end='')
                            elif temp == 2:
                                print("Vpravo ",end='')
                            elif temp == 3:
                                print("Vlavo ",end='')
                            elif temp == 4:
                                print("Dole ",end='')


                        #q_d_fifo.get() #popnem spolocny

                        #vypisovanie krokov pre cielovu vetvu, hodnoty musia byt obratene
                        while not q_d_fifo.empty():
                            temp = q_d_fifo.get()
                            #reverz posunu (opacne treba)
                            if temp == 1:
                                print("Dole ",end='')
                            elif temp == 2:
                                print("Vlavo ",end='')
                            elif temp == 3:
                                print("Vpravo ",end='')
                            elif temp == 4:
                                print("Hore ",end='')

                        #print("Maximalna hlbka: ", search_depth)
                        #print("Maximalny pocet stavov v queue: ",max_queue_size)

                        break
    
    if intersec == 0:
        print("Neuspech")                
  

#funkcia pre algoritmus bfs
def BFS(start_position, end_position, rows, columns):
    search_depth = 0
    max_queue_size = 0

    queue = collections.deque([Node(start_position, start_position.index(0))])
    visited = set()
   
    

    while queue:
        actual = queue.popleft()
        visited.add(actual.map)
            #navrat ak sme nasli cielovy stav
        if actual.state == end_position:
            return actual
            #vytvorenie potomkov
        posible_nodes = child_nodes(actual, rows, columns)
            #prechadzanie potomkov
        for node in posible_nodes:  
            if node.map not in visited:
                queue.append(node)
                visited.add(node.map)
                if node.depth > search_depth:   
                    search_depth += 1 
        if len(queue) > max_queue_size:
            max_queue_size = len(queue)

        
    #print("Maximalna hlbka: ", search_depth)
    #print("Maximalny pocet stavov v queue: ",max_queue_size)
        


def find_solution(pick_start,pick_end,rows, columns, bfs = True):
        #ak je mozne riesit ak niesu stavy zhodne
    if solvable(pick_start,rows, columns) == solvable(pick_end,rows, columns):
        if pick_start != pick_end:    
            print("Idem riesit...")
            print("Zaciatok: ")
            print_game(pick_start,rows,columns)
            print("Koniec: ")
            print_game(pick_end,rows,columns)

            print("Bidirectional_BFS")

            timer = 0.00000000
            start = time.time()

            Bidirectional_BFS(pick_start, pick_end,rows,columns)

            end = time.time()
            timer += end - start
            print("Cas: ", timer)

            #ak sa zvoli aj moznost s obicajnym bfs
            if bfs:
                print("BFS:")

                timer = 0.00000000
                start = time.time()

                final_node = BFS(pick_end, pick_start, rows, columns)

                end = time.time()
                timer += end - start
                print("Cas: ", timer)

            
                while final_node.state != pick_end:
                    #reverz posunu (opacne treba)
                    if final_node.move == 1:
                        print("Dole ",end='')
                    elif final_node.move == 2:
                        print("Vlavo ",end='')
                    elif final_node.move == 3:
                        print("Vpravo ",end='')
                    elif final_node.move == 4:
                        print("Hore ",end='')
                    final_node = final_node.parent



def tester():
    rows, columns = load_rows_and_colums()

    #   load_data()
    list_to_be_shuffled = list(range(0, rows * columns))
    list_of_tuples =[]
    to_be_generated = 200
    for i in range(0, to_be_generated):
        list_of_tuples.append(tuple(random.sample(list_to_be_shuffled,len(list_to_be_shuffled))))

    #print(list_of_tuples)

    how_much_iterations = input('Zadaj pocet vykonani: ')
    while (not how_much_iterations.isdigit()) or (int(how_much_iterations) < 1):
        print("Zly vstup")
        how_much_iterations = input('Zadaj pocet vykonani: ')
    how_much_iterations = int(how_much_iterations)


    for i in range(0,how_much_iterations):
        pick_start = list_of_tuples[i % to_be_generated]
        pick_end = list_of_tuples[(i + random.randrange(to_be_generated)) % to_be_generated]
        find_solution(pick_start,pick_end,rows, columns)
        

    
def input_correct(input_puzzle, rows, columns):
    
    saved_input =list(map(int, input_puzzle.split()))

    if len(saved_input) != rows * columns:
        return False
    #vytvorim si list ktory ma vsetky pozadovane cisla, a pomocou neho overim vstup
    state = list(range(0, rows * columns))
    #print(saved_input)

    # hladam vsetky hodnoty ktore tam maju byt
    result = all(elem in saved_input for elem in state)
    if result:
        return True
    else:
        return False
    

def load_puzzle():
    rows, columns = load_rows_and_colums()

    print("Zadaj zaciatocne rozlozenie, ",rows*columns," cisel odelene medzerou, nula reprezentuje prazdne policko (cisla od 0 po ", columns * rows - 1," vratane.")
    start_state = input()
    while not input_correct(start_state,rows,columns):
        print("Chybny vstup.")
        print("Zadaj zaciatocne rozlozenie, ",rows*columns," cisel odelene medzerou, nula reprezentuje prazdne policko (cisla od 0 po ", columns * rows - 1," vratane.")
        start_state = input()
    start_state = tuple(map(int, start_state.split()))



    print("Zadaj cielove rozlozenie, ",rows*columns," cisel odelene medzerou, nula reprezentuje prazdne policko (cisla od 0 po ", columns * rows - 1," vratane.")
    end_state = input()
    while not input_correct(end_state,rows,columns):
        print("Chybny vstup.")
        print("Zadaj cielove rozlozenie, ",rows*columns," cisel odelene medzerou, nula reprezentuje prazdne policko (cisla od 0 po ", columns * rows - 1," vratane.")
        end_state = input()
    end_state = tuple(map(int, end_state.split()))

    print(start_state)
    print(end_state)
    find_solution(start_state, end_state, rows, columns)
    

    




#funkcia na zvolenie co sa ma vykonat
def menu():
    while True:
        
        print()
        print("Dostupné možnosti:")
        print("     1 - Zadat hlavolam na vyriesenie")
        print("     2 - Tester")
        print("     0 - pre ukončenie")

        menu_option = int(input('Zadaj možnosť: '))


        if menu_option == 1:
            load_puzzle()
        elif menu_option == 2:
            tester()
        elif menu_option == 0:
            exit()
        else:
            print("Zadal si zlú možnosť.")




def main():
    name_print()
    menu()
    

if __name__ == '__main__':
    main()