# ==============================================================================
# FARMTECH SOLUTIONS - DATA SCIENCE EM R
# Mapa de Calor para Irrigação de Cafezal
# ==============================================================================

# Instalar e Carregar Pacotes Necessários
if(!require(ggplot2)) install.packages("ggplot2")
if(!require(dplyr)) install.packages("dplyr")

library(ggplot2)
library(dplyr)

# Carregar o Conjunto de Dados Históricos
dados <- read.csv("historico_cafe.csv")

# Convertendo acao_rele para Fator Categorico (0 = Desligado, 1 = Ligado)
dados$acao_rele <- as.factor(dados$acao_rele)

# Previsão da Ação do Relé com base na Umidade e pH
modelo <- glm(acao_rele ~ umidade + ph, data = dados, family = binomial)

summary(modelo)

#Criar Grade de Dados para Predição Contínua 
grid_dados <- expand.grid(
  umidade = seq(30, 85, length.out = 100),
  ph = seq(4.0, 8.0, length.out = 100)
)

# Calcular a Probabilidade de Acionamento da Irrigação
grid_dados$probabilidade <- predict(modelo, newdata = grid_dados, type = "response")

#Gerar o Mapa de Calor (Heatmap) com ggplot2
grafico <- ggplot() +
  # Camada do Heatmap de Probabilidade
  geom_tile(data = grid_dados, aes(x = umidade, y = ph, fill = probabilidade), alpha = 0.85) +
  scale_fill_gradient2(
    low = "#1E3A8A",      # Azul (Baixa Probabilidade / Seguro)
    mid = "#F59E0B",      # Amarelo (Transição / Alerta)
    high = "#DC2626",     # Vermelho (Alta Probabilidade / Ligar Relé)
    midpoint = 0.5,
    name = "Probabilidade\nde Irrigação"
  ) +
  # Linhas de Contorno da Faixa Ideal do Café
  geom_hline(yintercept = c(5.5, 6.5), linetype = "dashed", color = "white", size = 0.8) +
  geom_vline(xintercept = 60, linetype = "dashed", color = "white", size = 0.8) +
  # Adicionar os Pontos Reais Coletados
  geom_point(data = dados, aes(x = umidade, y = ph, color = acao_rele), size = 3, shape = 21, stroke = 1.2, fill = "white") +
  scale_color_manual(values = c("0" = "blue", "1" = "red"), labels = c("Desligado", "Ligado"), name = "Estado do Relé") +
  # Rótulos e Anotações no Gráfico
  labs(
    title = "Mapa de Decisão de Irrigação - Cultura do Café",
    subtitle = "Probabilidade de Acionamento da Bomba (Relé Azul)",
    x = "Umidade do Solo (%)",
    y = "Nível de pH do Solo",
    caption = "Infratec Solutions | Modelo treinado em R via ggplot2"
  ) +
  theme_minimal() +
  theme(
    plot.title = element_text(face = "bold", size = 14, color = "#1F2937"),
    plot.subtitle = element_text(size = 11, color = "#4B5563"),
    axis.title = element_text(face = "bold", size = 11),
    panel.grid.major = element_line(color = "#E5E7EB")
  )

# Exibir e Salvar o Gráfico em Alta Resolução
print(grafico)
ggsave("heatmap_irrigacao_cafe.png", plot = grafico, width = 8, height = 6, dpi = 300)

cat("\n>>> Análise concluída! A imagem 'heatmap_irrigacao_cafe.png' foi salva no diretório.\n")