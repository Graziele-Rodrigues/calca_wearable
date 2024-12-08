import asyncio
import csv
from bleak import BleakScanner, BleakClient

DEVICE_NAMES = ["SPU1_BLE_Gyroscope", "SPU2_BLE_Gyroscope", "SPU3_BLE_Gyroscope", "SPU4_BLE_Gyroscope"]
SERVICE_UUID = "181A"
CHARACTERISTIC_UUID = "2A5D"
OUTPUT_FILE = "dados_sensores.csv"

async def connect_and_listen(device_name):
    print(f"[{device_name}] Tentando conectar ao dispositivo BLE...")
    device = await BleakScanner.find_device_by_filter(
        lambda d, _: d.name == device_name, timeout=10
    )
    if not device:
        print(f"[{device_name}] Dispositivo não encontrado. Certifique-se de que está ligado e visível.")
        return

    print(f"[{device_name}] Dispositivo encontrado: {device.name}. Tentando conectar...")
    async with BleakClient(device.address) as client:
        try:
            print(f"[{device_name}] Conectado ao dispositivo.")
            
            # Abrindo o arquivo CSV para escrita
            with open(OUTPUT_FILE, mode="a", newline="") as file:
                writer = csv.writer(file)
                
                # Cabeçalho apenas na primeira execução
                if file.tell() == 0:
                    writer.writerow(["deviceID", "aX", "aY", "aZ", "gX", "gY", "gZ"])
                
                def notification_handler(_, data):
                    decoded_data = data.decode("utf-8").strip()
                    print(f"[{device_name}] Dados recebidos: {decoded_data}")
                    
                    # Dividindo os dados no formato esperado: id,aX,aY,aZ,gX,gY,gZ
                    try:
                        fields = decoded_data.split(",")
                        writer.writerow(fields)  # Escrevendo os dados no arquivo CSV
                    except Exception as e:
                        print(f"[{device_name}] Erro ao processar os dados: {e}")

                await client.start_notify(CHARACTERISTIC_UUID, notification_handler)
                print(f"[{device_name}] Notificações iniciadas. Recebendo dados...")
                while True:
                    await asyncio.sleep(1)
        except Exception as e:
            print(f"[{device_name}] Erro durante a operação: {e}")

async def main():
    tasks = [connect_and_listen(device_name) for device_name in DEVICE_NAMES]
    await asyncio.gather(*tasks)

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\n[SPU] Encerrando...")
