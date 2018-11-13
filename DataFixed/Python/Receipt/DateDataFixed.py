#!/usr/bin/env python2
# -*- coding:utf-8 -*-

import datetime
import re
import logging
import traceback

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel
from utils.dateutil import parser

class DateDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self, 'Date')
        self.__Patterns__ = u'年|月|日|-|\.| |/|:'
        self.__Now__ = datetime.datetime.now()
        self.__MonthSwapMapping_ = {u'13' : u'10', u'00' : '08'}

    
    def __FixedData__(self, resultJson):
        result_datelist = self.__ParseData__(resultJson)
        if len(result_datelist) == 0:
            logging.info(u'Date Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(result_datelist[0] + u' Fixed To ')

        date = ''
        confidencelevel = ConfidenceLevel.Bad

        try:
            for result in result_datelist:
                confidencelevel, date = self.__FixedDateData__(result)
                if result.find(u'年') != -1 or result.find(u'月') != -1 or result.find(u'日') != -1:
                    result = date[0] + u'年' + date[1] + u'月' + date[2] + u'日'
                elif result.find(u'-') != -1:
                    result = date[0] + u'-' + date[1] + u'-' + date[2] + ' '
                elif result.find(u'/') != -1:
                    result = date[0] + u'/' + date[1] + u'/' + date[2] + ' '
                elif result.find(u'.') != -1:
                    result = date[0] + u'.' + date[1] + u'.' + date[2] + ' '
                else:
                    result = date[0] + date[1] + date[2] + ' '

                if len(date) >= 5:
                    result = result + date[3] + ':' + date[4]

                if len(date) >= 6:
                    result = result + ':' + date[5]

                if (confidencelevel == ConfidenceLevel.Confident or confidencelevel == ConfidenceLevel.Fixed) and self.__CheckData__(result):
                    break

        except Exception as err:
            traceback.print_exc()
            logging.info(u'FixedReceiptData err: {}'.format(err))
            result = result_datelist[0]

        logging.info(result)

        return confidencelevel, result.strip()


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_datelist = self.__ParseData__(resultJson)
        validated_datelist = self.__ParseData__(validateJson)

        if len(validated_datelist) == 0 or len(result_datelist) == 0 or not self.__CheckData__(validated_datelist[0]):
            logging.info(u'Validated Data Error')
            return

        if validated_datelist[0] != result_datelist[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_datelist[0] + u' Fixed To ')

        date = ''
        confidencelevel = ConfidenceLevel.Bad

        for result in result_datelist:
            confidencelevel, date = self.__FixedDateData__(result)
            if result.find(u'-') != -1:
                result = date[0] + u'-' + date[1] + u'-' + date[2] + ' '
            elif result.find(u'/') != -1:
                result = date[0] + u'/' + date[1] + u'/' + date[2] + ' '
            elif result.find(u'.') != -1:
                result = date[0] + u'.' + date[1] + u'.' + date[2] + ' '
            elif result.find(u'年') != -1 or result.find(u'月') != -1 or result.find(u'日') != -1:
                result = date[0] + u'年' + date[1] + u'月' + date[2] + u'日'
            else:
                result = date[0] + date[1] + date[2] + ' '

            if len(date) >= 5:
                result = result + date[3] + ':' + date[4]

            if len(date) >= 6:
                result = result + ':' + date[5]

            if (confidencelevel == ConfidenceLevel.Confident or confidencelevel == ConfidenceLevel.Fixed) and self.__CheckData__(result):
                break

        logging.info(date)

        if validated_datelist[0] == date:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_datelist[0])
            logging.info(u'Fixed Falied!')


    def __ParseData__(self, jsondata):
        datelist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return datelist

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None:
                continue

            cls = region[u'cls']
            if cls == 3:
                for result in region[u'result']:
                    if len(result):
                        datelist.append(result)

                if region.get(u'ref_result') != None:
                    for result in region[u'ref_result']:
                        if len(result):
                            datelist.append(result)

        return datelist


    def __CheckData__(self, data):
        if len(data) == 0:
           return False
        
        date = re.split(self.__Patterns__, data)
        date = list(filter(lambda x: len(x) != 0, date))

        for _date in date:
            if not self.__CheckNumber__(_date):
                return False

        dateint = list(map(lambda x: int(x), date))

        if len(date) >= 3:
            if len(date[0]) > 4 or len(date[1]) > 2 or len(date[2]) > 2:
                return False

            if (dateint[0] > self.__Now__.year 
                or dateint[1] > 12 or dateint[1] < 1 
                or dateint[2] > 31 or dateint[2] < 1 ):
                return False

        if len(date) >= 5:
            if len(date[3]) > 2 or len(date[4]) > 2:
                return False

            if (dateint[3] > 24 or dateint[3] < 0 
                or dateint[4] > 60 or dateint[4] < 0):
                return False

        if len(date) >= 6:
            if len(date[5]) > 2:
                return False

            if (dateint[5] > 60 or dateint[5] < 0):
                return False

        return True


    def __FixedDateData__(self, data):
        year = ''
        month = ''
        day = ''
        hour = ''
        minute = ''
        second = ''

        yearlist = list(re.findall(u'^(\d*)\D', data))
        if len(yearlist):
            year = yearlist[0]

            if len(year) > 4:
                while (len(year) > 4 and year[0] != u'2'):
                    year = year[1:]
                    data = data[1:]

            if len(year) > 4:
                year = year[:4]
                data = data[len(year):]
            else:
                data = data[len(year) + 1:]
                if len(year) == 4:
                    if year[0] != u'2':
                        year = u'2' + year[1:]
                    elif year == u'2077':
                        year = u'2017'
                elif len(year) == 3:
                    year = u'2' + year
                elif len(year) == 2:
                    year = u'20' + year
                elif len(year) == 1 :
                    now = str(self.__Now__.year)
                    year = now[0:len(now) - 1] + year
                else:
                    return ConfidenceLevel.Bad, (year, month, day)
        else:
            if len(data) >= 4:
                year = data[0:4]
                data = data[4:]
            else:
                return ConfidenceLevel.Bad, (year, month, day) 

        data = data.lstrip()

        monthlist = list(re.findall(u'^(\d*)\D', data))
        if len(monthlist):
            month = monthlist[0]
            if len(month) > 2:
                month = month[:2]
                data = data[len(month):]
                if month in self.__MonthSwapMapping_:
                    month = self.__MonthSwapMapping_[month]
            else:
                data = data[len(month) + 1:]
                if month in self.__MonthSwapMapping_:
                    month = self.__MonthSwapMapping_[month]
                elif len(month) == 2:
                    imonth = int(month)
                    if imonth < 1:
                        return False, year, month, day
                    elif imonth > 12 and imonth < 20:
                        if month[1] == u'7':
                            month = month[0] + u'1'
                elif len(month) == 1:
                    month = u'0' + month
        else:
            if len(data) >= 2:
                month = data[0:2]
                data = data[2:]
            else:
                return ConfidenceLevel.Bad, (year, month, day)

        data = data.lstrip()

        daylist = list(re.findall(u'^(\d*)\D?', data))
        if len(daylist):
            day = daylist[0]
            if len(day) > 2:
                day = day[0:2]
                data = data[len(month):]
            else:
                data = data[len(day):]
                if len(day) == 2:
                    iday = int(day)
                    if iday > 31 and iday < 40:
                        if day[1] == u'6':
                            day = day[0] + u'0'
                elif len(day) == 1:
                    day = u'0' + day
        else:
            if len(data) >= 2:
                month = data[0:2]
                data = data[2:]
            else:
                return ConfidenceLevel.Bad, (year, month, day)

        while (len(data) and (data[0] > '9' or data[0] < '0')):
            data = data[1:]

        if len(data) == 0:
            return ConfidenceLevel.Fixed, (year, month, day)

        hourlist = list(re.findall(u'^(\d*)\D', data))
        if len(hourlist):
            hour = hourlist[0]
            if len(hour) > 2:
                hour = hour[0:2]
                data = data[len(hour):]
            else:
                data = data[len(hour) + 1:]
        else:
            if len(data) >=1:
                hour = data[0:min(len(data), 2)]
                data = data[min(len(data), 2):]
            else:
                return ConfidenceLevel.Fixed, (year, month, day)

        while (len(data) and (data[0] > '9' or data[0] < '0')):
            data = data[1:]

        minutelist = list(re.findall(u'^(\d*)\D?', data))
        if len(minutelist):
            minute = minutelist[0]
            if len(hour) > 2:
                hour = hour[0:2]
                data = data[len(minute):]
            else:
                data = data[len(minute) + 1:]
        else:
            if len(data) >=1:
                minute = data[0:min(len(data), 2)]
                data = data[min(len(data), 2):]
            else:
                return ConfidenceLevel.Fixed, (year, month, day, hour)

        while (len(data) and (data[0] > '9' or data[0] < '0')):
            data = data[1:]

        if len(data) == 0:
            return ConfidenceLevel.Fixed, (year, month, day, hour, minute)

        secondlist = list(re.findall(u'^(\d*)\D?', data))
        if len(secondlist):
            second = secondlist[0]
        else:
            if len(data) >=1:
                second = data[0:min(len(data), 2)]
            else:
                return ConfidenceLevel.Fixed, (year, month, day, hour, minute)

        return ConfidenceLevel.Fixed, (year, month, day, hour, minute, second)

