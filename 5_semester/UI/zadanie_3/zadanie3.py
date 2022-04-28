 #!/usr/bin/env python
import numpy as np
import time
import random
import os

class Monk:
    def __init__(self, fitness, decisions : list, steps : list, spawns : list):
        self.fitness = fitness  #pocet policok ktore pohrabal
        self.decisions = decisions  #list rozhodnuti [1,-1,1,...]
        self.steps = steps  #list krokov, ktore vykonal, (startovacia pozicia monka steps.[0])
        self.spawns = spawns #list krajnych policok, kde moze vstupit do zahrady

#=====================
#=====================
#=====nastavenia======
#=====================
#=====================

decisions_size = 4                 #urcuje aj pocet susedov s zmenenymi rozhodnutiami (nesmie byt 0)
number_of_monks_random_start = 4    #pocet susedov s rovnakymi rozhodnutiami ale inym startom
tabu_list_size = 20                 #velkost tabu listu
annealing_temperature = 40          #teplota simulovaneho zihania
annealing_cooling = 0.25             #o kolko sa teplota ochladi po cykle
iterations_of_algorithms = 200      #maximalny pocet vykonani kazdeho algoritmu



#=====================
#=====================
#====nacitanie dat====
#=====================
#=====================

def load_garden_size():
    #nacitanie rozmerov
    while True:
        try:
            rows = int(input("Zadaj pocet riadkov: "))
            columns = int(input("Zadaj pocet stlpcov: "))
        except ValueError:
            print("Chyba pri zadavani.")
            continue
        else:
            if rows <= 0 or columns <= 0:
                print("Zadaj kladne rozmery.")
                continue
            return (rows, columns)

def load_stone_count(rows, columns):
    #nacitanie poctu kamenov
    while True:
        try:
            stone_count = int(input("Zadaj pocet kamenov (max: {0}): ".format(rows*columns -1 )))
            if stone_count > rows*columns-1:
                print("Zadal si viac.")
                continue
        except ValueError:
            print("Chyba pri zadavani.")
            continue
        else:
            if stone_count < 0:
                print("Zadaj kladne pocet.")
                continue
            return stone_count

def load_stones_positions(count, r, c):
    #nacitanie suradnic kamenov
    stones = []
    rows = r
    columns = c
    stone_count = count
    while stone_count > 0:
        print("Zadaj suradnice kamena x (0 az {0}) y (0 az {1}): ".format(columns-1,rows-1))
        try:
            x = int(input("x: "))
            y = int(input("y: "))
            stone = (y,x)
        except ValueError:
            print("Chyba pri zadavani.")
        else:
            if x < 0 or x >= columns or y < 0 or y >= rows:
                print("Zadaj spravne suradnice kamena.")
                continue
            stone_count -= 1
            stones.append(stone)
    return stones
    #print(stones)

#nacitanie dat zo suboru
def load_data_from_file():

    filename = input("Zadaj cestu k súboru alebo \"exit\" pre ukončenie programu: ")
    #ziskanie nazvu suboru
    while not os.path.exists(filename) or not filename.endswith('.txt'):
        if filename == 'exit':
            exit()
        print("Zlá cesta alebo zly typ súboru.")
        filename = input("Zadaj cestu k súboru alebo \"exit\" pre ukončenie programu: ")
        os.path.exists(filename)

    #citanie suboru
    with open(filename,"r") as file:
        rows, columns = [int(x) for x in next(file).split()] #prvy riadok
        stones = []
        for line in file: #ostatne 
            stone = line.split()
            stones.append([int(stone[0]),int(stone[1])])

    return (rows, columns, stones)


#=====================
#=====================
#=====vytvaranie======
#=====================
#=====================

def create_garden(rows, columns, stones):
    #inicializacia zahrady
    garden = []
    for row in range(rows):
        column = []
        for col in range(columns):
            column.append(0)
        garden.append(column)

    #vlozenie kamenov 
    for stone in stones:
        garden[stone[0]][stone[1]] = -1
    
    return garden

