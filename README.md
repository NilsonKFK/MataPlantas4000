# 🌱 Sistema de Riego Inteligente IoT **(MataPlantas4000)** 

## 📖 Descripción

Proyecto desarrollado para la asignatura de Sistemas Operativos Embebidos utilizando comunicación **ESP-NOW**, una aplicación web en **Flask** y una base de datos **SQLite**.

El sistema simula un sistema de riego inteligente utilizando tres microcontroladores.

---

# 👨‍💻 Integrantes

- Nilson Ayala
- Melissa Rojas
- Hugo Cornejo

---
# 👨‍🏫 Profesor 

Profesor Don José Fernando Poblete Cabezas (El Unico Mas Fuerte que Batman 🦹🏻‍♂️)

---


# ⚙ Tecnologías utilizadas

- ESP8266
- ESP32
- ESP-NOW
- Python
- Flask
- SQLite
- HTML
- CSS
- JavaScript

---

# 🛠 Arquitectura

```
              ESP1
        (Temperatura y Humedad)
               │
               │ ESP-NOW
               ▼
          ESP3 CENTRAL
        (Servidor Flask)
               ▲
               │ ESP-NOW
               │
            ESP2
       (Nivel del estanque)
```

---

# ESP1

Funciones

- Lectura de temperatura.
- Lectura de humedad.
- LED Verde:
  - Sistema normal.
- LED Rojo:
  - Temperatura alta.
  - Humedad baja.
  - Error del sensor.

---

# ESP2

Funciones

- Simulación del nivel del estanque mediante potenciómetro.
- LED Amarillo:
  - Nivel bajo del estanque.

---

# ESP3

Funciones

- Receptor ESP-NOW.
- Comunicación con Flask.
- Control de bomba mediante LED.
- Envío de datos a la página web.
- Registro de eventos.

---

# Página Web

La aplicación web permite visualizar:

- Temperatura
- Humedad
- Nivel del estanque
- Estado del sistema
- Alertas
- Estado de la bomba
- Encendido y apagado de la bomba

---

# Base de Datos

Se utiliza SQLite.

Archivo

riego.db

La base de datos almacena:

- Encendido de bomba.
- Apagado de bomba.
- Cambios de estado.
- Alertas del sistema.

---

# Archivos del proyecto

```
actividad3eva/

app.py

requirements.txt

README.md

riego.db

templates/
    index.html

static/
    style.css
```

---

# Ejecutar

Instalar dependencias

```
pip install -r requirements.txt
```

Ejecutar

```
python3 app.py
```

Abrir navegador

```
http://127.0.0.1:5000
```

---

# Autor

Nilson Ayala
Melissa Rojas
Hugo Cornejo
INACAP
2026
