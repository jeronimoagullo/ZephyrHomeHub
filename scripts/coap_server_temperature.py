#!/usr/bin/env python3
"""
Simple CoAP server with /temp-node resource for testing ZephyrHomeHub
"""

import asyncio
import logging
from aiocoap import *
from aiocoap import resource
from urllib.parse import parse_qs

# Enable detailed logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("coap_server")


class TempNodeResource(resource.Resource):
    """Handle requests to the /temp-node resource"""

    def _parse_query_params(self, request):
        """Parse URI query parameters into a dictionary"""
        query_dict = {}
        
        if request.opt.uri_query:
            try:
                # Handle nested tuple structure: (('id=FA436E48&type=temperature',),)
                # or multiple parameters: (('id=FA436E48', 'type=temperature'),)
                for inner_tuple in request.opt.uri_query:
                    logger.debug(f"Processing query item: {inner_tuple}")
                    
                    # Split by '&' if multiple parameters in one string
                    param_pairs = inner_tuple.split('&')
                    for pair in param_pairs:
                        if '=' in pair:
                            key, value = pair.split('=', 1)  # Split on first '=' only
                            query_dict[key] = value
                            logger.debug(f"Added to query_dict: {key} = {value}")
                
                logger.info(f"Options URI dict: {query_dict}")
                
            except Exception as e:
                logger.error(f"Error parsing query parameters: {e}")
        
        return query_dict

    def _extract_node_id(self, request):
        """Extract node_id from URI query parameters using dictionary"""
        query_dict = self._parse_query_params(request)
        node_id = query_dict.get('id')
        
        if node_id:
            logger.debug(f" Extracted node_id: {node_id}")
        else:
            logger.error("No node_id found in query")
        
        return node_id
    
    async def render_get(self, request):
        """Handle GET requests to /temp-node"""
        logger.info(f"GET request from {request.remote}")
        return Message(payload=b"Hello from CoAP server! (GET)")
    
    async def render_post(self, request):
        """Handle POST requests to /temp-node"""
        
        # Extract node_id from URI query
        node_id = self._extract_node_id(request)
        
        # Extract payload
        payload = request.payload.decode('utf-8') if request.payload else "No payload"
        
        if node_id:
            logger.info(f"POST from Node {node_id} at {request.remote}: '{payload}'")
        else:
            logger.info(f"POST from {request.remote}: '{payload}' (no node ID)")
        
        response_payload = f"OK".encode('utf-8')

        return Message(payload=response_payload, code=CHANGED)


async def main():
    # Create root resource tree
    root = resource.Site()
    
    # Add the temp-node resource
    root.add_resource(['temp-node'], TempNodeResource())
    
    # Add a welcome resource at the root
    class WelcomeResource(resource.Resource):
        async def render_get(self, request):
            welcome_msg = """ZephyrHomeHub Test Server
Available resources:
- /temp-node (GET, POST)
- /.well-known/core (GET)"""
            return Message(payload=welcome_msg.encode('utf-8'))
    
    root.add_resource([''], WelcomeResource())
    
    # Create and start the server
    server = await Context.create_server_context(root, bind=('0.0.0.0', 5683))
    
    logger.info("CoAP server running on port 5683")
    logger.info("Available resources:")
    logger.info("  - /temp-node (GET, POST)")
    logger.info("  - /.well-known/core (GET)")
    logger.info("Use Ctrl+C to stop the server")
    
    # Keep server running
    try:
        await asyncio.get_running_loop().create_future()
    except KeyboardInterrupt:
        logger.info("Server stopped")


if __name__ == "__main__":
    asyncio.run(main())