import telethon
from telethon.tl import TLObject
from telethon.tl.custom.message import Message
import sys
import json
import time
import datetime
from datetime import tzinfo
from telethon.tl.types import *
import datetime
import base64

def dt2timestamp(d):
    try:
        t = d.timetuple()
        timeStamp = int(time.mktime(t))
        return timeStamp
    except ValueError as e:
        # print
        e
        # d = datetime.datetime.strptime(str2, "%Y-%m-%d %H:%M:%S")
        t = d.timetuple()
        timeStamp = int(time.mktime(t))
        return timeStamp

def array_to_json(array):
    ret=[]
    for j in array:
        if isinstance(j,(str,int)):
            ret.append(j)
        else:
            ret.append(class_to_json(j))

    return ret
def class_to_json(msg):
    ret = {}
    for i, j in vars(msg).items():
        if isinstance(j, (str, int )):
            ret[i] = j
        elif isinstance(j,list):
            ret[i] = array_to_json(j)
        elif isinstance(j, datetime.datetime):
            ret[i] = dt2timestamp(j)
        elif j is None:
            pass
        elif isinstance(j,bytes):
            ret[i]=str(j)
        else:
            ret[i]=class_to_json(j)
    ret["classname"]=type(msg).__name__
    return ret


if __name__ == '__main__':
    strmsg =sys.argv[1]#"Message(id=1, to_id=PeerUser(user_id=1022316322), date=datetime.datetime(2019, 11, 5, 9, 19, 22, tzinfo=datetime.timezone.utc), message='Please update Telegram to the latest version.\n The version you are using is out of date and may stop working soon.Update on Google Play to get dozens of new features including Scheduled Messages and Reminders, Archived Chats, Cloud Themes, Enhanced Privacy Settings, Animated Stickers and many more.https://play.google.com/store/apps/details?id=org.telegram.messenger', out=False, mentioned=False, media_unread=False, silent=False, post=False, from_scheduled=False, legacy=False, edit_hide=False, from_id=777000, fwd_from=None, via_bot_id=None, reply_to_msg_id=None, media=None, reply_markup=None, entities=[MessageEntityUrl(offset=302, length=68)], views=None, edit_date=None, post_author=None, grouped_id=None, restriction_reason=[])"
    #strmsg="VXNlcihpZD04MzU5MzE1MCwgaXNfc2VsZj1GYWxzZSwgY29udGFjdD1UcnVlLCBtdXR1YWxfY29udGFjdD1GYWxzZSwgZGVsZXRlZD1GYWxzZSwgYm90PUZhbHNlLCBib3RfY2hhdF9oaXN0b3J5PUZhbHNlLCBib3Rfbm9jaGF0cz1GYWxzZSwgdmVyaWZpZWQ9RmFsc2UsIHJlc3RyaWN0ZWQ9RmFsc2UsIG1pbj1GYWxzZSwgYm90X2lubGluZV9nZW89RmFsc2UsIGFjY2Vzc19oYXNoPTM2NzI2Mzc5NDQ1MjMwMzcyODEsIGZpcnN0X25hbWU9JysxNTUyMjkzNjM3MycsIGxhc3RfbmFtZT1Ob25lLCB1c2VybmFtZT0naGFycnlobzk3JywgcGhvbmU9Jzg2MTU1MjI5MzYzNzMnLCBwaG90bz1Vc2VyUHJvZmlsZVBob3RvKHBob3RvX2lkPTM1OTAyOTg0NTg3NTg2MTU1MiwgcGhvdG9fc21hbGw9RmlsZUxvY2F0aW9uKGRjX2lkPTUsIHZvbHVtZV9pZD04NTI5MzAzMjEsIGxvY2FsX2lkPTIwNTg4MCwgc2VjcmV0PTU3MzQ0Mjg0NDg0OTM5MTk0NTksIGZpbGVfcmVmZXJlbmNlPWInXHgwMF1ceGMyPXNjMlx4ODRceGY5XHhmY3xALCkpXHhhZVx4OTJceGZhXHgwMFx0XHhjYicpLCBwaG90b19iaWc9RmlsZUxvY2F0aW9uKGRjX2lkPTUsIHZvbHVtZV9pZD04NTI5MzAzMjEsIGxvY2FsX2lkPTIwNTg4Miwgc2VjcmV0PTUzMTAxNjYxOTA0NDQ4Nzk2MiwgZmlsZV9yZWZlcmVuY2U9YidceDAwXVx4YzI9c1x4YjclXHhhYlx4ODNceGYyPlx4YjVRNVx4OTdceDlhXHg4NSNceGVjXHhkNmEnKSksIHN0YXR1cz1Vc2VyU3RhdHVzUmVjZW50bHkoKSwgYm90X2luZm9fdmVyc2lvbj1Ob25lLCByZXN0cmljdGlvbl9yZWFzb249Tm9uZSwgYm90X2lubGluZV9wbGFjZWhvbGRlcj1Ob25lLCBsYW5nX2NvZGU9Tm9uZSk="
    #strmsg="TWVzc2FnZShpZD05NTgxOCwgdG9faWQ9UGVlckNoYW5uZWwoY2hhbm5lbF9pZD0xMDM4MjQ4NTE5KSwgZGF0ZT1kYXRldGltZS5kYXRldGltZSgyMDE5LCAxMSwgNiwgNiwgNTMsIHR6aW5mbz1kYXRldGltZS50aW1lem9uZS51dGMpLCBtZXNzYWdlPScnLCBvdXQ9RmFsc2UsIG1lbnRpb25lZD1GYWxzZSwgbWVkaWFfdW5yZWFkPUZhbHNlLCBzaWxlbnQ9RmFsc2UsIHBvc3Q9RmFsc2UsIGZyb21fc2NoZWR1bGVkPUZhbHNlLCBsZWdhY3k9RmFsc2UsIGVkaXRfaGlkZT1GYWxzZSwgZnJvbV9pZD04NTgyNTA0MzMsIGZ3ZF9mcm9tPU5vbmUsIHZpYV9ib3RfaWQ9Tm9uZSwgcmVwbHlfdG9fbXNnX2lkPU5vbmUsIG1lZGlhPU1lc3NhZ2VNZWRpYVBob3RvKHBob3RvPVBob3RvKGlkPTUwNTEwNDgxMzM3NDM3MTY0NzAsIGFjY2Vzc19oYXNoPTQ4MDAwMTYxMzQzNzk1MTgwMDIsIGZpbGVfcmVmZXJlbmNlPWInXFx4MDI9XFx4ZTJqR1xceDAwXFx4MDF2Sl1cXHhjMnI1XFx4ZDBcXHg5OFxceDhmXFx4OWFcXHIsXFx4YzRDKzhcXHgwMlxceDAzXFx4ZWJcXHg5ZFxceDgzXFx4OTknLCBkYXRlPWRhdGV0aW1lLmRhdGV0aW1lKDIwMTksIDExLCA2LCA2LCA1MywgdHppbmZvPWRhdGV0aW1lLnRpbWV6b25lLnV0YyksIHNpemVzPVtQaG90b1N0cmlwcGVkU2l6ZSh0eXBlPSdpJywgYnl0ZXM9YiJcXHgwMShcXHgxM1xceGFhOVxceDE4XFx4YzBcXHhmYVxceGQxXFx4OGRcXHhiY1xceGYwaVxceGYwXFx4YzEsXFx4ODBcXHhiMiErXFx4ZDM1J1xceGQ5J1xceGRhQFxceDhjXFx4OWNcXHhkNVxceDA4XFx4YWZFJVxceDE0XFx4MDFcXHhiZlxceGU0XFx4YzRcXHg4M1xceDg4XFx4ZDNcXHgxZkFOXFx4ZjJcXHhlMlxceGZlXFx4ZTJ+VFxceGYycHpcXHhmMUZqRkdcXHhmNmhcXHgwZlxceGZjXFx4YjFcXHg4ZlxceGZlXFx4ZjlcXHgxNFRcXHhiNFBcXHgwM0lcXHhlN1xceGJlPlxceDk0XFx4YmNcXHhlMlxceDhhKFxceDAxaFxceGEyXFx4OGFcXHgwMCIpLCBQaG90b1NpemUodHlwZT0nbScsIGxvY2F0aW9uPUZpbGVMb2NhdGlvblRvQmVEZXByZWNhdGVkKHZvbHVtZV9pZD04MDY2MjU4MDQsIGxvY2FsX2lkPTMwNDU0OCksIHc9MTUyLCBoPTMyMCwgc2l6ZT0xMTYyMiksIFBob3RvU2l6ZSh0eXBlPSd4JywgbG9jYXRpb249RmlsZUxvY2F0aW9uVG9CZURlcHJlY2F0ZWQodm9sdW1lX2lkPTgwNjYyNTgwNCwgbG9jYWxfaWQ9MzA0NTQ5KSwgdz0zNzksIGg9ODAwLCBzaXplPTQxOTQyKSwgUGhvdG9TaXplKHR5cGU9J3knLCBsb2NhdGlvbj1GaWxlTG9jYXRpb25Ub0JlRGVwcmVjYXRlZCh2b2x1bWVfaWQ9ODA2NjI1ODA0LCBsb2NhbF9pZD0zMDQ1NTApLCB3PTYwNiwgaD0xMjgwLCBzaXplPTcwNDU5KV0sIGRjX2lkPTEsIGhhc19zdGlja2Vycz1GYWxzZSksIHR0bF9zZWNvbmRzPU5vbmUpLCByZXBseV9tYXJrdXA9Tm9uZSwgZW50aXRpZXM9W10sIHZpZXdzPU5vbmUsIGVkaXRfZGF0ZT1Ob25lLCBwb3N0X2F1dGhvcj1Ob25lLCBncm91cGVkX2lkPU5vbmUsIHJlc3RyaWN0aW9uX3JlYXNvbj1bXSk="
    strmsg=strmsg.encode('utf-8')#bytes
    #print(strmsg)
    strmsg=base64.b64decode(strmsg)#pybase64.b64decode(strmsg)
    #print(strmsg)
    strmsg=strmsg.decode('utf-8')
    aa=""
    for c in strmsg:
        if c == '\\':
            aa+='\\'
        else:
            aa+=c

    #print("a",aa)
    strmsg=aa;
    strmsg=strmsg.strip('\n');
    #print(strmsg)

    strmsg = strmsg.replace('\n','\\n').replace('\r','').replace('\t','\\t')

    #print(strmsg)
    sb = eval(strmsg)
    print(json.dumps(class_to_json(sb)))
