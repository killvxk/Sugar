#!/usr/bin/env python2
# -*- coding:utf-8 -*-

import re
import logging

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel

class StationFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self, u'Station')
        self.__SpecialCharPatterns__ = r'/'

    def __FixedData__(self, resultJson):
        stations = self.__ParseData__(resultJson)

        if len(stations) == 0:
            logging.info(u'Station Error')
            return ConfidenceLevel.Bad, ''

        logging.info(stations[0] + u' Fixed To ')
        
        confidencelevel, station = self.__FixStation__(stations)

        logging.info(station)

        return confidencelevel, station


    def __ParseData__(self, resultJson):
        date_list = []

        if resultJson == None or not isinstance(resultJson, dict):
            return date_list
    
        for result in resultJson.get(u'result', []):
            if len(result):
                date_list.append(result.strip())

        for result in resultJson.get(u'ref_result', []):
            if len(result):
                date_list.append(result.strip())
    
        return date_list

    def __FixStation__(self, stations):
        station_name = stations[0]
        for ch in re.sub(self.__SpecialCharPatterns__, u'', station_name):
            if not ch in self.__NumberPatterns__ + self.__LetterPatterns__ + (u'/',):
                return ConfidenceLevel.Bad, self.__TripLetter__(station_name)

        return ConfidenceLevel.Bad, station_name

    def __TripLetter__(self, station_name):
        station_name = re.sub(r'\w|/', u'', station_name)
        return station_name
