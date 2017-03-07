#!/usr/bin/env python
from collections import Counter
import logging
import os
from tornado.ioloop import IOLoop
from tornado.options import define, options, parse_command_line
from tornado.web import RequestHandler, Application
from tornado.websocket import *
import six, re
import argparse
#import pinfo

class NetWebSocket(tornado.websocket.WebSocketHandler):
    def open(self):
        print("WebSocket opened")
    def do_hello_server(self):
        pass
    def on_message(self, message):
        p = str(message)
        print("WebSocket on_message: '%s' " %(p))
        #try:
        #    print (eval(p));
        #except Exception as e:
        #    print("WebSocket on_message: '%s' : %s" %(p,str(e)))
    def on_close(self):
        print("WebSocket closed")

class IndexHandler(RequestHandler):
    def get(self):
        print("[+] render index.html")
        files = []
        for filename in os.listdir("t"):
            mtime = os.stat(os.path.join("t", filename)).st_mtime
            files.append((mtime, filename))
        # sort by descending mtime then ascending filename
        files.sort(key=lambda x: (-x[0], x[1]))
        self.render('index.html', files=[f[1] for f in files])

class ViewHandler(RequestHandler):
    def get(self):
        global cmdlineopts
        print("[+] render force.html")
        self.render('pinfo.html', filename=self.get_argument("filename"), \
                    serverurl = ("%s:%d" %(cmdlineopts.base, cmdlineopts.port)))

class DataHandler(RequestHandler):
    def get(self):
        self.write(net_to_json(self.get_argument('filename')))

class netgui:
    def __init__(self, opt, address='localhost',port=11000):
        handlers = [
            ('/', IndexHandler),
            ('/ws', NetWebSocket),
            ('/view', ViewHandler),
            ('/data', DataHandler),
        ]
        settings=dict(
            debug=0,
            static_path=os.path.join(os.path.dirname(__file__), 'static'),
            template_path=os.path.join(os.path.dirname(__file__), 'templates'),
        )
        app = Application(handlers, **settings)
        app.listen(port, address=address)
        print("Gui started at http://%s:%s" % (address or 'localhost', port))

    def start(self):
        IOLoop.instance().start()

    def stop(self):
        print("Stop netgui")
        IOLoop.instance().stop();

def main():
    global cmdlineopts
    parser = argparse.ArgumentParser()
    parser.add_argument("-v", "--verbose", action="count", default=0)
    parser.add_argument("-p", "--port", type=int, help='default gui port', default=11000)
    parser.add_argument("-b", "--base", type=str, help='server url', default="localhost")
    cmdlineopts = parser.parse_args()
    n = netgui(cmdlineopts, port=cmdlineopts.port)
    n.start()

if __name__ == "__main__":
    main()
        
