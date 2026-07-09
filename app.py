from flask import Flask, render_template, jsonify
import serial
import threading
import sqlite3
from datetime import datetime

app = Flask(__name__)

ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

datos = {
    "temp": "--",
    "hum": "--",
    "nivel": "--",
    "estado": "--",
    "alerta": "--",
    "motor": "APAGADO"
}

ultimo_estado = ""
ultima_alerta = ""
ultimo_motor = "APAGADO"

def guardar_evento(tipo, descripcion):
    conn = sqlite3.connect("riego.db")
    cursor = conn.cursor()
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS eventos (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            fecha TEXT,
            tipo TEXT,
            descripcion TEXT
        )
    """)
    cursor.execute(
        "INSERT INTO eventos (fecha, tipo, descripcion) VALUES (?, ?, ?)",
        (datetime.now().strftime("%Y-%m-%d %H:%M:%S"), tipo, descripcion)
    )
    conn.commit()
    conn.close()

def leer_serial():
    global datos, ultimo_estado, ultima_alerta, ultimo_motor

    while True:
        try:
            linea = ser.readline().decode(errors="ignore").strip()

            if linea.startswith("DATA;"):
                partes = linea.split(";")

                if len(partes) >= 8:
                    nodo = partes[1]

                    if nodo == "ESP1":
                        datos["temp"] = partes[2]
                        datos["hum"] = partes[3]

                    if nodo == "ESP2":
                        datos["nivel"] = partes[4]

                    datos["estado"] = partes[5]
                    datos["alerta"] = partes[6]
                    datos["motor"] = partes[7]

                    if datos["estado"] != ultimo_estado:
                        guardar_evento("ESTADO", "Cambio a " + datos["estado"])
                        ultimo_estado = datos["estado"]

                    if datos["alerta"] != ultima_alerta:
                        guardar_evento("ALERTA", "Alerta: " + datos["alerta"])
                        ultima_alerta = datos["alerta"]

                    if datos["motor"] != ultimo_motor:
                        guardar_evento("MOTOR", "Motor: " + datos["motor"])
                        ultimo_motor = datos["motor"]

        except Exception as e:
            print("ERROR:", e)

threading.Thread(target=leer_serial, daemon=True).start()

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/datos")
def obtener_datos():
    return jsonify(datos)

@app.route("/motoron")
def motoron():
    global ultimo_motor
    datos["motor"] = "ENCENDIDO"
    ultimo_motor = "ENCENDIDO"
    ser.write(b"MOTOR_ON\n")
    guardar_evento("MOTOR", "Bomba encendida desde pagina")
    return jsonify({"motor": "ENCENDIDO"})

@app.route("/motoroff")
def motoroff():
    global ultimo_motor
    datos["motor"] = "APAGADO"
    ultimo_motor = "APAGADO"
    ser.write(b"MOTOR_OFF\n")
    guardar_evento("MOTOR", "Bomba apagada desde pagina")
    return jsonify({"motor": "APAGADO"})

@app.route("/eventos")
def eventos():
    conn = sqlite3.connect("riego.db")
    cursor = conn.cursor()
    cursor.execute("SELECT fecha, tipo, descripcion FROM eventos ORDER BY id DESC LIMIT 20")
    filas = cursor.fetchall()
    conn.close()
    return jsonify(filas)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
