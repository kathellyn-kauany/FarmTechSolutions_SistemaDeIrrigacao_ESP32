# FarmTech Solutions - Consulta a uma API meteorologica (item "Ir alem")
# A API usada e a OpenWeather (https://home.openweathermap.org)
import requests

parametros = {
    "q": "Sao Paulo,BR",
    "appid": "b3a12beeb6311de293ac62695b521863",
    "units": "metric",
    "lang": "pt_br"
}

# Endereco fixo com a localizacao da fazenda (SP).
endereco = "https://api.openweathermap.org/data/2.5/weather"

resultado = requests.get(endereco, params=parametros).json() 
descricao = resultado["weather"][0]["description"]
vai_chover = "rain" in resultado

print("===============================================")
print("  FarmTech Solutions - Consulta a uma API meteorologica    ")
print("===============================================")
print(f"Como está o clima agora : {descricao}")
print(f"Vai chover? {vai_chover}")
print("===============================================")
