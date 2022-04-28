from django.urls import path
#now import the views.py file into this code
from . import views
urlpatterns=[
  path('v1/health/',views.dota2),
  path('v2/patches/',views.patches),
  path('v2/players/<int:player_id>/game_exp/', views.player_id_game_exp),
  path('v2/players/<int:player_id>/game_objectives/', views.player_id_game_obj),
  path('v2/players/<int:player_id>/abilities/', views.player_id_game_abil),
  path('v3/matches/<int:match_id>/top_purchases/', views.top_purchases),
  path('v3/abilities/<int:ability_id>/usage/', views.usage),
  path('v3/statistics/tower_kills/', views.tower_kills),
  path('',views.index)
]