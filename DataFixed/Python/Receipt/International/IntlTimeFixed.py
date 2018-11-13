# -*- encoding: utf-8 -*-

import logging
import re

from ConfidenceLevel import ConfidenceLevel
from DataFixed import DataFixed
from utils.dateutil import parser

class IntlTimeFixed(DataFixed):
    def __init__(self):
        DataFixed.__init__(self, u'Internation Date')
        self.__ResultFormat__ = '%h:%m:%s'

    def __FixedData__(self, resultJson):
        time_list = self.__ParseData__(resultJson)
        if not len(time_list):
            logging.info(u'Validated Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(time_list[0] + u' Fixed To ')
        time_list = self.__TripDate__(time_list)

        time_confidence, time = self.__TimeFix__(time_list)

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
            if cls == 7:
                for result in region[u'result']:
                    if len(result):
                        result_list.append(result)

                if region.get(u'ref_result') != None:
                    for result in region[u'ref_result']:
                        if len(result):
                            result_list.append(result)

        return result_list


    def __TimeFix__(self, time_list):
        confidence_level = ConfidenceLevel.Bad
        result = u''

        for time in time_list:
            try:
                parse_time = parser.parse(time)
                confidence_level = ConfidenceLevel.Fixed

                result = self.__GetTime__(time, parse_time)
                break
            except Exception:
                continue

        if result.strip():
            return confidence_level, result
        
        # TODO check and fix result

        # similar to 13;44 fix to 13 44  -> fix to 13:44
        result = re.sub(r' ', u':', time_list[0].strip())

        return ConfidenceLevel.Bad, result


    def __TripDate__(self, time_list):
        result = []
        for time in time_list:
            tripped_time = re.sub(r'[^AP0-9\: ]', u' ', time.upper())
            result.append(tripped_time)

        return result

    def __GetTime__(self, origin, time):
        result = u''
        precision = origin.split(u':')
        if len(precision) == 2:
            result = u'{:0>2d}:{:0>2d}'.format(time.hour, time.minute)
        elif len(precision) == 3:
            result = u'{:0>2d}:{:0>2d}:{:0>2d}'.format(time.hour, time.minute, time.second)

        return result