def get_copy_garden(garden):
    garden_copy = [list(row) for row in garden]
    return garden_copy

# def get_garden_with_those_steps(garden,steps):
#     garden_c = get_copy_garden(garden)
#     for step in steps:
#         garden_c[step[0]][step[1]] = 1
#     return garden_c

#funkcia pre vytvorenie prveho monka
def create_monk(start_tile, spawns):
    decisions = []
    #nahodne vygenerovanie rozhodnuti (-1 -otocenie vlavo 1otocenie vpravo)
    for decision in range(decisions_size):
        decisions.append(random.choice([-1, 1]))
    
    spawns_c = list(spawns)
    monk = Monk(0,decisions,[start_tile],spawns_c)

    return monk

#funkcia pre vytvorenie suseda
def get_new_monk_neighbor(decisions, start_tile, spawns):
    monk_c = Monk(0, list(decisions), [start_tile], list(spawns))
    return monk_c


def generate_stones(count, rows, columns):
    stones = []
    for i in range(count):
        stone = [random.randrange(0,rows), random.randrange(0,columns)]
        while stone in stones:
            stone = [random.randrange(0,rows), random.randrange(0,columns)]
        stones.append(stone)
    return stones        

def create_input_file(rows, columns, stones):
    file = open("input.txt","w")
    file.write("{0} {1}\n".format(rows, columns))
    file = open("input.txt","a")
    for stone in stones:
        file.write("{0} {1}\n".format(stone[0], stone[1]))
    file.close()

#=====================
#=====================
#===pomocne funkcie===
#=====================
#=====================

def get_garden_maximum_fitness(garden):
    fitness = 0
    for row in garden:
        for col in row:
            if col != -1:
                fitness += 1 
    return fitness

#funkcia na ziskanie krajnych policok, kde nieje kamen
def get_possible_spawns(garden):
    possible_spawns = []
    
    rows = len(garden) - 1
    columns = len(garden[0]) - 1
    #vzorec na vypocitanie poctu okrajovych policok (rows + 1) * 2 + (columns + 1) * 2 - 4

    for row in range(rows + 1):
        for col in range(columns + 1):
            #ak to je krajne policko
            if spawn_tile([row,col], rows + 1, columns + 1):
                #kontrola ci tam nieje kamen
                if garden[row][col] != -1:
                    #pridavanie do listu [y,x,index policok]
                    if row == 0:
                        possible_spawns.append([row,col, row + col + 1])
                    elif row == rows:
                        possible_spawns.append([row,col, (rows + 1) * 2 + (columns + 1) * 2 - 4 - row + 1 - col])
                    elif col == 0:
                        possible_spawns.append([row,col, (rows + 1) * 2 + (columns + 1) * 2 - 4 - row + 1])
                    else:
                        possible_spawns.append([row,col, row + col + 1])   
    #print(possible_spawns)
    return possible_spawns

#funkcia na overenie ci policko je v rohu
def corner_tile(tile, rows, columns):
    if tile[0] == 0:
        if tile[1] == 0:
            return True
        if tile[1] == columns-1:
            return True
    elif tile[0] == rows -1:
        if tile[1] == 0:
            return True
        if tile[1] == columns -1:
            return True
    return False

#funkcia na overenie ci policko je na okraji
def spawn_tile(tile, rows, columns):
    if tile[0] == 0 or tile[0] == rows -1:
        return True 
    if tile[1] == 0 or tile[1] == columns -1:
            return True
    return False

# def get_number_of_stones(garden):
#     count = 0
#     for row in garden:
#         for col in row:
#             if col == -1:
#                 count += 1
#     return count

