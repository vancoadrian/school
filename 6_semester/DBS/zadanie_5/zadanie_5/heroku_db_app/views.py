import collections
from django.shortcuts import render
from django.http import HttpResponse, JsonResponse, HttpResponseNotAllowed, HttpResponseNotFound
import sys
import psycopg2
import simplejson as json

import os 
from decimal import *


db={'ENGINE': 'django.db.backends.postgresql_psycopg2',
"NAME": os.getenv("DB_NAME", "db_name"),
"USER": os.getenv("DB_USER", "db_user"),
"PASSWORD": os.getenv("DB_PASS", "db@password"),
"HOST": os.getenv("DB_HOST", "localhost"),
"PORT": os.getenv("DB_PORT", "5432")}

 
def index(request):
  return HttpResponse("Hello Geeks")


# pull data from database
def dota2(request):
    if request.method == "GET":
        conn = None
        #connect to database
        try:
            conn = psycopg2.connect(
                database=db['NAME'],
                user=db['USER'],
                password=db['PASSWORD'],
                host=db['HOST'],
                port=db['PORT']
                )

            #create cursor
            cur = conn.cursor()
            
            #execute first query
            cur.execute('SELECT version();')

            #get result
            db_version = cur.fetchone()

            #save result
            db_version_desc = cur.description
            db_version_colnames = [desc[0] for desc in db_version_desc]

            #execute second query
            cur.execute("SELECT pg_database_size('dota2')/1024/1024 as dota2_db_size;")

            #get result
            db_size = cur.fetchone()
            #save result
            db_size_desc = cur.description
            db_size_colnames = [desc[0] for desc in db_size_desc]

            #building json
            data_set = {}
            data_set[db_version_colnames[0]] = db_version[0]
            data_set[db_size_colnames[0]] = db_size[0]

            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            return HttpResponse(error)
            
        if conn is not None:
                conn.close()

        #return json
        return JsonResponse({'pgsql': data_set}, safe=False, status=200)

    else:
        return HttpResponseNotAllowed("Sorry this method is not allowed")


def patches(request):
    if request.method == "GET":
        conn = None
        #connect to database
        try:
            conn = psycopg2.connect(
                database=db['NAME'],
                user=db['USER'],
                password=db['PASSWORD'],
                host=db['HOST'],
                port=db['PORT']
                )

            #create cursor
            cur = conn.cursor()
            
            #execute first query
            cur.execute(''' 
                            SELECT pat.*, 
	                        m1.id AS match_id,
	                        ROUND(m1.duration/60.0,2) AS duration
                            FROM matches AS m1
                            RIGHT JOIN (
                                SELECT pa.name AS patch_version,
                                CAST(EXTRACT(epoch FROM pa.release_date) AS INT) AS patch_start_date,
                                COALESCE(LEAD(CAST(EXTRACT(epoch FROM pa.release_date) AS INT),1) OVER(ORDER BY pa.name),CAST(EXTRACT(epoch FROM NOW()) AS INT)) AS patch_end_date
                                FROM patches AS pa
                            ) AS pat
                            ON m1.start_time BETWEEN pat.patch_start_date AND pat.patch_end_date;
                        ''')

           #get results
            db_colnames = [desc[0] for desc in cur.description]
            
            #building json
            results = cur.fetchall()
            patches = [] 
            matches = []

            for result in results:
                patch = collections.OrderedDict()
                match = collections.OrderedDict()
                match_mapping = collections.OrderedDict()
                patch[db_colnames[0]] = result[0]   #patch_version
                patch[db_colnames[1]] = result[1]   #patch_start_date
                patch[db_colnames[2]] = result[2]   #patch_end_date
                patch['matches'] = []
                match[db_colnames[3]] = result[3]   #match_id
                match[db_colnames[4]] = result[4]   #match_duration

                if result[3] or result[3] == 0:
                    match_mapping[db_colnames[0]] = result[0]   #patch_version
                    match_mapping['match'] = match
                    matches.append(match_mapping)

                if patch not in patches:
                    patches.append(patch)

            #adding every match to correct patch
            for match_mapping in matches:
                for patch in patches:
                    if match_mapping['patch_version'] == patch['patch_version']:
                        patch['matches'].append(match_mapping['match'])
                
                # skusenie cez generator - robi duplicity
                #if result[3] or result[3] == 0:
                    #1 moznost
                    #temp = next(p for p, patch in enumerate(patches) if patch[db_colnames[0]] == result[0])
                    #patches[temp]['matches'].append(match)
                    #2 moznost
                    #next(patch for patch in patches if patch[db_colnames[0]] == result[0])['matches'].append(match)

            game=collections.OrderedDict()
            patches[-1][db_colnames[2]] = None
            game['patches'] = patches

            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            return HttpResponse(error)
            
        if conn is not None:
            conn.close()

        #return json
        return HttpResponse(json.dumps(game), content_type="application/json", status=200)
    else:
        return HttpResponseNotAllowed("Sorry this method is not allowed")

    


