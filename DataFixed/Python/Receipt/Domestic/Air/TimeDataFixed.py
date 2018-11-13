# -*- coding: utf-8 -*-

import logging
import re

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel

class TimeDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self, 'Time')
        self.__Seperator__ = u':'
        self.__SpecialSeperator__ = u'时|分'
        self.__Numbers__ = (u'0', u'1', u'2', u'3', u'4', u'5', u'6', u'7', u'8', u'9')

    
    def __FixedData__(self, resultJson):
        timelist = self.__ParseData__(resultJson)

        if len(timelist) == 0:
            logging.info(u'Time Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(timelist[0] + u' Fixed To ')
        
        confidencelevel, time = self.__FixedTimeData__(timelist)

        logging.info(time)

        return confidencelevel, time


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_timelist = self.__ParseData__(resultJson)
        validated_timelist = self.__ParseData__(validateJson)

        if len(result_timelist) == 0 or len(validated_timelist) == 0 or not self.__CheckData__(validated_timelist[0]):
            logging.info(u'Validated Data Error')
            return

        if result_timelist[0] != validated_timelist[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_timelist[0] + u' Fixed To ')

        confidencelevel, time = self.__FixedTimeData__(result_timelist)

        logging.info(time)

        if validated_timelist[0] == time:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_timelist[0])
            logging.info(u'Fixed Falied!')


    def __ParseData__(self, resultJson):
        times = []

        if resultJson == None or not isinstance(resultJson, dict):
            return times
    
        for result in resultJson.get(u'result', []):
            if len(result):
                times.append(result)

        for result in resultJson.get(u'ref_result', []):
            if len(result):
                times.append(result)
    
        return times


    def __CheckData__(self, data):
        seperator_times = data.split(self.__Seperator__)
        tag = True
        have_less_than_two = False
        for time in seperator_times:
            if len(time) > 2:
                return False

            if len(time) < 2:
                have_less_than_two = True

        if have_less_than_two:
            return have_less_than_two

        return tag and (len(data) == 5 or len(data) == 8)


    def __FixedTimeData__(self, timelist):
        for time in timelist:
            if self.__CheckData__(time):
                return ConfidenceLevel.Bad, time

        seperator_times = re.split(self.__SpecialSeperator__, timelist[0])
        if len(seperator_times) > 1:
            return ConfidenceLevel.Bad, timelist[0]

        seperator_times = timelist[0].split(self.__Seperator__)
        fixed_time = ''
        for time in seperator_times:
            if len(time) == 4:
                temp_time_list = list(time)
                temp_time_list.insert(2, self.__Seperator__)
                if len(fixed_time):
                    fixed_time += self.__Seperator__
                fixed_time += u''.join(temp_time_list)
            elif len(time) <= 2:
                if len(fixed_time):
                    fixed_time += self.__Seperator__
                fixed_time += time

            elif len(time) == 5:
                if len(fixed_time):
                    fixed_time += self.__Seperator__
                temp_time_list = list(time)
                temp_time_list[2] = self.__Seperator__
                fixed_time += u''.join(temp_time_list)
            
            elif len(time) == 3:
                if len(fixed_time):
                    fixed_time += self.__Seperator__

                candidate_char = list(filter(lambda x: x in self.__Numbers__, time))

                if len(candidate_char) > 2:
                    fixed_time += u''.join(candidate_char[:2])
                else:
                    fixed_time += u''.join(candidate_char)

            elif len(time) == 6:
                candidate_char = list(time)
                candidate_char.insert(4, self.__Seperator__)
                candidate_char.insert(2, self.__Seperator__)
                fixed_time = u''.join(candidate_char)
        
        if self.__CheckData__(fixed_time):
            return ConfidenceLevel.Bad, fixed_time

        return ConfidenceLevel.Bad, timelist[0]