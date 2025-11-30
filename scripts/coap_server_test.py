#!/usr/bin/env python3
"""
Simple CoAP server with /test resource for testing ZephyrHomeHub
"""

import asyncio
import logging
from aiocoap import *
from aiocoap import resource

# Enable detailed logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("coap_server")


class HelloResource(resource.Resource):
    """Handle requests to the /test resource"""
    
    async def render_get(self, request):
        """Handle GET requests to /test"""
        logger.info(f"GET request from {request.remote}")
        return Message(payload=b"Hello from CoAP server! (GET)")
    
    async def render_post(self, request):
        """Handle POST requests to /test"""
        payload = request.payload.decode('utf-8') if request.payload else "No payload"
        logger.info(f"POST request from {request.remote}: '{payload}'")
        
        response_payload = f"Server received: {payload}".encode('utf-8')
        return Message(payload=response_payload, code=CHANGED)


async def main():
    # Create root resource tree
    root = resource.Site()
    
    # Add the test resource
    root.add_resource(['test'], HelloResource())
    
    # Add a welcome resource at the root
    class WelcomeResource(resource.Resource):
        async def render_get(self, request):
            welcome_msg = """ZephyrHomeHub Test Server
Available resources:
- /test (GET, POST)
- /.well-known/core (GET)"""
            return Message(payload=welcome_msg.encode('utf-8'))
    
    root.add_resource([''], WelcomeResource())
    
    # Create and start the server
    server = await Context.create_server_context(root, bind=('0.0.0.0', 5683))
    
    logger.info("CoAP server running on port 5683")
    logger.info("Available resources:")
    logger.info("  - /test (GET, POST)")
    logger.info("  - /.well-known/core (GET)")
    logger.info("Use Ctrl+C to stop the server")
    
    # Keep server running
    try:
        await asyncio.get_running_loop().create_future()
    except KeyboardInterrupt:
        logger.info("Server stopped")


if __name__ == "__main__":
    asyncio.run(main())