def player_id_game_exp(request,player_id):       
    if request.method == "GET":
        conn = None
        #connect to database
        try:
            conn = psycopg2.connect(
                database=db['NAME'],
                user=db['USER'],
                password=db['PASSWORD'],
                host=db['HOST'],
                port=db['PORT']
                )

            #create cursor
            cur = conn.cursor()
            
            #execute first query
            cur.execute(''' 
                            SELECT players.id, 
                                COALESCE(players.nick,'unknown') AS player_nick, 
                                heroes.localized_name AS hero_localized_name, 
                                ROUND(matches.duration/60.0,2) AS match_duration_minutes ,
                                COALESCE(matches_players_details.xp_hero, 0) +
                                COALESCE(matches_players_details.xp_creep, 0) +
                                COALESCE(matches_players_details.xp_other, 0) +
                                COALESCE(matches_players_details.xp_roshan, 0) AS experiences_gained,
                                matches_players_details.level AS level_gained,
                                CASE
                                    WHEN matches_players_details.player_slot BETWEEN 0 AND 4 AND matches.radiant_win = true THEN true
                                    WHEN matches_players_details.player_slot BETWEEN 128 AND 132 AND matches.radiant_win = false THEN true
                                    ELSE false
                                END AS winner,
                                matches_players_details.match_id
                            FROM matches_players_details
                            JOIN players
                                ON players.id = matches_players_details.player_id
                            JOIN heroes
                                ON heroes.id = matches_players_details.hero_id
                            JOIN matches
                                ON matches.id = matches_players_details.match_id
                            
                            WHERE players.id = {0} 
                            ORDER BY matches.id
                        '''.format(player_id))

            #get results
            db_colnames = [desc[0] for desc in cur.description]
            
            #building json
            results = cur.fetchall()
            matches = [] 

            for result in results:
                match = collections.OrderedDict()
                match[db_colnames[7]] = result[7]   #match_id
                match[db_colnames[2]] = result[2]   #hero_localized_name
                match[db_colnames[3]] = result[3]   #match_duration_minutes
                match[db_colnames[4]] = result[4]   #experiences_gained
                match[db_colnames[5]] = result[5]   #level_gained
                match[db_colnames[6]] = result[6]   #winner
                matches.append(match)

            player=collections.OrderedDict()
            player[db_colnames[0]] = results[0][0]  #id
            player[db_colnames[1]] = results[0][1]  #player_nick
            player['matches'] = matches
            
            #close
            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            return HttpResponse(error)
            
        if conn is not None:
            conn.close()
        
        #return json
        return HttpResponse(json.dumps(player), content_type="application/json", status=200)
    else:
        return HttpResponseNotAllowed("Sorry this method is not allowed")

