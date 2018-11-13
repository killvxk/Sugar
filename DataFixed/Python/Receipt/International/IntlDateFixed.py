# -*- encoding: utf-8 -*-

import logging
import re

from ConfidenceLevel import ConfidenceLevel
from DataFixed import DataFixed
from utils.dateutil import parser

class IntlDateFixed(DataFixed):
    def __init__(self):
        DataFixed.__init__(self, u'Internation Date')
        self.__ResultFormat__ = '%Y-%m-%d'
        self.__MappingPatterns__ = r'(JANUARY|FEBRUARY|MARCH|APRIL|MAY|JUNE|JULY|AUGUST|SEPTEMBER|OCTOBER|NOVEMBER|DECEMBRE)'


    def __FixedData__(self, resultJson):
        date_list = self.__ParseData__(resultJson)
        if not len(date_list):
            logging.info(u'Validated Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(date_list[0] + u' Fixed To ')
        date_list = self.__TripDate__(date_list)

        time_confidence, time = self.__DateFix__(date_list)

        logging.info(time)

        return time_confidence, time


    def __ParseData__(self, jsondata):
        result_list = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return result_list

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None:
                continue

            cls = region[u'cls']
            if cls == 2:
                for result in region[u'result']:
                    if len(result):
                        result_list.append(result)

                if region.get(u'ref_result') != None:
                    for result in region[u'ref_result']:
                        if len(result):
                            result_list.append(result)

        return result_list


    def __DateFix__(self, date_list):
        confidence_level = ConfidenceLevel.Bad
        result = u''

        for date in date_list:
            try:
                parse_date = parser.parse(date, dayfirst=False)
                result = parse_date.strftime(self.__ResultFormat__)
                confidence_level = ConfidenceLevel.Fixed
                # TODO check and fix result
                break
            except Exception:
                continue

        if result.strip():
            return confidence_level, result

        return ConfidenceLevel.Bad, date_list[0]


    def __TripDate__(self, date_list):
        result = []
        for date in date_list:
            tripped_date = re.sub(r'[^A-Z0-9\.\-\/\: ]', u' ', date.upper())

            # full name to shorthand
            regex = re.search(self.__MappingPatterns__, tripped_date)
            if regex:
                tripped_date = re.sub(regex.group(1), regex.group(1)[0:3], tripped_date)

            # similar to '07FEB' , format to '07 FEB'
            regex = re.search(r'(\d{1,})(JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC)', tripped_date)
            if regex:
                index = tripped_date.find(regex.group(2))
                date_chars = list(tripped_date)
                date_chars.insert(index, u' ')
                tripped_date = u''.join(date_chars)

            result.append(tripped_date)

        return result
