#!/usr/bin/env python3
"""Serve only the public demonstration folder on localhost; no uploads or API."""
from functools import partial
from http.server import ThreadingHTTPServer, SimpleHTTPRequestHandler
from pathlib import Path
import argparse
parser=argparse.ArgumentParser()
parser.add_argument('--port',type=int,default=8765)
args=parser.parse_args()
folder=Path(__file__).resolve().parents[1]/'web'
print(f'Demo: http://127.0.0.1:{args.port} (durdur: Ctrl+C)',flush=True)
ThreadingHTTPServer(('127.0.0.1',args.port),partial(SimpleHTTPRequestHandler,directory=str(folder))).serve_forever()
