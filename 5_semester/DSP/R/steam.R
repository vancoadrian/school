#------Programovanie pre datovu vedu------
#----------------Projekt 1----------------
#-----------------------------------------
#---Vztah medzi pomerom pozitivnych------- 
#------a negativnych hodnoteni------------
#---------------a cenou-------------------
#-----------------------------------------
#Autori: Michaela Hankova, Adrian Vanco---


#pouzite kniznice
library(ggplot2)
library(tidyverse)
library(gmodels)
library(dplyr)

#nacitanie datasetu, v rovnakom priecinku ako sa nachadza steam.R
steam_data = read_csv(file.path("steam.csv"))
#getwd()

#-----------------------------------------
#-----------Predstavenie dat--------------
#-----------------------------------------

#zakladne informacie o datach
glimpse(steam_data)


#kontrola duplicity (ci je kazda hra jedinecna na zaklade prideleneho appid, kedze rozdielne hry mozu mat rovnaky nazov)
steam_data$appid[duplicated(steam_data$appid)]

#skontrolujeme kde vsade chybaju hodnoty (NA) vidim ze len v stlpci publicher a 6 krat, takze mozme pokracovat ten stlpec aj tak nebudeme potrebovat
steam_data %>% filter_all(any_vars(is.na(.)))

#skontroluje pre istotu stlpce, ktore budeme potrebovat na overenie nasej hypotezy
steam_data[is.na(steam_data$price),]
steam_data[is.nan(steam_data$price),]
steam_data[is.na(steam_data$positive_ratings),]
steam_data[is.na(steam_data$negative_ratings),]
steam_data[is.nan(steam_data$positive_ratings),]
steam_data[is.nan(steam_data$negative_ratings),]


#-----------------------------------------
#-------------SUBSETOVANIE----------------
#-----------------------------------------

#pred subsetom 27075 hier a 18 stlpcov
dim(steam_data)

#odobratie nepotrebnych stlpcov
steam_data = subset(steam_data, select = -c(appid,release_date, english, developer, publisher, platforms, required_age,
                                           categories, genres, steamspy_tags, achievements,median_playtime))

#po odobrati nepotrebnych stlpcov 27075 hier a 6 stlpcov
dim(steam_data)
#zakomentovane funkcie na kontrolu
#steam_data
#names(steam_data)
#unique(steam_data$owners)


#konvertovanie libier na eura
steam_data$price = steam_data$price*1.18


#odfiltrovanie hier, ktore su irelevantne
steam_data = filter(steam_data,positive_ratings >= 1000,negative_ratings >= 1000,average_playtime >=1)

#po odfiltrovani irelevantnych hier zostalo 722 hier
dim(steam_data)


#rating vypocitame ako podiel pozitivnych a negativnych hodnoteni (vypocet pridame ako stlpec)
steam_data <- steam_data %>% mutate(rating =(steam_data$positive_ratings / steam_data$negative_ratings))

#mame o stlpec viac
dim(steam_data)

# graf znazornujuci pocet vlastnikov hry podla ceny hry
graf <- steam_data %>%
  mutate(owners = fct_relevel(owners, 
                              "20000-50000", "50000-100000", "100000-200000", 
                              "200000-500000", "500000-1000000", "1000000-2000000", 
                              "2000000-5000000", "5000000-10000000", "10000000-20000000",
                              "20000000-50000000", "50000000-100000000", "100000000-200000000")) %>%
  ggplot(aes(x=price, y=owners)) +
  geom_point()

graf


#-----------------------------------------
#----------krizova validacia--------------
#----------(Cross validation)-------------
#-----------------------------------------


# vytvorenie 40 podvzoriek zo zdrojovych dat, kazda ma 72 hier
data <- map(1:40, ~ steam_data[sort(sample(1:dim(steam_data)[1], size = 0.1*dim(steam_data)[1])),])
#summary(data)


#fitnem linearny regression model na kazdu podvzorku
models <- map(data, ~ lm(.x$rating ~ .x$price))

#koeficienty pre kazdu podvzorku, mozeme vidiet ze vacsina beta 1 je zaporna takze so stupajucou cenou, rating klesa
map_dbl(models, ~ coef(.x)[2])%>% print(.)
#models


#extrakcia beta koeficientov a rezidualov z kazdeho modelu
listOfFunctions <- list(coefficients = coef, residuals = residuals)
f <- function(x) {sapply(listOfFunctions, function(g) g(x))}
extractedData <- map(models, ~ f(.x)) 
extractedData
#str(extractedData)

#vypocet standardnej odchylky hodnot urovnovej konstanty (beta 0) a sklonu (beta 1)
sd(map_dbl(models, ~ coef(.x)[1]))
sd(map_dbl(models, ~ coef(.x)[2]))


#vypocet smerodajnej chyby zvyskoveho suctu stvorcov
rss <- map_dbl(models, ~ sum(resid(.x)^2))
rse <- map_dbl(rss, ~ sqrt(.x/0.1*dim(steam_data)[1]-2))
boxplot(rss)
boxplot(rse)



#kontrola zavislosti
cfs <- map_dbl(models, ~ coef(.x)[2])
#vidime ze hodnotenie klesa v priemere priblizne o 2,8% za euro
t.test(cfs, mu=0)




#-----------------------------------------
#--------------Vizualizacia---------------
#-----------------------------------------

graf <- steam_data %>%
  mutate(owners = fct_relevel(owners, 
                "20000-50000", "50000-100000", "100000-200000", 
                "200000-500000", "500000-1000000", "1000000-2000000", 
                "2000000-5000000", "5000000-10000000", "10000000-20000000",
                "20000000-50000000", "50000000-100000000", "100000000-200000000")) %>%
  ggplot(aes(x=price, y=rating, color = owners)) +
  geom_point() + geom_smooth(formula = y ~ x, method = "lm", colour = "Red")


graf + scale_colour_manual(values = c("red", "blue", "green",
                                   "yellow", "black", "firebrick4",
                                   "darkgreen", "violet", "orange",
                                   "olivedrab1", "grey47", "darkorchid4"))


#iba pre zaujimavost rozdelenie do plotov podla poctu vlastnikov
#ggplot(steam_data,aes(x = price,y = rating, color = owners)) + geom_point() + geom_smooth(formula = y ~ x, method = "lm", colour = "Red") + facet_wrap(~ owners,nrow = 3)