def player_id_game_obj(request,player_id):       
    if request.method == "GET":
        conn = None
        #connect to database
        try:
            conn = psycopg2.connect(
                database=db['NAME'],
                user=db['USER'],
                password=db['PASSWORD'],
                host=db['HOST'],
                port=db['PORT']
                )

            #create cursor
            cur = conn.cursor()
            
            #execute first query
            cur.execute('''   
                            SELECT players.id, 
                                COALESCE(players.nick,'unknown') AS player_nick, 
                                heroes.localized_name AS hero_localized_name, 
                                matches_players_details.match_id,
                                COALESCE(game_objectives.subtype,'NO_ACTION') AS hero_action,
                                COUNT(COALESCE(game_objectives.subtype,'NO_ACTION')) AS "count"
                            FROM matches_players_details
                            JOIN players
                                ON players.id = matches_players_details.player_id
                            JOIN heroes
                                ON heroes.id = matches_players_details.hero_id
                            LEFT JOIN game_objectives
                                ON game_objectives.match_player_detail_id_1 =  matches_players_details.id
                            WHERE players.id = {0}
                            GROUP BY players.id,
                                heroes.localized_name,
                                matches_players_details.match_id,
                                matches_players_details.match_id,
                                game_objectives.subtype
                            ORDER BY matches_players_details.match_id
                        '''.format(player_id))
            #get results
            db_colnames = [desc[0] for desc in cur.description]
            
            #building json
            results = cur.fetchall()

            actions=[]

            matches = [] 

            for result in results:
                match = collections.OrderedDict()
                action_mapping = collections.OrderedDict()
                action = collections.OrderedDict()

                match[db_colnames[3]] = result[3]   #match_id
                match[db_colnames[2]] = result[2]   #hero_localized_name
                match['actions'] = []
                action_mapping[db_colnames[3]] = result[3]
                action[db_colnames[4]] = result[4]   #hero_action
                action[db_colnames[5]] = result[5]   #count
                action_mapping['action'] = action
                actions.append(action_mapping)
                
                if match not in matches:
                    matches.append(match)

            for action_mapping in actions:
                for match in matches:
                    if action_mapping['match_id'] == match['match_id']:
                        match['actions'].append(action_mapping['action'])


            player=collections.OrderedDict()
            player[db_colnames[0]] = results[0][0]  #id
            player[db_colnames[1]] = results[0][1]  #player_nick
            player['matches'] = matches
            
            # for result in results:
            #     print(result)

            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            return HttpResponse(error)
            
        if conn is not None:
            conn.close()

        #return json
        return HttpResponse(json.dumps(player), content_type="application/json", status=200)



    else:
        return HttpResponseNotAllowed("Sorry this method is not allowed")


def player_id_game_abil(request,player_id):       
    if request.method == "GET":
        conn = None
        #connect to database
        try:
            conn = psycopg2.connect(
                database=db['NAME'],
                user=db['USER'],
                password=db['PASSWORD'],
                host=db['HOST'],
                port=db['PORT']
                )

            #create cursor
            cur = conn.cursor()
            
            #execute first query
            cur.execute('''   
                            SELECT players.id, 
                                COALESCE(players.nick,'unknown') AS player_nick, 
                                heroes.localized_name AS hero_localized_name, 
                                matches_players_details.match_id,
                                abilities.name AS ability_name,
                                COUNT(abilities.name) AS "count",
                                MAX(ability_upgrades.level) AS upgrade_level
                            FROM matches_players_details
                            JOIN players
                                ON players.id = matches_players_details.player_id
                            JOIN heroes
                                ON heroes.id = matches_players_details.hero_id
                            JOIN ability_upgrades
                                ON ability_upgrades.match_player_detail_id = matches_players_details.id
                            JOIN abilities
                                ON abilities.id = ability_upgrades.ability_id
                            WHERE players.id = {0}
                            GROUP BY players.id,
                                heroes.localized_name,
                                matches_players_details.match_id,
                                abilities.name
                            ORDER BY matches_players_details.match_id
                        '''.format(player_id))

            #get results
            db_colnames = [desc[0] for desc in cur.description]
            
            #building json
            results = cur.fetchall()

            abilities=[]

            matches = [] 

            for result in results:
                match = collections.OrderedDict()
                ability_mapping = collections.OrderedDict()
                ability = collections.OrderedDict()

                match[db_colnames[3]] = result[3]   #match_id
                match[db_colnames[2]] = result[2]   #hero_localized_name
                match['abilities'] = []
                ability_mapping[db_colnames[3]] = result[3]
                ability[db_colnames[4]] = result[4]   #ability_name
                ability[db_colnames[5]] = result[5]   #count
                ability[db_colnames[6]] = result[6]   #upgrade_level
                ability_mapping['ability'] = ability
                abilities.append(ability_mapping)
                
                if match not in matches:
                    matches.append(match)

            for ability_mapping in abilities:
                for match in matches:
                    if ability_mapping['match_id'] == match['match_id']:
                        match['abilities'].append(ability_mapping['ability'])


            player=collections.OrderedDict()
            player[db_colnames[0]] = results[0][0]  #id
            player[db_colnames[1]] = results[0][1]  #player_nick
            player['matches'] = matches
            
            for result in results:
                print(result)
            

            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            return HttpResponse(error)
            
        if conn is not None:
            conn.close()

        #return json
        return HttpResponse(json.dumps(player), content_type="application/json", status=200)



    else:
        return HttpResponseNotAllowed("Sorry this method is not allowed")




