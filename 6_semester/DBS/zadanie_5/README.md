# DBS

https://fiit-dbs-xvancoa-app.herokuapp.com

### Endpoint /v2/patches query:

```sql
SELECT 	pat.*, 
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
```

### Endpoint /v2/players/{player_id}/game_exp query:

```sql
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
```

### Endpoint /v2/players/{player_id}/game_objectives query:

```sql
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
```

### Endpoint /v2/players/{player_id}/abilities query:

```sql
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
```

### Endpoint /v3/matches/{match_id}/top_purchases/ query:

```sql
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
```

### Endpoint /v3/abilities/{ability_id}/usage/ query:

```sql
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
```

### Endpoint /v3/statistics/tower_kills/ query:

```sql
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
```
