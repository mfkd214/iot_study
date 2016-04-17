import xmlrpc.client
import sys

s = xmlrpc.client.ServerProxy("http://localhost:8000")
s.move()

