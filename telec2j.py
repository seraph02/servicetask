import telethon
from telethon.tl import TLObject
#from telethon.tl.custom.message import Message
import sys
import json
import time
import datetime
from datetime import tzinfo
from telethon.tl.types import *
import datetime
import base64

from telethon.tl.types.messages import ChannelMessages

def dt2timestamp(d):
    try:
        return d.timestamp()
    except:
        return 0



def clean_empty4json(obj):
    if isinstance(obj, list):
        t_list = clean_list_empty4json(obj)
        if is_empty(t_list):
            t_json = None
        else:
            t_json = t_list
    else:
        t_json = clean_dict_empty4json(obj)

    return t_json


def is_empty(obj):
    t_str = str(obj).strip()
    if t_str == '[]':
        return True
    elif t_str == '{}':
        return True
    elif t_str == '':
        return True
    elif t_str == "''":
        return True
    elif t_str == '""':
        return True
    elif t_str == "'NULL":
        return True
    elif t_str == '"NULL"':
        return True
    elif t_str == 'NULL':
        return True
    elif t_str == 'None':
        return True
    elif len(t_str) < 1:
        return True
    else:
        return False


def clean_dict_empty4json(obj):
    t_json = {}
    for key, value in obj.items():

        if isinstance(value, list):
            t_list = clean_list_empty4json(value)
            if is_empty(t_list):
                value = None
            else:
                t_json[key] = t_list
        elif isinstance(value, dict):
            t_dic = clean_dict_empty4json(value)
            if is_empty(t_dic):
                value = None
            else:
                t_json[key] = t_dic
        elif is_empty(value):
            value = None

        if value is None:
            # print(key, value, 'None')
            pass
        else:
            t_json[key] = value

    return t_json


def clean_list_empty4json(obj):
    t_list = []
    if str(obj) == '[]':
        return t_list

    for it in obj:
        if isinstance(it, dict):
            dic = clean_dict_empty4json(it)
            t_list.append(dic)
        else:
            t_list.append(it)

    return t_list


def md5_hash(obj):
    if isinstance(obj, dict):
        obj = json.dumps(obj)
    md5 = hashlib.md5()
    md5.update(obj.encode('utf-8'))

    return md5.hexdigest()




class Message(object):
    def __init__(
            self,
            **kwargs
    ):
        for k, v in kwargs.items():
            #print(k, v)
            #print(type(v))
            if isinstance(v, (str, list, int, float, datetime.datetime)):
                setattr(self, k, v)
            elif v is None:
                continue
            else:
                setattr(self, k,  class_to_json(v))

class ChannelFull(object):
    def __init__(
            self,
            **kwargs
    ):
        for k, v in kwargs.items():
            #print(k, v)
            if isinstance(v, (str, list, int, float, datetime.datetime)):
                setattr(self, k, v)
            elif v is None:
                continue
            else:
                setattr(self, k,  json.dumps(class_to_json(v)))

class ChatFull(object):
    def __init__(
            self,
            **kwargs
    ):
        for k, v in kwargs.items():
            #print(k, v)
            if isinstance(v, (str, list, int, float, datetime.datetime)):
                setattr(self, k, v)
            elif v is None:
                continue
            else:
                setattr(self, k,  json.dumps(class_to_json(v)))


class WebPage(object):
    def __init__(
            self,
            **kwargs
    ):
        for k, v in kwargs.items():
            #print(k, v)
            if isinstance(v, (str, list, int, float, datetime.datetime)):
                setattr(self, k, v)
            elif v is None:
                continue
            else:
                setattr(self, k,  json.dumps(class_to_json(v)))