# ===========================
# ===========================
# zadanie 5
# ===========================
# ===========================
def top_purchases(request, match_id):
    if request.method == "GET":
        conn = None
        #connect to database
        try:
            conn = psycopg2.connect(
                database=db['NAME'],
                user=db['USER'],
                password=db['PASSWORD'],
                host=db['HOST'],
                port=db['PORT']
            )

            #create cursor
            cur = conn.cursor()

            #execute first query
            cur.execute('''   
                           SELECT ranks.hero_id AS "id", ranks.localized_name AS "name", ranks.item_id AS "id", ranks.name, ranks.count FROM(
                                SELECT hero_id, heroes.localized_name,item_id,items.name,
                                    COUNT(item_id) AS "count",
                                    ROW_NUMBER() OVER (PARTITION BY hero_id ORDER BY hero_id,COUNT(item_id) DESC,items.name) AS hero_rank
                                FROM matches
                                JOIN matches_players_details
                                    ON matches.id = matches_players_details.match_id AND 
                                    CASE
                                        WHEN matches_players_details.player_slot BETWEEN 0 AND 4 AND matches.radiant_win = true THEN true
                                        WHEN matches_players_details.player_slot BETWEEN 128 AND 132 AND matches.radiant_win = false THEN true
                                        ELSE false
                                    END
                                JOIN purchase_logs
                                    ON purchase_logs.match_player_detail_id = matches_players_details.id
                                JOIN items
                                    ON items.id = purchase_logs.item_id	
                                JOIN heroes
                                    ON heroes.id = matches_players_details.hero_id
                                WHERE match_id = {0}
                                GROUP BY player_id,hero_id,heroes.localized_name,item_id,items.name,matches_players_details.match_id
                                ) ranks	
                            WHERE hero_rank <= 5;
                        '''.format(match_id))

            #get results
            db_colnames = [desc[0] for desc in cur.description]

            #building json
            results = cur.fetchall()

            items = []
            heroes = [] 

            for result in results:
                hero = collections.OrderedDict()
                hero[db_colnames[0]] = result[0]  # id
                hero[db_colnames[1]] = result[1]  # name
                hero['top_purchases'] = []  # top_purchases

                item_mapping = collections.OrderedDict()
                item_mapping[db_colnames[0]] = result[0]

                item = collections.OrderedDict()
                item[db_colnames[2]] = result[2]  # id
                item[db_colnames[3]] = result[3]  # name
                item[db_colnames[4]] = result[4]  # count
                item_mapping['item'] = item
                items.append(item_mapping)

                if hero not in heroes:
                    heroes.append(hero)

            for item_mapping in items:
                for hero in heroes:
                    if item_mapping[db_colnames[0]] == hero[db_colnames[0]]:
                        hero['top_purchases'].append(item_mapping['item'])
                        break



            game = collections.OrderedDict()
            game['id'] = match_id  # id
            game['heroes'] = heroes  # heroes

            # for result in results:
            #     print(result)

            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            return HttpResponse(error)

        if conn is not None:
            conn.close()

        #return json
        return HttpResponse(json.dumps(game), content_type="application/json", status=200)

    else:
        return HttpResponseNotAllowed("Sorry this method is not allowed")