#funkcia na urcenie smeru pre startovacie policko
def get_direction(tile, rows, columns, monk : Monk, decision_index):
    r = rows - 1
    c = columns - 1
    #smer
    left = 0
    up = 1
    right = 2
    down = 3

    #up side
    if tile[0] == 0:
        #left up corner
        if tile[1] == 0:
            if monk.decisions[decision_index] == 1: #bud dole alebo vpravo
                return down #dole
            else:
                return right #vpravo
        #right up corner
        elif tile[1] == c:
            if monk.decisions[decision_index] == 1: #bud dole alebo vlavo
                return down #dole
            else:
                return left #vlavo 
        else:
            return down #dole
    #down side
    elif tile[0] == r:
        #left down corner
        if tile[1] == 0:
            if monk.decisions[decision_index] == 1: #bud hore alebo vpravo
                return up #hore
            else:
                return right #vpravo 
        #right down corner
        elif tile[1] == c:
            if monk.decisions[decision_index] == 1: #bud hore alebo vlavo
                return up #hore
            else:
                return left #vlavo 
        else:
            return up #hore
    #left side without corners
    elif tile[1] == 0:
        return right #vpravo
    #right side without corners
    else:
        return left #vlavo

#funkcia na ziskanie policka pre dalsi vstup do zahrady
def get_next_nearest_spawn(tile, spawns : list, rows, columns, garden):
    maximum_spawn_number = rows * 2 + columns * 2 - 4
    tile_number_incr = tile[2] + 1
    tile_number_derc  = tile[2] - 1

    for i in range(int((maximum_spawn_number / 2))):
        if tile_number_derc == 0:
            tile_number_derc = maximum_spawn_number
        for spawn in spawns:
            if garden[spawn[0]][spawn[1]] != 0:
                    spawns.remove(spawn)
                    continue
            elif spawn[2] == tile_number_incr:
                return spawn
            elif spawn[2] == tile_number_derc:
                return spawn
        tile_number_incr += 1
        tile_number_derc -= 1
    return []
    
#funkcia na overenie ci pole moze byt pohrabane
def tile_can_be_raked(garden, pos, rows, columns):
    x = pos[1]
    y = pos[0]
    #sme vonku zo zahrady
    if x < 0 or y < 0 or x >= columns or y >= rows:
        return -1
    #nebolo pohrabane, takze moze byt pohrabane
    if garden[y][x] == 0:
        return 1
    #bolo uz pohrabane alebo tam je kamen
    return 0

