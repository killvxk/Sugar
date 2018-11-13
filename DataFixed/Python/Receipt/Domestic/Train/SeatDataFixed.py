#!/usr/bin/env python2
# -*- coding:utf-8 -*-

import logging

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel
from utils import StrUtil

SeatList = (u'新空调二等座', u'动卧', u'二等座', u'新空调硬座', u'商务座特等座', u'一等座', u'高级软卧', u'软卧', u'硬卧', u'硬座', u'无座', u'新空调软座', u'新空调硬卧', u'新空调一等座', u'新空调硬卧', u'新空调软卧', u'软座', u'商务座', u'新空软卧', u'空调软座', u'特等座', u'硬卧代硬座', u'新空调硬座快速')

class SeatDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self, 'Seat')

    
    def __FixedData__(self, resultJson):
        result_seatlist = self.__ParseData__(resultJson)
        if len(result_seatlist) == 0:
            logging.info(u'Seat Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(result_seatlist[0] + u' Fixed To ')
        
        confidencelevel, seat = self.__FixedSeatData__(result_seatlist)

        logging.info(seat)

        return confidencelevel, seat


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_seatlist = self.__ParseData__(resultJson)
        validated_seatlist = self.__ParseData__(validateJson)

        if len(validated_seatlist) == 0 or len(result_seatlist) == 0:
            logging.info(u'Validated Data Error')
            return

        if validated_seatlist[0] != result_seatlist[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_seatlist[0] + u' Fixed To ')
        
        confidencelevel, seat = self.__FixedSeatData__(result_seatlist)

        logging.info(seat)

        if validated_seatlist[0] == seat:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_seatlist[0])
            logging.info(u'Fixed Falied!')


    def __ParseData__(self, jsondata):
        seatlist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return seatlist

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None:
                continue

            cls = region[u'cls']
            if cls == 18:
                for result in region[u'result']:
                    if len(result):
                        seatlist.append(result)

                if region.get(u'ref_result') != None:
                    for result in region[u'ref_result']:
                        if len(result):
                            seatlist.append(result)

        return seatlist


    def __FixedSeatData__(self, datalist):
        min_similarity = 1000
        result = ''

        for data in datalist:
            if data in SeatList:
                return ConfidenceLevel.Confident, data

            for seat in SeatList:
                similarity = StrUtil.Similarity(seat, data)
                if similarity < len(data) and min_similarity >= similarity:
                    if (min_similarity == similarity and len(result) >= len(seat)):
                        continue
                    
                    min_similarity = similarity
                    result = seat

            if min_similarity == 1000:
                return ConfidenceLevel.Bad, data
            else:
                return ConfidenceLevel.Fixed, result

        return ConfidenceLevel.Bad, ''