def usage(request,ability_id):
    if request.method == "GET":
        conn = None
        #connect to database
        try:
            conn = psycopg2.connect(
                database=db['NAME'],
                user=db['USER'],
                password=db['PASSWORD'],
                host=db['HOST'],
                port=db['PORT']
            )

            #create cursor
            cur = conn.cursor()

            #execute first query
            cur.execute('''
                            SELECT idk3.id, idk3.name, idk3.hero_id AS "id", idk3.localized_name AS "name", idk3.win, idk3.bucket, idk3.win_part AS "count"
                            FROM (
                                SELECT *
                                    ,MAX(idk2.win_part) OVER(PARTITION BY idk2.win,idk2.hero_id)
                                FROM(
                                    SELECT *
                                        ,COUNT(idk.win) OVER (PARTITION BY idk.bucket , idk.win, idk.hero_id ) AS win_part
                                        ,ROW_NUMBER() OVER (PARTITION BY idk.bucket , idk.win, idk.hero_id ) AS row_numbering
                                    FROM(
                                        SELECT  abilities.id, abilities.name, matches_players_details.hero_id, heroes.localized_name
                                            ,CASE
                                                WHEN matches_players_details.player_slot >= 0 AND matches_players_details.player_slot <= 4 AND matches.radiant_win = true THEN true
                                                WHEN matches_players_details.player_slot >= 128 AND matches_players_details.player_slot <=132 AND matches.radiant_win = false THEN true
                                                ELSE false
                                            END win,

                                            CASE
                                                WHEN ability_upgrades.time * 100 / matches.duration < 10 AND ability_upgrades.time * 100 / matches.duration >= 0 THEN '0-9'
                                                WHEN ability_upgrades.time * 100 / matches.duration < 20 AND ability_upgrades.time * 100 / matches.duration >= 10 THEN '10-19'
                                                WHEN ability_upgrades.time * 100 / matches.duration < 30 AND ability_upgrades.time * 100 / matches.duration >= 20 THEN '20-29'
                                                WHEN ability_upgrades.time * 100 / matches.duration < 40 AND ability_upgrades.time * 100 / matches.duration >= 30 THEN '30-39'
                                                WHEN ability_upgrades.time * 100 / matches.duration < 50 AND ability_upgrades.time * 100 / matches.duration >= 40 THEN '40-49'
                                                WHEN ability_upgrades.time * 100 / matches.duration < 60 AND ability_upgrades.time * 100 / matches.duration >= 50 THEN '50-59'
                                                WHEN ability_upgrades.time * 100 / matches.duration < 70 AND ability_upgrades.time * 100 / matches.duration >= 60 THEN '60-69'
                                                WHEN ability_upgrades.time * 100 / matches.duration < 80 AND ability_upgrades.time * 100 / matches.duration >= 70 THEN '70-79'
                                                WHEN ability_upgrades.time * 100 / matches.duration < 90 AND ability_upgrades.time * 100 / matches.duration >= 80 THEN '80-89'
                                                WHEN ability_upgrades.time * 100 / matches.duration < 100 AND ability_upgrades.time * 100 / matches.duration >= 90 THEN '90-99'
                                                ELSE '100-109'
                                            END bucket
                                        FROM matches_players_details
                                        JOIN matches 
                                            ON matches.id = matches_players_details.match_id
                                        JOIN ability_upgrades
                                            ON 	ability_upgrades.match_player_detail_id = matches_players_details.id
                                        JOIN heroes
                                            ON heroes.id = matches_players_details.hero_id
                                        JOIN abilities
                                            ON abilities.id = ability_upgrades.ability_id
                                        WHERE abilities.id = {0}
                                        ) idk
                                    )idk2
                                WHERE idk2.row_numbering = 1
                                ORDER BY win_part DESC
                                )idk3
                            WHERE idk3.max = idk3.win_part
                            ORDER BY idk3.hero_id,win DESC;
                        '''.format(ability_id))

            #get results
            db_colnames = [desc[0] for desc in cur.description]

            #building json
            results = cur.fetchall()

            print(len(results))

            if(len(results) == 0):
                return HttpResponseNotFound('<h1>Page not found</h1>')

            ability = collections.OrderedDict()
            ability[db_colnames[0]] = ability_id  # id
            ability[db_colnames[1]] = results[0][1]  # heroes
            ability['heroes'] = []

            hero = collections.OrderedDict()


            for result in results:
                hero[db_colnames[2]] = result[2]  # id
                hero[db_colnames[3]] = result[3]  # name

                win = result[4]

                bucket = collections.OrderedDict()
                bucket[db_colnames[5]] = result[5]
                bucket[db_colnames[6]] = result[6]

                if(win):
                    hero['usage_winners'] = bucket

                if(not win):
                    hero['usage_loosers'] = bucket

                hero_in_arr = next((x for x in ability['heroes']
                     if x[db_colnames[2]] == hero[db_colnames[2]]), None)

                if hero_in_arr != None:
                    hero = collections.OrderedDict()

                if hero_in_arr == None:
                    ability['heroes'].append(hero)

                # if hero not in ability['heroes']:
                #     ability['heroes'].append(hero)
                #     hero = collections.OrderedDict()


            # for result in results:
            #     print(result)

            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            return HttpResponse(error)

        if conn is not None:
            conn.close()

        #return json
        return HttpResponse(json.dumps(ability), content_type="application/json", status=200)

    else:
        return HttpResponseNotAllowed("Sorry this method is not allowed")