#funkcia na hrabanie zahrady
def rake_garden(garden, monk : Monk, rows, columns):
    #vytvori sa kopia zahrady pre hrabanie
    garden_c = get_copy_garden(garden)
    #ziskanie prveho pristupu do zahrady
    tile = monk.steps[0]
    #pomocna na krokovanie kolko krokov spravil pokial nevysiel alebo neskoncil zaseknutim
    steper = 0
    #pomocna pre rozhodnutie ci sa ide znova vstupovat do zahrady
    next_raking = False

    decision_index = 0 #ukazovatel na dane rozhodnutie
    number_of_decisions = len(monk.decisions)   #rozhodnutia mnicha
    direction = get_direction(tile, rows, columns, monk, decision_index)    #ziskanie smeru, ktorym ma mnich ist

    rotations =np.array([
        (0, -1),    #left
        (-1, 0),     #up
        (0, 1),     #right
        (1, 0)     #down
    ])

    number_of_rotations = len(rotations)
    rake_number = 1 #oznacovac hrabania

    #hrabanie
    while True:

        #tam kde je mnich tam to pohrabe
        garden_c[tile[0]][tile[1]] = rake_number
        #zvisim fitness
        monk.fitness += 1
        #zvisim pocet vykonani pre spawn
        steper += 1

        #ak moze ist v damom smere
        if tile_can_be_raked(garden_c, [tile[0] + rotations[direction][0],tile[1] + rotations[direction][1]], rows, columns) == 1:

            tile = [tile[0] + rotations[direction][0],tile[1] + rotations[direction][1]] #posunie sa
            monk.steps.append(direction)    #prida pohyb, ktory vykonal
            continue

        #ak nemoze tak zmena smeru
        if tile_can_be_raked(garden_c, [tile[0] + rotations[direction][0],tile[1] + rotations[direction][1]], rows, columns) == 0:
            #ziskanie inforacii ci je policko vlavo a v pravo volne (nepohrabane)
            can_turn_left = tile_can_be_raked(garden_c, [tile[0] + rotations[(direction - 1) % number_of_rotations][0],tile[1] + rotations[(direction - 1) % number_of_rotations][1]], rows, columns)
            can_turn_right = tile_can_be_raked(garden_c, [tile[0] + rotations[(direction + 1) % number_of_rotations][0],tile[1] + rotations[(direction + 1) % number_of_rotations][1]], rows, columns)
            
            #rozhodnutie do ktore smeru sa monk vyda
            if can_turn_left == 1 and can_turn_right == 1:
                
                direction = (direction + monk.decisions[decision_index]) % number_of_rotations  #zmena smeru podla rozhodnutia
                tile = [tile[0] + rotations[direction][0],tile[1] + rotations[direction][1]]    #posunie sa
                decision_index = (decision_index + 1) % number_of_decisions     #zvisenie ukozovatela na rozhodnutie
                monk.steps.append(direction)    #prida pohyb, ktory vykonal
                continue
            
            #ak nema na vyber tak ide do mozneho smeru
            if can_turn_left == 1:
               
                direction = (direction - 1) % number_of_rotations   #zmena smeru
                tile = [tile[0] + rotations[direction][0],tile[1] + rotations[direction][1]]    #posunie sa
                monk.steps.append(direction)    #prida pohyb, ktory vykonal
                continue

            if can_turn_right == 1:
                
                direction = (direction + 1) % number_of_rotations   #zmena smeru
                tile = [tile[0] + rotations[direction][0],tile[1] + rotations[direction][1]]    #posunie sa
                monk.steps.append(direction)    #prida pohyb, ktory vykonal
                continue


        #ak nemohol ani zmenit smer tak novy spawn (vstup do zahrady)
        #kontrola ci sa neskoncilo niekde inde ako na okraji alebo hned na spawne na zaciatku, ktory nieje v rohu
        if  steper != 1 and spawn_tile(tile, rows, columns) or steper == 1 and corner_tile(tile, rows, columns):
            for spawn in monk.spawns:
                if spawn[0] == tile[0] and spawn[1] == tile[1]:
                    monk.steps.append(spawn)    #pridame vyjdenie zo zahrady do krokov mnicha
                    monk.spawns.remove(spawn)   #vyhodime policko z listu pre mozne vstupy do zahrady
                    #ziskanie noveho policka pre vstup do zahrady
                    tile = get_next_nearest_spawn(spawn, monk.spawns, rows, columns, garden_c)
                    #ak uz sme pouzili vsetky 
                    if len(tile) == 0: 
                        break
                    
                    next_raking = True  #pojde sa dalej hrabat z ineho vstupu
                    direction = get_direction(tile, rows, columns, monk, decision_index)    #ziskanie smeru
                    rake_number += 1    #zvysenie oznacovaca hrabania
                    steper = 0  #vynulovanie krokov
                    monk.steps.append(tile) #pridanie vstupu
                    break
       
        #pokracovanie v hrabani od noveho spawnu
        if next_raking == True: 
            next_raking = False
            continue

        break

    return garden_c


def get_neighbors(garden, monk: Monk, start_tile, spawns : list, rows, columns, sorting):
    
    neighborhood = []
    #generovanie susedov so zmenenym jednym rozhodnutim
    for i in range(len(monk.decisions)):
        monk_neighbor = get_new_monk_neighbor(monk.decisions, start_tile, spawns)
        monk_neighbor.decisions[i] *= -1
        rake_garden(garden, monk_neighbor, rows, columns)
        neighborhood.append(monk_neighbor)

    #generovanie susedov s inym startovacim polickom
    if len(spawns) != 0:
        for i in range(number_of_monks_random_start % len(spawns)):
            new_start_tile = spawns[i]
            monk_neighbor = get_new_monk_neighbor(monk.decisions, new_start_tile, spawns)
            rake_garden(garden, monk_neighbor, rows, columns)
            neighborhood.append(monk_neighbor)

    #ak chcem aby mnich s najlepsim fitness bol prvy
    if sorting:
        neighborhood.sort(key=lambda x: x.fitness, reverse=True)

    return neighborhood

