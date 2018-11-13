#!/usr/bin/env python2
# -*- coding:utf-8 -*-

import re
import datetime
import logging
import datetime

from DateDataFixed import DateDataFixed
from ConfidenceLevel import ConfidenceLevel

class FlightDateDataFixed(DateDataFixed):
    """description of class"""

    def __init__(self):
        DateDataFixed.__init__(self)
        self.__Now__ = datetime.datetime.now() + datetime.timedelta(days = 366)
        self.__SuffixPatterns__ = (u'JAN', u'FEB', u'MAR', u'APR', u'MAY', u'JUN', u'JUL', u'AUG', u'SEP', u'OCT', u'NOV', u'DEC')

    def __FixedData__(self, resultJson):
        date_list = self.__ParseData__(resultJson)

        # 日期格式 MM-DD
        for date in date_list:
            if len(date) <= 6 and len(date.split(u'-')) == 2:
                if self.__CheckData__(u'{}-{}'.format(self.__Now__.year, date)):
                    logging.info(date_list[0] + u' Fixed To ')
                    logging.info(date)
                    return ConfidenceLevel.Bad, date

        # 英文日期
        for date in date_list:
            if len(date) >= 5 and date[len(date) - 3:] in self.__SuffixPatterns__:
                date_num = u''
                for ch in self.__ReplaceConfluingChar__(date[:len(date) - 3]):
                    if ch in self.__NumberPatterns__:
                        logging.info(ch)
                        date_num += ch
                        if len(date_num) > 2:
                            break

                formatted_date = date_num + date[len(date) - 3:]
                if self.__CheckDateEn__(formatted_date):
                    logging.info(date_list[0] + u' Fixed To ')
                    logging.info(formatted_date)
                    return ConfidenceLevel.Bad, formatted_date

        date_confidence_level, date = super(FlightDateDataFixed, self).__FixedData__(resultJson)

        # 机票没有 YYYYMMDD 格式
        regex = re.search(r'(^\d{8}$)', date.strip())
        if regex:
            date = regex.group()
            date = u'{}-{}-{}'.format(date[:4], date[4:6], date[6:])

        return date_confidence_level, date


    def __ParseData__(self, resultJson):
        date_list = []

        if resultJson == None or not isinstance(resultJson, dict):
            return date_list
    
        for result in resultJson.get(u'result', []):
            if len(result):
                date_list.append(result)

        for result in resultJson.get(u'ref_result', []):
            if len(result):
                date_list.append(result)
    
        return date_list

    def __CheckDateEn__(self, date):
        if len(date) != 5:
            return False

        if not date[len(date) - 3:] in self.__SuffixPatterns__ or not date[:len(date) - 3].isdigit():
            return False

        return True

    def __ReplaceConfluingChar__(self, date):
        return re.sub(r'O|o', u'0', date)
