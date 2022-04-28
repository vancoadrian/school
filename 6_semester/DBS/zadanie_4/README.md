### users

Tabuľka obsahuje základné informácie o užívateľovi ako ID teamu, ak sa v nejakom nachádza, meno, prezývku, heslo a email

### registrations

Tabuľka uchováva informácie z registrácie, čas registrácie, dátum overenia, a to či sa overil správne. Záznam je spojený s ID užívateľa.

### friend_invites

Tabuľka registruje všetky žiadosti o priateľstvo, obsahuje ID oboch hráčov, aby sme vedeli kam smerovať pozvánku a odpoveď. Nachádza sa tu aj premenná status, ktorá môže nadobudnúť 3 stavy: prijatá, zamietnutá alebo čaka na odpoveď

### friends

Tabuľka slúži na spájanie jednotlivých kamarátov. Záznam je tvorený dvoma ID užívateľov, ktorí poslali alebo prijali pozvánku.

### users_chats

Tabuľka zaznamenáva správy medzi užívateľmi, ID odosielateľa a príjemcu a čas kedy boli odoslané. Preto, aby nedošlo k spamu musí hra najprv skontrolovať či sú uživtelia kamaráti, cez tabuľku friends, až potom odoslať správu.

### teams

Tabuľka uchováva informácie o jednotlivých tímoch ako meno tímu a majiteľa tímu. Ak užívateľ opustí tím tak sa jeho pozvánka (team_invites) vymaže a tiež pole s ID tímu v zázname o užívateľovi (users)

### team_invites

Tabuľka registruje všetky pozvánky do tímu, obsahuje ID hráča a tímu, do ktorého je pozvaný, aby sme vedeli kam smerovať pozvánku. Nachádza sa tu aj premenná status, ktorá môže nadobudnúť 3 stavy: prijatá, zamietnutá alebo čaká na odpoveď

### team_chats

Tabuľka zaznamenáva správy v tímovom chate, ID odosielateľa a ID tímu a čas kedy boli odoslané. 

### characters 

Tabuľka zaznamenáva všetky postavy, ktoré si hráči vytvoria. Sú v nej štatistiky a informácie o postave
- user_id: ID užívatela, ktorý si postavičku vytvoril
- game_id: ID na hru, v ktorej sa postava nachádza
- role_id: rola, ktorú si hráč vybral 
- role_level_id: level, na ktorej sa postava nachádza
- item_id: predmet, ktorý hráč používa
- name: meno postavy
- experience: skusenostné body postavy
- health: počet životov
- defence: obranné číslo

### items 

Zoznam predmetov, ktoré sa v hre nachádzajú. Obsahuje aj informácie o jednotlivom predmete.

### roles

Zoznam ról, ktoré sa v hre nachádzajú.

### skills

Zoznam schopnosti postáv. Každý záznam obsahuje aj informácie a požidavky pre schopnosť
- role_id: ID role, pre ktorú je schopnosť určená
- required_skill_id: požadavaná schopnosť, ktorú si užívateľ musí vybrať pred výberom tejto
- invalid_skill_id: ak užívatel má schopnosť nachádzajúcu sa v tomto políčku, nemôže si vybrať túto schopnosť
- name
- damage_modifier: násobok zlepšenia/zhoršenia útoku postavy
- health_modifier: násobok zlepšenia/zhoršenia životov postavy
- defence_modifier: násobok zlepšenia/zhoršenia obrany postavy
- required_level: požadovaný level 
- skill_type: typ schopnosti (pasívna, aktívna...)

### chosen_skills

Tabuľka zaznamenáva aké schopnosti si aká postava vybrala

### role_levels

Zoznam levlov, ktoré môže postava získať s prihliadnutím na jej rolu. Obsahuje základné informácie ako číslo levelu, počet skúsenosti na dosiahnutie levelu a počet pridaných životov, útočného čísla či obranného čísla

### games

Obsahuje ID mapy.

### maps

Obsahuje informácie o veľkosti mapy.

### map_positions

Obsahuje informácie o každom možnom políčku mapy. Jej súradnice, či na políčku môže byť predmet alebo monštrum. Pre políčko môže byť aj priradené nejaká špecifická lokalita

### locations

Obsahuje informácie o lokalitách hry

### quests

Tabulka obsahuje zoznam úloh, ktoré môže hráč plniť.
- description: opis úlohy
- name: meno úlohy
- recomended_level: odporúčaný level postavy pre dokončenie levelu
- required_level: level potrebný na začatie úlohy
- location_id: lokácia, v ktorej sa dá úloha vykonávať 
- exp_reward: počet pridaných skúseností po dokončení úlohy
- item_id: pole pre určenie predmetu, ktorý postava obdrží po dokončení úlohy
- needed_kills: počet zabitých príšer pre dokončenie úlohy

### monsters

Zátnam obsahuje základné informácie o príšere. Ako aj level od ktorého sa objavuje, množstvo skúsenostných bodov ktoré postava obdrží po jej zabití či predmet, ktorý "padne" po jej zabití.  

### kill_log 

V tejto tabuľke  zaznamenávame aká postava zabila akú príšeru, na ktorom políčku mapy z dôvodu aby sme zaznamenávali ktoré príšery hŕač a vypočátali aké skúsenosti hráč má. 

### quest_log

V tejto tabuľke zaznamenávame aké úlohy hráč splnil. Tiež vieme vypočítať skúsenosti za splnené úlohy.

### combat_log

Tabuľka slúži na zaznamenávanie všetkých útokov postáv. 