def choose_alg(garden, start_tile, rows, columns, spawns):
    print("Dostupne algoritmy:")
    print("     1 - Hill climbing")
    print("     2 - Tabu search")
    print("     3 - Simulated annealing")
    while True:
        try:
            option = int(input("Zadaj cislo algoritmu: "))
        except ValueError:
            print("Chyba pri zadavani.")
            continue
        if option == 1:
            return hill_climb(garden, start_tile, rows, columns, spawns)
        elif option == 2:
            return  tabu_search(garden, start_tile, rows, columns, spawns)
        elif option == 3:
            return  simulated_annealing(garden, start_tile, rows, columns, spawns)
                

#=====================
#=====================
#======algoritmy======
#=====================
#=====================

def hill_climb(garden, start_tile, rows, columns, spawns):

    map_fitness = get_garden_maximum_fitness(garden)    #ziskanie fitness mapy
    first_monk = create_monk(start_tile, spawns)    #vytvorenie mnicha
    rake_garden(garden,first_monk, rows, columns)   #pohrabanie 
    
    best_monk = first_monk
    best_candidate_monk = first_monk
    
    sorting = True  #pomocou zapnuteho sortingu je najlepsi vzdy na [0] indexe
    i=0
    #cyklus na hladanie najlepsieho mnicha
    while (best_monk.fitness != map_fitness and i != iterations_of_algorithms):
        monk_neighborhood = get_neighbors(garden, best_candidate_monk, best_candidate_monk.steps[0], spawns, rows, columns, sorting)
        best_candidate_monk = monk_neighborhood[0]  #so zapnutym sortingom je najlepsi hned na zaciatku
        if best_candidate_monk.fitness > best_monk.fitness: #ak je lepsi ako globalny
            best_monk = best_candidate_monk
        else:
            break
        i+=1
    
    return best_monk


def tabu_search(garden, start_tile, rows, columns ,spawns):

    map_fitness = get_garden_maximum_fitness(garden)    #ziskanie fitness mapy
    first_monk = create_monk(start_tile, spawns)    #vytvorenie mnicha
    rake_garden(garden,first_monk, rows, columns)   #pohrabanie 

    best_monk = first_monk
    best_candidate_monk = first_monk

    tabu_list = []
    tabu_list.append([best_candidate_monk.steps[0],best_candidate_monk.decisions])  #vlozenie do tabu listu

    sorting = True  #pomocou zapnuteho sortingu je najlepsi vzdy na [0] indexe
    i=0
    while (best_monk.fitness != map_fitness and i != iterations_of_algorithms):
        monk_neighborhood = get_neighbors(garden, best_candidate_monk, best_candidate_monk.steps[0], spawns, rows, columns, sorting)
        for candidate_monk in monk_neighborhood:
            if [candidate_monk.steps[0],candidate_monk.decisions] not in tabu_list:
                if candidate_monk.fitness > best_candidate_monk.fitness:
                    best_candidate_monk = candidate_monk
                    break   #so zapnutym sortingom je najlepsi hned na zaciatku preto mozeme dat break
                else:
                    tabu_list.append([best_candidate_monk.steps[0],best_candidate_monk.decisions])
                    best_candidate_monk = candidate_monk
                    break
        
        if best_candidate_monk.fitness > best_monk.fitness: #porovnanie s globalne najlepsim
            best_monk = best_candidate_monk

        if len(tabu_list) > tabu_list_size: #kontrola ci sa nepresiahla pri vkladani velkost tabu listu
            tabu_list.pop(0)    #ak ano vyhodim prveho
        
        i+=1
    
    return best_monk


