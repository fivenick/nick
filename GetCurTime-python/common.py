import os
import shutil
import sys
import time
import traceback

def GetCurTime():
    t = time.localtime(time.time())
    tt = str(t.tm_year) + (str(t.tm_mon) if len(str(t.tm_mon)) == 2 else ('0' + str(t.tm_mon))) + (str(t.tm_mday) if len(str(t.tm_mday)) == 2 else ('0' + str(t.tm_mday))) + (str(t.tm_hour) if len(str(t.tm_hour)) == 2 else ('0' + str(t.tm_hour))) + (str(t.tm_min) if len(str(t.tm_min)) == 2 else ('0' + str(t.tm_min))) + (str(t.tm_sec) if len(str(t.tm_sec)) == 2 else ('0' + str(t.tm_sec)))
    return tt
def GetFormatTime():
    t = time.localtime(time.time())
    tt = str(t.tm_year) + '/' + str(t.tm_mon) + '/' + str(t.tm_mday) + ' ' + str(t.tm_hour) + ':' + str(t.tm_min) + ':' + str(t.tm_sec)
    return tt


