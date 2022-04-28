 #!/usr/bin/env python
import random
from time import time
import numpy as np
from matplotlib import pyplot as plt
import math

colors ={0:'#e41a1c',1:'#377eb8',2:'#4daf4a',3:'#984ea3',4:'#ff7f00',5:'#ffff33',6:'#a65628',7:'#f781bf',8:'#999999',9:'#66c2a5',10:'#fc8d62',11:'#8da0cb',12:'#e78aff',13:'#a6d854',14:'#ffd92f',15:'#e5c494',16:'#b3b3b3',17:'#8dd3c7',18:'#ffffb3',19:'#bebada'}

#NASTAVENIA
max_iterations = 300
global_interval = (-5000,5000)
global_count_random_points = 20
global_count_points_to_generate_using_offsets = 20000
global_x_offset_interval = (-100,100)
global_y_offset_interval = (-100,100)
global_number_of_cluster = 20


def plot_clusters(centers, clusters):
    key = 0
    for cluster in clusters:
        plot_p=np.array(list(cluster)).transpose()
        plt.scatter(plot_p[0],plot_p[1],s= 5,color=colors[key])
        key += 1
        key = key%20
    plot_p=np.array(list(centers)).transpose()
    plt.scatter(plot_p[0],plot_p[1],marker="x",color='black')

    
    #plot_function(centers)
    plt.show()


def random_spawns_in_interval(count, interval):
    points = set()

    for i in range(count):
        while True:
            x = random.randrange(interval[0], interval[1] + 1)
            y = random.randrange(interval[0], interval[1] + 1)

            point = (x,y)

            if point not in points:
                #print(point)
                points.add(point)
                break

    return points


def add_spawn_points_using_offsets(points, count, x_offset_interval, y_offset_interval, interval):
    points_list = list(points)

    local_x_offset_interval = list(x_offset_interval)
    local_y_offset_interval = list(y_offset_interval)

    for i in range(count):
        base_point = random.choice(points_list)

        x_base = base_point[0]
        y_base = base_point[1]

        #x offset range update
        left_diference = abs(interval[0] - x_base)
        right_diference = abs(interval[1] - x_base)

        if left_diference < abs(local_x_offset_interval[0]):
            local_x_offset_interval[0] = -left_diference
        elif right_diference < abs(local_x_offset_interval[1]):
            local_x_offset_interval[1] = right_diference

        #y offset range update
        bottom_diference = abs(interval[0] - y_base)
        top_diference = abs(interval[1] - y_base)

        if bottom_diference < abs(local_y_offset_interval[0]):
            local_y_offset_interval[0] = -bottom_diference
        elif top_diference < abs(local_y_offset_interval[1]):
            local_y_offset_interval[1] = top_diference

        while True:
            x = x_base + random.randrange(local_x_offset_interval[0], local_x_offset_interval[1] + 1)
            y = y_base + random.randrange(local_y_offset_interval[0], local_y_offset_interval[1] + 1)

            point = (x,y)

            if point not in points:
                #print(point)
                points.add(point)
                points_list.append(point)
                break

    return points


def points_to_closest_cluster(centroids, points_list, clusters, clusters_count):

    for point in points_list:

        distance = math.dist([point[0], point[1]], [centroids[0][0], centroids[0][1]])
        cluster_id = 0

        for j in range(1,clusters_count):
            actual_distance = math.dist([point[0], point[1]], [centroids[j][0], centroids[j][1]])
            if distance > actual_distance:
                distance = actual_distance
                cluster_id = j

        clusters[cluster_id].append(point)



def recalculate_medoids(medoids, clusters):
    recalculate_centroids(medoids, clusters)
    for i in range(len(clusters)):
        if len(clusters[i]) == 0:
            continue
        numpy_cluster = np.array(list(clusters[i]))

        distance = math.dist([medoids[i][0], medoids[i][1]], [numpy_cluster[0][0], numpy_cluster[0][1]])
        point_id = 0

        for j in range(1,numpy_cluster.shape[0]):
            actual_distance = math.dist([medoids[i][0], medoids[i][1]], [numpy_cluster[j][0], numpy_cluster[j][1]])
            if distance > actual_distance:
                distance = actual_distance
                point_id = j
        #print("totok",numpy_cluster[point_id])
        medoids[i] = (numpy_cluster[point_id][0],numpy_cluster[point_id][1])

