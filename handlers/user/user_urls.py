#coding=utf-8
from handlers.user.login_handler import LoginHandler
from handlers.user.register_handler import RegisterHandler
user_urls = [
    (r"/login",LoginHandler),
    (r"/register",RegisterHandler)
]