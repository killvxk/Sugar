import logging
import re

import Domestic.Train.TrainNumberList as TrainNumberList
from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel
from utils import StrUtil


class TrainNumberDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self, 'TrainNumber')

    
    def __FixedData__(self, resultJson):
        result_trainnumberlist = self.__ParseData__(resultJson)
        if len(result_trainnumberlist) == 0:
            logging.info(u'TrainNumber Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(result_trainnumberlist[0] + u' Fixed To ')
        
        confidencelevel, trainnumber = self.__FixedTrainNumberData__(result_trainnumberlist)

        logging.info(trainnumber)

        return confidencelevel, trainnumber


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_trainnumberlist = self.__ParseData__(resultJson)
        validated_trainnumberlist = self.__ParseData__(validateJson)

        if len(validated_trainnumberlist) == 0 or len(result_trainnumberlist) == 0 or not self.__CheckData__(validated_trainnumberlist[0]):
            logging.info(u'Validated Data Error')
            return

        if validated_trainnumberlist[0] != result_trainnumberlist[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_trainnumberlist[0] + u' Fixed To ')
        
        confidencelevel, trainnumber = ConfidenceLevel.Bad, result_trainnumberlist[0]

        logging.info(trainnumber)

        if validated_trainnumberlist[0] == trainnumber:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_trainnumberlist[0])
            logging.info(u'Fixed Falied!')


    def __ParseData__(self, jsondata):
        trainnumberlist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return trainnumberlist

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None:
                continue

            cls = region[u'cls']
            if cls == 17:
                for result in region[u'result']:
                    if len(result):
                        trainnumberlist.append(result)

                for result in region[u'ref_result']:
                    if len(result):
                        trainnumberlist.append(result)

        return trainnumberlist


    def __CheckData__(self, data):
        if len(data) == 0:
            return False

        return self.__CheckNumber__(data)


    def __FixedTrainNumberData__(self, datalist):
        trainnumber = re.match(u'[a-zA-Z]?\d{1,4}', datalist[0])
        if trainnumber and trainnumber.group() == datalist[0]:
            return ConfidenceLevel.Fixed, datalist[0]

        min_similarity = 1000
        result = datalist[0]

        for data in datalist:
            if data in TrainNumberList.TrainNumberList:
                return ConfidenceLevel.Confident, data

            for trainnumber in TrainNumberList.TrainNumberList:
                similarity = StrUtil.Similarity(trainnumber, data)
                if similarity < len(data) and min_similarity >= similarity:
                    min_similarity = similarity
                    result = trainnumber

            if min_similarity == 1000:
                return ConfidenceLevel.Bad, data
            else:
                return ConfidenceLevel.Fixed, result

        return ConfidenceLevel.Bad, ''