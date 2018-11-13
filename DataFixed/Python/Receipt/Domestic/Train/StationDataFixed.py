import logging

import Domestic.Train.StationList as StationList
from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel
from utils import StrUtil


class StationDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self, 'Station')

    
    def __FixedData__(self, resultJson):
        result_startstationlist, result_endstationlist = self.__ParseData__(resultJson)
        if len(result_startstationlist) == 0 or len(result_endstationlist) == 0:
            logging.info(u'Station Data Error')
            return ConfidenceLevel.Bad, '', ''

        logging.info(result_startstationlist[0] + ', ' + result_endstationlist[0] + u' Fixed To ')
        
        startconfidencelevel, startstation = self.__FixedStationData__(result_startstationlist)
        endstationconfidencelevel, endstation = self.__FixedStationData__(result_endstationlist)
        confidencelevel = {ConfidenceLevel.Bad : 0, ConfidenceLevel.Confident : 0, ConfidenceLevel.Fixed : 0}
        if startconfidencelevel == ConfidenceLevel.Bad or endstationconfidencelevel == ConfidenceLevel.Bad:
            confidencelevel = ConfidenceLevel.Bad
        elif startconfidencelevel == ConfidenceLevel.Fixed or endstationconfidencelevel == ConfidenceLevel.Fixed:
            confidencelevel = ConfidenceLevel.Fixed
        else:
            confidencelevel = ConfidenceLevel.Confident

        logging.info(startstation + ', ' + endstation)

        return confidencelevel, startstation, endstation


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_startstationlist, result_endstationlist = self.__ParseData__(resultJson)
        validated_startstationlist, validated_endstationlist = self.__ParseData__(validateJson)

        if len(validated_startstationlist) == 0 or len(validated_endstationlist) == 0 or len(result_startstationlist) == 0 or len(result_endstationlist) == 0:
            logging.info(u'Validated Data Error')
            return

        if validated_startstationlist[0] != result_startstationlist[0] or validated_endstationlist[0] != result_endstationlist[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_startstationlist[0] + ', ' + result_endstationlist[0] + u' Fixed To ')
        
        startconfidencelevel, startstation = self.__FixedStationData__(result_startstationlist)
        endstationconfidencelevel, endstation = self.__FixedStationData__(result_endstationlist)
        confidencelevel = {ConfidenceLevel.Bad : 0, ConfidenceLevel.Confident : 0, ConfidenceLevel.Fixed : 0}

        logging.info(startstation + ', ' + endstation)

        if validated_startstationlist[0] == startstation and validated_endstationlist[0] == endstation:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_startstationlist[0] + ', ' + validated_endstationlist[0])
            logging.info(u'Fixed Falied!')


    def __ParseData__(self, jsondata):
        startstationlist = []
        endstationlist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return startstationlist, endstationlist

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None:
                continue

            cls = region[u'cls']
            if cls == 15:
                for result in region[u'result']:
                    if len(result):
                        startstationlist.append(result)

                if region.get(u'ref_result') != None:
                    for result in region[u'ref_result']:
                        if len(result):
                            startstationlist.append(result)

            elif cls == 16:
                for result in region[u'result']:
                    if len(result):
                        endstationlist.append(result)

                if region.get(u'ref_result') != None:
                    for result in region[u'ref_result']:
                        if len(result):
                            endstationlist.append(result)

        return startstationlist, endstationlist


    def __FixedStationData__(self, datalist):
        min_similarity = 1000
        result = ''

        for data in datalist:
            if data in StationList.StationList:
                return ConfidenceLevel.Confident, data

            for station in StationList.StationList:
                similarity = StrUtil.Similarity(station, data)
                if similarity < len(data) and min_similarity >= similarity:
                    if (min_similarity == similarity and len(result) >= len(station)):
                        continue
                    
                    min_similarity = similarity
                    result = station

            if min_similarity == 1000:
                return ConfidenceLevel.Bad, data
            else:
                return ConfidenceLevel.Fixed, result

        return ConfidenceLevel.Bad, ''


