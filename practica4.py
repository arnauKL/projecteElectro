import asyncio
from bleak import BleakClient, BleakError
import struct
import matplotlib.pyplot as plt     # Per mostrar el plot

SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"
ADDRESS = "D8:13:2A:73:74:A6"

dadesSin = []
dadesCos = []

def callback(sender, data):

    [dadaSin1, dadaSin2, dadaCos1, dadaCos2] = struct.unpack('ffff', data)  # Rebre les 2 dades de bluetooth i descomprimir-les amb unpack

    dadesSin.append(dadaSin1)
    dadesSin.append(dadaSin2)
    dadesCos.append(dadaCos1)
    dadesCos.append(dadaCos2)

    # Plot del sinus
    plt.subplot(1,2,1)
    plt.plot(dadesSin, color=(0.4, 0.5, 0.5))
    plt.xlabel('dada n')
    plt.ylabel('OPAMPtud')
    plt.ylim(-110, 110)
    plt.title('Funció sinus')
    bb = dict(boxstyle="round", ec=(0.3, 0.4, 0.4), fc=(0.4, 0.5, 0.5),)
    plt.text(0, -80, f"Amplitud: {dadesSin[len(dadesSin)-1]}\n", fontsize=12, bbox=bb)

    # Plot del cosinus
    plt.subplot(1,2,2)
    plt.plot(dadesCos, color=(0.8,0.8,0.1))
    plt.xlabel('dada n')
    plt.ylabel('Amplitud')
    plt.ylim(-110, 110)
    plt.title('Funció cosinus')
    bb = dict(boxstyle="round", ec=(0.7, 0.7, 0), fc=(0.8,0.8,0.1))
    plt.text(0, -80, f"Amplitud: {dadesCos[len(dadesCos)-1]}\n", fontsize=12, bbox=bb)

    plt.pause(0.1)  # Mostra el plot i pausa per 0.1 segons
    plt.clf()       # un cop mostrat, esborrem el plot per evitar que apareixi diferents

    # Per mantenir una mida raonable de dades (menys de 100) i una finestra que llisca
    while (len(dadesSin) > 100):
        dadesSin.pop(0)        # Eliminar el primer element (posició 0)
    while (len(dadesCos) > 100):
        dadesCos.pop(0)

async def run():  
    try:
        async with BleakClient(ADDRESS) as client:
            print("Connectat")
            await client.start_notify(CHARACTERISTIC_UUID, callback)
            while True:
                await asyncio.sleep(1)
    except BleakError as e:
        print(f"Error rebuda: {e}")

if __name__ == "__main__":
    asyncio.run(run())