def simulated_annealing(garden, start_tile, rows, columns ,spawns):

    map_fitness = get_garden_maximum_fitness(garden)    #ziskanie fitness mapy
    first_monk = create_monk(start_tile, spawns)    #vytvorenie mnicha
    rake_garden(garden,first_monk, rows, columns)   #pohrabanie 

    best_monk = first_monk
    best_candidate_monk = first_monk

    global annealing_temperature

    sorting = False
    temperature =  annealing_temperature
    i=0
    while (best_monk.fitness != map_fitness and i != iterations_of_algorithms and temperature > 0):
        monk_neighborhood = get_neighbors(garden, best_candidate_monk, best_candidate_monk.steps[0], spawns, rows, columns, sorting)
        best_candidate_monk = monk_neighborhood[0]
        for candidate_monk in monk_neighborhood:
            if candidate_monk.fitness > best_candidate_monk.fitness:
                best_candidate_monk = candidate_monk
                break
            elif random.randrange(0,101) < temperature:
                best_candidate_monk = candidate_monk
                break
        temperature -= annealing_cooling    #ochladzovanie

        if best_candidate_monk.fitness > best_monk.fitness: #porovnanie s globalne najlepsim
            best_monk = best_candidate_monk
        i+=1
    
    return best_monk


#=====================
#=====================
#======zo suboru======
#=====================
#=====================

def file_input():
    #nacitanie zo suboru
    rows, columns, stones = load_data_from_file()
    
    #inicializacia zahrady
    garden = create_garden(rows, columns, stones)
    
    #ziskanie policok, kde moze mnich zacat
    spawns = get_possible_spawns(garden)
    start_tile = random.choice(spawns)

    if len(spawns) == 0:
        print("Vytvorila sa mapa bez pristupu do zahrady, opakuj volbu pripadne s mensim poctom kamenov")
        return

    print_garden(garden)
    #print("start na policku: ",start_tile)
    #print(spawns)
    print()

    best_monk = choose_alg(garden, start_tile, rows, columns, spawns)

    print()
    print("Fitness mapy: ",get_garden_maximum_fitness(garden),"Monk max fitness: ", best_monk.fitness)
    #print("Kroky: ",best_monk.steps)
    print("Rozhodnutia: ",best_monk.decisions)
    print()
    
    best_monk.spawns = spawns
    #vykreslenie pohrabanej zahrady s najlepsim mnichom
    print_garden(rake_garden(garden,best_monk,rows,columns))


#=====================
#=====================
#===rucne zadavanie===
#=====================
#=====================

def manual_input():
    #nacitanie rozmerov
    rows, columns = load_garden_size()

    #nacitanie poctu kamenov
    stone_count = load_stone_count(rows, columns)

    #nacitanie suradnic kamenov
    stones = load_stones_positions(stone_count, rows, columns)
    
    #inicializacia zahrady
    garden = create_garden(rows, columns, stones)
    
    #ziskanie policok, kde moze mnich zacat
    spawns = get_possible_spawns(garden)
    start_tile = random.choice(spawns)
    
    if len(spawns) == 0:
        print("Vytvorila sa mapa bez pristupu do zahrady, opakuj volbu pripadne s mensim poctom kamenov")
        return

    print_garden(garden)
    #print("start na policku: ",start_tile)
    #print(spawns)
    print()

    best_monk = choose_alg(garden, start_tile, rows, columns, spawns)

    print()
    print("Fitness mapy: ",get_garden_maximum_fitness(garden),"Monk max fitness: ", best_monk.fitness)
    #print("Kroky: ",best_monk.steps)
    print("Rozhodnutia: ",best_monk.decisions)
    print()

    best_monk.spawns = spawns
    #vykreslenie pohrabanej zahrady
    print_garden(rake_garden(garden,best_monk,rows,columns))


#=====================
#=====================
#========tester=======
#=====================
#=====================