def tower_kills(request):
    if request.method == "GET":
        conn = None
        #connect to database
        try:
            conn = psycopg2.connect(
                database=db['NAME'],
                user=db['USER'],
                password=db['PASSWORD'],
                host=db['HOST'],
                port=db['PORT']
            )

            #create cursor
            cur = conn.cursor()

            #execute first query
            cur.execute('''
                            SELECT idk4.hero_id AS "id", idk4.localized_name AS "name", idk4.max AS "tower_kills"
                            FROM(
                                SELECT idk3.hero_id, idk3.localized_name, idk3.match_id, idk3.count
                                    ,MAX(idk3.count) OVER (PARTITION BY idk3.hero_id)
                                FROM (
                                    SELECT idk2.hero_id, idk2.localized_name, idk2.match_id
                                        ,COUNT(*) OVER(PARTITION BY idk2.sum, match_id ORDER BY match_id,idk2.time)
                                    FROM (
                                        SELECT *
                                        ,SUM(CASE WHEN idk.hero_id = idk.prev_hero_id THEN 0 ELSE 1 END) OVER(ORDER BY match_id,idk.time)
                                        FROM(
                                            SELECT hero_id, heroes.localized_name,match_id, game_objectives.time
                                            ,LAG(match_id) OVER(ORDER BY match_id,game_objectives.time) AS prev_match_id
                                            ,LAG(hero_id) OVER(ORDER BY match_id,game_objectives.time) AS prev_hero_id
                                                FROM game_objectives
                                            JOIN matches_players_details
                                                ON matches_players_details.id = game_objectives.match_player_detail_id_1
                                            JOIN heroes
                                                ON matches_players_details.hero_id = heroes.id
                                            WHERE match_player_detail_id_1 IS NOT null AND subtype = 'CHAT_MESSAGE_TOWER_KILL' 
                                            ORDER BY match_id,game_objectives.time
                                            ) idk
                                        ) idk2
                                    ) idk3	
                                ORDER BY idk3.count DESC
                                )idk4
                            GROUP BY hero_id, idk4.localized_name,idk4.max
                            ORDER BY idk4.max DESC, idk4.localized_name;
                        ''')

            #get results
            db_colnames = [desc[0] for desc in cur.description]

            #building json
            results = cur.fetchall()
            
            

            heroes = []


            for result in results:
                hero = collections.OrderedDict()
                hero[db_colnames[0]] = result[0] 
                hero[db_colnames[1]] = result[1] 
                hero[db_colnames[2]] = result[2]
                heroes.append(hero)

            tower_kills = collections.OrderedDict()
            tower_kills['heroes'] = heroes

            # for result in results:
            #     print(result)

            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            return HttpResponse(error)

        if conn is not None:
            conn.close()

        #return json
        return HttpResponse(json.dumps(tower_kills), content_type="application/json", status=200)

    else:
        return HttpResponseNotAllowed("Sorry this method is not allowed")