#
# class ChannelFull(TLObject):
#     CONSTRUCTOR_ID = 0x2d895c74
#     SUBCLASS_OF_ID = 0xd49a2697
#
#     # noinspection PyShadowingBuiltins
#     def __init__(self, id: int = None, about: str = None, read_inbox_max_id: int = None, read_outbox_max_id: int = None,
#                  unread_count: int = None,
#                  chat_photo: 'TypePhoto' = None, notify_settings: 'TypePeerNotifySettings' = None,
#                  exported_invite: 'TypeExportedChatInvite' = None, bot_info: List['TypeBotInfo'] = None,
#                  pts: int = None,
#                  can_view_participants: Optional[bool] = None, can_set_username: Optional[bool] = None,
#                  can_set_stickers: Optional[bool] = None, hidden_prehistory: Optional[bool] = None,
#                  can_view_stats: Optional[bool] = None, can_set_location: Optional[bool] = None,
#                  has_scheduled: Optional[bool] = None, participants_count: Optional[int] = None,
#                  admins_count: Optional[int] = None, kicked_count: Optional[int] = None,
#                  banned_count: Optional[int] = None, online_count: Optional[int] = None,
#                  migrated_from_chat_id: Optional[int] = None, migrated_from_max_id: Optional[int] = None,
#                  pinned_msg_id: Optional[int] = None, stickerset: Optional['TypeStickerSet'] = None,
#                  available_min_id: Optional[int] = None, folder_id: Optional[int] = None,
#                  linked_chat_id: Optional[int] = None, location: Optional['TypeChannelLocation'] = None,
#                  slowmode_seconds: Optional[int] = None, slowmode_next_send_date: Optional[datetime.datetime] = None):
#         """
#         Constructor for ChatFull: Instance of either ChatFull, ChannelFull.
#         """
#         self.id = id
#         self.about = about
#         self.read_inbox_max_id = read_inbox_max_id
#         self.read_outbox_max_id = read_outbox_max_id
#         self.unread_count = unread_count
#         self.chat_photo = chat_photo
#         self.notify_settings = notify_settings
#         self.exported_invite = exported_invite
#         self.bot_info = bot_info
#         self.pts = pts
#         self.can_view_participants = can_view_participants
#         self.can_set_username = can_set_username
#         self.can_set_stickers = can_set_stickers
#         self.hidden_prehistory = hidden_prehistory
#         self.can_view_stats = can_view_stats
#         self.can_set_location = can_set_location
#         self.has_scheduled = has_scheduled
#         self.participants_count = participants_count
#         self.admins_count = admins_count
#         self.kicked_count = kicked_count
#         self.banned_count = banned_count
#         self.online_count = online_count
#         self.migrated_from_chat_id = migrated_from_chat_id
#         self.migrated_from_max_id = migrated_from_max_id
#         self.pinned_msg_id = pinned_msg_id
#         self.stickerset = stickerset
#         self.available_min_id = available_min_id
#         self.folder_id = folder_id
#         self.linked_chat_id = linked_chat_id
#         self.location = location
#         self.slowmode_seconds = slowmode_seconds
#         self.slowmode_next_send_date = slowmode_next_send_date
#
#
# class ChatFull(TLObject):
#     CONSTRUCTOR_ID = 0x1b7c9db3
#     SUBCLASS_OF_ID = 0xd49a2697
#
#     # noinspection PyShadowingBuiltins
#     def __init__(self, id: int = None, full_chat=None, chats: list = None, users: list = None, about: str = None,
#                  participants: 'TypeChatParticipants' = None,
#                  notify_settings: 'TypePeerNotifySettings' = None, exported_invite: 'TypeExportedChatInvite' = None,
#                  can_set_username: Optional[bool] = None, has_scheduled: Optional[bool] = None,
#                  chat_photo: Optional['TypePhoto'] = None, bot_info: Optional[List['TypeBotInfo']] = None,
#                  pinned_msg_id: Optional[int] = None, folder_id: Optional[int] = None):
#         """
#         Constructor for ChatFull: Instance of either ChatFull, ChannelFull.
#         """
#         self.id = id
#         self.full_chat = full_chat
#         self.chats = chats
#         self.users = users
#         self.about = about
#         self.participants = participants
#         self.notify_settings = notify_settings
#         self.exported_invite = exported_invite
#         self.can_set_username = can_set_username
#         self.has_scheduled = has_scheduled
#         self.chat_photo = chat_photo
#         self.bot_info = bot_info
#         self.pinned_msg_id = pinned_msg_id
#         self.folder_id = folder_id


def array_to_json(array):
    ret = []
    for j in array:
        if isinstance(j, (str, int)):
            ret.append(j)
        else:
            ret.append(class_to_json(j))

    return ret


def class_to_json(msg):
    ret = {}
    if isinstance(msg, dict):
        return msg

    for i, j in vars(msg).items():

        if isinstance(j, (int, float)):
            ret[i] = j
        elif isinstance(j, str):
            ret[i] = j
            #.replace("'", "\\'").replace("\"", "\\\"")
        elif isinstance(j, list):
            ret[i] = array_to_json(j)
        elif isinstance(j, datetime.datetime):
            ret[i] = dt2timestamp(j)
        elif j is None:
            pass
        elif isinstance(j, bytes):
            ret[i] = str(j)
        else:
            ret[i] = class_to_json(j)

        if i == "cached_page" and j != None:
            # print("aaa")
            ret[i] = json.dumps(ret[i])

    ret["classname"] = type(msg).__name__
    return ret


def class_to_dict(msg):
    ret = {}
    for i in msg:
        key = i
        value = msg[key]
        if '_' == key:
            msg["classname"] = str(value)
            del msg[key]
            continue
        if 'dict' == type(value).__name__:
            value = class_to_dict(value)
        elif 'datetime' == type(value).__name__:
            value = dt2timestamp(value)
            # if str(value).find("{") == 0:
            #     print(str(value).rfind("}"))
            #     dt = eval(value)
            #     msg[key] = dt2timestamp(dt)
            #     value = msg[key]
        ret[key] = value

    return ret


def msg2json(str):
    eval_obj = None
    # try:
    #     eval_obj = eval(str)
    # except Exception as e:
    #     return None
    eval_obj = eval(str)

    stype = type(eval_obj).__name__
    if 'dict' == stype:
        if ('Message' != eval_obj["_"]) and ('User' != eval_obj["_"]) and ('Channel' != eval_obj["_"]):
            # print(str)
            return
        json_o = class_to_dict(eval_obj)
    else:
        if ('Message' != stype) and ('User' != stype) and ('Channel' != stype) and ('ChatFull' != stype) and ('MessageService' != stype):
            # print(str)
            return
        json_o = class_to_json(eval_obj)
    json_o = clean_empty4json(json_o)
    # print(json.dumps(class_to_json(sb)))
    return json_o


if __name__ == '__main__':
    strmsg = sys.argv[1]
    if len(sys.argv) > 2:
       f = open(strmsg)
       aa = f.readline()
       strmsg = aa
    strmsg = strmsg.encode('utf-8')#bytes
    #print(strmsg)
    strmsg = base64.b64decode(strmsg)#pybase64.b64decode(strmsg)
    #print(strmsg)
    strmsg = strmsg.decode('utf-8')
    aa = ""
    for c in strmsg:
        if c == '\\':
            aa += '\\'
        else:
            aa += c

    #print("a",aa)
    strmsg = aa
    str_msg = strmsg.strip('\n')
    #print(strmsg)

    str_msg = str_msg.replace('\n', '\\n').replace('\r', '').replace('\t', '\\t')
    # print(strmsg)
    sb = msg2json(str_msg)
    print(json.dumps(class_to_json(sb)))

