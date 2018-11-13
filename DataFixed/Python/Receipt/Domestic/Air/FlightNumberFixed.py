# -*- coding: utf-8 -*-

import logging
import re

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel

class FlightNumberFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self, u'FlightNo')
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0


    def StartFixedFromJson(self, resultJson):
        return self.__FixedData__(resultJson)


    def __BeforeFixed__(self):
        logging.info(u'Start Fixed Flight NO=================>\n')

    
    def __FixedData__(self, resultJson):
        result_numberlist = self.__ParseData__(resultJson)
        if len(result_numberlist) == 0:
            logging.info(u'Flight NO Error')
            return ConfidenceLevel.Bad, ''

        logging.info(result_numberlist[0] + u' Fixed To ')
        
        confidencelevel, number = self.__FixedNumberData__(result_numberlist)

        logging.info(number)

        return confidencelevel, number


    def __ParseData__(self, resultJson):
        flights = []

        if resultJson == None or not isinstance(resultJson, dict):
            return flights
    
        for result in resultJson.get(u'result', []):
            if len(result):
                flights.append(result)

        for result in resultJson.get(u'ref_result', []):
            if len(result):
                flights.append(result)

        tripped_flights = []
        for flight_no in flights:
            tripped_flights.append(self.__TripSpecialChar__(flight_no))
    
        return tripped_flights


    def __AfterFixed__(self):
        logging.info(u'Error Count ' + str(self.__ErrorCount__) + u', Fixed Count ' + str(self.__FixedCount__))

        logging.info(u'\n<=================End Fixed Flight NO')


    def __FixedNumberData__(self, flights):
        for flight_number in flights:
            regex = re.search(r'([A-Z][A-Z]|[0-9][A-Z]|[A-Z][0-9])([0-9]{3,4})', flight_number)
            if regex:
                return ConfidenceLevel.Bad, regex.group(1) + regex.group(2)

        for flight_number in flights:
            regex = re.search(r'.*([A-Z][A-Z]|[0-9][A-Z]|[A-Z][0-9]).*?([0-9]{3,4})', flight_number)
            if regex:
                return ConfidenceLevel.Bad, regex.group(1) + regex.group(2)

        return ConfidenceLevel.Bad, flights[0]


    def __TripSpecialChar__(self, number):
        formatted_number = u''
        for ch in number:
            if ch.upper() in self.__LetterPatterns__ or ch.upper() in self.__NumberPatterns__ :
                formatted_number += ch.upper()

        return formatted_number