def tester():
    #zakomentovanie ak chcem testovat zahradu zo suboru
    #potom treba posunut do else vsetko az po inicializaciu zahrady
    # if input("zo suboru?: ") == "1":
    #     rows, columns, stones = load_data_from_file()
    # else:

    print("Zadaj rozmery zahrad pre testovanie: ")
    rows, columns = load_garden_size()

    #nacitanie poctu kamenov
    stone_count = load_stone_count(rows, columns)
    
    #vygenerovanie nahodnych pozicii kamenov
    stones = generate_stones(stone_count, rows, columns)

    #vytvorenie suboru so zadanymi rozmermi, poctom kamenov a poziciami vygenerovanych kamenov info nad akou mapou prebehlo testovanie v pripadneho testovania optimalizacie nad danou mapou
    create_input_file(rows, columns, stones)

    #inicializacia zahrady
    garden = create_garden(rows, columns, stones)
    
    #ziskanie policok, kde moze mnich zacat
    spawns = get_possible_spawns(garden)

    if len(spawns) == 0:
        print("Vytvorila sa mapa bez pristupu do zahrady, opakuj volbu pripadne s mensim poctom kamenov")
        return

    print_garden(garden)
    #print(spawns)

    monks = []
    monk_max = 0
    fitness_average = 0
    timer_average = 0.00000000
    start = time.time()

    #testovanie hill climbing alg pre vsetky zaciatocne policka
    for start_tile in spawns:
        monk = hill_climb(garden, start_tile, rows, columns, spawns)
        monks.append(monk)
    end = time.time()
    timer_average += end - start
    monk_max = max(monks,key=lambda x: x.fitness)
    fitness_average = sum(x.fitness for x in monks)/len(spawns)

    print("Hill climb priemerny cas: ", timer_average/len(spawns))
    print("Fitness mapy: ",get_garden_maximum_fitness(garden),"Monk max fitness: ", monk_max.fitness)
    print("Monk average fitness: ", fitness_average)
    print()


    monks = []
    timer_average = 0.00000000
    start = time.time()

    #testovanie tabu search alg pre vsetky zaciatocne policka
    for start_tile in spawns:
        monk = tabu_search(garden, start_tile, rows, columns, spawns)
        monks.append(monk)

    end = time.time()
    timer_average += end - start
    monk_max = max(monks,key=lambda x: x.fitness)
    fitness_average = sum(x.fitness for x in monks)/len(spawns)

    print("Tabu search priemerny cas: ", timer_average/len(spawns))
    print("Fitness mapy: ",get_garden_maximum_fitness(garden),"Monk max fitness: ", monk_max.fitness)
    print("Monk average fitness: ", fitness_average)
    print()


    monks = []
    timer_average = 0.00000000
    start = time.time()

    #testovanie simulated annealing alg pre vsetky zaciatocne policka
    for start_tile in spawns:
        monk = simulated_annealing(garden, start_tile, rows, columns, spawns)
        monks.append(monk)

    end = time.time()
    timer_average += end - start
    monk_max = max(monks,key=lambda x: x.fitness)
    fitness_average = sum(x.fitness for x in monks)/len(spawns)

    print("Annealing priemerny cas: ", timer_average/len(spawns))
    print("Fitness mapy: ",get_garden_maximum_fitness(garden),"Monk max fitness: ", monk_max.fitness)
    print("Monk average fitness: ", fitness_average)
    print()


#=====================
#=====================
#=======PROGRAM=======
#======interface======
#=====================

def print_garden(garden):
    for row in garden:
        print(row)

#hlavička programu
def name_print():
    print()
    print("------------Zenova zahrada------------")
    print()
    print("          Autor: Adrián Vančo         ")
    print()


def menu():
    while True:
        print()
        print("Dostupné možnosti:")
        print("     1 - Rucne zadanie")
        print("     2 - Zo suboru")
        print("     3 - Tester")
        print("     0 - Pre ukončenie")

        try:
            menu_option = int(input('Zadaj možnosť: '))
        except ValueError:
            print("Chyba pri zadavani.")
            continue

        if menu_option == 1:
            manual_input()   
        elif menu_option == 2:
            file_input() 
        elif menu_option == 3:
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