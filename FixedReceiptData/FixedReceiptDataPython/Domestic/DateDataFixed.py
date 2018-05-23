#!/usr/bin/env python2
# -*- coding:utf-8 -*-

import datetime
import re
import logging

from DataFixed import DataFixed
from DataFixed import ConfidenceLevel

class DateDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self)
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0
        self.__Patterns__ = u'年|月|日|-|\\.| '
        self.__Now__ = datetime.datetime.now()
        self.__MonthSwapMapping_ = {u'13' : u'10'}


    def __BeforeFixed__(self):
        print(u'Start Fixed Date Data=================>\n')

    
    def __FixedData__(self, resultJson):
        result_datelist = self.__ParseData__(resultJson)
        if len(result_datelist) == 0:
            print(u'Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(result_datelist[0] + u' Fixed To ')

        date = ''
        confidencelevel = ConfidenceLevel.Bad
        for result in result_datelist:
            confidencelevel, year, month, day = self.__FixedDateData__(result)
            date = year + u'年' + month + u'月' + day + u'日'
            if (confidencelevel == ConfidenceLevel.Confident or confidencelevel == ConfidenceLevel.Fixed) and self.__CheckData__(date):
                break

        logging.info(date)

        return confidencelevel, date


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_datelist = self.__ParseData__(resultJson)
        validated_datelist = self.__ParseData__(validateJson)

        if len(validated_datelist) == 0 or len(result_datelist) == 0 or not self.__CheckData__(validated_datelist[0]):
            print(u'Validated Data Error')
            return

        if validated_datelist[0] != result_datelist[0]:
            self.__ErrorCount__ += 1
        else:
            print(u'Validated Equal To Result')
            return

        print(u'Validated Not Equal To Result')
        logging.info(result_datelist[0] + u' Fixed To ')

        date = ''
        for result in result_datelist:
            confidencelevel, year, month, day = self.__FixedDateData__(result)
            date = year + u'年' + month + u'月' + day + u'日'
            if (confidencelevel == ConfidenceLevel.Confident or confidencelevel == ConfidenceLevel.Fixed) and self.__CheckData__(date):
                break

        logging.info(date)

        if validated_datelist[0] == date:
            self.__FixedCount__ += 1
            print(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_datelist[0])
            print(u'Fixed Falied!')


    def __AfterFixed__(self):
        print(u'Error Count ' + str(self.__ErrorCount__) + u', Fixed Count ' + str(self.__FixedCount__))

        print(u'\n<=================End Fixed Date Data')


    def __ParseData__(self, jsondata):
        datelist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return datelist

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None:
                continue

            cls = region[u'cls']
            if cls == 3:
                for result in region[u'result']:
                    if len(result):
                        datelist.append(result)

                for result in region[u'ref_result']:
                    if len(result):
                        datelist.append(result)

        return datelist


    def __CheckData__(self, data):
        if len(data) == 0:
           return False
        
        date = re.split(self.__Patterns__, data)
        date = list(filter(lambda x: len(x) != 0, date))

        if len(date) != 3:
            return False

        if len(date[0]) != 4 or len(date[1]) != 2 or len(date[2]) != 2:
            return False

        date = list(map(lambda x: int(x), date))

        if date[0] > self.__Now__.year or date[1] > 12 or date[1] < 1 or date[2] > 31 or date[2] < 1:
            return False

        return True


    def __FixedDateData__(self, data):
        if self.__CheckData__(data):
            date = re.split(self.__Patterns__, data)
            date = list(filter(lambda x: len(x) != 0, date))
            return ConfidenceLevel.Confident, date[0], date[1], date[2]

        year = ''
        month = ''
        day = ''

        yearlist = list(re.findall(u'^(\\d*)[年|-|\\.]', data))
        if len(yearlist):
            year = yearlist[0]
            if len(year) > 4:
                while (len(year) > 4 and year[0] != u'2'):
                    year = year[1:]

            if len(year) == 4:
                if year[0] != u'2':
                    year = u'2' + year[1:]
                pass
            elif len(year) == 3:
                year = u'2' + year
            elif len(year) == 2:
                year = u'20' + year
            else:
                return ConfidenceLevel.Bad, year, month, day
        else:
            if len(data) > 4:
                year = data[0:4]
                data = data[0:4] + u'年' + data[4:]

        monthlist = list(re.findall(u'[年|-|\\.](\d*)[月|-|\\.]', data))
        if len(monthlist):
            month = monthlist[0]
            if month in self.__MonthSwapMapping_:
                data.replace(month, self.__MonthSwapMapping_[month])
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
            yearindex = re.search(u'[年|-|\\.]', data).span()[1]
            temp = data[yearindex:]
            if len(temp) > 2:
                month = temp[0:2]
                data = data[0:yearindex] + month + u'月' + data[yearindex + 2:]

        daylist = list(re.findall(u'[月|-|\\.](\\d*)日?', data))
        if len(daylist):
            day = daylist[0]
            if len(day) > 2:
                day = day[0:2]
            elif len(day) == 2:
                iday = int(day)
                if iday > 31 and iday < 40:
                    if day[1] == u'6':
                        day = day[0] + u'0'
            elif len(day) == 1:
                day = u'0' + day
        else:
            return ConfidenceLevel.Bad, year, month, day

        return ConfidenceLevel.Fixed, year, month, day

