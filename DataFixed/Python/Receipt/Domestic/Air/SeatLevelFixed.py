# -*- encoding: utf-8 -*-

import logging
import re

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel

class SeatLevelFixed(DataFixed):
    """description of class"""
    def __init__(self):
        DataFixed.__init__(self, u'SeatLevel')
        self.__SeatLevels__ = list(u'FACDYSBHKLMNQTXUEWROZVGP')

    def __FixedData__(self, resultJson):
        seat_levels = self.__ParseData__(resultJson)
        if len(seat_levels) == 0:
            logging.info(u'Seat Level Error')
            return ConfidenceLevel.Bad, ''

        logging.info(seat_levels[0] + u' Fixed To ')
        
        confidencelevel, seat_level = self.__FixedSeatLevel__(seat_levels)

        logging.info(seat_level)

        return confidencelevel, seat_level


    def __ParseData__(self, resultJson):
        seat_levels = []

        if resultJson == None or not isinstance(resultJson, dict):
            return seat_levels
    
        for result in resultJson.get(u'result', []):
            if len(result):
                seat_levels.append(result)

        for result in resultJson.get(u'ref_result', []):
            if len(result):
                seat_levels.append(result)
    
        return seat_levels


    def __CheckData__(self, seat_level):
        return seat_level in self.__SeatLevels__


    def __FixedSeatLevel__(self, seat_levels):
        for seat_level in seat_levels:
            if self.__CheckData__(seat_level):
                return ConfidenceLevel.Confident, seat_level

        return ConfidenceLevel.Bad, seat_levels[0]

    