def recalculate_centroid(centroid, cluster):
    numpy_cluster = np.array(list(cluster))
    #print("cluster",i,numpy_cluster)
    length = numpy_cluster.shape[0]
    sum_x = np.sum(numpy_cluster[:, 0])
    sum_y = np.sum(numpy_cluster[:, 1])
    centroid = (int(sum_x/length),int(sum_y/length))

def recalculate_centroids(centroids, clusters):

    for i in range(len(clusters)):
        if len(clusters[i]) == 0:
            continue
        recalculate_centroid(centroids[i], clusters[i])
        # numpy_cluster = np.array(list(clusters[i]))
        # #print("cluster",i,numpy_cluster)
        # length = numpy_cluster.shape[0]
        # sum_x = np.sum(numpy_cluster[:, 0])
        # sum_y = np.sum(numpy_cluster[:, 1])
        # centroids[i] = (int(sum_x/length),int(sum_y/length))


def calculate_cluster_avg_distance(center, cluster):
    temp = 0
    for point in cluster:
        temp += math.dist([center[0], center[1]], [point[0], point[1]])
    return temp / len(cluster)





def k_means(points, clusters_count, recalculate):
    centers = list()
    points_list = list(points)
    clusters = [[] for i in range(clusters_count)]
    #pociatocky nahodny vyber
    #mozno prerobit na while len(centroids) < clusters_count kedze set vyhody duplicity
    for i in range(clusters_count):
        while True:
            point = random.choice(points_list)
            if point not in centers:
                centers.append(point)
                break

    # timer = 0.00000000
    # start = time()
    points_to_closest_cluster(centers, points_list, clusters, clusters_count)
    # end = time()
    # timer += end - start
    # print("Čas 1 iteracie", timer)

    # plot_p=np.array(list(centers)).transpose()
    # print(plot_p)
    # plt.scatter(plot_p[0],plot_p[1],s= 5 )
    # plt.yticks
    # plt.title('Example', fontweight ="bold")
    # plt.show()

    iteration = 1
    while True:

        #print("old",centers)
        old_centroids = list(centers)
        #print(centers)
        recalculate(centers, clusters)
        #print("old",centers)
        if len(set(old_centroids) & set(centers)) == len(centers) or max_iterations == iteration:
            if max_iterations == iteration:
                print("dosiahnuty maximalny pocet iteracii")
            break
        #else: print(set(old_centroids) & set(centroids))
        clusters = [[] for i in range(clusters_count)]
        points_to_closest_cluster(centers, points_list, clusters, clusters_count)
        iteration +=1

        # plot_p=np.array(list(old_centroids)).transpose()
        # print(plot_p)
        # plt.scatter(plot_p[0],plot_p[1],s= 5 )
        # plt.yticks
        # plt.title('Example', fontweight ="bold")
        # plt.show()



    successful_clusterer = True
    success_count = 0 
    for cluster_id in range(len(clusters)):
        
        avg_dist = calculate_cluster_avg_distance(centers[cluster_id], clusters[cluster_id])
        if avg_dist > 500:
            successful_clusterer = False
        else:
            success_count += 1


        #print("cluster ID:",cluster_id," priemerna vzdialenost od stredu:",avg_dist)
    if successful_clusterer:
        print("Uspesny")
    else:
        print("Neuspesny")
    print("Percentualna uspesnost:",success_count/len(clusters)*100,"%")

    return centers, clusters;

    
