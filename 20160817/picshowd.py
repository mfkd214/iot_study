from http.server import HTTPServer, SimpleHTTPRequestHandler

host = '192.168.42.100'
port = 8000
httpd = HTTPServer((host, port), SimpleHTTPRequestHandler)
print('serving at port', port)
httpd.serve_forever()
