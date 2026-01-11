#!/usr/bin/env python3
"""
Cliente COAP para probar el servidor ZephyrHomeHub
Envía datos de sensores simulados al recurso /temp-node
"""

import asyncio
import logging
import argparse
import random
from aiocoap import Context, Message, Code
from urllib.parse import urlencode

# Configurar logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("coap_client")

class ZephyrHomeHubClient:
    """Cliente para enviar datos al servidor COAP de ZephyrHomeHub"""
    
    def __init__(self, server_address="127.0.0.1", port=5683):
        self.server_url = f"coap://{server_address}:{port}"
        self.context = None
    
    async def connect(self):
        """Crear contexto COAP"""
        self.context = await Context.create_client_context()
        logger.info(f"✅ Cliente conectado a {self.server_url}")
    
    async def send_temperature(self, node_id, temperature=None, sensor_type="temperature"):
        """
        Enviar datos de temperatura al servidor
        
        Args:
            node_id (str): ID del nodo (ej: 'ESP32_A1B2')
            temperature (float): Temperatura a enviar (si es None, se genera aleatoria)
            sensor_type (str): Tipo de sensor
        """
        if self.context is None:
            await self.connect()
        
        # Generar temperatura si no se proporciona
        if temperature is None:
            temperature = round(random.uniform(18.0, 30.0), 2)
        
        # Crear URI con parámetros de consulta
        query_params = {
            "id": node_id,
            "type": sensor_type
        }
        uri = f"{self.server_url}/temp-node?id={node_id}"#&type=temperature"
        
        # Crear payload
        payload = f"{temperature:.2f}".encode('utf-8')
        
        # Crear mensaje COAP
        request = Message(
            code=Code.POST,
            uri=uri,
            payload=payload,
            observe=False
        )
        
        logger.info(f"📤 Enviando datos del nodo {node_id}:")
        logger.info(f"   URI: {uri}")
        logger.info(f"   Payload: {temperature}°C")
        logger.info(f"   Tipo: {sensor_type}")
        
        try:
            # Enviar request y esperar respuesta
            response = await self.context.request(request).response
            logger.info(f"✅ Respuesta del servidor: {response.code}")
            logger.info(f"📥 Payload recibido: {response.payload.decode('utf-8')}")
            return True
        except Exception as e:
            logger.error(f"❌ Error al enviar datos: {e}")
            return False
    
    async def discover_resources(self):
        """Descubrir recursos disponibles en el servidor"""
        if self.context is None:
            await self.connect()
        
        uri = f"{self.server_url}/.well-known/core"
        request = Message(code=Code.GET, uri=uri)
        
        logger.info(f"🔍 Descubriendo recursos en {uri}")
        
        try:
            response = await self.context.request(request).response
            logger.info(f"✅ Recursos encontrados:")
            logger.info(f"   {response.payload.decode('utf-8')}")
            return response.payload.decode('utf-8')
        except Exception as e:
            logger.error(f"❌ Error al descubrir recursos: {e}")
            return None
    
    async def send_multiple_nodes(self, num_nodes=3, interval=2):
        """
        Simular múltiples nodos enviando datos simultáneamente
        
        Args:
            num_nodes (int): Número de nodos a simular
            interval (int): Intervalo entre envíos (segundos)
        """
        tasks = []
        
        for i in range(num_nodes):
            node_id = f"ESP32_{i:04X}"
            temperature = round(random.uniform(18.0 + i, 28.0 + i), 2)
            
            task = self.send_temperature(
                node_id=node_id,
                temperature=temperature,
                sensor_type="temperature"
            )
            tasks.append(task)
            
            # Pequeño retardo entre envíos
            await asyncio.sleep(0.1)
        
        # Ejecutar todas las tareas concurrentemente
        results = await asyncio.gather(*tasks, return_exceptions=True)
        
        logger.info(f"📊 Resumen: {sum(1 for r in results if r is True)}/{num_nodes} envíos exitosos")
    
    async def continuous_simulation(self, node_id="ESP32_TEST", interval=5):
        """
        Simular un nodo enviando datos continuamente
        
        Args:
            node_id (str): ID del nodo
            interval (int): Intervalo entre envíos (segundos)
        """
        logger.info(f"🔄 Iniciando simulación continua para nodo {node_id}")
        logger.info(f"   Intervalo: {interval} segundos")
        logger.info(f"   Presiona Ctrl+C para detener")
        
        counter = 0
        try:
            while True:
                counter += 1
                # Temperatura con pequeña variación aleatoria
                base_temp = 22.0
                variation = random.uniform(-2.0, 2.0)
                temperature = round(base_temp + variation, 2)
                
                success = await self.send_temperature(
                    node_id=node_id,
                    temperature=temperature
                )
                
                if success:
                    logger.info(f"📈 Envío #{counter} exitoso. Próximo en {interval}s...")
                else:
                    logger.warning(f"⚠️  Envío #{counter} fallido")
                
                await asyncio.sleep(interval)
                
        except KeyboardInterrupt:
            logger.info("🛑 Simulación detenida por el usuario")
    
    async def close(self):
        """Cerrar conexión"""
        if self.context:
            await self.context.shutdown()
            logger.info("🔌 Cliente desconectado")

async def main():
    """Función principal con interfaz de línea de comandos"""
    parser = argparse.ArgumentParser(
        description="Cliente COAP para ZephyrHomeHub",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Ejemplos de uso:
  %(prog)s --single --node ESP32_A1B2 --temp 23.5
  %(prog)s --multiple --nodes 5
  %(prog)s --continuous --node TEST_NODE --interval 10
  %(prog)s --discover
        """
    )
    
    parser.add_argument("--server", default="127.0.0.1", help="Dirección del servidor (por defecto: 127.0.0.1)")
    parser.add_argument("--port", type=int, default=5683, help="Puerto del servidor (por defecto: 5683)")
    
    # Modos de operación
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--single", action="store_true", help="Enviar un solo mensaje")
    group.add_argument("--multiple", action="store_true", help="Simular múltiples nodos")
    group.add_argument("--continuous", action="store_true", help="Envío continuo")
    group.add_argument("--discover", action="store_true", help="Descubrir recursos")
    
    # Parámetros para modo single
    parser.add_argument("--node", help="ID del nodo (para modos single y continuous)")
    parser.add_argument("--temp", type=float, help="Temperatura a enviar (para modo single)")
    parser.add_argument("--type", default="temperature", help="Tipo de sensor")
    
    # Parámetros para modo multiple
    parser.add_argument("--nodes", type=int, default=3, help="Número de nodos (para modo multiple)")
    
    # Parámetros para modo continuous
    parser.add_argument("--interval", type=int, default=5, help="Intervalo en segundos (para modo continuous)")
    
    args = parser.parse_args()
    
    # Crear cliente
    client = ZephyrHomeHubClient(server_address=args.server, port=args.port)
    
    try:
        if args.discover:
            await client.discover_resources()
        
        elif args.single:
            if not args.node:
                parser.error("El modo --single requiere --node")
            
            await client.send_temperature(
                node_id=args.node,
                temperature=args.temp,
                sensor_type=args.type
            )
        
        elif args.multiple:
            await client.send_multiple_nodes(
                num_nodes=args.nodes,
                interval=args.interval
            )
        
        elif args.continuous:
            if not args.node:
                parser.error("El modo --continuous requiere --node")
            
            await client.continuous_simulation(
                node_id=args.node,
                interval=args.interval
            )
    
    except KeyboardInterrupt:
        logger.info("Operación cancelada por el usuario")
    
    finally:
        await client.close()

if __name__ == "__main__":
    asyncio.run(main())