def aglomerative(points, clusters_count):
    centers = []
    clusters = []
    points_list = list(points)
    #print("len points",len(points))
    for i in range(len(points)):
        centers.append(points_list[i])
        clusters.append([points_list[i]])

    c_arr = np.array([[complex(center[0], center[1]) for center in centers]])
    # print(c_arr)
    # print(c_arr.T)
    distance_matrix = abs(c_arr.T-c_arr)
    #print(distance_matrix)

    np.fill_diagonal(distance_matrix, np.inf)
    #print(distance_matrix)
    #ind = np.unravel_index(np.argmin(distance_matrix, axis=None), distance_matrix.shape)
    #print(closest_points)
    #index_1, index_2 = min_distance
    # for i in closest_points:
    #print(distance_matrix[ind])
    #print(ind)
    #     print()

    while len(clusters) != clusters_count:
       #print("cluster len",len(clusters))
        #print(distance_matrix.shape)

        indexes = np.unravel_index(np.argmin(distance_matrix, axis=None), distance_matrix.shape)
        first_index, second_index = indexes
        #print(distance_matrix[indexes])
        #print(first_index,second_index)
        for i in clusters[second_index]:
            #print(i)
            clusters[first_index].append(i)
        #print("otot",clusters[first_index])
        recalculate_centroid(centers[first_index], clusters[first_index])
        #del distance_matrix[second_index]
        
        #print("totok",distance_matrix.shape[0])
        for i in range(distance_matrix.shape[0]):
            if i == first_index: continue
            #print("icko",i)
            distance_matrix[i][first_index] = math.dist([centers[first_index][0], centers[first_index][1]], [centers[i][0], centers[i][1]])
            #del distance_matrix[i][second_index]
            

        for i in range(distance_matrix.shape[1]):
            if i == first_index: continue
            distance_matrix[first_index][i] = math.dist([centers[first_index][0], centers[first_index][1]], [centers[i][0], centers[i][1]])
        distance_matrix = np.delete(distance_matrix,second_index,axis=0)
        distance_matrix = np.delete(distance_matrix,second_index,axis=1)
        del clusters[second_index]
        del centers[second_index]

    successful_clusterer = True
    success_count = 0 
    for cluster_id in range(len(clusters)):
        
        avg_dist = calculate_cluster_avg_distance(centers[cluster_id], clusters[cluster_id])
        if avg_dist > 500:
            successful_clusterer = False
        else:
            success_count += 1


        #print("cluster ID:",cluster_id," priemerna vzdialenost od stredu:",avg_dist)
    if successful_clusterer:
        print("Uspesny")
    else:
        print("Neuspesny")
    print("Percentualna uspesnost:",success_count/len(clusters)*100,"%")

    return centers, clusters;
        

def divisive(points, clusters_count, recalculate):
    clusters = 2
    centers, clusters = k_means(points, 2, recalculate)
    



def start(option):
    for i in range(10):
        points = random_spawns_in_interval(global_count_random_points, global_interval)
        #print(points)
        points = add_spawn_points_using_offsets(points, global_count_points_to_generate_using_offsets, global_x_offset_interval, global_y_offset_interval, global_interval)
        #print(points)
        #plot_p=np.array(list(points)).transpose()
        #print(plot_p)
        #plt.scatter(plot_p[0],plot_p[1],s= 5 )
        #plt.yticks
        #plt.title('Example', fontweight ="bold")
        #plt.show()

        

        if option == 1 or option == 2:
            if option == 1:
                recalculate = recalculate_centroids
            else:
                recalculate = recalculate_medoids
            
            print("Running...")
            timer = 0.00000000
            start = time()
            centers, clusters = k_means(points, global_number_of_cluster, recalculate)
            plot_clusters(centers, clusters)
            end = time()
            timer += end - start
            print("Čas ", timer)

        elif option == 3:
            print("Running...")
            timer = 0.00000000
            start = time()
            centers, clusters = aglomerative(points, global_number_of_cluster)
            plot_clusters(centers, clusters)
            end = time()
            timer += end - start
            print("Čas ", timer)
        elif option == 4:
            print("Running...")
            timer = 0.00000000
            start = time()
            centers, clusters = divisive(points, global_number_of_cluster, recalculate_centroids)
            plot_clusters(centers, clusters)
            end = time()
            timer += end - start
            print("Čas ", timer)









#hlavička programu
def name_print():
    print()
    print("----------4b – klastrovanie-----------")
    print()
    print("          Autor: Adrián Vančo         ")
    print()

def menu():
    while True:
        print()
        print("Dostupné možnosti:")
        print("     1 - k-means, kde stred je centroid")
        print("     2 - k-means, kde stred je medoid")
        print("     3 - aglomeratívne zhlukovanie, kde stred je centroid")
        print("     4 - divízne zhlukovanie, kde stred je centroid")
        print("     0 - Pre ukončenie")

        try:
            menu_option = int(input('Zadaj možnosť: '))
        except ValueError:
            print("Chyba pri zadavani.")
            continue

        if menu_option == 1:
            start(1)
        elif menu_option == 2:
            start(2)
        elif menu_option == 3:
            start(3)
        elif menu_option == 4:
            start(4)
        elif menu_option == 0:
            exit()
        else:
            print("Zadal si zlú možnosť.")

def main():
    name_print()
    menu()


if __name__ == '__main__':
    main()