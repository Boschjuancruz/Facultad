# ==============================================================================
# PROYECTO: ANÁLISIS DE CARGA DE AUTOS ELÉCTRICOS
# ==============================================================================

# 1. CARGA DE LIBRERÍAS --------------------------------------------------------

library(tidyverse)
library(psych)    
library(e1071)    
library(viridis)  

# 2. CARGA DE DATOS ------------------------------------------------------------
# Asegúrate de que la ruta sea correcta en tu computadora
ruta <- "C:/Users/*****/Downloads/autos_electricos.csv"
autos_electricos <- read.csv(ruta)

# 3. LIMPIEZA Y TRANSFORMACIÓN (WRANGLING) -------------------------------------

autos_electricos <- autos_electricos %>%
  # A. Manejo de Fechas y Tiempo
  mutate(
    Charging.Start.Time = as.POSIXct(Charging.Start.Time, format = "%d/%m/%Y %H:%M"),
    Charging.End.Time   = as.POSIXct(Charging.End.Time, format = "%d/%m/%Y %H:%M"),
    # Calculamos duración en minutos usando difftime para mayor precisión
    duracion_de_carga   = as.numeric(difftime(Charging.End.Time, Charging.Start.Time, units = "mins"))
  ) %>%
  
  # B. Limpieza de State of Charge (SoC) - Porcentajes de batería
  mutate(
    # El SoC no puede ser > 100 ni < 0
    State.of.Charge..Start... = case_when(
      State.of.Charge..Start... > 100 ~ 100,
      State.of.Charge..Start... < 0   ~ 0,
      TRUE ~ State.of.Charge..Start...
    ),
    # El fin de carga no puede ser menor al inicio ni mayor a 100
    State.of.Charge..End... = pmax(State.of.Charge..Start..., pmin(100, State.of.Charge..End...)),
    
    # Diferencia real de carga
    cantidad_de_carga = State.of.Charge..End... - State.of.Charge..Start...
  ) %>%
  
  # C. Ordenar días de la semana como un factor (para que salgan en orden en los gráficos)
  mutate(
    Day.of.Week = factor(Day.of.Week, 
                         levels = c("Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"))
  )

# 4. IMPUTACIÓN DE VALORES FALTANTES (NAs) --------------------------------------

# Imputamos la distancia basada en el promedio según el tipo de usuario
autos_electricos <- autos_electricos %>%
  group_by(User.Type) %>%
  mutate(Distance.Driven..since.last.charge...km. = replace_na(
    Distance.Driven..since.last.charge...km., 
    mean(Distance.Driven..since.last.charge...km., na.rm = TRUE))
  ) %>%
  ungroup()

# Imputamos energía basada en el promedio según el tipo de cargador
autos_electricos <- autos_electricos %>%
  group_by(Charger.Type) %>%
  mutate(Energy.Consumed..kWh. = replace_na(
    Energy.Consumed..kWh., 
    mean(Energy.Consumed..kWh., na.rm = TRUE))
  ) %>%
  ungroup()

# Imputamos Battery Capacity usando la moda (valor más común) por Modelo de Vehículo
autos_electricos <- autos_electricos %>%
  group_by(Vehicle.Model) %>%
  mutate(Battery.Capacity..kWh. = as.numeric(names(sort(table(Battery.Capacity..kWh.), decreasing = TRUE)[1]))) %>%
  ungroup()

# 5. ELIMINACIÓN DE COLUMNAS NO DESEADAS ---------------------------------------
autos_electricos <- autos_electricos %>%
  select(-User.ID, -Charging.Duration..hours., -Charging.Station.ID)

# 6. ANÁLISIS DE OUTLIERS (VALORES ATÍPICOS) -----------------------------------

# Función para identificar filas que son outliers estadísticos
identificar_outliers <- function(data, columna) {
  stats <- boxplot.stats(data[[columna]])
  outliers <- data %>% filter(data[[columna]] %in% stats$out)
  return(outliers)
}

outliers_costo <- identificar_outliers(autos_electricos, "Charging.Cost..USD.")
print("Resumen de outliers en costo:")
print(outliers_costo)

# 7. VISUALIZACIÓN DE DATOS ----------------------------------------------------

# Gráfico 1: Uso por día de la semana
ggplot(autos_electricos, aes(x = Day.of.Week, fill = Day.of.Week)) +
  geom_bar() +
  scale_fill_viridis_d() +
  labs(title = "Frecuencia de Carga por Día", x = "Día", y = "Cantidad de Sesiones") +
  theme_minimal() +
  theme(legend.position = "none")

# Gráfico 2: Relación Energía vs Costo
ggplot(autos_electricos, aes(x = Energy.Consumed..kWh., y = Charging.Cost..USD.)) +
  geom_point(alpha = 0.5, color = "steelblue") +
  geom_smooth(method = "lm", color = "darkred") +
  labs(title = "Relación: Energía Consumida vs Costo", x = "Energía (kWh)", y = "Costo (USD)") +
  theme_minimal()

# Gráfico 3: Comparación de costos por Ubicación y Cargador
ggplot(autos_electricos, aes(x = Charging.Station.Location, y = Charging.Cost..USD., fill = Charger.Type)) +
  stat_summary(fun = "mean", geom = "bar", position = "dodge") +
  labs(title = "Costo Promedio por Ubicación y Tipo de Cargador", y = "Costo Promedio (USD)") +
  theme_minimal() +
  theme(axis.text.x = element_text(angle = 45, hjust = 1))

# 8. ANÁLISIS ESTADÍSTICO (ANOVA) ----------------------------------------------
# ¿El tipo de cargador afecta significativamente el costo?

modelo_anova <- aov(Charging.Cost..USD. ~ Charger.Type, data = autos_electricos)
print(summary(modelo_anova))

# Si el ANOVA es significativo, ver qué grupos difieren (Tukey)
if(summary(modelo_anova)[[1]][["Pr(>F)"]][1] < 0.05) {
  print(TukeyHSD(modelo_anova))
}

