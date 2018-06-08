import logging

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel


class StationDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self)
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0


    def __BeforeFixed__(self):
        logging.info(u'Start Fixed Station Data=================>\n')

    
    def __FixedData__(self, resultJson):
        result_startstationlist, result_endstationlist = self.__ParseData__(resultJson)
        if len(result_startstationlist) == 0 or len(result_endstationlist) == 0:
            logging.info(u'Station Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(result_startstationlist[0] + ', ' + result_endstationlist[0] + u' Fixed To ')
        
        confidencelevel, result_startstation, result_endstation = ConfidenceLevel.Bad, result_startstationlist[0], result_endstationlist[0]

        logging.info(result_startstation + ', ' + result_endstation)

        return confidencelevel, result_startstation, result_endstation


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
        
        confidencelevel, result_startstation, result_endstation = ConfidenceLevel.Bad, result_startstationlist[0], result_endstationlist[0]

        logging.info(result_startstation + ', ' + result_endstation)

        if validated_startstationlist[0] == result_startstationlist[0] and validated_endstationlist[0] == result_endstationlist[0]:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_startstationlist[0] + ', ' + validated_endstationlist[0])
            logging.info(u'Fixed Falied!')


    def __AfterFixed__(self):
        logging.info(u'Error Count ' + str(self.__ErrorCount__) + u', Fixed Count ' + str(self.__FixedCount__))

        logging.info(u'\n<=================End Fixed Station Data')


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