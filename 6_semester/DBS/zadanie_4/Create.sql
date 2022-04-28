CREATE TABLE "combat_logs" (
  "id" int4 NOT NULL,
  "character_id" int4 NOT NULL,
  "monster_id" int4 NOT NULL,
  "action" varchar(255) NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE "friend_invites" (
  "id" int4 NOT NULL,
  "sender_id" int4 NOT NULL,
  "receiver_id" int4 NOT NULL,
  "status" varchar(255) NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE "friends" (
  "id" int4 NOT NULL,
  "user_id" int4 NOT NULL,
  "friend_id" int4 NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE "games" (
  "id" int4 NOT NULL,
  "map_id" int4 NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE "characters" (
  "id" int4 NOT NULL,
  "user_id" int4 NOT NULL,
  "game_id" int4 NOT NULL,
  "role_id" int4 NOT NULL,
  "role_level_id" int4 NOT NULL,
  "item_id" int4,
  "name" varchar(255) NOT NULL,
  "experience" int4 NOT NULL,
  "health" int4 NOT NULL,
  "defence" int4 NOT NULL,
  "damage" int4 NOT NULL,
  "saved_map_level" varchar NOT NULL,
  "using_item" bool NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE "choosen_skills" (
  "id" int4 NOT NULL,
  "character_id" int4 NOT NULL,
  "skill_id" int4 NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE "items" (
  "id" int4 NOT NULL,
  "health_bonus" varchar(255) NOT NULL,
  "defence_bonus" int4 NOT NULL,
  "damage_bonus" int4 NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE "kill_logs" (
  "id" int4 NOT NULL,
  "character_id" int4 NOT NULL,
  "monster_id" int4 NOT NULL,
  "map_position_id" int4 NOT NULL,
  "kill_info" varchar(255) NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE "locations" (
  "id" int4 NOT NULL,
  "name" varchar(255) NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE "maps" (
  "id" int4 NOT NULL,
  "max_x_axis" int4 NOT NULL,
  "max_y_axis" int4 NOT NULL,
  "max_depth" int4 NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE "maps_positions" (
  "id" int4 NOT NULL,
  "map_id" int4 NOT NULL,
  "location_id" int4,
  "monster_id" int4,
  "item_id" int4,
  "x_axis" int4 NOT NULL,
  "y_axis" int4 NOT NULL,
  "depth" int4 NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE "monsters" (
  "id" int4 NOT NULL,
  "item_id" int4,
  "name" varchar(255) NOT NULL UNIQUE,
  "damage" int4 NOT NULL,
  "health" int4 NOT NULL,
  "defence" int4 NOT NULL,
  "exp_reward" int4 NOT NULL,
  "level_range" interval NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE "quest_logs" (
  "id" int4 NOT NULL,
  "character_id" int4 NOT NULL,
  "quest_id" int4 NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE "quests" (
  "id" int4 NOT NULL,
  "location_id" int4 NOT NULL,
  "item_id" int4 NOT NULL,
  "name" varchar(255) NOT NULL UNIQUE,
  "description" varchar(255) NOT NULL,
  "recomended_level" int4 NOT NULL,
  "required_level" int4 NOT NULL,
  "exp_reward" int4 NOT NULL,
  "needed_kills" int4 NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE "registrations" (
  "id" int4 NOT NULL,
  "user_id" int4 NOT NULL,
  "creation_date" date NOT NULL,
  "verified_date" date,
  "is_verified" bool NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE "roles" (
  "id" int4 NOT NULL,
  "name" varchar(255) NOT NULL UNIQUE,
  PRIMARY KEY ("id")
);

CREATE TABLE "roles_levels" (
  "id" int4 NOT NULL,
  "role_id" int4 NOT NULL,
  "level_number" int4 NOT NULL,
  "exp_needed" int4 NOT NULL,
  "health_bonus" int4 NOT NULL,
  "defence_bonus" int4 NOT NULL,
  "damage_bonus" int4 NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE "roles_skills" (
  "id" int4 NOT NULL,
  "invalid_skill_id" int4,
  "required_skill_id" int4,
  "name" varchar(255) NOT NULL UNIQUE,
  "skill_type" varchar(255) NOT NULL,
  "required_level" int4 NOT NULL,
  "health_modifier" int4 NOT NULL,
  "defence_modifier" int4 NOT NULL,
  "damage_modifier" int4 NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE "team_invites" (
  "id" int4 NOT NULL,
  "user_id" int4 NOT NULL,
  "team_id" int4 NOT NULL,
  "status" varchar(255),
  PRIMARY KEY ("id")
);

CREATE TABLE "teams" (
  "id" int4 NOT NULL,
  "owner_id" int4 NOT NULL,
  "name" varchar(255) NOT NULL UNIQUE,
  PRIMARY KEY ("id")
);

CREATE TABLE "teams_chats" (
  "id" int4 NOT NULL,
  "sender_id" int4 NOT NULL,
  "team_id" int4 NOT NULL,
  "message" varchar(255) NOT NULL,
  "time" date NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE "users" (
  "id" int4 NOT NULL,
  "team_id" int4,
  "username" varchar(255) NOT NULL UNIQUE,
  "nickname" varchar(255) NOT NULL,
  "password" varchar(255) NOT NULL,
  "email" varchar(255) NOT NULL UNIQUE,
  PRIMARY KEY ("id")
);

CREATE TABLE "users_chats" (
  "id" int4 NOT NULL,
  "from_user_id" int4 NOT NULL,
  "to_user_id" int4 NOT NULL,
  "message" varchar(255) NOT NULL,
  "time" date NOT NULL,
  PRIMARY KEY ("id")
);

ALTER TABLE "combat_logs" ADD CONSTRAINT "fk_combat_logs_characters_1" FOREIGN KEY ("character_id") REFERENCES "characters" ("id");
ALTER TABLE "combat_logs" ADD CONSTRAINT "fk_combat_logs_monsters_1" FOREIGN KEY ("monster_id") REFERENCES "monsters" ("id");
ALTER TABLE "friend_invites" ADD CONSTRAINT "fk_friend_invites_users_1" FOREIGN KEY ("sender_id") REFERENCES "users" ("id");
ALTER TABLE "friend_invites" ADD CONSTRAINT "fk_friend_invites_users_2" FOREIGN KEY ("receiver_id") REFERENCES "users" ("id");
ALTER TABLE "friends" ADD CONSTRAINT "fk_friends_users_1" FOREIGN KEY ("user_id") REFERENCES "users" ("id");
ALTER TABLE "friends" ADD CONSTRAINT "fk_friends_users_2" FOREIGN KEY ("friend_id") REFERENCES "users" ("id");
ALTER TABLE "games" ADD CONSTRAINT "fk_games_maps_1" FOREIGN KEY ("map_id") REFERENCES "maps" ("id");
ALTER TABLE "characters" ADD CONSTRAINT "fk_characters_users_1" FOREIGN KEY ("user_id") REFERENCES "users" ("id");
ALTER TABLE "characters" ADD CONSTRAINT "fk_characters_games_1" FOREIGN KEY ("game_id") REFERENCES "games" ("id");
ALTER TABLE "characters" ADD CONSTRAINT "fk_characters_roles_1" FOREIGN KEY ("role_id") REFERENCES "roles" ("id");
ALTER TABLE "characters" ADD CONSTRAINT "fk_characters_roles_levels_1" FOREIGN KEY ("role_level_id") REFERENCES "roles_levels" ("id");
ALTER TABLE "characters" ADD CONSTRAINT "fk_characters_items_1" FOREIGN KEY ("item_id") REFERENCES "items" ("id");
ALTER TABLE "choosen_skills" ADD CONSTRAINT "fk_choosen_skills_characters_1" FOREIGN KEY ("character_id") REFERENCES "characters" ("id");
ALTER TABLE "choosen_skills" ADD CONSTRAINT "fk_choosen_skills_roles_skills_1" FOREIGN KEY ("skill_id") REFERENCES "roles_skills" ("id");
ALTER TABLE "kill_logs" ADD CONSTRAINT "fk_kill_logs_characters_1" FOREIGN KEY ("character_id") REFERENCES "characters" ("id");
ALTER TABLE "kill_logs" ADD CONSTRAINT "fk_kill_logs_monsters_1" FOREIGN KEY ("monster_id") REFERENCES "monsters" ("id");
ALTER TABLE "kill_logs" ADD CONSTRAINT "fk_kill_logs_maps_positions_1" FOREIGN KEY ("map_position_id") REFERENCES "maps_positions" ("id");
ALTER TABLE "maps_positions" ADD CONSTRAINT "fk_maps_positions_maps_1" FOREIGN KEY ("map_id") REFERENCES "maps" ("id");
ALTER TABLE "maps_positions" ADD CONSTRAINT "fk_maps_positions_locations_1" FOREIGN KEY ("location_id") REFERENCES "locations" ("id");
ALTER TABLE "maps_positions" ADD CONSTRAINT "fk_maps_positions_monsters_1" FOREIGN KEY ("monster_id") REFERENCES "monsters" ("id");
ALTER TABLE "maps_positions" ADD CONSTRAINT "fk_maps_positions_items_1" FOREIGN KEY ("item_id") REFERENCES "items" ("id");
ALTER TABLE "monsters" ADD CONSTRAINT "fk_monsters_items_1" FOREIGN KEY ("item_id") REFERENCES "items" ("id");
ALTER TABLE "quest_logs" ADD CONSTRAINT "fk_quest_logs_characters_1" FOREIGN KEY ("character_id") REFERENCES "characters" ("id");
ALTER TABLE "quest_logs" ADD CONSTRAINT "fk_quest_logs_quests_1" FOREIGN KEY ("quest_id") REFERENCES "quests" ("id");
ALTER TABLE "quests" ADD CONSTRAINT "fk_quests_locations_1" FOREIGN KEY ("location_id") REFERENCES "locations" ("id");
ALTER TABLE "quests" ADD CONSTRAINT "fk_quests_items_1" FOREIGN KEY ("item_id") REFERENCES "items" ("id");
ALTER TABLE "registrations" ADD CONSTRAINT "fk_registrations_users_1" FOREIGN KEY ("user_id") REFERENCES "users" ("id");
ALTER TABLE "roles_levels" ADD CONSTRAINT "fk_roles_levels_roles_1" FOREIGN KEY ("role_id") REFERENCES "roles" ("id");
ALTER TABLE "roles_skills" ADD CONSTRAINT "fk_roles_skills_roles_skills_1" FOREIGN KEY ("invalid_skill_id") REFERENCES "roles_skills" ("id");
ALTER TABLE "roles_skills" ADD CONSTRAINT "fk_roles_skills_roles_skills_2" FOREIGN KEY ("required_skill_id") REFERENCES "roles_skills" ("id");
ALTER TABLE "team_invites" ADD CONSTRAINT "fk_team_invites_users_1" FOREIGN KEY ("user_id") REFERENCES "users" ("id");
ALTER TABLE "team_invites" ADD CONSTRAINT "fk_team_invites_teams_1" FOREIGN KEY ("team_id") REFERENCES "teams" ("id");
ALTER TABLE "teams" ADD CONSTRAINT "fk_teams_users_1" FOREIGN KEY ("owner_id") REFERENCES "users" ("id");
ALTER TABLE "teams_chats" ADD CONSTRAINT "fk_teams_chats_users_1" FOREIGN KEY ("sender_id") REFERENCES "users" ("id");
ALTER TABLE "teams_chats" ADD CONSTRAINT "fk_teams_chats_teams_1" FOREIGN KEY ("team_id") REFERENCES "teams" ("id");
ALTER TABLE "users" ADD CONSTRAINT "fk_users_teams_1" FOREIGN KEY ("team_id") REFERENCES "teams" ("id");
ALTER TABLE "users_chats" ADD CONSTRAINT "fk_users_chats_users_1" FOREIGN KEY ("from_user_id") REFERENCES "users" ("id");
ALTER TABLE "users_chats" ADD CONSTRAINT "fk_users_chats_users_2" FOREIGN KEY ("to_user_id") REFERENCES "users" ("id");
