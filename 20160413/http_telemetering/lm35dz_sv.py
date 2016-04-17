# -*- coding: utf-8 -*-
#*****************************************************************************#
#   lm35dz_sv.py
#
#   2016.03.19  mfkd    1.0.0   START
#*****************************************************************************#
import os
import sqlite3
import tornado.ioloop
import tornado.web
import tornado.websocket
from tornado.ioloop import PeriodicCallback

DEBUG = True

# CSSパス
CSS_PATH = '../../../../../static/css/bootstrap.css'

# JavaScriptパス(必ずjqueryが先)
JS_PATHS = list()
JS_PATHS.append('../../../../../static/js/jquery-1.11.3.min.js')
JS_PATHS.append('../../../../../static/js/bootstrap.min.js')

DB = sqlite3.connect(":memory:")

#----------------------------------------------------------#
#   DB 設定
#----------------------------------------------------------#
def _setting_db():
    DB.execute("""CREATE TABLE lm35dz (
                   YMDHHMM     TEXT
                 , TEMPERATURE TEXT
                 );""")

#----------------------------------------------------------#
#   ルーティング
#----------------------------------------------------------#
def _routing():
    """ ルーティング
    """
    app = tornado.web.Application([
            (r"/lm35dz/([0-9]{12})/(.*)/", DataStackHandler),
            (r"/lm35dz/av/",               ShowAllValueHandler),
            (r"/lm35dz/ws/",               SendWebSocketHandler),
            (r"/lm35dz/",                  IndexHandler),
            ],
            template_path = os.path.join(os.getcwd(), "templates"),
            static_path   = os.path.join(os.getcwd(), "static")
          )
    return app

#----------------------------------------------------------#
#   INSERT
#----------------------------------------------------------#
def _insert_db(iDatetime, iTemperature):
    """
    """
    DB.execute("""INSERT INTO lm35dz (
                    YMDHHMM
                  , TEMPERATURE
                  ) VALUES (
                    %s
                  , %s
                  )""" % (iDatetime, iTemperature))

#----------------------------------------------------------#
#   SELECT
#----------------------------------------------------------#
def _select_db_all():
    """
    """
    rec = list()
    results = list()

    cur = DB.execute("SELECT * FROM lm35dz ORDER BY YMDHHMM DESC")
    for r in cur:
        rec = list()
        rec.append(r[0])
        rec.append(r[1])

        results.append(rec)

    return results

#======================================================================#
#   class   DataStackHandler
#   /lm35dz/(ymd)/(temperatute)/
#======================================================================#
class DataStackHandler(tornado.web.RequestHandler):
    def get(self, iDatetime, iTemp):
        """
        """
        if DEBUG:
            print("[START] DataStackHandler " + iDatetime + "/" + iTemp)

        # insert
        _insert_db(iDatetime, iTemp)

        # render
        self.write("DataStackHandler")

#======================================================================#
#   class ShowAllValueHandler
#   /lm35dz/mem/
#======================================================================#
class ShowAllValueHandler(tornado.web.RequestHandler):
    def get(self):
        """
        """
        if DEBUG:
            print("[START] ShowAllValueHandler")

        results = _select_db_all()

        # render
        self.render("show_allvalue.html",
                    css_path    =   CSS_PATH,
                    js_paths    =   JS_PATHS,
                    results     =   results
        )

#======================================================================#
#   class SendWebSocketHandler
#   /lm35dz/ws/
#======================================================================#
class SendWebSocketHandler(tornado.websocket.WebSocketHandler):
    # on_message    recieve data
    # write_message send data
    def open(self):
        self.callback = PeriodicCallback(self._send_message, 10000)
        self.callback.start()
        print("[START] WebSocket")

    def on_message(self, message):
        print("[START] WebSocket on_message")
        print(message)

    def _send_message(self):
        cur = DB.execute("SELECT * FROM lm35dz ORDER BY YMDHHMM DESC")
        rec = cur.fetchone()

        send_value = ""
        if rec == None:
            send_value = "Data Nothing"
        else:
            send_value = "%s %s" % (rec[0], rec[1])

        self.write_message(send_value)

    def on_close(self):
        self.callback.stop()
        print("[ENDED] WebSocket")

#======================================================================#
#   class IndexHandler
#   /lm35dz/
#======================================================================#
class IndexHandler(tornado.web.RequestHandler):
    def get(self):
        """
        """
        if DEBUG:
            print("[START] IndexHandler")

        # render
        self.render("index_ws.html")

#******************************************************************************#
#   START
#******************************************************************************#
if __name__ == "__main__":

    _setting_db()

    app = _routing()
    app.listen(8888)
    tornado.ioloop.IOLoop.instance().start()

